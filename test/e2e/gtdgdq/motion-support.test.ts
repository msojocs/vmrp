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

  it("传感器检测", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    fs.rmSync(ws.path('mythroad/gtdgdq'), { recursive: true, force: true })
    
    // gtcm 面向 320x480 竖屏真机,启动时经 plat(101,3) 请求横屏,
    // 模拟器窗口自动翻转为 480x320——断言坐标仍是横屏坐标。
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
    }
    {
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen("menu");
        // rgb(248, 248, 240)
        expect(screen.pixel(168, 162)).toEqual([248, 248, 240]);
      })
      await vmrp.key('LEFT_SOFT', 1_000);
    }
    {
      // 支持动感功能，只有两种颜色提示（黑色背景+绿色文字）
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen("menu");
        // rgb(248, 248, 240)
        expect(screen.uniqueColorCount()).toEqual(2);
      })
    }
  });
  
});
