import { afterEach, describe, expect, it, vi } from "vitest";
import { VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";
import fs from "fs";

describe("geyaxz", () => {
  let vmrp: VmrpE2e | undefined;
  let ws: VmrpWorkspace | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it("游戏正常启动", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    fs.rmSync(ws.path('mythroad/geyaxz'), { recursive: true, force: true })
    
    // gtcm 面向 320x480 竖屏真机,启动时经 plat(101,3) 请求横屏,
    // 模拟器窗口自动翻转为 480x320——断言坐标仍是横屏坐标。
    vmrp = await VmrpE2e.start("test/fixtures/geyaxz.mrp", { workDir: ws.dir });

    await vi.waitFor(async () => {
      if (!vmrp) throw new Error('vmrp not defined')
      const screen = await vmrp.screen('bgm-select')
      // rgb(128, 220, 232)
      expect(screen.pixel(112, 125)).toEqual([128, 220, 232])
    }, {
      timeout: 10_000,
      interval: 1_000
    })
    // 点击右键，不开启声音
    await vmrp.key('RIGHT_SOFT', 1_000)
    await vi.waitFor(async () => {
      if (!vmrp) throw new Error('vmrp not defined')
      const screen = await vmrp.screen('bgm-select')
      // rgb(128, 220, 232)
      expect(screen.pixel(112, 125)).not.toEqual([128, 220, 232])
    }, {
      timeout: 10_000,
      interval: 1_000
    })
  });
  
});
