import { spawn, type ChildProcessByStdio } from "node:child_process";
import type { Readable } from "node:stream";
import { createWriteStream, cpSync } from "node:fs";
import { copyFile, mkdtemp, readFile, rm, stat } from "node:fs/promises";
import { createConnection } from "node:net";
import { tmpdir } from "node:os";
import path from "node:path";
import { fileURLToPath } from "node:url";

export type Rgb = readonly [number, number, number];

export interface VmrpE2eOptions {
  bin?: string;
  workDir?: string;
  timeoutMs?: number;
  /** 屏幕分辨率(--screen WxH),如 "480x320"。默认由 vmrp 决定(240x320)。 */
  screen?: `${number}x${number}`;
  /** 应用可见内存(--memory),档位 1M/2M/4M/6M/8M/16M。默认由 vmrp 决定(1M)。 */
  memory?: "1M" | "2M" | "4M" | "6M" | "8M" | "16M";
}

export interface PpmImage {
  width: number;
  height: number;
  pixel(x: number, y: number): Rgb;
  uniqueColorCount(): number;
  diffPixelCount(other: PpmImage, rect?: { x: number; y: number; width: number; height: number }): number;
}

type KeyName =
  | "ENTER" | "SELECT"
  | "ESC" | "ESCAPE" | "POWER"
  | "SOFTLEFT" | "LEFT_SOFT"
  | "SOFTRIGHT" | "RIGHT_SOFT"
  | "UP" | "DOWN" | "LEFT" | "RIGHT"
  | "SEND"
  | "STAR" | "*" 
  | "POUND" | "HASH" | "#"
  | `${number}`

export class VmrpE2e {
  readonly tmpDir: string;
  readonly socketPath: string;
  readonly stdoutPath: string;
  readonly stderrPath: string;
  readonly defaultScreenPath: string;

  private readonly bin: string;
  private readonly workDir: string;
  private readonly timeoutMs: number;
  /** 命名避免与 screen() 方法冲突:实例字段会遮蔽原型方法。 */
  private readonly screenSize?: string;
  private readonly memorySize?: string;
  private process?: ChildProcessByStdio<null, Readable, Readable>;

  private constructor(tmpDir: string, options: VmrpE2eOptions = {}) {
    this.tmpDir = tmpDir;
    this.socketPath = path.join(tmpDir, "vmrp-e2e.sock");
    this.stdoutPath = path.join(tmpDir, "stdout.log");
    this.stderrPath = path.join(tmpDir, "stderr.log");
    this.defaultScreenPath = path.join(tmpDir, "screen.ppm");
    this.bin = options.bin ?? process.env.VMRP_BIN ?? "build/vmrp";
    this.workDir = options.workDir ?? process.env.VMRP_WORK_DIR ?? ".";
    this.timeoutMs = options.timeoutMs ?? Number(process.env.VMRP_TIMEOUT_MS ?? 30_000);
    this.screenSize = options.screen;
    this.memorySize = options.memory;
  }

  static async start(mrpPath: string, options: VmrpE2eOptions = {}): Promise<VmrpE2e> {
    const tmpDir = await mkdtemp(path.join(tmpdir(), "vmrp-e2e-"));
    const vmrp = new VmrpE2e(tmpDir, options);
    await vmrp.spawn(await vmrp.prepareMrp(mrpPath));
    return vmrp;
  }

  /**
   * 确保 MRP 文件位于 work-dir 之下,否则复制一份进去再启动。
   *
   * ARM EXT 执行器把包名 alias 解析为"相对 cwd 的最短可打开路径"
   * (src/arm_ext_executor.c arm_ext_init_pack_names),且部分 EXT helper
   * 只有固定 32 字节的包名缓冲区。当 MRP 位于 work-dir 之外时 alias 退化
   * 为截断的绝对路径,包自身无法被重新打开,wbrw 等应用会误判包异常并
   * 触发联网版本上报,改变启动流程导致像素断言失败。仓库根作为 work-dir
   * 时 fixtures 本就在 cwd 下,此函数不做任何复制,保持原有行为。
   */
  private async prepareMrp(mrpPath: string): Promise<string> {
    const workDir = path.resolve(this.workDir);
    const absMrp = path.resolve(mrpPath);
    if (absMrp.startsWith(workDir + path.sep)) return mrpPath;
    const dest = path.join(workDir, path.basename(absMrp));
    await copyFile(absMrp, dest);
    return dest;
  }

  async close(): Promise<void> {
    await this.stop();
    if (process.env.VMRP_E2E_KEEP_TMP !== "1") {
      await rm(this.tmpDir, { recursive: true, force: true });
    }
  }

  async stop(): Promise<void> {
    const proc = this.process;
    if (proc && proc.exitCode === null) {
      try {
        await this.command("QUIT", 2_000);
      } catch {
        proc.kill("SIGTERM");
      }
      if (!(await this.waitForExit(5_000))) {
        proc.kill("SIGKILL");
        await this.waitForExit(2_000);
      }
    }
  }

