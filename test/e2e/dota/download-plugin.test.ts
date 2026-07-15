import { afterEach, describe, expect, it, vi } from "vitest";
import { VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";
import fs from "fs";

describe("dota pixel flow", () => {
  let vmrp: VmrpE2e | undefined;
  let ws: VmrpWorkspace | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it("下载浏览器插件 - 返回重进", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    // 删除后，继续游戏会进入下载浏览器插件界面。
    fs.rmSync(ws.path('mythroad/plugins/embrw.mrp'), { force: true });
    vmrp = await VmrpE2e.start("test/fixtures/dota.mrp", { workDir: ws.dir });

    await vmrp.delay(6000);
    const boot = await vmrp.screen("bgm-select");
    // rgb(216, 24, 96)
    expect(boot.pixel(229, 306)).toEqual([216, 24, 96]);

    // 是否开启音乐？-> 否
    await vmrp.click(228, 308, 1_000);
    await vmrp.delay(2_000);

    // 任意键进入主菜单
    await vmrp.click(50, 50, 1_000);
    await vmrp.delay(1_000);

    // 进入主菜单
    const wake = await vmrp.screen("menu");
    // rgb(144, 20, 40)
    expect(wake.pixel(202, 203)).toEqual([144, 20, 40]);
    // rgb(40, 8, 16)
    expect(wake.pixel(205, 297)).toEqual([40, 8, 16]);
    {
      // 切换菜单到游戏社区
      await vmrp.key('UP', 3_000);
      await vmrp.delay(1_000);
      await vmrp.key('UP', 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("menu");
      // rgb(216, 32, 80)
      expect(screen.pixel(57, 222)).toEqual([216, 32, 80]);
    }

    // 点击确定，进入插件下载界面
    await vmrp.key('LEFT_SOFT', 3_000);
    await vmrp.delay(1_000);
    const menu = await vmrp.screen("download-plugin");
    // rgb(0, 4, 0)
    expect(menu.pixel(80, 80)).toEqual([0, 4, 0]);

    {
      // 点击返回游戏，进入主菜单界面
      await vmrp.key('RIGHT_SOFT', 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("menu-main");
      // rgb(144, 20, 40)
      expect(screen.pixel(202, 203)).toEqual([144, 20, 40]);
      // rgb(40, 8, 16)
      expect(screen.pixel(205, 297)).toEqual([40, 8, 16]);
    }
    {
      // 点击确定，进入插件下载界面
      await vmrp.key('LEFT_SOFT', 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("download-plugin");
      // rgb(0, 4, 0)
      expect(screen.pixel(80, 80)).toEqual([0, 4, 0]);

    }
    {
      // 点击返回游戏，进入主菜单界面
      await vmrp.key('RIGHT_SOFT', 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("menu-main");
      // rgb(144, 20, 40)
      expect(screen.pixel(202, 203)).toEqual([144, 20, 40]);
      // rgb(40, 8, 16)
      expect(screen.pixel(205, 297)).toEqual([40, 8, 16]);
    }
    {
      // 点击确定，进入插件下载界面
      await vmrp.key('LEFT_SOFT', 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("download-plugin");
      // rgb(0, 4, 0)
      expect(screen.pixel(80, 80)).toEqual([0, 4, 0]);

    }
    {
      // 点击返回游戏，进入主菜单界面
      await vmrp.key('RIGHT_SOFT', 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("menu-main");
      // rgb(144, 20, 40)
      expect(screen.pixel(202, 203)).toEqual([144, 20, 40]);
      // rgb(40, 8, 16)
      expect(screen.pixel(205, 297)).toEqual([40, 8, 16]);
    }
    {
      // 点击确定，进入插件下载界面
      await vmrp.key('LEFT_SOFT', 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("download-plugin");
      // rgb(0, 4, 0)
      expect(screen.pixel(80, 80)).toEqual([0, 4, 0]);

    }
  });

  it("下载浏览器插件 - 成功", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    // 删除后，继续游戏会进入下载浏览器插件界面。
    fs.rmSync(ws.path('mythroad/brw'), { force: true, recursive: true });
    fs.rmSync(ws.path('mythroad/plugins/embrw.mrp'), { force: true });
    fs.rmSync(ws.path('mythroad/plugins/brwcore.mrp'), { force: true });
    fs.rmSync(ws.path('mythroad/plugins/brwgui.mrp'), { force: true });
    fs.rmSync(ws.path('mythroad/plugins/brwmain.mrp'), { force: true });
    fs.rmSync(ws.path('mythroad/plugins/brwshell.mrp'), { force: true });
    vmrp = await VmrpE2e.start("test/fixtures/dota.mrp", { workDir: ws.dir });

    await vmrp.delay(6000);
    const boot = await vmrp.screen("bgm-select");
    // rgb(216, 24, 96)
    expect(boot.pixel(229, 306)).toEqual([216, 24, 96]);

    // 是否开启音乐？-> 否
    await vmrp.click(228, 308, 1_000);
    await vmrp.delay(2_000);

    // 任意键进入主菜单
    await vmrp.click(50, 50, 1_000);
    await vmrp.delay(1_000);

    // 进入主菜单
    const wake = await vmrp.screen("menu");
    // rgb(144, 20, 40)
    expect(wake.pixel(202, 203)).toEqual([144, 20, 40]);
    // rgb(40, 8, 16)
    expect(wake.pixel(205, 297)).toEqual([40, 8, 16]);
    {
      // 切换菜单到游戏社区
      await vmrp.key('UP', 3_000);
      await vmrp.delay(1_000);
      await vmrp.key('UP', 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("menu");
      // rgb(216, 32, 80)
      expect(screen.pixel(57, 222)).toEqual([216, 32, 80]);
    }

    // 点击确定，进入插件下载界面
    await vmrp.key('LEFT_SOFT', 3_000);
    await vmrp.delay(1_000);
    const menu = await vmrp.screen("download-plugin");
    // rgb(0, 4, 0)
    expect(menu.pixel(80, 80)).toEqual([0, 4, 0]);

    {
      // 点击确定，下载浏览器插件，进入下载结果界面
      await vmrp.key('LEFT_SOFT', 1_000);
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error('vmrp not defined')
        const screen = await vmrp.screen("download-result");
        // rgb(0, 252, 0)
        expect(screen.pixel(152, 146)).toEqual([0, 252, 0]);
      }, {
        timeout: 20_000,
        interval: 1_000
      })
    }
    {
      // 点击确定，确认下载结果，开始加载浏览器
      await vmrp.key('LEFT_SOFT', 3_000);
      await vmrp.delay(1_000);
      // 确定使用cmnet
      await vmrp.key('LEFT_SOFT', 3_000);
      await vmrp.delay(1_000);
      // 已确认进入目标界面，不可使用循环检测，循环等待会导致退出该界面
      let screen = await vmrp.screen("download-browser-components");
      expect(screen.uniqueColorCount()).toBeGreaterThan(100);
      expect(screen.pixel(95, 88)).not.toEqual([0, 0, 0]);
      expect(screen.pixel(120, 190)).not.toEqual([0, 0, 0]);
    }
    {
      const before = await vmrp.drawCount();
      await vmrp.delay(20_000);
      const after = await vmrp.drawCount();
      const screen = await vmrp.screen("browser-running");
      expect(after).toBeGreaterThan(before);
      expect(screen.uniqueColorCount()).toBeGreaterThan(4);
      // rgb(27, 34, 27)
      expect(screen.pixel(95, 88)).toEqual([240, 244, 240]);
    }
  });
});
