import { describe, it, afterEach } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";
import fs from "fs";

describe("gghjt capture", () => {
  let vmrp: VmrpE2e | undefined;
  afterEach(async () => { await vmrp?.close(); vmrp = undefined; });
  it("capture scene", async () => {
    if (!fs.existsSync('mythroad/plugins/netpay.mrp')) fs.cpSync('test/fixtures/plugins/netpay.mrp', 'mythroad/plugins/netpay.mrp');
    fs.rmSync('mythroad/gghjt', { force: true, recursive: true });
    fs.cpSync('test/fixtures/gghjt', 'mythroad/gghjt', { recursive: true });
    vmrp = await VmrpE2e.start("mythroad/gghjt.mrp");
    await vmrp.delay(4000);
    await vmrp.screen("bgm-select");
    await vmrp.click(230, 308, 1_000); await vmrp.delay(1_000);
    await vmrp.click(227, 308, 1_000); await vmrp.delay(1_000);
    await vmrp.click(227, 308, 1_000); await vmrp.delay(1_000);
    await vmrp.click(227, 308, 1_000); await vmrp.delay(1_000);
    await vmrp.click(227, 308, 1_000); await vmrp.delay(1_000);
    await vmrp.screen("menu");
    await vmrp.click(162, 291, 3_000); await vmrp.delay(1_000);
    await vmrp.click(116, 291, 3_000); await vmrp.delay(1_000);
    await vmrp.screen("pay-start");
    await vmrp.key('LEFT_SOFT', 3_000); await vmrp.delay(2_000);
    await vmrp.screen("game-start");
    await vmrp.key('ENTER', 3_000); await vmrp.delay(1_000);
    await vmrp.key('ENTER', 3_000); await vmrp.delay(8_000);
    await vmrp.screen("scene-render");
    fs.cpSync(vmrp.tmpDir + "/scene-render.ppm", "/tmp/scene-render.ppm");
    fs.cpSync(vmrp.tmpDir + "/stdout.log", "/tmp/gghjt-stdout.log");
  });
});
