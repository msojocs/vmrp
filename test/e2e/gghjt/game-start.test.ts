import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";
import fs from "fs";

describe("gghjt 开始游戏", () => {
  let vmrp: VmrpE2e | undefined;
  const memCheckTime = 15_000

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
  });

  it("游戏正式开始 - 不花屏", async () => {
    // 删除后，继续游戏会进入下载netpay插件界面。
    if (!fs.existsSync('mythroad/plugins/netpay.mrp')) {
      fs.cpSync('test/fixtures/plugins/netpay.mrp', 'mythroad/plugins/netpay.mrp');
    }
    fs.rmSync('mythroad/gghjt', { force: true, recursive: true });
    fs.rmSync('mythroad/cache', { force: true, recursive: true });
    fs.cpSync('test/fixtures/gghjt', 'mythroad/gghjt', { recursive: true });
    vmrp = await VmrpE2e.start("test/fixtures/gghjt.mrp");

    {
      // 检测内存
      await vmrp.delay(1_000);
      await vmrp.key('LEFT_SOFT', 1_000);
      await vmrp.delay(1_000);

    }
    await vmrp.delay(memCheckTime);
    const boot = await vmrp.screen("bgm-select");
    // rgb(72,88,0)
    expect(boot.pixel(227, 308)).toEqual([0, 0, 0]);

    // 是否开启音乐？-> 否
    await vmrp.click(230, 308, 1_000);
    await vmrp.delay(1_000);

    // 跳过启动剧情
    await vmrp.click(227, 308, 1_000);
    await vmrp.delay(1_000);
    await vmrp.click(227, 308, 1_000);
    await vmrp.delay(1_000);
    await vmrp.click(227, 308, 1_000);
    await vmrp.delay(1_000);
    await vmrp.click(227, 308, 1_000);
    await vmrp.delay(1_000);

    // 进入主菜单
    const wake = await vmrp.screen("menu");
    // rgb(152, 112, 32)
    expect(wake.pixel(110, 27)).toEqual([152, 112, 32]);
    {
      // 切换菜单
      await vmrp.click(162, 291, 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("menu-continue");
      // rgb(232, 196, 104)
      expect(screen.pixel(162, 291)).toEqual([232, 196, 104 ]);
    }
    {
      // 点击继续游戏，进入付费界面
      await vmrp.click(116, 291, 3_000);
      await vmrp.delay(1_000);
      const pay = await vmrp.screen("pay-start");
      // rgb(104, 104, 224)
      expect(pay.pixel(104, 147)).toEqual([104, 104, 224]);
    }

    {
      // 点击确定付费，进入游戏界面
      await vmrp.key('LEFT_SOFT', 3_000);
      await vmrp.delay(4_000);
      const screen = await vmrp.screen("game-start");
      // rgb(0, 0, 0)
      expect(screen.pixel(88, 44)).toEqual([0, 0, 0]);
    }

    {
      // 两下回车，对话确认后，渲染场景
      await vmrp.key('ENTER', 3_000);
      await vmrp.delay(1_000);
      await vmrp.key('ENTER', 3_000);
      await vmrp.delay(8_000);
      const screen = await vmrp.screen("scene-render");
      // rgb(192, 148, 96)
      expect(screen.pixel(55, 302)).toEqual([192, 148, 96]);
      // 楼房背景由离屏 cache 合成后复制到主屏；这些点曾是深色条纹坏色。
      expect(screen.pixel(92, 95)).toEqual([72, 60, 72]);
      expect(screen.pixel(108, 95)).toEqual([128, 128, 112]);
      expect(screen.pixel(124, 95)).toEqual([128, 128, 112]);
      expect(screen.pixel(172, 95)).toEqual([208, 200, 136]);
    }

  });
  it("游戏直接开始 - 不缺渲染", async () => {
    // 删除后，继续游戏会进入下载netpay插件界面。
    if (!fs.existsSync('mythroad/plugins/netpay.mrp')) {
      fs.cpSync('test/fixtures/plugins/netpay.mrp', 'mythroad/plugins/netpay.mrp');
    }
    fs.rmSync('mythroad/cache', { force: true, recursive: true });
    fs.rmSync('mythroad/gghjt', { force: true, recursive: true });
    vmrp = await VmrpE2e.start("test/fixtures/gghjt.mrp");

    {
      // 检测内存
      await vmrp.delay(1_000);
      await vmrp.key('LEFT_SOFT', 1_000);
      await vmrp.delay(1_000);

    }
    await vmrp.delay(memCheckTime);
    const boot = await vmrp.screen("bgm-select");
    // rgb(72,88,0)
    expect(boot.pixel(227, 308)).toEqual([0, 0, 0]);

    // 是否开启音乐？-> 否
    await vmrp.click(230, 308, 1_000);
    await vmrp.delay(1_000);

    // 跳过启动剧情
    await vmrp.click(227, 308, 1_000);
    await vmrp.delay(1_000);
    await vmrp.click(227, 308, 1_000);
    await vmrp.delay(1_000);
    await vmrp.click(227, 308, 1_000);
    await vmrp.delay(1_000);
    await vmrp.click(227, 308, 1_000);
    await vmrp.delay(1_000);

    // 进入主菜单
    const wake = await vmrp.screen("menu");
    // rgb(152, 112, 32)
    expect(wake.pixel(110, 27)).toEqual([152, 112, 32]);
    {
      // 点击开始游戏，进入游戏界面
      await vmrp.click(116, 291, 3_000);
      await vmrp.delay(3_000);
      const screen = await vmrp.screen("game-start");
      // rgb(248, 252, 0)
      expect(screen.pixel(42, 140)).toEqual([248, 252, 0]);
    }

    {
      // 六次回车，对话确认后，渲染场景
      for (let i=0; i<6; i++) {
        await vmrp.key('ENTER', 3_000);
        await vmrp.delay(1_000);
      }
      const screen = await vmrp.screen("scene-render");
      // rgb(192, 148, 96)
      expect(screen.pixel(55, 302)).toEqual([192, 148, 96]);
      // 火车顶部场景；这些点覆盖离屏背景和底部栏，防止只显示中间视口。
      expect(screen.pixel(120, 20)).toEqual([8, 8, 0]);
      expect(screen.pixel(120, 300)).toEqual([192, 148, 96]);
    }

  });
});
