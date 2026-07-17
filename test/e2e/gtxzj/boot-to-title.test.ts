import { afterEach, describe, expect, it } from "vitest";
import { VmrpE2e, VmrpWorkspace } from "../vmrp-e2e.js";

// 钢铁战将(240x320, FULL Lua + cfunction.ext 私有 game.ext)。fixture SHA-256:
// 65f41d75b54b66d9fe747c7df0b50b5d2808ff71c08bdbf4a860989d976773da。
describe("gtxzj 启动", () => {
  let vmrp: VmrpE2e | undefined;
  let ws: VmrpWorkspace | undefined;

  afterEach(async () => {
    await vmrp?.close();
    vmrp = undefined;
    await ws?.dispose();
    ws = undefined;
  });

  it("通过启动画面并处理软键、方向键和确认键", async () => {
    // 独立运行树同时提供基础字库，避免宿主存档和字体安装状态污染结果。
    ws = await VmrpWorkspace.create();
    vmrp = await VmrpE2e.start("test/fixtures/gtxzj.mrp", {
      workDir: ws.dir,
    });

    await vmrp.delay(4_000);
    const soundMenu = await vmrp.waitForPixel(79, 160, [248, 0, 0], {
      name: "sound-menu",
      timeoutMs: 10_000,
      intervalMs: 200,
    });
    expect(soundMenu.pixel(119, 120)).toEqual([248, 252, 248]);

    // 右软键选择“关闭”后进入带人物立绘和“开始游戏”的标题画面。
    await vmrp.key("RIGHT_SOFT", 2_000);
    const title = await vmrp.waitForPixel(105, 287, [248, 244, 152], {
      name: "title",
      timeoutMs: 10_000,
      intervalMs: 200,
    });
    // 等到底部选择文字出现后才可输入；同时核对标题立绘的稳定顶部高光。
    expect(title.pixel(120, 50)).toEqual([240, 120, 56]);
    expect(title.uniqueColorCount()).toBeGreaterThan(50);

    // DOWN 把选择从“开始游戏”移到“加载游戏”，变化集中在底部菜单文字。
    await vmrp.key("DOWN", 2_000);
    const loadSelected = await vmrp.screen("load-selected");
    expect(title.diffPixelCount(loadSelected, {
      x: 80,
      y: 280,
      width: 45,
      height: 25,
    })).toBeGreaterThan(200);

    // 无存档时 ENTER 打开灰色提示框，证明按键分发和游戏状态机均可交互。
    await vmrp.key("ENTER", 2_000);
    const noSaveDialog = await vmrp.waitForPixel(10, 100, [120, 120, 120], {
      name: "no-save-dialog",
      timeoutMs: 5_000,
      intervalMs: 100,
    });
    expect(noSaveDialog.pixel(10, 95)).toEqual([248, 252, 248]);
  }, 40_000);
});
