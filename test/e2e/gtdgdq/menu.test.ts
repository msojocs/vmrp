import { afterEach, describe, expect, it, vi } from "vitest";
import { VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";
import fs from "fs";

describe("gtdgdq", () => {
  let vmrp: VmrpE2e | undefined;
  let ws: VmrpWorkspace | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it("帮助-关于", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    fs.rmSync(ws.path('mythroad/gtdgdq'), { recursive: true, force: true })
    
    // gtdgdq 使用默认 240x320 画布；断言坐标对应应用的原生竖屏布局。
    vmrp = await VmrpE2e.start("test/fixtures/gtdgdq.mrp", { workDir: ws.dir });

    {
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen("bgm-select");
        // rgb(0, 200, 248)
        expect(screen.pixel(219, 312)).toEqual([0, 200, 248]);
      })
      // 不开启音乐
      await vmrp.key('RIGHT_SOFT', 1_000);
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen("menu");
        // rgb(248, 248, 240)
        expect(screen.pixel(168, 162)).toEqual([248, 248, 240]);
      })
    }
    {
      await vmrp.key('DOWN', 1_000);
      const menu = await vmrp.screen("menu");
      await vmrp.delay(500)
      await vmrp.key('LEFT_SOFT', 1_000);
      // 平台文本页是黑底绿字；同时确认标题、正文和返回栏均已绘制，
      // 避免仅凭任意一处 draw/diff 将空白本地窗口误判为帮助页。
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen("help");
        expect(screen.diffPixelCount(menu)).toBeGreaterThan(0);
        expect(screen.uniqueColorCount()).toBe(2);
        expect(screen.pixel(9, 8)).toEqual([0, 252, 0]);
        expect(screen.pixel(9, 34)).toEqual([0, 252, 0]);
        expect(screen.pixel(0, 294)).toEqual([0, 252, 0]);
        expect(screen.pixel(212, 300)).toEqual([0, 252, 0]);
        expect(screen.pixel(120, 160)).toEqual([0, 0, 0]);
      })

      // MR_DIALOG_CANCEL 页面由右软键关闭；释放后应露出未被本地 UI
      // 覆盖的同一 guest 菜单帧。
      await vmrp.key('RIGHT_SOFT', 1_000);
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const restored = await vmrp.screen("menu-restored");
        expect(restored.diffPixelCount(menu)).toBe(0);
      })
    }
  });
  
});
