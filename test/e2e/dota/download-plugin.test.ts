import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";
import fs from "fs";

describe("dota pixel flow", () => {
  let vmrp: VmrpE2e | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
  });

  it("下载浏览器插件 - 返回重进", async () => {
    // 删除后，继续游戏会进入下载浏览器插件界面。
    fs.rmSync('mythroad/plugins/browser.mrp', { force: true });
    vmrp = await VmrpE2e.start("test/fixtures/dota.mrp");

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
});
