import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";
import fs from "fs";

describe("opglqa 进入主菜单", () => {
  let vmrp: VmrpE2e | undefined;

  const expectMainScreen = async (name: string) => {
    for (let i = 0; i < 8; i++) {
      const screen = await vmrp!.screen(name);
      // rgb(144, 212, 248)
      if (screen.pixel(9, 188).join(",") === "144,212,248") {
        return;
      }
      // Font installation briefly presents a black transition frame after the
      // success dialog closes; wait for the first stable main-screen paint.
      await vmrp!.delay(500);
    }
    const screen = await vmrp!.screen(name);
    expect(screen.pixel(9, 188)).toEqual([144, 212, 248]);
  };

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
  });

  it("程序准备 - 顶部正常", async () => {
    // 删除后，继续游戏会进入下载netpay插件界面。
    if (!fs.existsSync('mythroad/plugins/netpay.mrp')) {
      fs.cpSync('test/fixtures/plugins/netpay.mrp', 'mythroad/plugins/netpay.mrp');
    }
    // 本用例验证前台 advbar 插件与游戏主画面的屏幕合成，运行前显式准备插件资源。
    if (!fs.existsSync('mythroad/plugins/advbar.mrp')) {
      fs.cpSync('test/fixtures/plugins/advbar.mrp', 'mythroad/plugins/advbar.mrp');
    }
    // Start from the first-run font state; this test then verifies the second
    // launch reuses the files created by the first launch.
    fs.rmSync('mythroad/system/gb12v2.uc2', { force: true });
    fs.rmSync('mythroad/system/gb12v2.adl', { force: true });
    fs.rmSync('mythroad/system/___sftmp.mrp', { force: true });
    fs.rmSync('mythroad/vld/77ae410', { force: true, recursive: true });
    vmrp = await VmrpE2e.start("test/fixtures/opglqa_v4450.mrp");
    {
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("download-font");
      // rgb(40, 176, 216)
      expect(screen.pixel(87, 295)).toEqual([40, 176, 216]);
    }
    {
      // 开始下载字体资源，下载完成后显示下载完成结果。
      await vmrp.key('LEFT_SOFT', 1_000)
      await vmrp.delay(6_000);
      const screen = await vmrp.screen("confirm-result");
      // rgb(0, 0, 0)
      expect(screen.pixel(155, 153)).toEqual([0, 0, 0]);
    }
    {
      // 确认下载结果，进入主界面。
      await vmrp.key('LEFT_SOFT', 1_000)
      await expectMainScreen("main-screen1");
    }

    await vmrp.close();
    await vmrp.delay(1_000);
    vmrp = await VmrpE2e.start("test/fixtures/opglqa_v4450.mrp");
    {
      // 直接进入主界面。
      await vmrp.key('LEFT_SOFT', 1_000)
      await expectMainScreen("main-screen2");
    }
  });
});
