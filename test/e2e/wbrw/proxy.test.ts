import { afterEach, describe, expect, it } from "vitest";
import { type PpmImage, VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";
import fs from "fs";
import { readFile } from "fs/promises";

const targetUrl = Buffer.from("http://mrp.gddhy.net");
// The requested trailing-slash URL canonicalizes to this link path on the site.
const targetDetailPath = "/mrp/sky_SaiNes-v1010";
const targetDownloadUrl = "http://mrp.gddhy.net/mrp-files/sky_SaiNes-v1010.mrp";
const targetDownloadFilename = "sky_SaiNes-v1010.mrp";
// WAP网关代理简化后的页面包含原始标题
const targetTitle = Buffer.from("WAP下载站");
// 首页自身的URL/标题;子链接页面 = 同站任意其他URL、标题同样以站点后缀结尾
const homeUrl = "http://mrp.gddhy.net/";
const homeTitle = "第1页 - WAP下载站";
const siteTitleSuffix = " - WAP下载站";

function readCachedPages(ws: VmrpWorkspace): Buffer[] {
  const cacheDirs = [
    ws.path('mythroad/brw/http/cache3'),
    ws.path('mythroad/brw/http/percache')
  ];
  return cacheDirs.flatMap(cacheDir => {
    if (!fs.existsSync(cacheDir)) return [];
    return fs.readdirSync(cacheDir)
      .filter(name => name.endsWith('.dat') && name !== 'index.dat')
      .map(name => fs.readFileSync(`${cacheDir}/${name}`));
  });
}

function hasTargetCache(ws: VmrpWorkspace): boolean {
  return readCachedPages(ws)
    .some(page => page.includes(targetUrl) && page.includes(targetTitle));
}

async function stdoutHasPage2(vmrp: VmrpE2e): Promise<boolean> {
  const stdout = await readFile(vmrp.stdoutPath, "utf8").catch(() => "");
  return stdout.includes("POST /page2");
}

async function waitForProxyRequest(vmrp: VmrpE2e, ws: VmrpWorkspace, timeoutMs: number): Promise<boolean> {
  const deadline = Date.now() + timeoutMs;
  while (Date.now() < deadline) {
    if (hasTargetCache(ws) || await stdoutHasPage2(vmrp)) return true;
    await vmrp.delay(500);
  }
  return false;
}

async function submitUrl(vmrp: VmrpE2e, ws: VmrpWorkspace): Promise<void> {
  for (let attempt = 0; attempt < 4; attempt++) {
    // The URL editor can still be settling after clipboard paste on saturated
    // E2E runs.  Gate retries on the real proxy request/cache, not on redraws
    // from the home page's image refreshes.
    await vmrp.delay(1_000);
    try {
      await vmrp.key("ENTER", 5_000);
    } catch (error) {
      if (!String(error).includes("wait_draw_timeout")) throw error;
    }
    if (await waitForProxyRequest(vmrp, ws, 10_000)) return;
  }
  throw new Error("WBRW did not submit the target URL to proxy2 /page2");
}

// 解析缓存页字符串池头两个条目(docs/wbrw-sky-format.md §2):
// 10字节头 + [u16be len]URL(ASCII) + [u16be len]标题(UTF-8)
function parseCachedPageHead(page: Buffer): { url: string; title: string } | null {
  if (page.length < 14) return null;
  const urlLen = page.readUInt16BE(10);
  const titleOff = 12 + urlLen;
  if (titleOff + 2 > page.length) return null;
  const titleLen = page.readUInt16BE(titleOff);
  if (titleOff + 2 + titleLen > page.length) return null;
  return {
    url: page.subarray(12, 12 + urlLen).toString("latin1"),
    title: page.subarray(titleOff + 2, titleOff + 2 + titleLen).toString("utf8"),
  };
}

// 从缓存的 .sky 页面解析第一个链接的 href(docs/wbrw-sky-format.md §4b-3):
// 真机320个0x38锚点都以pool[captionIndex-1]保存目标URL;后置0x54是
// companion/suffix,不是当前锚点的href。页头标题是0x54,故首个0x38即首链接。
function parseLinkHrefs(page: Buffer): string[] {
  const poolEnd = page.readUInt16BE(8) + 10;
  const pool: Buffer[] = [];
  for (let off = 10; off + 2 <= poolEnd;) {
    const len = page.readUInt16BE(off);
    pool.push(page.subarray(off + 2, off + 2 + len));
    off += 2 + len;
  }
  // 跳过13字节DL信封和7字节0F状态节,记录流从poolEnd+20开始。
  const hrefs: string[] = [];
  let off = poolEnd + 20;
  while (off + 6 <= page.length) {
    const flag = page.readUInt16BE(off + 2);
    let p = off + 4;
    if (flag & 0x8000) p += 2;
    if (flag & 0x4000) p += 2;
    const op = page[p];
    const len = page[p + 1];
    if (op === 0x38 && len >= 2) {
      const captionIndex = page.readUInt16BE(p + 2);
      if (captionIndex > 0) {
        const href = pool[captionIndex - 1]?.toString("latin1");
        if (href) hrefs.push(href);
      }
    }
    off = p + 2 + len;
  }
  return hrefs;
}

function parseFirstLinkHref(page: Buffer): string | null {
  return parseLinkHrefs(page)[0] ?? null;
}

function readHomeCache(ws: VmrpWorkspace): Buffer | undefined {
  return readCachedPages(ws).find(page => parseCachedPageHead(page)?.url === homeUrl);
}

// 子链接页面判定: 打开的必须正是首页第一个链接指向的 URL,标题为该站点风格。
function hasSubLinkCache(ws: VmrpWorkspace, expectedUrl: string): boolean {
  return readCachedPages(ws).some(page => {
    const head = parseCachedPageHead(page);
    return head != null
      && head.url === expectedUrl
      && head.title !== homeTitle
      && head.title.endsWith(siteTitleSuffix);
  });
}

// 等待子链接页面进入前台: 缓存出现子页面数据,且标题栏与正文区域都相对首页变化。
// 只看标题栏会被加载过程的"数据请求..."进度标题误判为完成;正文(y=30..280)在
// 渲染完成前仍显示旧页面,必须一并要求变化才代表子页面真正渲染。
async function waitForSubLinkPage(vmrp: VmrpE2e, ws: VmrpWorkspace, loaded: PpmImage, expectedUrl: string): Promise<PpmImage> {
  const deadline = Date.now() + 60_000;
  let last = await vmrp.screen("sublink");
  while (Date.now() < deadline) {
    if (hasSubLinkCache(ws, expectedUrl)
      && last.diffPixelCount(loaded, { x: 0, y: 0, width: 240, height: 24 }) > 500
      && last.diffPixelCount(loaded, { x: 0, y: 30, width: 240, height: 250 }) > 3_000) return last;
    await vmrp.delay(1_000);
    last = await vmrp.screen("sublink");
  }
  return last;
}

function isTargetPageForeground(screen: PpmImage, home: PpmImage): boolean {
  // 加载的页面标题栏为黄色[248,240,168],标题"第1页 - WAP下载站"文本为灰色[96,100,96];
  // (79,10)落在"-"前后汉字的稳定笔画上,(41,10)是字形间隙的黄色背景
  return screen.diffPixelCount(home, { x: 0, y: 0, width: 240, height: 90 }) > 4_500
    && screen.pixel(41, 10).join(",") === "248,240,168"
    && screen.pixel(79, 10).join(",") === "96,100,96";
}

function countColor(screen: PpmImage, color: readonly [number, number, number]): number {
  let count = 0;
  for (let y = 0; y < screen.height; y++) {
    for (let x = 0; x < screen.width; x++) {
      if (screen.pixel(x, y).join(",") === color.join(",")) count++;
    }
  }
  return count;
}

function listFiles(root: string): string[] {
  if (!fs.existsSync(root)) return [];
  return fs.readdirSync(root, { withFileTypes: true }).flatMap(entry => {
    const entryPath = `${root}/${entry.name}`;
    return entry.isDirectory() ? listFiles(entryPath) : [entryPath];
  });
}

function readCompleteMrp(path: string): Buffer | null {
  const body = fs.readFileSync(path);
  if (body.length < 12 || body.subarray(0, 4).toString("ascii") !== "MRPG") return null;
  // MRP头offset 8保存完整文件长度；只认实际长度完全匹配的产物，避免把
  // 下载器曾经误报成功的MRPG前缀/临时分段当成完成文件。
  return body.readUInt32LE(8) === body.length ? body : null;
}

function findNewCompleteMrp(ws: VmrpWorkspace, existingFiles: ReadonlySet<string>): string | null {
  const candidate = ws.path(`mythroad/${targetDownloadFilename}`);
  // This regression must publish the requested basename, not merely any new
  // structurally valid MRP that another background task happened to create.
  return !existingFiles.has(candidate) && fs.existsSync(candidate) && readCompleteMrp(candidate) != null
    ? candidate
    : null;
}

async function waitForTargetPage(vmrp: VmrpE2e, ws: VmrpWorkspace, home: PpmImage): Promise<PpmImage> {
  const deadline = Date.now() + 60_000;
  let last = await vmrp.screen("loaded");
  while (Date.now() < deadline) {
    if (hasTargetCache(ws) && isTargetPageForeground(last, home)) return last;
    await vmrp.delay(1_000);
    last = await vmrp.screen("loaded");
  }
  return last;
}

describe("wbrw ", () => {
  let vmrp: VmrpE2e | undefined;
  let ws: VmrpWorkspace | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it("输入文字", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    fs.rmSync(ws.path('mythroad/brw'), { recursive: true, force: true })

    const previousHoldMs = process.env.VMRP_E2E_HOLD_MS;
    process.env.VMRP_E2E_HOLD_MS = String(Math.max(Number(previousHoldMs) || 0, 1500));
    try {
      vmrp = await VmrpE2e.start("test/fixtures/wbrw.mrp", { workDir: ws.dir });
    } finally {
      if (previousHoldMs == null) delete process.env.VMRP_E2E_HOLD_MS;
      else process.env.VMRP_E2E_HOLD_MS = previousHoldMs;
    }
    let home: PpmImage;
    {
      await vmrp.delay(5000);
      home = await vmrp.screen("home");
      // rgb(248, 252, 248)
      expect(home.pixel(76, 252)).toEqual([248, 252, 248]);
      // rgb(64, 144, 208)
      expect(home.pixel(57, 306)).toEqual([64, 144, 208]);
    }
    {
      // 移动光标到输入框
      await vmrp.key('UP', 1_000);
      // 进入输入框界面
      await vmrp.key('ENTER', 1_000);
      // 打开输入界面
      await vmrp.key('ENTER', 1_000);
      const beforePaste = await vmrp.screen("before-paste");
      await vmrp.paste("http://mrp.gddhy.net");
      const afterPaste = await vmrp.screen("after-paste");
      const stderr = await readFile(vmrp.stderrPath, "utf8");

      expect(stderr).toContain("editGetText(): 'http://mrp.gddhy.net'");
      expect(afterPaste.diffPixelCount(beforePaste, { x: 0, y: 0, width: 240, height: 32 })).toBeGreaterThan(500);
      expect(afterPaste.pixel(107, 17)).toEqual([0, 0, 0]);
      // rgb(248, 252, 248)
      expect(afterPaste.pixel(108, 17)).toEqual([248, 252, 248]);
      // 开始访问
      await submitUrl(vmrp, ws);
      const loaded = await waitForTargetPage(vmrp, ws, home);
      const cachedPages = readCachedPages(ws);

      // 代理生成的普通网页需要进入前台,不能只落盘后停留在内置首页。
      expect(loaded.diffPixelCount(home, { x: 0, y: 0, width: 240, height: 90 })).toBeGreaterThan(1_500);
      expect(loaded.diffPixelCount(home)).toBeGreaterThan(5_000);
      expect(isTargetPageForeground(loaded, home)).toBe(true);
      expect(cachedPages.some(page => page.includes(targetUrl) && page.includes(targetTitle))).toBe(true);

      {
        // 点击页面第一个子链接。交互语义(用户确认): 回车=确定,短按直接打开焦点链接;
        // 数字键 5 才是打开右键/上下文菜单;长按回车(≥1500ms)也会弹出链接菜单。
        // DOWN 用短按 120ms 单步移动(全局 VMRP_E2E_HOLD_MS=1500 会触发按键重复连滚)。
        // 焦点移动是几何搜索"最近的可聚焦框"(game.ext 0x366c),可聚焦判据只看
        // op==0x38(0x275B4)。页头标题已改为不可聚焦的 0x54 纯文本(tool/proxy.js),
        // 因此 DOWN×1 焦点即落在第一个链接上,短按回车直接打开它。
        // 子页面应经代理抓取并渲染,不能卡在“数据请求”。
        const homeCache = readHomeCache(ws);
        expect(homeCache).toBeDefined();
        const firstHref = parseFirstLinkHref(homeCache!);
        expect(firstHref).toMatch(new RegExp(`^${targetUrl.toString()}`));
        expect(firstHref).not.toBe(homeUrl);
        expect(new URL(firstHref!).pathname).toBe(targetDetailPath);
        await vmrp.key("DOWN", 2_000, 120);
        await vmrp.screen("focus");
        try {
          // 短按回车直接导航(120ms 足够被应用定时器观测;更长会接近长按菜单阈值)
          await vmrp.key("ENTER", 5_000, 120);
        } catch (error) {
          if (!String(error).includes("wait_draw_timeout")) throw error;
        }
        const sub = await waitForSubLinkPage(vmrp, ws, loaded, firstHref!);
        expect(hasSubLinkCache(ws, firstHref!)).toBe(true);
        // 子页面标题栏(如“mynes - WAP下载站”)与首页标题不同,顶部区域必须变化;
        // 正文区域也必须变化(排除只有进度标题"数据请求..."在变的卡住情形)
        expect(sub.diffPixelCount(loaded, { x: 0, y: 0, width: 240, height: 24 })).toBeGreaterThan(500);
        expect(sub.diffPixelCount(loaded, { x: 0, y: 30, width: 240, height: 250 })).toBeGreaterThan(3_000);

        const subCache = readCachedPages(ws).find(page => parseCachedPageHead(page)?.url === firstHref);
        expect(subCache).toBeDefined();
        // The generated SKY page must retain the exact native-download
        // arguments even though the visible anchor href becomes skyscript:.
        expect(subCache!.includes(Buffer.from(targetDownloadUrl, "latin1"))).toBe(true);
        expect(subCache!.includes(Buffer.from(targetDownloadFilename, "utf8"))).toBe(true);
        expect(subCache!.includes(Buffer.from("application/sky-mrp", "latin1"))).toBe(true);
        const subLinks = parseLinkHrefs(subCache!);
        const downloadLinkIndex = subLinks.findIndex(href => href.startsWith("skyscript:download"));
        expect(downloadLinkIndex).toBeGreaterThanOrEqual(0);

        // 链接可能位于首屏下方；DOWN 会先滚动正文，再给首个可见锚点加焦点。
        // 以浏览器实际焦点背景色作为门槛，避免把固定滚动次数当成链接序号。
        let focused: PpmImage | undefined;
        for (let attempt = 0; attempt < 12; attempt++) {
          await vmrp.key("DOWN", 2_000, 120);
          focused = await vmrp.screen(`download-focus-seek-${attempt}`);
          if (countColor(focused, [208, 232, 240]) > 100) break;
        }
        expect(focused).toBeDefined();
        expect(countColor(focused!, [208, 232, 240])).toBeGreaterThan(100);

        // 首个焦点对应 subLinks[0]。目标锚点在当前视口下方时，第一次 DOWN
        // 只滚动到下一行，第二次才切换焦点，因此包含目标序号这一拍。
        for (let index = 0; index <= downloadLinkIndex; index++) {
          await vmrp.key("DOWN", 2_000, 120);
        }
        const downloadFocus = await vmrp.screen("download-focus");
        expect(countColor(downloadFocus, [208, 232, 240])).toBeGreaterThan(100);
        const existingFiles = new Set(listFiles(ws.path("mythroad")));
        try {
          await vmrp.key("ENTER", 5_000, 120);
        } catch (error) {
          if (!String(error).includes("wait_draw_timeout")) throw error;
        }

        // 下载提示和进度页是定时器驱动的瞬态 UI；保留连续PPM，并等待真正
        // 完整的MRP落盘。仅出现“下载成功”画面不足以证明数据没有被截断。
        let downloadScreen = await vmrp.screen("download-activated-0");
        let downloadedMrp: string | null = null;
        const downloadDeadline = Date.now() + 90_000;
        for (let frame = 1; Date.now() < downloadDeadline; frame++) {
          await vmrp.delay(2_000);
          downloadScreen = await vmrp.screen(`download-progress-${frame}`);
          downloadedMrp = findNewCompleteMrp(ws, existingFiles);
          if (downloadedMrp) break;
        }
        expect(downloadedMrp).not.toBeNull();
        expect(downloadedMrp!.endsWith(`/${targetDownloadFilename}`)).toBe(true);
        expect(readCompleteMrp(downloadedMrp!)?.subarray(0, 4).toString("ascii")).toBe("MRPG");
        expect(fs.existsSync(ws.path("mythroad/brw/download/dllist1"))).toBe(true);
        expect(fs.statSync(ws.path("mythroad/brw/download/dwnlist.dat")).size).toBe(4);
        await vmrp.screen("download-complete");
        expect(downloadScreen.diffPixelCount(sub)).toBeGreaterThan(1_000);
      }
    }
  });
});
