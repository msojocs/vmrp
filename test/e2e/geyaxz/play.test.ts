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

  it("游玩卡死测试", async () => {
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
    {
      // 点击右键，不开启声音
      await vmrp.key('RIGHT_SOFT', 1_000)
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen('home')
        // rgb(128, 220, 232)
        expect(screen.pixel(112, 125)).not.toEqual([128, 220, 232])
        // rgb(240, 204, 72)
        expect(screen.pixel(101, 148)).toEqual([240, 204, 72])
      }, {
        timeout: 10_000,
        interval: 1_000
      })
    }
    {
      // 开始游戏，进入选择关卡界面
      await vmrp.key('ENTER', 1_000)
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen('select')
        // rgb(104, 148, 144)
        expect(screen.pixel(99, 92)).toEqual([104, 148, 144])
      }, {
        timeout: 10_000,
        interval: 1_000
      })
    }
    {
      // 回车选择第一个关卡
      await vmrp.key('ENTER', 1_000)
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen('game')
        // rgb(96, 84, 144)
        expect(screen.pixel(32, 229)).toEqual([96, 84, 144])
      }, {
        timeout: 10_000,
        interval: 1_000
      })
    }
    {
      // 复现卡死
      await vmrp.key('RIGHT', 1_000)
      await vmrp.delay(500)
      await vmrp.key('RIGHT', 1_000)
      await vmrp.delay(500)
      await vmrp.key('DOWN', 1_000)
      await vmrp.delay(500)
      await vmrp.key('DOWN', 1_000)
      await vmrp.delay(500)
      await vmrp.key('ENTER', 1_000)
      await vmrp.delay(500)
      await vmrp.key('ENTER', 1_000)
      await vmrp.delay(500)
      await vmrp.key('RIGHT_SOFT', 1_000)
      await vmrp.delay(500)
      await vmrp.key('LEFT_SOFT', 1_000)
      await vmrp.delay(500)
      for (let i=0; i<99; i++) {
        await vmrp.key('ENTER', 1_000)
        await vmrp.delay(500)
      }
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen('failed-menu')
        // rgb(192, 216, 208)
        expect(screen.pixel(69, 239)).toEqual([192, 216, 208])
      }, {
        timeout: 30_000,
        interval: 1_000
      })
      await vmrp.key('DOWN', 1_000)
      await vmrp.delay(500)
      await vmrp.key('ENTER', 1_000)
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen('retry')
        // rgb(192, 216, 208)
        expect(screen.pixel(69, 239)).not.toEqual([192, 216, 208])
        // rgb(96, 84, 144)
        expect(screen.pixel(32, 229)).not.toEqual([96, 84, 144])
      }, {
        timeout: 30_000,
        interval: 1_000
      })
    }
  });
  
});
