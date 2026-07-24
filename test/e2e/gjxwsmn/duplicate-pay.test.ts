import { afterEach, describe, expect, it, vi } from "vitest";
import { SkyEngineE2e, SkyEngineWorkspace } from "../engine-e2e.js";
import { cpSync } from "node:fs";
import { stat } from "node:fs/promises";
import { createServer, type IncomingMessage, type Server } from "node:http";
import type { AddressInfo } from "node:net";
import { setTimeout as sleep } from "node:timers/promises";

const NON_ENTITLING_BODY = Buffer.concat([
  tlv(0x03f1, Buffer.from("000000006", "ascii")),
  tlv(0x044f, Buffer.from([0, 0, 0, 0])),
]);

interface LocalPayServer {
  readonly dnsMap: string;
  readonly stages: string[];
  readonly acceptedRegs: number;
  readonly regRequests: number;
  close(): Promise<void>;
}

async function startLocalPayServer(): Promise<LocalPayServer> {
  const stages: string[] = [];
  let acceptedRegs = 0;
  let regRequests = 0;

  const server = createServer(async (req, res) => {
    const body = await readBody(req);
    const fields = parseTlvMap(body);
    const stage = fields.get(0x0452)?.toString("ascii") ?? "";
    stages.push(stage);

    let response = NON_ENTITLING_BODY;
    if (stage === "REG") {
      regRequests++;
      const txn = fields.get(0x045b);
      if (acceptedRegs === 0 && txn?.length === 4) {
        acceptedRegs++;
        // netpay REG mode requires response type 101 to echo request 0x045b;
        // action 12 is the SDK's durable registration continuation.
        response = Buffer.concat([tlv(101, txn), tlv(100, u32be(200)), tlv(200, Buffer.from([12]))]);
      }
    } else {
      // Model the real subscription-state query latency: the UI can already
      // accept confirmation before this non-entitling PREREG response returns.
      await sleep(3_000);
    }

    res.writeHead(200, {
      "Content-Type": "application/x-tar",
      "Connection": "close",
      "Content-Length": String(response.length),
    });
    res.end(response);
  });

  await new Promise<void>((resolve, reject) => {
    server.once("error", reject);
    server.listen(0, "127.0.0.1", () => {
      server.off("error", reject);
      resolve();
    });
  });

  const address = server.address() as AddressInfo;
  return {
    dnsMap: `rop.skymobiapp.com->127.0.0.1:${address.port}`,
    stages,
    get acceptedRegs() {
      return acceptedRegs;
    },
    get regRequests() {
      return regRequests;
    },
    close: () => closeServer(server),
  };
}

function tlv(type: number, value: Buffer): Buffer {
  return Buffer.concat([u32be(type), u32be(value.length), value]);
}

function u32be(value: number): Buffer {
  const out = Buffer.alloc(4);
  out.writeUInt32BE(value);
  return out;
}

async function readBody(req: IncomingMessage): Promise<Buffer> {
  const chunks: Buffer[] = [];
  for await (const chunk of req) {
    chunks.push(Buffer.isBuffer(chunk) ? chunk : Buffer.from(chunk));
  }
  return Buffer.concat(chunks);
}

function parseTlvMap(body: Buffer): Map<number, Buffer> {
  const out = new Map<number, Buffer>();
  let offset = 0;
  for (; offset + 8 <= body.length;) {
    const type = body.readUInt32BE(offset);
    const length = body.readUInt32BE(offset + 4);
    const next = offset + 8 + length;
    if (next > body.length) return new Map();
    out.set(type, body.subarray(offset + 8, next));
    offset = next;
  }
  return offset === body.length ? out : new Map();
}

function closeServer(server: Server): Promise<void> {
  return new Promise((resolve, reject) => {
    server.close(error => error ? reject(error) : resolve());
  });
}

async function waitForBgmPrompt(engine: SkyEngineE2e, name: string): Promise<void> {
  await vi.waitFor(async () => {
    const screen = await engine.screen(name);
    // rgb(232, 48, 0)
    expect(screen.pixel(147, 87)).toEqual([232, 48, 0]);
    // rgb(176, 192, 208)
    expect(screen.pixel(116, 122)).toEqual([176, 192, 208]);
  }, { timeout: 10_000, interval: 1_000 });
}

async function waitForMenu(engine: SkyEngineE2e, name: string): Promise<void> {
  await vi.waitFor(async () => {
    const screen = await engine.screen(name);
    // rgb(24, 8, 16)
    expect(screen.pixel(206, 44)).toEqual([24, 8, 16]);
    // rgb(248, 192, 192)
    expect(screen.pixel(74, 219)).toEqual([248, 192, 192]);
  }, { timeout: 10_000, interval: 1_000 });
}


