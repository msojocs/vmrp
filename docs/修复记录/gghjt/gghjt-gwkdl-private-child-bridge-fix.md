# gghjt / gwkdl private child bridge compatibility

## 2026-06-25

### Symptom

After `fix: 大闹天宫图像无法渲染`, the targeted regression:

```bash
pnpm vitest run test/e2e/gghjt/download-plugin.test.ts -t "下载付费插件 - 直接返回"
```

failed at the final download-screen pixel check. The captured screen was still
the gghjt main menu: pixel `(80,80)` was `[192,152,64]` instead of the expected
download UI color `[0,4,0]`.

### Evidence

The kept diagnostic run at `/tmp/skyengine-e2e-VlevcR` showed:

- Startup still loads `game.ext` and the MTK platform graphics band correctly:
  `synced file=0x40018800 len=1048576`.
- When clicking "继续游戏", the wrapper loads `verdload.ext`:
  `mr_cacheSync(..., 18496)` followed by `synced file=0x327C20 len=18404`.
- `18404` matches the decompressed `verdload.ext` in both
  `test/fixtures/gghjt.mrp` and `test/fixtures/gwkdl_v1003.mrp`.
- The ce382f4 change had added a `compact-sdk` RW mirror layout
  (`record[26] -> rw+0x20`, `record[3..19] -> rw+0x24..0x64`) alongside the
  proven verdload layout. That layout was applied first to the newly loaded
  `verdload.ext` child.

The `compact-sdk` layout came from an earlier gwkdl graphics investigation and
was not the final gwkdl image fix. The final gwkdl fix is the ARM-visible
platform/device memory bands (`0x40000000..0x401FFFFF` and
`0xA0000000..0xA01FFFFF`), not the compact RW bridge mirror.

### Fix

Keep the generic record-source bridge repair, because it only fills blank
module-record slots whose master `EXT_TABLE` entry is a standard self-pointer
bridge.

Keep the proven `verdload.ext` RW mirror:

- `record[0x68] -> rw+0x16C`
- `record[0x0C..0x4C] -> rw+0x170..0x1B0`

Remove the unproven compact RW mirror from `private_child_rw_layouts`. RW target
offsets are child-module data-layout details; they must not be promoted to a
global rule without module-specific disassembly/runtime anchors. Applying the
compact layout to `verdload.ext` overwrote private child state fields and
prevented the download UI from replacing the main menu.

### Verification

- `cmake --build build --target skyengine -j2`
- `pnpm vitest run test/e2e/gghjt/download-plugin.test.ts -t "下载付费插件 - 直接返回"`
- `pnpm vitest run test/e2e/gwkdl/game-prepare.test.ts`

