import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";
import fs from "fs";

describe("opbzqe 进入主菜单", () => {
  let vmrp: VmrpE2e | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
  });

  it("游戏准备 - 顶部正常", async () => {
    // 删除后，继续游戏会进入下载netpay插件界面。
    if (!fs.existsSync('mythroad/plugins/netpay.mrp')) {
      fs.cpSync('test/fixtures/plugins/netpay.mrp', 'mythroad/plugins/netpay.mrp');
    }
    // 本用例验证前台 advbar 插件与游戏主画面的屏幕合成，运行前显式准备插件资源。
    if (!fs.existsSync('mythroad/plugins/advbar.mrp')) {
      fs.cpSync('test/fixtures/plugins/advbar.mrp', 'mythroad/plugins/advbar.mrp');
    }
    vmrp = await VmrpE2e.start("test/fixtures/optwar.mrp");

    await vmrp.delay(2000);
    const boot = await vmrp.screen("bgm-select");
    expect(boot.pixel(150, 308)).toEqual([0, 0, 0]);
    // rgb(248, 0, 0)
    expect(boot.pixel(227, 301)).toEqual([248, 0, 0]);

    // 是否开启音乐？-> 否
    await vmrp.click(227, 301, 1_000);
    await vmrp.delay(1_000);

    // 进入主菜单
    const wake = await vmrp.screen("menu");
    // rgb(128, 48, 40)
    expect(wake.pixel(110, 27)).toEqual([128, 48, 40]);
    expect(wake.pixel(120, 20)).toEqual([176, 120, 120]);
    // rgb(24, 24, 24)
    expect(wake.pixel(83, 267)).toEqual([24, 24, 24]);
    // rgb(0, 252, 0)
    expect(wake.pixel(98, 264)).toEqual([0, 252, 0]);

    {
      // 第一次方向键先被前台 advbar 关闭流程消费，只应让顶部广告条消失。
      await vmrp.key('RIGHT', 1_000)
      await vmrp.delay(1_000);
      const afterRight = await vmrp.screen("after-right");
      expect(afterRight.pixel(110, 27)).not.toEqual([128, 48, 40]);
      // rgb(0, 252, 0)
      expect(afterRight.pixel(98, 264)).toEqual([0, 252, 0]);

      // 下一次方向键才进入游戏菜单状态机，避免同一个按键被原始事件和 Lua 转发重复处理。
      await vmrp.key('RIGHT', 1_000)
      await vmrp.delay(1_000);
      const afterSecondRight = await vmrp.screen("after-second-right");
      expect(afterSecondRight.pixel(98, 264)).not.toEqual(afterRight.pixel(98, 264));
    }
  });
});