describe("gjxwsmn", () => {
  let engine: SkyEngineE2e | undefined;
  let ws: SkyEngineWorkspace | undefined;
  let payServer: LocalPayServer | undefined;

  afterEach(async () => {
    await engine?.close();
    engine = undefined;
    await payServer?.close();
    payServer = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it("重复付费", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await SkyEngineWorkspace.create();
    cpSync("test/fixtures/gjxwsmn", ws.path("mythroad/gjxwsmn"), { recursive: true });
    payServer = await startLocalPayServer();
    engine = await SkyEngineE2e.start("test/fixtures/gjxwsmn.mrp", {
      workDir: ws.dir,
      dnsMap: payServer.dnsMap,
      // Match the fixture's persisted device date to isolate payment state
      // from the game's unrelated daily-state rollover.
      deviceDate: "2026-07-23",
    });

    {
      if (!engine) throw new Error("engine is undefined");
      await waitForBgmPrompt(engine, `bgm-select-1`);

      // 取消背景音乐，进入菜单
      await engine.key('RIGHT_SOFT', 1_000);
      await waitForMenu(engine, `menu-1`);

      // 进入默认菜单，存档界面
      await engine.key('ENTER', 1_000);
      await vi.waitFor(async () => {
        const screen = await engine!.screen('save-1.0');
        // rgb(248, 220, 144)
        expect(screen.pixel(173, 156)).toEqual([248, 220, 144]);
        // rgb(48, 52, 16)
        expect(screen.pixel(41, 39)).toEqual([48, 52, 16]);
      }, { timeout: 10_000, interval: 1_000 });

      // 回车进入第一个存档
      await engine.key('ENTER', 1_000);

      await vi.waitFor(async () => {
        const screen = await engine!.screen('pay-1');
        // rgb(104, 104, 224)
        expect(screen.pixel(33, 99)).toEqual([104, 104, 224]);
      }, { timeout: 10_000, interval: 1_000 });
      await engine.delay(1_000)
      // 点击确定，确认付费操作
      await engine.key("LEFT_SOFT", 1_000, 500);
      await vi.waitFor(async () => {
        const screen = await engine!.screen('save-1.1');
        // rgb(248, 220, 144)
        expect(screen.pixel(173, 156)).toEqual([248, 220, 144]);
        // rgb(48, 52, 16)
        expect(screen.pixel(41, 39)).toEqual([48, 52, 16]);
      }, { timeout: 10_000, interval: 1_000 });
      await engine.close();
      engine = undefined;
    }
    {
      const regRequestsBeforeRelaunch = payServer.regRequests;
      const acceptedRegsBeforeRelaunch = payServer.acceptedRegs;
      engine = await SkyEngineE2e.start("test/fixtures/gjxwsmn.mrp", {
        workDir: ws.dir,
        dnsMap: payServer.dnsMap,
        deviceDate: "2026-07-23",
      });
      if (!engine) throw new Error("engine is undefined");
      await waitForBgmPrompt(engine, `bgm-select-2`);
      // 取消，不开启背景音乐，进入菜单
      await engine.key('RIGHT_SOFT', 1_000);
      await waitForMenu(engine, `menu-2`);
      // 回车进入存档界面
      await engine.key('ENTER', 1_000);
      await vi.waitFor(async () => {
        const screen = await engine!.screen('save-2.0');
        // rgb(248, 220, 144)
        expect(screen.pixel(173, 156)).toEqual([248, 220, 144]);
        // rgb(48, 52, 16)
        expect(screen.pixel(41, 39)).toEqual([48, 52, 16]);
      }, { timeout: 10_000, interval: 1_000 });

      // 回车选择第一个存档，理论上第一次付费后不会显示付费信息，直接进入游戏场景
      await engine.key('LEFT_SOFT', 1_000);
      await vi.waitFor(async () => {
        if (!engine) throw new Error("engine is undefined");
        const screen = await engine.screen("not-pay");
        // rgb(104, 104, 224)
        expect(screen.pixel(75, 171)).not.toEqual([104, 104, 224]);
      }, { timeout: 3_000, interval: 1_000 });
      expect(payServer.regRequests).toBe(regRequestsBeforeRelaunch);
      expect(payServer.acceptedRegs).toBe(acceptedRegsBeforeRelaunch);
    }
  });
});
