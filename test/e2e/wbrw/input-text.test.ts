import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";
import fs from "fs";
import { readFile } from "fs/promises";

describe("wbrw 输入文字", () => {
  let vmrp: VmrpE2e | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
  });

  it("输入文字", async () => {
    fs.rmSync('mythroad/brw', { recursive: true, force: true })

    vmrp = await VmrpE2e.start("test/fixtures/wbrw.mrp");
    {
      await vmrp.delay(5000);
      const boot = await vmrp.screen("home");
      // rgb(248, 252, 248)
      expect(boot.pixel(76, 252)).toEqual([248, 252, 248]);
      // rgb(64, 144, 208)
      expect(boot.pixel(57, 306)).toEqual([64, 144, 208]);
    }
    {
      // 移动光标到输入框
      await vmrp.key('UP', 1_000);
      // 进入输入框界面
      await vmrp.key('ENTER', 1_000);
      // 打开输入界面
      await vmrp.key('ENTER', 1_000);
      const beforePaste = await vmrp.screen("before-paste");
      await vmrp.paste("http://wap.baidu.com");
      const afterPaste = await vmrp.screen("after-paste");
      const stderr = await readFile(vmrp.stderrPath, "utf8");

      expect(stderr).toContain("editGetText(): 'http://wap.baidu.com'");
      expect(afterPaste.diffPixelCount(beforePaste, { x: 0, y: 0, width: 240, height: 32 })).toBeGreaterThan(500);
      expect(afterPaste.pixel(99, 17)).toEqual([0, 0, 0]);
      // rgb(248, 252, 248)
      expect(afterPaste.pixel(98, 17)).toEqual([248, 252, 248]);
    }
  });
});
