import { afterEach, describe, expect, it, vi } from "vitest";
import { VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";
import fs from "fs";

describe("istore 进入主界面", () => {
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
    
    // istore 是横屏(480x320)构建,按其原生分辨率运行以显示完整画面。
    vmrp = await VmrpE2e.start("test/fixtures/sky_istore.mrp", { workDir: ws.dir, memory: '2M' });

    {
      await vi.waitFor(async () => {
        if (!vmrp) throw new Error("vmrp 未初始化");
        // 无网络提示
        const screen = await vmrp.screen("home");
        // rgb(112, 152, 208)
        expect(screen.pixel(123, 78)).toEqual([112, 152, 208]);
        // rgb(216, 220, 216)
        expect(screen.pixel(143, 162)).toEqual([216, 220, 216]);
        // rgb(168, 208, 80)
        expect(screen.pixel(121, 205)).toEqual([168, 208, 80]);
      }, {
        timeout: 30_000,
        interval: 1000
      })
    }
  });
  
});
