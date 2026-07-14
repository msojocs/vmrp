import { afterEach, describe, expect, it, vi } from "vitest";
import { VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";
import fs from "fs";

describe("opbzqe 进入主菜单", () => {
  let vmrp: VmrpE2e | undefined;
  let ws: VmrpWorkspace | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it("游戏准备 - 顶部正常", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    // 删除后，继续游戏会进入下载netpay插件界面。
    if (!fs.existsSync(ws.path('mythroad/plugins/netpay.mrp'))) {
      fs.cpSync('test/fixtures/plugins/netpay.mrp', ws.path('mythroad/plugins/netpay.mrp'));
    }
    // 本用例验证前台 advbar 插件与游戏主画面的屏幕合成，运行前显式准备插件资源。
    if (!fs.existsSync(ws.path('mythroad/plugins/advbar.mrp'))) {
      fs.cpSync('test/fixtures/plugins/advbar.mrp', ws.path('mythroad/plugins/advbar.mrp'));
    }
    vmrp = await VmrpE2e.start("test/fixtures/opbzqe.mrp", { workDir: ws.dir });

    await vmrp.delay(7000);
    const boot = await vmrp.screen("bgm-select");
    expect(boot.pixel(227, 308)).toEqual([0, 0, 0]);
    // rgb(248, 252, 248)
    expect(boot.pixel(126, 165)).toEqual([248, 252, 248]);

    // 是否开启音乐？-> 否
    await vmrp.click(214, 293, 1_000);
    await vmrp.delay(1_000);

    // 进入主菜单
    const wake = await vmrp.screen("menu");
    // rgb(128, 48, 40)
    expect(wake.pixel(110, 27)).toEqual([128, 48, 40]);
    expect(wake.pixel(120, 20)).toEqual([176, 120, 120]);
    expect(wake.pixel(113, 124)).toEqual([184, 192, 48]);

    {
      // 按一下右方向键，广告条消失
      await vmrp.key('RIGHT', 1_000)
      await vmrp.delay(1_000);
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error("vmrp is undefined");
        const afterRight = await vmrp.screen("after-right");
        // 消失检查
        expect(afterRight.pixel(110, 27)).not.toEqual([128, 48, 40]);
        expect(afterRight.pixel(10, 41)).not.toEqual(wake.pixel(10, 41));
      }, {
        timeout: 5_000,
        interval: 1_000
      })
    }
  });
});
