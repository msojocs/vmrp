import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e } from "../vmrp-e2e.js";
import fs from 'fs'

describe("talkcat 进入游戏", () => {
  let vmrp: VmrpE2e | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
  });

  it("游戏启动正常", async () => {
    fs.rmSync('mythroad/talkcat', { force: true, recursive: true })

    vmrp = await VmrpE2e.start("test/fixtures/talkcat.mrp");

    await vmrp.delay(60_000);
    const boot = await vmrp.screen("main");
    // rgb(232, 236, 232)
    expect(boot.pixel(27, 273)).toEqual([232, 236, 232]);
    // rgb(0, 12, 16)
    expect(boot.pixel(216, 27)).toEqual([0, 12, 16]);
    // rgb(64, 64, 64)
    expect(boot.pixel(221, 279)).toEqual([64, 64, 64]);

  });
  it("下载喝水资源包并安装", async () => {
    fs.rmSync('mythroad/talkcat', { force: true, recursive: true })

    vmrp = await VmrpE2e.start("test/fixtures/talkcat.mrp");

    await vmrp.delay(40_000);
    const boot = await vmrp.screen("main");
    // rgb(232, 236, 232)
    expect(boot.pixel(27, 273)).toEqual([232, 236, 232]);
    // rgb(0, 12, 16)
    expect(boot.pixel(216, 27)).toEqual([0, 12, 16]);
    // rgb(64, 64, 64)
    expect(boot.pixel(221, 279)).toEqual([64, 64, 64]);

    {
      // 点击水杯图标，触发下载提示
      await vmrp.click(22, 280, 1_000)
      await vmrp.delay(1_000)
      // 检查像素
      const screen = await vmrp.screen("download-confirm");
      // rgb(32, 64, 120)
      expect(screen.pixel(78, 280)).toEqual([32, 64, 120]);
    }
    {
      // 点击确定开始下载
      await vmrp.click(78, 280, 1_000)
      await vmrp.delay(5_000)
      // rgb(32, 212, 0)
      const screen = await vmrp.screen("downloading");
      expect(screen.pixel(79, 257)).toEqual([32, 212, 0]);
      await vmrp.delay(20_000)
    }
    {
      // 检查是否显示安装提示
      // rgb(32, 64, 120)
      const screen = await vmrp.screen("install-confirm");
      expect(screen.pixel(77, 279)).toEqual([32, 64, 120]);
      // 点击确定开始安装
      await vmrp.click(77, 279, 1_000)
      await vmrp.delay(5_000)
    }
    {
      // rgb(32, 212, 0)
      const screen = await vmrp.screen("installing");
      expect(screen.pixel(54, 257)).toEqual([32, 212, 0]);
      await vmrp.delay(80_000)
      const stdout = fs.readFileSync(vmrp.stdoutPath, 'utf-8')
      expect(stdout).not.contain('Invalid memory read')
    }
  });
});
