import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";
import fs from "fs";

describe("gtcm 进入主界面", () => {
  let vmrp: VmrpE2e | undefined;
  let ws: VmrpWorkspace | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it("游戏正常启动", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    fs.rmSync(ws.path('mythroad/gtcm'), { recursive: true, force: true })
    
    // gtcm 是横屏(480x320)构建,按其原生分辨率运行以显示完整画面。
    vmrp = await VmrpE2e.start("test/fixtures/gtcm.mrp", { workDir: ws.dir, screen: "480x320" });

    await vmrp.delay(7000);
    // 不开启音乐
    await vmrp.key('RIGHT_SOFT', 1_000);
    await vmrp.delay(2_000);
    {  
      await vmrp.key('ENTER', 1_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("home");
      // 白色云朵 rgb(248, 252, 248)
      expect(screen.pixel(344, 17)).toEqual([248, 252, 248]);
      // 标题"贪吃猫"黑色描边 rgb(0, 0, 0)
      expect(screen.pixel(280, 46)).toEqual([0, 0, 0]);
      // 灰色地面 rgb(128, 128, 128)
      // (背景蓝色射线区域是旋转动画,颜色随帧变化,不能用作断言点)
      expect(screen.pixel(20, 300)).toEqual([128, 128, 128]);
    }
    {
      // 开始游戏(进入场景会二次加载 UID.scene——大小写与安装时不同,
      // 依赖 dsm.c get_filename 的大小写不敏感回退)
      await vmrp.delay(1_000);
      await vmrp.key('ENTER', 1_000);
      await vmrp.delay(3_000);
      const screen = await vmrp.screen("game");
      // 白云 rgb(248, 252, 248)
      expect(screen.pixel(240, 120)).toEqual([248, 252, 248]);
      // 天空渐变中部 rgb(120, 192, 232)
      expect(screen.pixel(20, 160)).toEqual([120, 192, 232]);
      // 天空顶部 rgb(0, 140, 216)
      expect(screen.pixel(240, 10)).toEqual([0, 140, 216]);
    }
  });
  
  it("付费界面修正", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    fs.rmSync(ws.path('mythroad/gtcm'), { recursive: true, force: true })
    fs.rmSync(ws.path('mythroad/plugins/netpay.mrp'), { recursive: true, force: true })
    
    // gtcm 是横屏(480x320)构建,按其原生分辨率运行以显示完整画面。
    vmrp = await VmrpE2e.start("test/fixtures/gtcm.mrp", { workDir: ws.dir, screen: "480x320" });

    await vmrp.delay(7_000);
    // 不开启音乐
    await vmrp.key('RIGHT_SOFT', 1_000);
    await vmrp.delay(2_000);
    {  
      await vmrp.key('ENTER', 1_000);
      await vmrp.delay(1_000);
      const screen = await vmrp.screen("home");
      // 白色云朵 rgb(248, 252, 248)
      expect(screen.pixel(344, 17)).toEqual([248, 252, 248]);
      // 标题"贪吃猫"黑色描边 rgb(0, 0, 0)
      expect(screen.pixel(280, 46)).toEqual([0, 0, 0]);
      // 灰色地面 rgb(128, 128, 128)
      // (背景蓝色射线区域是旋转动画,颜色随帧变化,不能用作断言点)
      expect(screen.pixel(20, 300)).toEqual([128, 128, 128]);
    }
    {
      // 开始游戏(进入场景会二次加载 UID.scene——大小写与安装时不同,
      // 依赖 dsm.c get_filename 的大小写不敏感回退)
      await vmrp.delay(1_000);
      await vmrp.key('ENTER', 1_000);
      await vmrp.delay(3_000);
      const screen = await vmrp.screen("game");
      // 白云 rgb(248, 252, 248)
      expect(screen.pixel(240, 120)).toEqual([248, 252, 248]);
      // 天空渐变中部 rgb(120, 192, 232)
      expect(screen.pixel(20, 160)).toEqual([120, 192, 232]);
      // 天空顶部 rgb(0, 140, 216)
      expect(screen.pixel(240, 10)).toEqual([0, 140, 216]);
    }
    {
      await vmrp.delay(30_000);
      await vmrp.key('ENTER', 1_000);
      await vmrp.delay(2_000);
      // rgb(40, 40, 40)
      const screen = await vmrp.screen("pay");
      expect(screen.pixel(95, 312)).toEqual([40, 40, 40]);
      expect(screen.pixel(374, 310)).toEqual([40, 40, 40]);
    }
  });
});
