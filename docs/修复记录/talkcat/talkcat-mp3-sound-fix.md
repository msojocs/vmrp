# talkcat 启动音乐(MP3)无声修复

日期: 2026-07-04

## 现象

`build/skyengine build/mythroad/talkcat.mrp` 启动后完全无声,stdout 反复输出:

```
native_playSound: unsupported compressed sound type 2 len=10560
native_playSound: unsupported compressed sound type 2 len=5304
...
```

type 2 即 `MR_SOUND_MP3`(见 `src/native_dsm_funcs.c` 的 `NATIVE_SOUND_MP3`)。

## 根因

音频链路本身是通的:talkcat 通过 `mr_playSound`/`mr_platEx(MR_MEDIA_*)`
把整段 MP3 流传到 `native_playSound`(`src/native_dsm_funcs.c`),但 native 层
只实现了 MIDI/WAV/PCM 三种解码,`NATIVE_SOUND_MP3` 分支直接返回
`MR_FAILED`,导致所有 MP3 音效/音乐静默失败。

talkcat 的启动音与交互音效均为 MPEG1 Layer III(启动音 24KHz mono,
资源包音效 32KHz/16KHz/11.025KHz mono)。

## 修复

1. vendored 单头文件解码器 `third_party/minimp3/minimp3.h`
   (https://github.com/lieff/minimp3, CC0,见该目录 README.md)。
2. `src/native_dsm_funcs.c` 新增 `native_audio_play_mp3()`:
   - `MINIMP3_ONLY_MP3` 只保留 Layer III 支持;
   - 全量解码到交织 S16 PCM(动态扩容,MRP 音效通常仅数秒);
   - 以首帧采样率/声道数为准,中途参数变化的畸形流截断处理,避免变调;
   - 解码结果交给已有的 `native_audio_set_pcm()` 统一重采样为
     44.1KHz/S16/stereo,SDL 与 Flutter(`skyengine_api_audio_render_s16le`)
     两条渲染路径都自动受益;
   - 成功时打印 `native_playSound: mp3 len=... -> ...Hz/...ch samples=...`,
     供 e2e 从 stdout 判断链路是否触发。
3. `native_playSound` 的 `NATIVE_SOUND_MP3` 分支改为调用上述函数;
   M4A/AMR/AMR_WB 仍按原样返回 `MR_FAILED` 并打日志。

## 兼容性影响

- 未改动任何 ABI/函数表/事件码;只是把原本必然失败的 MP3 分支实现了。
- `mr_playSound` 返回值从 `MR_FAILED` 变为 `MR_SUCCESS`(解码成功时),
  依赖失败返回值的应用行为会变化——这正是预期效果(dsm.c 的
  `dsm_media_play` 会把 media status 置为 `MR_MEDIA_PLAYING`)。
- MP3 播放与 PCM/WAV/MIDI 共享单一 `native_audio` 通道,新播放会打断
  旧播放,与修复前其它格式的行为一致。

## 验证

- `cmake --build build --target skyengine` 通过;`skyengine-shared` 构建通过。
- 手动运行(work-dir 隔离,MRP 位于 work-dir 内,参见 e2e 工作区隔离经验):
  stdout 中原 `unsupported compressed sound type 2` 全部消失,替换为
  `native_playSound: mp3 len=10560 -> 24000Hz/1ch samples=50112 loop=0` 等。
- `SDL_AUDIODRIVER=disk` 捕获 SDL 回调输出的 PCM,确认非全零(有实际波形)。
- 用独立小程序对 `build/mythroad/talkcat/` 的多个 MP3 全量解码,帧数/
  采样率/时长与 `file` 命令解析一致。
- `pnpm vitest run test/e2e/talkcat/game-prepare.test.ts -t "游戏启动正常"` 通过。
- `build/test_skyengine`、`build/test_skyengine_api`、`build/test_skyengine_filelib` 均通过。

## 剩余风险

- M4A/AMR/AMR_WB 仍未实现(维持原状,遇到会打日志)。
- minimp3 定点路径未启用 SIMD 特化的平台(如某些交叉编译目标)解码
  速度较慢,但 MRP 音效体积小,影响可忽略。
