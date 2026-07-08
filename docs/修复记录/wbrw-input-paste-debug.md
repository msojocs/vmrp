# wbrw input paste debug

## 2026-07-01 19:01 +0800 progress

- Target flow: `test/e2e/wbrw/input-text.test.ts` starts `test/fixtures/wbrw.mrp`, waits for home, sends `UP`, `ENTER`, `ENTER`, then enters the platform edit state.
- Baseline artifact: `/tmp/vmrp-e2e-gt4MbE` from `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/wbrw/input-text.test.ts --reporter=verbose`.
- Baseline stderr shows `INFO: title: '网址', text: '', type: 0, max_size: 255`, proving the app calls `editCreate` for the URL edit box.
- Root-cause evidence from source and disassembly:
  - `src/mythroad/dsm.c` converts host UTF-8 edit text to GB and then UCS2-BE when `FLAG_USE_UTF8_EDIT` is enabled.
  - `src/mythroad/src/lib/mr_iolib_target.c` pushes edit text with `mr_wstrlen(text) + 2`, confirming Mythroad edit text is a byte buffer with embedded zero bytes.
  - `src/arm_ext_executor.c` table slot 77 previously copied `mr_editGetText` with `alloc_string`, which uses `strlen`. For ASCII UCS2-BE text like `http://wap.baidu.com`, the first byte is `0x00`, so `strlen` returns zero and ARM browser code receives an empty string.
  - `objdump -d -S build/vmrp` maps `editGetText` to the log site and confirms the current binary had the source-level `alloc_string` path before the fix.
- Implemented direction:
  - Copy table[77] return values with `wstrlen(text) + 2` raw bytes, not `strlen`.
  - Add an E2E `PASTE` command that sets the SDL clipboard and injects Ctrl+V with a Ctrl modifier, so the regression uses the same edit-mode path as manual paste.

## 2026-07-01 19:06 +0800 verification

- Built target: `cmake --build build --target vmrp -j2` passed.
- Focused regression: `VMRP_E2E_KEEP_TMP=1 pnpm vitest run test/e2e/wbrw/input-text.test.ts --reporter=verbose` passed.
- Final verification artifact: `/tmp/vmrp-e2e-4w5oYJ`.
- Runtime log evidence: `/tmp/vmrp-e2e-4w5oYJ/stderr.log` includes `INFO: editGetText(): 'http://wap.baidu.com'`.
- PPM evidence:
  - `before-paste.ppm` and `after-paste.ppm` are both `240x320`.
  - Top address-bar diff after paste: `1030` changed pixels, all inside bbox `x=10..233, y=3..26`.
  - Visual inspection of the converted PPMs shows the before image has an empty address bar and the after image shows `http://wap.baidu.com`.
- Post-build disassembly evidence:
  - `objdump -d -S build/vmrp` shows table[77] now calls `mr_editGetText`, then `wstrlen`, adds `2`, and calls `alloc_bytes` instead of the previous `alloc_string`/`strlen` path.
