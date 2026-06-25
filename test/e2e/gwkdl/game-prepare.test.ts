import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";
import fs from "fs";

describe("gwkdl 进入准备界面", () => {
  let vmrp: VmrpE2e | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
  });

  it("游戏准备 - 界面正常", async () => {
    // 删除后，继续游戏会进入下载netpay插件界面。
    if (!fs.existsSync('mythroad/plugins/netpay.mrp')) {
      fs.cpSync('test/fixtures/plugins/netpay.mrp', 'mythroad/plugins/netpay.mrp');
    }
    // 本用例验证前台 advbar 插件与游戏主画面的屏幕合成，运行前显式准备插件资源。
    if (!fs.existsSync('mythroad/plugins/advbar.mrp')) {
      fs.cpSync('test/fixtures/plugins/advbar.mrp', 'mythroad/plugins/advbar.mrp');
    }
    fs.rmSync('mythroad/cache/exdet.dat', { force: true })
    vmrp = await VmrpE2e.start("test/fixtures/gwkdl_v1003.mrp");

    await vmrp.delay(1000);
    {
      const boot = await vmrp.screen("mem-check");
      // rgb(248, 252, 248)
      expect(boot.pixel(60, 82)).toEqual([248, 252, 248]);
    }
    // 是否检测内存？-> 是
    await vmrp.key('LEFT_SOFT', 1_000);
    await vmrp.delay(7000);
    const boot = await vmrp.screen("bgm-select");
    expect(boot.pixel(150, 308)).toEqual([0, 0, 0]);
    // rgb(248, 240, 0)
    expect(boot.pixel(116, 84)).toEqual([248, 240, 0]);

    // 是否开启音乐？-> 否
    await vmrp.click(227, 301, 1_000);
    await vmrp.delay(3_000);

    // 进入“请按任意键界面”
    const wake = await vmrp.screen("menu");
    // 图片资源缺失时该画面只剩背景色和少量文字色；正常加载后会出现位图的多色像素。
    expect(wake.uniqueColorCount()).toBeGreaterThan(16);
  });
});
