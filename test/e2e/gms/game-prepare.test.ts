import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";
import fs from "fs";

describe("gms 付费慢测试", () => {
  let vmrp: VmrpE2e | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
  });

  it("付费慢测试", async () => {
    // 删除后，继续游戏会进入下载netpay插件界面。
    if (!fs.existsSync('mythroad/plugins/netpay.mrp')) {
      fs.cpSync('test/fixtures/plugins/netpay.mrp', 'mythroad/plugins/netpay.mrp');
    }
    // 本用例验证前台 advbar 插件与游戏主画面的屏幕合成，运行前显式准备插件资源。
    if (!fs.existsSync('mythroad/plugins/advbar.mrp')) {
      fs.cpSync('test/fixtures/plugins/advbar.mrp', 'mythroad/plugins/advbar.mrp');
    }
    vmrp = await VmrpE2e.start("test/fixtures/gms.mrp");

    await vmrp.delay(4_000);
    const boot = await vmrp.screen("bgm-select");
    // rgb(48, 164, 200)
    expect(boot.pixel(143, 143)).toEqual([48, 164, 200]);

    // 是否开启音乐？-> 否
    await vmrp.key('RIGHT_SOFT', 1_000);
    await vmrp.delay(1_000);

    // 进入主菜单
    const wake = await vmrp.screen("menu");
    // rgb(248, 232, 32)
    expect(wake.pixel(68, 119)).toEqual([248, 232, 32]);

    {
      // 确定，开始游戏
      await vmrp.key('LEFT_SOFT', 1_000)
      await vmrp.delay(1_000);
      const afterRight = await vmrp.screen("start");
      // rgb(40, 80, 248)
      expect(afterRight.pixel(113, 293)).toEqual([40, 80, 248]);

      for (let i=0; i< 28; i++) {
        await vmrp.key('ENTER', 1_000)
        await vmrp.delay(1_000);
      }

      // 领取任务
      await vmrp.key('RIGHT_SOFT', 1_000)
      await vmrp.delay(1_000);

      // 打开商店
      await vmrp.key('RIGHT_SOFT', 1_000)
      await vmrp.delay(1_000);
      const afterSecondRight = await vmrp.screen("open-store");
      // rgb(232, 228, 48)
      expect(afterSecondRight.pixel(36, 55)).toEqual([232, 228, 48]);
      {   
        // 进入付费界面
        await vmrp.key('LEFT_SOFT', 1_000)
        await vmrp.delay(1_000);
        await vmrp.key('LEFT_SOFT', 1_000)
        await vmrp.delay(1_000);
        const screen = await vmrp.screen("open-store");
        // rgb(248, 80, 0)
        expect(screen.pixel(61, 119)).toEqual([248, 80, 0]);
      }
      {   
        // 确定付费,成功
        await vmrp.key('LEFT_SOFT', 1_000)
        await vmrp.delay(1_000);
        const screen = await vmrp.screen("pay-end");
        // rgb(232, 64, 8)
        expect(screen.pixel(25, 305)).toEqual([232, 64, 8]);
      }
    }
  });
});
