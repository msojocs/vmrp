import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";
import fs from "fs";

describe("wbrw 进入主界面", () => {
  let vmrp: VmrpE2e | undefined;
  let ws: VmrpWorkspace | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it("浏览器正常启动", async () => {
    // 每个用例使用独立的 mythroad 数据副本,避免并发执行时互相覆盖插件/缓存/存档。
    ws = await VmrpWorkspace.create();
    fs.rmSync(ws.path('mythroad/brw'), { recursive: true, force: true })
    
    vmrp = await VmrpE2e.start("test/fixtures/wbrw.mrp", { workDir: ws.dir });

    await vmrp.delay(5000);
    const boot = await vmrp.screen("home");
    // rgb(248, 252, 248)
    expect(boot.pixel(76, 252)).toEqual([248, 252, 248]);
    // rgb(64, 144, 208)
    expect(boot.pixel(57, 306)).toEqual([64, 144, 208]);

  });
});
