import { afterEach, describe, expect, it, vi } from "vitest";
import { VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";
import fs from "fs";

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
        // 主页面
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
        // 主页面
        const screen = await vmrp.screen("file-manage-top");
        // rgb(96, 112, 136)
        expect(screen.pixel(235, 68)).toEqual([96, 112, 136]);
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
        const screen = await vmrp.screen("file-manage-bottom");
        // rgb(96, 112, 136)
        expect(screen.pixel(236, 249)).toEqual([96, 112, 136]);
      }, {
        timeout: 10_000,
        interval: 1000
      })
    }
    {
      await vmrp.key('LEFT_SOFT', 1_000)
      
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error("vmrp 未初始化");
        // 主页面
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
      // 启动MRP
      await vmrp.delay(10_000)
    }
  });
  
});
