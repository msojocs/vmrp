import { spawn, type ChildProcessByStdio } from "node:child_process";
import type { Readable } from "node:stream";
import { createWriteStream } from "node:fs";
import { mkdtemp, readFile, rm, stat } from "node:fs/promises";
import { createConnection } from "node:net";
import { tmpdir } from "node:os";
import path from "node:path";

export type Rgb = readonly [number, number, number];

export interface VmrpE2eOptions {
  bin?: string;
  workDir?: string;
  timeoutMs?: number;
}

export interface PpmImage {
  width: number;
  height: number;
  pixel(x: number, y: number): Rgb;
  uniqueColorCount(): number;
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
  }

  static async start(mrpPath: string, options: VmrpE2eOptions = {}): Promise<VmrpE2e> {
    const tmpDir = await mkdtemp(path.join(tmpdir(), "vmrp-e2e-"));
    const vmrp = new VmrpE2e(tmpDir, options);
    await vmrp.spawn(mrpPath);
    return vmrp;
  }

  async close(): Promise<void> {
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
    if (process.env.VMRP_E2E_KEEP_TMP !== "1") {
      await rm(this.tmpDir, { recursive: true, force: true });
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
    this.process = spawn(this.bin, ["--work-dir", this.workDir, mrpPath], {
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
    }
  };
}

function isSpace(byte: number | undefined): boolean {
  return byte === 9 || byte === 10 || byte === 11 || byte === 12 || byte === 13 || byte === 32;
}

function sleep(ms: number): Promise<void> {
  return new Promise(resolve => setTimeout(resolve, ms));
}
