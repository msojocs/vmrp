# minimp3

- 来源: https://github.com/lieff/minimp3 (master 分支, 2026-07-04 获取)
- 许可: CC0 / Public Domain(见 minimp3.h 头部声明)
- 用途: `src/native_dsm_funcs.c` 中 `mr_playSound(MR_SOUND_MP3)` 的 MP3 解码,
  解码结果经 `native_audio_set_pcm` 统一重采样为 44.1KHz/S16/stereo。
- 仅 vendored 单头文件 `minimp3.h`(核心解码器),未引入 `minimp3_ex.h`。
- 验证方式: 用 `mp3dec_decode_frame` 全量解码 `build/mythroad/talkcat/` 下的
  MP3 音效(32KHz mono, MPEG1 Layer III),帧数/时长与 `file` 命令解析一致。
