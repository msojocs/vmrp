import { afterEach, describe, expect, it, vi } from "vitest";
import { VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";

describe("cookie", () => {
  let vmrp: VmrpE2e | undefined;
  let ws: VmrpWorkspace | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it("应用正常启动", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    // 准备文件
    
    // istore
    vmrp = await VmrpE2e.start("test/fixtures/cookie_v6110.mrp", { workDir: ws.dir, memory: '2M' });

    {
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error("vmrp 未初始化");
        // 从第一项向上循环到末项“文件管理”，直接验证末行高亮背景。
        const screen = await vmrp.screen("home");
        // rgb(32, 72, 112)
        expect(screen.pixel(75, 276)).toEqual([32, 72, 112]);
      }, {
        timeout: 10_000,
        interval: 1000
      })
    }
    {
      await vmrp.key('LEFT_SOFT', 1_000, 250)
      
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error("vmrp 未初始化");
        // 主页面
        const screen = await vmrp.screen("menu-first");
        // rgb(248, 192, 32)
        expect(screen.pixel(94, 166)).toEqual([248, 192, 32]);
      }, {
        timeout: 10_000,
        interval: 1000
      })
    }
    {
      await vmrp.key('UP', 1_000)
      
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error("vmrp 未初始化");
        // 主页面
        const screen = await vmrp.screen("menu-last");
        // rgb(248, 192, 32)
        expect(screen.pixel(99, 274)).toEqual([248, 192, 32]);
      }, {
        timeout: 10_000,
        interval: 1000
      })
    }
    {
      await vmrp.key('LEFT_SOFT', 1_000)
      
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error("vmrp 未初始化");
        // 文件管理根目录加载完成后第一项高亮；目录可全部显示时右侧不会绘制滚动滑块。
        const screen = await vmrp.screen("file-manage-top");
        // rgb(248, 200, 24)
        expect(screen.pixel(100, 48)).toEqual([248, 200, 24]);
      }, {
        timeout: 10_000,
        interval: 1000
      })
    }
    {
      await vmrp.key('UP', 1_000)
      
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error("vmrp 未初始化");
        // 从第一项向上循环到末项“应用列表”，直接验证末行高亮背景。
        const screen = await vmrp.screen("file-manage-bottom");
        // rgb(248, 200, 24)
        expect(screen.pixel(100, 216)).toEqual([248, 200, 24]);
      }, {
        timeout: 10_000,
        interval: 1000
      })
    }
    {
      await vmrp.key('LEFT_SOFT', 1_000)
      
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error("vmrp 未初始化");
        // 文件菜单第一项“进入”的高亮背景证明菜单已打开。
        const screen = await vmrp.screen("file-manage-menu");
        // rgb(248, 192, 32)
        expect(screen.pixel(86, 251)).toEqual([248, 192, 32]);
      }, {
        timeout: 10_000,
        interval: 1000
      })
    }
    {
      await vmrp.key('LEFT_SOFT', 1_000)

      const secondApp = await vmrp.waitForPixel(
        120,
        180,
        [24, 160, 200],
        {
          name: "second-app",
          timeoutMs: 10_000,
          intervalMs: 250,
        },
      );
      // 同时验证应用列表标题栏，避免把旧应用的启动等待页误判为接力成功。
      expect(secondApp.pixel(120, 10)).toEqual([24, 104, 136]);
    }
  });
  
});
