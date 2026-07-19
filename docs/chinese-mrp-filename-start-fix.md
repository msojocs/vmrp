# Chinese MRP Filename Start Fix

## Problem

Launching an imported MRP whose host filename contains Chinese characters can
fail before the package is parsed.  The Flutter/Android importer stores the file
under `mythroad/` and passes a UTF-8 path such as `mythroad/中文游戏.mrp`.

The Mythroad path bridge previously treated every filesystem path as GB text
when `FLAG_USE_UTF8_FS` was enabled.  That converted an already UTF-8 host path
as if it were GB, so the final path no longer matched the copied file.

Windows also used narrow CRT/Win32 path APIs in several host-file entry points,
which prevents UTF-8 Chinese paths from opening reliably on desktop builds.

## Fix

- `src/mythroad/dsm.c`: `get_filename()` now first checks whether the raw path
  already exists on the UTF-8 host filesystem.  Existing host paths are kept as
  UTF-8; missing MRP-internal paths still follow the old GB-to-UTF-8 conversion.
- `src/fileLib.c` and `src/include/dirent_win.h`: Windows host file operations
  now convert internal UTF-8 paths to UTF-16 and call wide APIs.
- Startup-time direct MRP reads now use `skyengine_host_fopen()` so app compat header
  probing, ARM EXT cache parsing, and smoke loading all follow the same host path
  behavior.
- `src/skyengine_args.c`: Windows executable and CLI path resolution now uses wide
  Windows APIs and converts back to internal UTF-8.

## Verification

- `flutter test test\widget_test.dart --plain-name "runtime MRP path preserves Chinese file name"`
  passed.
- `android\gradlew.bat :app:externalNativeBuildDebug` passed for `arm64-v8a`
  and `armeabi-v7a`.

Full `flutter test` and a broader widget/local test run were attempted but timed
out in this workspace before completion.