  async drawCount(): Promise<number> {
    const response = await this.command("DRAW_COUNT");
    const match = /^OK draw_count (\d+)$/.exec(response);
    if (!match) throw new Error(`Unexpected DRAW_COUNT response: ${response}`);
    return Number(match[1]);
  }

  async click(x: number, y: number, timeoutMs = 2_000): Promise<void> {
    const previous = await this.drawCount();
    await this.command(`CLICK ${x} ${y}`);
    await this.waitDrawAfter(previous, timeoutMs);
  }

  async delay(ms: number): Promise<void> {
    await sleep(ms);
  }

  /**
   * Supported key names:
   * ENTER/SELECT, ESC/ESCAPE/POWER, SOFTLEFT/LEFT_SOFT, SOFTRIGHT/RIGHT_SOFT,
   * UP, DOWN, LEFT, RIGHT, SEND, STAR/*, POUND/HASH/#, digits 0-9, letters A-Z.
   */
  async key(name: KeyName, timeoutMs = 2_000): Promise<void> {
    const previous = await this.drawCount();
    await this.command(`KEY ${name}`);
    await this.waitDrawAfter(previous, timeoutMs);
  }

  async paste(text: string, timeoutMs = 5_000): Promise<void> {
    const previous = await this.drawCount();
    await this.command(`PASTE ${text}`);
    await this.waitDrawAfter(previous, timeoutMs);
  }

  async screen(name = "screen"): Promise<PpmImage> {
    const ppmPath = path.join(this.tmpDir, `${name}.ppm`);
    await this.command(`SCREEN ${ppmPath}`);
    return readPpm(ppmPath);
  }

  async command(command: string, timeoutMs = this.timeoutMs): Promise<string> {
    return new Promise((resolve, reject) => {
      const socket = createConnection(this.socketPath);
      let response = "";
      socket.setEncoding("utf8");
      socket.setTimeout(timeoutMs);
      socket.once("connect", () => socket.write(`${command}\n`));
      socket.on("data", chunk => {
        response += chunk;
        if (response.includes("\n")) socket.end();
      });
      socket.once("timeout", () => {
        socket.destroy();
        reject(new Error(`Timed out waiting for response to ${command}`));
      });
      socket.once("error", reject);
      socket.once("end", () => {
        const line = response.trim();
        if (line.startsWith("OK")) resolve(line);
        else reject(new Error(line || `Empty response to ${command}`));
      });
    });
  }

  private async spawn(mrpPath: string): Promise<void> {
    const args = ["--work-dir", this.workDir];
    if (this.screenSize) args.push("--screen", this.screenSize);
    if (this.memorySize) args.push("--memory", this.memorySize);
    args.push(mrpPath);
    this.process = spawn(this.bin, args, {
      env: {
        ...process.env,
        SDL_VIDEODRIVER: process.env.SDL_VIDEODRIVER ?? "dummy",
        SDL_AUDIODRIVER: process.env.SDL_AUDIODRIVER ?? "dummy",
        VMRP_E2E_SOCKET: this.socketPath,
        VMRP_PPM_PATH: this.defaultScreenPath
      },
      stdio: ["ignore", "pipe", "pipe"]
    });

    const stdout = createWriteStream(this.stdoutPath);
    const stderr = createWriteStream(this.stderrPath);
    this.process.stdout.pipe(stdout);
    this.process.stderr.pipe(stderr);

    await this.waitForSocket();
  }

  private async waitForSocket(): Promise<void> {
    const start = Date.now();
    while (Date.now() - start < this.timeoutMs) {
      if (this.process?.exitCode !== null) {
        throw new Error(`vmrp exited before E2E socket was ready: ${this.process?.exitCode}`);
      }
      try {
        const info = await stat(this.socketPath);
        if (info.isSocket()) return;
      } catch {
        // Socket is created asynchronously after SDL and VM startup.
      }
      await sleep(25);
    }
    throw new Error(`E2E socket was not ready after ${this.timeoutMs}ms`);
  }

  private async waitDrawAfter(previous: number, timeoutMs: number): Promise<void> {
    await this.command(`WAIT_DRAW ${previous} ${timeoutMs}`);
  }

  private async waitForExit(timeoutMs: number): Promise<boolean> {
    const proc = this.process;
    if (!proc || proc.exitCode !== null) return true;
    return new Promise(resolve => {
      const timer = setTimeout(() => resolve(false), timeoutMs);
      proc.once("exit", () => {
        clearTimeout(timer);
        resolve(true);
      });
    });
  }
}

