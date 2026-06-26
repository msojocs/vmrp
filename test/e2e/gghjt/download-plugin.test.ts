import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";
import fs from "fs";

describe("gghjt pixel flow", () => {
  let vmrp: VmrpE2e | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
  });

  it("下载付费插件 - 直接返回", async () => {
    // 删除后，继续游戏会进入下载netpay插件界面。
    fs.rmSync('mythroad/plugins/netpay.mrp', { force: true });
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
    await vmrp.delay(6_000);
    const boot = await vmrp.screen("bgm-select");
    // rgb(0, 0, 0)
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

    // 切换菜单
    await vmrp.click(162, 291, 3_000);
    await vmrp.delay(1_000);
    // rgb(232, 196, 104)
    expect(wake.pixel(162, 291)).toEqual([232, 196, 104 ]);

    // 点击继续游戏，进入插件下载界面
    await vmrp.click(116, 291, 3_000);
    await vmrp.delay(1_000);
    const menu = await vmrp.screen("download-plugin");
    // rgb(0, 4, 0)
    expect(menu.pixel(80, 80)).toEqual([0, 4, 0]);
  });
  it("下载付费插件 - 返回重进", async () => {
    // 删除后，继续游戏会进入下载netpay插件界面。
    fs.rmSync('mythroad/plugins/netpay.mrp', { force: true });
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
    await vmrp.delay(6_000);
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

    // 切换菜单
    await vmrp.click(162, 291, 3_000);
    await vmrp.delay(1_000);
    // rgb(232, 196, 104)
    expect(wake.pixel(162, 291)).toEqual([232, 196, 104 ]);

    // 点击继续游戏，进入插件下载界面
    await vmrp.click(116, 291, 3_000);
    await vmrp.delay(1_000);
    const menu1 = await vmrp.screen("download-plugin");
    // rgb(0, 4, 0)
    expect(menu1.pixel(80, 80)).toEqual([0, 4, 0]);
    
    // 取消下载返回主菜单
    await vmrp.click(227, 308, 1_000);
    await vmrp.delay(2_000);
    // 切换菜单
    await vmrp.click(162, 291, 3_000);
    await vmrp.delay(1_000);
    // rgb(232, 196, 104)
    expect(wake.pixel(162, 291)).toEqual([232, 196, 104 ]);

    
    // 点击继续游戏，第二次进入插件下载界面
    await vmrp.click(116, 291, 3_000);
    await vmrp.delay(1_000);
    const menu2 = await vmrp.screen("download-plugin");
    // rgb(0, 4, 0)
    expect(menu2.pixel(80, 80)).toEqual([0, 4, 0]);
  
  });
  it("下载付费插件 - 下载完毕", async () => {
    // 删除后，继续游戏会进入下载netpay插件界面。
    fs.rmSync('mythroad/plugins/netpay.mrp', { force: true });
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
    await vmrp.delay(6_000);
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

    // 切换菜单
    await vmrp.click(162, 291, 3_000);
    await vmrp.delay(1_000);
    const continueMenu = await vmrp.screen("continueMenu");
    // rgb(232, 196, 104)
    expect(continueMenu.pixel(162, 291)).toEqual([232, 196, 104 ]);

    // 点击继续游戏，进入插件下载界面
    await vmrp.click(116, 291, 3_000);
    await vmrp.delay(1_000);
    const menu1 = await vmrp.screen("download-plugin");
    // rgb(0, 4, 0)
    expect(menu1.pixel(80, 80)).toEqual([0, 4, 0]);
    
    // 确定下载
    await vmrp.click(5, 308, 1_000);
    // 等待下载开始
    await vmrp.delay(5_000);
    // 检查屏幕是否成功的绿色
    const downloading = await vmrp.screen("download-ing");
    expect(downloading.pixel(70, 176)).toEqual([0, 252, 0]);
    // 等待下载完成
    await vmrp.delay(10_000);
    // 检查屏幕是否成功的绿色
    const success = await vmrp.screen("download-end");
    expect(success.pixel(101, 148)).toEqual([0, 252, 0]);
  
    // 点击确定进入付费界面
    await vmrp.click(15, 308, 1_000);
    await vmrp.delay(2_000);
    const pay = await vmrp.screen("pay-start");
    expect(pay.pixel(104, 147)).toEqual([104, 104, 224]);
  });
  it("下载付费插件 - 下载完毕返回重进", async () => {
    // 删除后，继续游戏会进入下载netpay插件界面。
    fs.rmSync('mythroad/plugins/netpay.mrp', { force: true });
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
    await vmrp.delay(6_000);
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

    // 切换菜单
    await vmrp.click(162, 291, 3_000);
    await vmrp.delay(1_000);
    const continueMenu = await vmrp.screen("continueMenu");
    // rgb(232, 196, 104)
    expect(continueMenu.pixel(162, 291)).toEqual([232, 196, 104 ]);

    // 点击继续游戏，进入插件下载界面
    await vmrp.click(116, 291, 3_000);
    await vmrp.delay(1_000);
    const menu1 = await vmrp.screen("download-plugin");
    // rgb(0, 4, 0)
    expect(menu1.pixel(80, 80)).toEqual([0, 4, 0]);
    
    // 确定下载
    await vmrp.click(5, 308, 1_000);
    // 等待下载开始
    await vmrp.delay(5_000);
    // 检查屏幕是否成功的绿色
    const downloading = await vmrp.screen("download-ing");
    expect(downloading.pixel(70, 176)).toEqual([0, 252, 0]);
    // 等待下载完成
    await vmrp.delay(10_000);
    // 检查屏幕是否成功的绿色
    const success = await vmrp.screen("download-end");
    expect(success.pixel(101, 148)).toEqual([0, 252, 0]);
  
    // 点击确定进入付费界面
    await vmrp.click(15, 308, 1_000);
    
    await vmrp.delay(2_000);
    const pay = await vmrp.screen("pay-start");
    expect(pay.pixel(104, 147)).toEqual([104, 104, 224]);
      expect(pay.pixel(12, 302)).toEqual([248, 252, 248]);

    {
      // 取消下载返回主菜单
      await vmrp.click(227, 308, 1_000);
      await vmrp.delay(2_000);
      const screen = await vmrp.screen("menu-default");
      expect(screen.pixel(162, 291)).toEqual([232, 196, 104 ]);
    }
    {
      // 切换菜单
      await vmrp.click(162, 291, 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("menu-continue");
      // rgb(232, 196, 104)
      expect(screen.pixel(162, 291)).toEqual([232, 196, 104 ]);
    }
    {
      // 点击继续游戏，进入付费界面（前面下载完付费插件了）
      await vmrp.click(116, 291, 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("pay-start-2");
      // rgb(104, 104, 224)
      expect(screen.pixel(104, 147)).toEqual([104, 104, 224]);
    }
  });
  it("下载付费插件 - 下载完毕付费超时返回重进", async () => {
    // 删除后，继续游戏会进入下载netpay插件界面。
    fs.rmSync('mythroad/plugins/netpay.mrp', { force: true });
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
    await vmrp.delay(6_000);
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

    // 切换菜单
    await vmrp.click(162, 291, 3_000);
    await vmrp.delay(1_000);
    const continueMenu = await vmrp.screen("continueMenu");
    // rgb(232, 196, 104)
    expect(continueMenu.pixel(162, 291)).toEqual([232, 196, 104 ]);

    // 点击继续游戏，进入插件下载界面
    await vmrp.click(116, 291, 3_000);
    await vmrp.delay(1_000);
    const menu1 = await vmrp.screen("download-plugin");
    // rgb(0, 4, 0)
    expect(menu1.pixel(80, 80)).toEqual([0, 4, 0]);
    
    // 确定下载
    await vmrp.click(5, 308, 1_000);
    // 等待下载开始
    await vmrp.delay(5_000);
    // 检查屏幕是否成功的绿色
    const downloading = await vmrp.screen("download-ing");
    expect(downloading.pixel(70, 176)).toEqual([0, 252, 0]);
    // 等待下载完成
    await vmrp.delay(10_000);
    // 检查屏幕是否成功的绿色
    const success = await vmrp.screen("download-end");
    expect(success.pixel(101, 148)).toEqual([0, 252, 0]);
  
    // 点击确定进入付费界面
    await vmrp.click(15, 308, 1_000);
    await vmrp.delay(2_000);
    const pay = await vmrp.screen("pay-start");
    expect(pay.pixel(104, 147)).toEqual([104, 104, 224]);

    {
      await vmrp.delay(60_000); // 等待付费超时
      const pay = await vmrp.screen("pay-timeout");
      // 确定按钮消失
      // rgb(0, 104, 208)
      expect(pay.pixel(12, 302)).toEqual([0, 104, 208]);
    }
    {
      // 取消下载返回主菜单
      await vmrp.click(227, 308, 1_000);
      await vmrp.delay(2_000);
      const screen = await vmrp.screen("menu-default");
      expect(screen.pixel(162, 291)).toEqual([232, 196, 104 ]);
    }
    {
      // 切换菜单
      await vmrp.click(162, 291, 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("menu-continue");
      // rgb(232, 196, 104)
      expect(screen.pixel(162, 291)).toEqual([232, 196, 104 ]);
    }
    {
      // 点击继续游戏，进入付费界面（前面下载完付费插件了）
      await vmrp.click(116, 291, 3_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("pay-start-2");
      // rgb(104, 104, 224)
      expect(screen.pixel(104, 147)).toEqual([104, 104, 224]);
    }
  });
});
