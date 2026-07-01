import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";
import fs from "fs";

describe("wbrw 进入主界面", () => {
  let vmrp: VmrpE2e | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
  });

  it("浏览器正常启动", async () => {
    fs.rmSync('mythroad/brw', { recursive: true, force: true })
    
    vmrp = await VmrpE2e.start("test/fixtures/wbrw.mrp");

    await vmrp.delay(5000);
    const boot = await vmrp.screen("home");
    // rgb(248, 252, 248)
    expect(boot.pixel(76, 252)).toEqual([248, 252, 248]);
    // rgb(64, 144, 208)
    expect(boot.pixel(57, 306)).toEqual([64, 144, 208]);

  });
});
