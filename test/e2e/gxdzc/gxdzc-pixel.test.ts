import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";

describe("gxdzc pixel flow", () => {
  let vmrp: VmrpE2e | undefined;
  let ws: VmrpWorkspace | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it("checks screen pixels after each click", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    vmrp = await VmrpE2e.start("test/fixtures/gxdzc.mrp", { workDir: ws.dir });

    const boot = await vmrp.screen("boot");
    expect(boot.pixel(120, 160)).toEqual([0, 0, 0]);

    await vmrp.click(0, 0, 3_000);
    const wake = await vmrp.screen("after-wake");
    expect(wake.pixel(120, 160)).toEqual([0, 0, 0]);

    await vmrp.delay(5000);
    await vmrp.click(228, 309, 3_000);
    await vmrp.delay(5000);
    const menu = await vmrp.screen("after-menu");
    // 点击 (228,309) 后进入标题界面（"侠盗之城 / 开始游戏"），(80,80) 落在标题背景上。
    expect(menu.pixel(80, 80)).toEqual([208, 164, 72]);
  });
});
