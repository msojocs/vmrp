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

  it("游戏正常启动1", async () => {
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

  it("游戏正常启动2", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    fs.rmSync(ws.path('mythroad/geyaxz'), { recursive: true, force: true })
    fs.rmSync(ws.path('mythroad/plugins'), { recursive: true, force: true })

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
        const screen = await vmrp.screen('bgm-select')
        // rgb(128, 220, 232)
        expect(screen.pixel(112, 125)).not.toEqual([128, 220, 232])
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
      // 通过固定触点快速完成第一关。
      await vmrp.click(76, 101, 1_000)
      await vmrp.delay(500)
      await vmrp.click(76, 101, 1_000)
      await vmrp.delay(500)
      await vmrp.click(76, 101, 1_000)
      await vmrp.delay(500)
      // 金手指
      await vmrp.click(214, 305, 1_000)
      await vmrp.delay(500)
      await vmrp.click(24, 305, 1_000)
      await vmrp.delay(500)
      // 开始完成第一关
      await vmrp.click(15, 74, 1_000)
      await vmrp.delay(500)
      await vmrp.click(15, 74, 1_000)
      await vmrp.delay(500)
      await vmrp.click(41, 74, 1_000)
      await vmrp.delay(500)
      await vmrp.click(75, 74, 1_000)
      await vmrp.delay(500)
      await vmrp.click(45, 133, 1_000)
      await vmrp.delay(500)
      await vmrp.click(12, 133, 1_000)
      await vmrp.delay(500)
      await vmrp.click(12, 162, 1_000)
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen('game-1-ok')
        // rgb(184, 216, 200)
        expect(screen.pixel(11, 106)).toEqual([184, 216, 200])
      }, {
        timeout: 10_000,
        interval: 1_000
      })
    }
    {
      await vmrp.key('ENTER', 1_000)
      await vmrp.delay(5_000)
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen('download-plugin')
        // 通用下载提示只有五种颜色;正文区域的深绿色可排除高彩色游戏菜单。
        expect(screen.pixel(80, 80)).toEqual([0, 4, 0])
        expect(screen.pixel(112, 125)).toEqual([0, 4, 0])
        expect(screen.uniqueColorCount()).toBe(5)
      }, {
        timeout: 10_000,
        interval: 1_000
      })
    }
  });
});
