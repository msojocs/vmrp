import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";

describe("gxdzc pixel flow", () => {
  let vmrp: VmrpE2e | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
  });

  it("checks screen pixels after each click", async () => {
    vmrp = await VmrpE2e.start("test/fixtures/gxdzc.mrp");

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