/**
 * 每个测试用例独立的 mythroad 数据副本。
 *
 * 测试并发执行时,所有用例共享仓库根目录 mythroad/(wasm/dist/fs/mythroad
 * 的符号链接)会互相覆盖插件/缓存/存档,导致数据竞争。VmrpWorkspace 在临时
 * 目录中复制一份模板,把该目录作为 vmrp 的 --work-dir,实现文件级隔离。
 *
 * 生命周期须覆盖整个 it()(而非单个 VmrpE2e 实例):opglqa/font.test.ts
 * 会在同一用例内重启第二个 vmrp 实例并验证第一次启动落盘的文件被复用。
 */
export class VmrpWorkspace {
  /** 传给 VmrpE2e.start 的 workDir。 */
  readonly dir: string;

  private constructor(dir: string) {
    this.dir = dir;
  }

  static async create(): Promise<VmrpWorkspace> {
    const tmpDir = await mkdtemp(path.join(tmpdir(), "vmrp-ws-"));
    // 用 import.meta.url 定位模板源,不依赖 worker 的 cwd;直接指向符号链接
    // 目标 wasm/dist/fs/mythroad。dereference: true 确保拷贝真实文件。
    const src = fileURLToPath(new URL("../../wasm/dist/fs/mythroad", import.meta.url));
    // preserveTimestamps: true —— 模拟器/MRP 应用会依据文件时间戳判断
    // “是否需要重新联网校验”(如 wbrw 启动时的 simpleDownload 上报);
    // 不保留 mtime 会触发额外的真实网络请求,拖慢启动导致像素断言失败。
    cpSync(src, path.join(tmpDir, "mythroad"), {
      recursive: true,
      dereference: true,
      preserveTimestamps: true
    });
    return new VmrpWorkspace(tmpDir);
  }

  /** 把工作区内的相对路径(如 "mythroad/plugins/netpay.mrp")转成绝对路径。 */
  path(rel: string): string {
    return path.join(this.dir, rel);
  }

  async dispose(): Promise<void> {
    if (process.env.VMRP_E2E_KEEP_TMP !== "1") {
      await rm(this.dir, { recursive: true, force: true });
    }
  }
}

export async function readPpm(filePath: string): Promise<PpmImage> {
  const buffer = await readFile(filePath);
  let offset = 0;

  const token = (): string => {
    while (offset < buffer.length && isSpace(buffer[offset])) offset++;
    if (buffer[offset] === 35) {
      while (offset < buffer.length && buffer[offset] !== 10) offset++;
      return token();
    }
    const start = offset;
    while (offset < buffer.length && !isSpace(buffer[offset])) offset++;
    return buffer.subarray(start, offset).toString("ascii");
  };

  const magic = token();
  if (magic !== "P6") throw new Error(`Unsupported PPM magic: ${magic}`);
  const width = Number(token());
  const height = Number(token());
  const maxValue = Number(token());
  if (maxValue !== 255) throw new Error(`Unsupported PPM max value: ${maxValue}`);
  if (isSpace(buffer[offset])) offset++;

  const pixels = buffer.subarray(offset);
  const expectedBytes = width * height * 3;
  if (pixels.length < expectedBytes) {
    throw new Error(`PPM pixel data is truncated: got ${pixels.length}, expected ${expectedBytes}`);
  }
  return {
    width,
    height,
    pixel(x: number, y: number): Rgb {
      if (x < 0 || y < 0 || x >= width || y >= height) {
        throw new Error(`Pixel out of bounds: ${x},${y} for ${width}x${height}`);
      }
      const index = (y * width + x) * 3;
      return [pixels[index], pixels[index + 1], pixels[index + 2]];
    },
    uniqueColorCount(): number {
      const colors = new Set<number>();
      for (let i = 0; i < expectedBytes; i += 3) {
        colors.add((pixels[i] << 16) | (pixels[i + 1] << 8) | pixels[i + 2]);
      }
      return colors.size;
    },
    diffPixelCount(other: PpmImage, rect = { x: 0, y: 0, width, height }): number {
      if (other.width !== width || other.height !== height) {
        throw new Error(`PPM dimensions differ: ${width}x${height} vs ${other.width}x${other.height}`);
      }
      let count = 0;
      const x0 = Math.max(0, rect.x);
      const y0 = Math.max(0, rect.y);
      const x1 = Math.min(width, rect.x + rect.width);
      const y1 = Math.min(height, rect.y + rect.height);
      for (let y = y0; y < y1; y++) {
        for (let x = x0; x < x1; x++) {
          const index = (y * width + x) * 3;
          const rhs = other.pixel(x, y);
          if (pixels[index] !== rhs[0] || pixels[index + 1] !== rhs[1] || pixels[index + 2] !== rhs[2]) {
            count++;
          }
        }
      }
      return count;
    }
  };
}

function isSpace(byte: number | undefined): boolean {
  return byte === 9 || byte === 10 || byte === 11 || byte === 12 || byte === 13 || byte === 32;
}

function sleep(ms: number): Promise<void> {
  return new Promise(resolve => setTimeout(resolve, ms));
}
