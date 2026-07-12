import { afterEach, describe, expect, it } from "vitest";
import fs from "node:fs";
import { type PpmImage, VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";

const targetUrl = "http://mrp.gddhy.net/";
const targetTitle = "第1页 - WAP下载站";

interface CachedPage {
  url: string;
  title: string;
}

function readCachedPages(ws: VmrpWorkspace): CachedPage[] {
  const cacheDirs = [
    ws.path("mythroad/brw/http/cache3"),
    ws.path("mythroad/brw/http/percache")
  ];
  return cacheDirs.flatMap(cacheDir => {
    if (!fs.existsSync(cacheDir)) return [];
    return fs.readdirSync(cacheDir)
      .filter(name => name.endsWith(".dat") && name !== "index.dat")
      .flatMap(name => {
        const page = fs.readFileSync(`${cacheDir}/${name}`);
        // cache pool header: 10-byte prefix, BE u16 URL length + URL,
        // then BE u16 UTF-8 title length + title.
        if (page.length < 14) return [];
        const urlLength = page.readUInt16BE(10);
        const urlStart = 12;
        const titleLengthOffset = urlStart + urlLength;
        if (titleLengthOffset + 2 > page.length) return [];
        const titleLength = page.readUInt16BE(titleLengthOffset);
        const titleStart = titleLengthOffset + 2;
        if (titleStart + titleLength > page.length) return [];
        return [{
          url: page.subarray(urlStart, titleLengthOffset).toString("utf8"),
          title: page.subarray(titleStart, titleStart + titleLength).toString("utf8")
        }];
      });
  });
}

function hasTargetCache(ws: VmrpWorkspace): boolean {
  return readCachedPages(ws)
    .some(page => page.url === targetUrl && page.title === targetTitle);
}

async function waitForTargetPage(
  vmrp: VmrpE2e,
  ws: VmrpWorkspace,
  home: PpmImage
): Promise<PpmImage> {
  const deadline = Date.now() + 60_000;
  let loaded = await vmrp.screen("loaded");
  while (Date.now() < deadline) {
    const titleChanged = loaded.diffPixelCount(home, { x: 0, y: 0, width: 240, height: 28 }) > 500;
    const bodyChanged = loaded.diffPixelCount(home, { x: 0, y: 30, width: 240, height: 250 }) > 3_000;
    const titleFinished = loaded.pixel(40, 15).every(
      (channel, index) => channel === [248, 240, 168][index]
    );
    // Cache provenance proves which request completed; the two PPM regions prove
    // that its page reached the foreground.  The settled yellow title pixel
    // excludes the blue "receiving data" progress title seen before final draw.
    if (hasTargetCache(ws) && titleChanged && bodyChanged && titleFinished) return loaded;
    await vmrp.delay(500);
    loaded = await vmrp.screen("loaded");
  }
  const cached = readCachedPages(ws)
    .map(page => `${page.url} (${page.title})`)
    .join(", ");
  throw new Error(`目标页面等待超时；已解析缓存: ${cached || "无"}`);
}

describe("wbrw 网页加载", () => {
  let vmrp: VmrpE2e | undefined;
  let ws: VmrpWorkspace | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it.each([10_000, 5_000])("空闲 %dms 后仍能访问输入的网址", async idleMs => {
    ws = await VmrpWorkspace.create();
    fs.rmSync(ws.path("mythroad/brw"), { recursive: true, force: true });
    vmrp = await VmrpE2e.start("test/fixtures/wbrw.mrp", { workDir: ws.dir });

    await vmrp.delay(2_000);
    const home = await vmrp.screen("home");
    expect(home.pixel(76, 252)).toEqual([248, 252, 248]);
    expect(home.pixel(57, 306)).toEqual([64, 144, 208]);

    // 同时覆盖报告中的5/10秒分界，以及后台help.proxy更新可能介入的窗口。
    await vmrp.delay(idleMs);
    await vmrp.screen("idle");
    await vmrp.key("UP", { timeoutMs: 2_000, holdMs: 120 });
    await vmrp.screen("after-up");
    await vmrp.delay(500);
    await vmrp.key("ENTER", { timeoutMs: 2_000, holdMs: 120 });
    await vmrp.screen("after-first-enter");
    await vmrp.delay(500);
    // editCreate成功进入宿主编辑器但不绘图；等待draw会把后台/image误当成功信号。
    await vmrp.key("ENTER", { holdMs: 120, waitForDraw: false });
    const beforePaste = await vmrp.screen("before-paste");
    await vmrp.paste("http://mrp.gddhy.net");
    const afterPaste = await vmrp.screen("after-paste");
    expect(afterPaste.diffPixelCount(beforePaste, { x: 0, y: 0, width: 240, height: 32 }))
      .toBeGreaterThan(500);

    // guest在dialog回调后的timer tick中提交输入状态；保持原用例已验证的动作间隔。
    await vmrp.delay(500);
    // 与前面的单步操作使用相同短按，避免默认长按重复提交或切回主页。
    await vmrp.key("ENTER", { timeoutMs: 5_000, holdMs: 120 });
    const loaded = await waitForTargetPage(vmrp, ws, home);
    expect(hasTargetCache(ws)).toBe(true);
    expect(loaded.diffPixelCount(home, { x: 0, y: 0, width: 240, height: 28 }))
      .toBeGreaterThan(500);
    expect(loaded.diffPixelCount(home, { x: 0, y: 30, width: 240, height: 250 }))
      .toBeGreaterThan(3_000);
    expect(loaded.uniqueColorCount()).toBeGreaterThan(20);
  });
});
