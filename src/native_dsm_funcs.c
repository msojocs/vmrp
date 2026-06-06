#include "./include/native_dsm_funcs.h"
#include "./include/compat_msvc.h"

#include <errno.h>
#include <math.h>
#ifndef _MSC_VER
#include <pthread.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef __linux__
#include <sys/mman.h>
#endif

#if defined(VMRP_SDL_AUDIO)
#ifdef _MSC_VER
#include <SDL.h>
#elif defined(_WIN32)
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#endif

#include "./include/bridge.h"
#include "./include/fileLib.h"
#include "./include/network.h"
#include "./include/utils.h"
#include "./include/vmrp.h"

#define NATIVE_DSM_MEM_SIZE (4 * 1024 * 1024)

static void *native_mem_base;
static uint32 native_mem_len;
static uint64_t native_uptime_base;
static char readdir_buf[128];

#if defined(VMRP_SDL_AUDIO)
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNELS 2
#define AUDIO_FRAMES 1024
#define MIDI_MAX_EVENTS 4096
#define MIDI_MAX_TRACKS 32
#define MIDI_DRUM_CHANNEL 9
#define MIDI_DEFAULT_TEMPO_US 500000
#define MIDI_GAIN 0.22f

typedef enum {
    AUDIO_SOURCE_NONE = 0,
    AUDIO_SOURCE_PCM,
    AUDIO_SOURCE_MIDI
} NativeAudioSource;

typedef struct {
    uint32 tick;
    uint32 value;
    uint8 status;
    uint8 a;
    uint8 b;
} MidiEvent;

typedef struct {
    float phase;
    float freq;
    float velocity;
    float release;
    uint8 note;
    uint8 channel;
    uint8 program;
    uint8 active;
    uint8 releasing;
} MidiVoice;

typedef struct {
    SDL_AudioDeviceID device;
    SDL_AudioSpec spec;
    NativeAudioSource source;
    uint8 *pcm;
    uint32 pcm_len;
    uint32 pcm_pos;
    int loop;
    MidiEvent midi_events[MIDI_MAX_EVENTS];
    uint32 midi_event_count;
    uint32 midi_event_pos;
    uint32 midi_tick;
    uint32 midi_sample_in_tick;
    uint32 midi_samples_per_tick;
    uint32 midi_loop_ticks;
    uint8 midi_program[16];
    float midi_volume[16];
    float midi_pan[16];
    MidiVoice midi_voices[48];
} NativeAudioState;

static NativeAudioState native_audio;

/* dsm.h 在 VMRP 构建下只暴露平台函数表，不带 mrporting.h 里的声音枚举。
 * 这里固定使用 mrc_base.h/mrporting.h 的 ABI 数值，保证 native 层解读一致。 */
enum {
    NATIVE_SOUND_MIDI = 0,
    NATIVE_SOUND_WAV = 1,
    NATIVE_SOUND_MP3 = 2,
    NATIVE_SOUND_PCM = 3,
    NATIVE_SOUND_M4A = 4,
    NATIVE_SOUND_AMR = 5,
    NATIVE_SOUND_AMR_WB = 6
};
#endif

static void native_test(void) {}

static void native_log(char *msg) {
    puts(msg ? msg : "(null)");
}

static void native_exit(void) {
    puts("mythroad exit.");
    exit(0);
}

static int32 native_mem_get(char **mem_base, uint32 *mem_len) {
    if (native_mem_base == NULL) {
#ifdef __linux__
#ifdef __x86_64__
        native_mem_base = mmap(NULL, NATIVE_DSM_MEM_SIZE, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
        if (native_mem_base == MAP_FAILED) native_mem_base = NULL;
#endif
#endif
        if (native_mem_base == NULL) {
            native_mem_base = malloc(NATIVE_DSM_MEM_SIZE);
        }
        if (native_mem_base == NULL) {
            return MR_FAILED;
        }
        native_mem_len = NATIVE_DSM_MEM_SIZE;
    }
    *mem_base = (char *)native_mem_base;
    *mem_len = native_mem_len;
    printf("native_mem_get base=%p len=%u(%u kb)\n", native_mem_base, native_mem_len, native_mem_len / 1024);
    return MR_SUCCESS;
}

static int32 native_mem_free(char *mem, uint32 mem_len) {
    (void)mem;
    (void)mem_len;
    return MR_SUCCESS;
}

static uint32 native_get_uptime_ms(void) {
    return (uint32)((uint64_t)get_uptime_ms() - native_uptime_base);
}

static int32 native_sleep(uint32 ms) {
    usleep(ms * 1000);
    return MR_SUCCESS;
}

static char *native_readdir(int32 f) {
    char *r = my_readdir(f);
    if (r == NULL) return NULL;
    strncpy(readdir_buf, r, sizeof(readdir_buf) - 1);
    readdir_buf[sizeof(readdir_buf) - 1] = '\0';
    return readdir_buf;
}

static int32 native_initNetwork(NETWORK_CB cb, const char *mode, void *userData) {
    return my_initNetwork(NULL, NULL, mode, NULL);
}

static int32 native_getHostByName(const char *ptr, NETWORK_CB cb, void *userData) {
    (void)ptr;
    (void)cb;
    (void)userData;
    return MR_FAILED;
}

#if defined(VMRP_SDL_AUDIO)
static uint16 read_be16(const uint8 *p) {
    return (uint16)(((uint16)p[0] << 8) | p[1]);
}

static uint32 read_be32(const uint8 *p) {
    return ((uint32)p[0] << 24) | ((uint32)p[1] << 16) | ((uint32)p[2] << 8) | p[3];
}

static int read_midi_vlq(const uint8 *data, uint32 len, uint32 *pos, uint32 *out) {
    uint32 value = 0;
    for (int i = 0; i < 4; i++) {
        if (*pos >= len) {
            return 0;
        }
        uint8 b = data[(*pos)++];
        value = (value << 7) | (uint32)(b & 0x7f);
        if ((b & 0x80) == 0) {
            *out = value;
            return 1;
        }
    }
    return 0;
}

static int midi_event_compare(const void *a, const void *b) {
    const MidiEvent *ea = (const MidiEvent *)a;
    const MidiEvent *eb = (const MidiEvent *)b;
    if (ea->tick < eb->tick) return -1;
    if (ea->tick > eb->tick) return 1;

    uint8 ta = ea->status & 0xf0;
    uint8 tb = eb->status & 0xf0;
    int ra = (ea->status == 0xff) ? 0 : (ta == 0xb0 || ta == 0xc0) ? 1 : (ta == 0x80) ? 2 : (ta == 0x90) ? 3 : 4;
    int rb = (eb->status == 0xff) ? 0 : (tb == 0xb0 || tb == 0xc0) ? 1 : (tb == 0x80) ? 2 : (tb == 0x90) ? 3 : 4;
    if (ra < rb) return -1;
    if (ra > rb) return 1;
    return (int)ea->status - (int)eb->status;
}

static float midi_note_freq(uint8 note) {
    return 440.0f * powf(2.0f, ((float)note - 69.0f) / 12.0f);
}

static float midi_program_wave(uint8 program, float phase) {
    float s = sinf(phase);
    if (program >= 32 && program <= 39) {
        return (s >= 0.0f) ? 0.75f : -0.75f;
    }
    if (program >= 24 && program <= 31) {
        return 0.70f * s + 0.22f * sinf(phase * 2.0f) + 0.08f * sinf(phase * 3.0f);
    }
    if (program >= 48 && program <= 63) {
        return 0.82f * s + 0.18f * sinf(phase * 2.0f);
    }
    return s;
}

static void midi_note_off(NativeAudioState *s, uint8 channel, uint8 note) {
    for (uint32 i = 0; i < sizeof(s->midi_voices) / sizeof(s->midi_voices[0]); i++) {
        MidiVoice *v = &s->midi_voices[i];
        if (v->active && !v->releasing && v->channel == channel && v->note == note) {
            v->releasing = 1;
            v->release = 1.0f;
        }
    }
}

static void midi_note_on(NativeAudioState *s, uint8 channel, uint8 note, uint8 velocity) {
    MidiVoice *slot = NULL;
    for (uint32 i = 0; i < sizeof(s->midi_voices) / sizeof(s->midi_voices[0]); i++) {
        if (!s->midi_voices[i].active) {
            slot = &s->midi_voices[i];
            break;
        }
    }
    if (!slot) {
        slot = &s->midi_voices[0];
        for (uint32 i = 1; i < sizeof(s->midi_voices) / sizeof(s->midi_voices[0]); i++) {
            if (s->midi_voices[i].velocity < slot->velocity) {
                slot = &s->midi_voices[i];
            }
        }
    }
    memset(slot, 0, sizeof(*slot));
    slot->active = 1;
    slot->note = note;
    slot->channel = channel;
    slot->program = s->midi_program[channel & 0x0f];
    slot->freq = midi_note_freq(note);
    slot->velocity = ((float)velocity / 127.0f) * s->midi_volume[channel & 0x0f];
    slot->release = 1.0f;
}

static void midi_apply_event(NativeAudioState *s, const MidiEvent *e) {
    if (e->status == 0xff) {
        s->midi_samples_per_tick = e->value ? e->value : 1;
        return;
    }
    uint8 status = e->status & 0xf0;
    uint8 channel = e->status & 0x0f;
    if (status == 0x80 || (status == 0x90 && e->b == 0)) {
        midi_note_off(s, channel, e->a);
    } else if (status == 0x90) {
        midi_note_on(s, channel, e->a, e->b);
    } else if (status == 0xb0) {
        if (e->a == 7) {
            s->midi_volume[channel] = (float)e->b / 127.0f;
        } else if (e->a == 10) {
            s->midi_pan[channel] = ((float)e->b - 64.0f) / 64.0f;
        }
    } else if (status == 0xc0) {
        s->midi_program[channel] = e->a;
    }
}

static void midi_reset_playback(NativeAudioState *s) {
    s->midi_event_pos = 0;
    s->midi_tick = 0;
    s->midi_sample_in_tick = 0;
    memset(s->midi_voices, 0, sizeof(s->midi_voices));
    for (int i = 0; i < 16; i++) {
        s->midi_program[i] = 0;
        s->midi_volume[i] = 1.0f;
        s->midi_pan[i] = 0.0f;
    }
}

static void midi_render(NativeAudioState *s, Sint16 *stream, int frames) {
    for (int i = 0; i < frames; i++) {
        while (s->midi_event_pos < s->midi_event_count &&
               s->midi_events[s->midi_event_pos].tick <= s->midi_tick) {
            midi_apply_event(s, &s->midi_events[s->midi_event_pos++]);
        }

        float left = 0.0f;
        float right = 0.0f;
        for (uint32 vi = 0; vi < sizeof(s->midi_voices) / sizeof(s->midi_voices[0]); vi++) {
            MidiVoice *v = &s->midi_voices[vi];
            if (!v->active) continue;

            float amp = v->velocity * v->release;
            float sample;
            if (v->channel == MIDI_DRUM_CHANNEL) {
                sample = ((v->phase < 3.14159265f) ? 1.0f : -1.0f) * amp;
                v->release *= 0.992f;
            } else {
                sample = midi_program_wave(v->program, v->phase) * amp;
                if (v->releasing) {
                    v->release *= 0.997f;
                }
            }

            float pan = s->midi_pan[v->channel & 0x0f];
            float lmul = (pan <= 0.0f) ? 1.0f : (1.0f - pan);
            float rmul = (pan >= 0.0f) ? 1.0f : (1.0f + pan);
            left += sample * lmul;
            right += sample * rmul;

            v->phase += 6.2831853f * v->freq / (float)AUDIO_SAMPLE_RATE;
            if (v->phase >= 6.2831853f) {
                v->phase -= 6.2831853f;
            }
            if (v->releasing && v->release < 0.005f) {
                v->active = 0;
            }
        }

        left *= MIDI_GAIN;
        right *= MIDI_GAIN;
        if (left > 1.0f) left = 1.0f;
        if (left < -1.0f) left = -1.0f;
        if (right > 1.0f) right = 1.0f;
        if (right < -1.0f) right = -1.0f;
        stream[i * 2] = (Sint16)(left * 32767.0f);
        stream[i * 2 + 1] = (Sint16)(right * 32767.0f);

        s->midi_sample_in_tick++;
        if (s->midi_sample_in_tick >= s->midi_samples_per_tick) {
            s->midi_sample_in_tick = 0;
            s->midi_tick++;
            if (s->midi_tick > s->midi_loop_ticks) {
                if (s->loop) {
                    midi_reset_playback(s);
                } else {
                    s->source = AUDIO_SOURCE_NONE;
                    memset(s->midi_voices, 0, sizeof(s->midi_voices));
                    break;
                }
            }
        }
    }
}

static void native_audio_callback(void *userdata, Uint8 *stream, int len) {
    NativeAudioState *s = (NativeAudioState *)userdata;
    memset(stream, 0, (size_t)len);
    if (s->source == AUDIO_SOURCE_PCM) {
        uint32 need = (uint32)len;
        uint8 *dst = stream;
        while (need > 0 && s->pcm_len > 0) {
            uint32 avail = s->pcm_len - s->pcm_pos;
            uint32 copy = (avail < need) ? avail : need;
            SDL_memcpy(dst, s->pcm + s->pcm_pos, copy);
            dst += copy;
            need -= copy;
            s->pcm_pos += copy;
            if (s->pcm_pos >= s->pcm_len) {
                if (s->loop) {
                    s->pcm_pos = 0;
                } else {
                    s->source = AUDIO_SOURCE_NONE;
                    break;
                }
            }
        }
    } else if (s->source == AUDIO_SOURCE_MIDI) {
        midi_render(s, (Sint16 *)stream, len / (int)(sizeof(Sint16) * AUDIO_CHANNELS));
    }
}

static int native_audio_open(void) {
    if (native_audio.device) {
        return MR_SUCCESS;
    }
    SDL_AudioSpec want;
    SDL_zero(want);
    want.freq = AUDIO_SAMPLE_RATE;
    want.format = AUDIO_S16SYS;
    want.channels = AUDIO_CHANNELS;
    want.samples = AUDIO_FRAMES;
    want.callback = native_audio_callback;
    want.userdata = &native_audio;
    native_audio.device = SDL_OpenAudioDevice(NULL, 0, &want, &native_audio.spec, 0);
    if (!native_audio.device) {
        printf("native_playSound: SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        return MR_FAILED;
    }
    SDL_PauseAudioDevice(native_audio.device, 0);
    return MR_SUCCESS;
}

static void native_audio_clear_locked(NativeAudioState *s) {
    s->source = AUDIO_SOURCE_NONE;
    free(s->pcm);
    s->pcm = NULL;
    s->pcm_len = 0;
    s->pcm_pos = 0;
    s->midi_event_count = 0;
    s->midi_event_pos = 0;
    s->midi_tick = 0;
    s->midi_sample_in_tick = 0;
    s->midi_loop_ticks = 0;
    memset(s->midi_voices, 0, sizeof(s->midi_voices));
}

static int native_audio_set_pcm(const void *data, uint32 dataLen, int32 loop,
                                SDL_AudioFormat format, int freq, int channels) {
    if (!data || dataLen == 0) {
        printf("native_playSound: empty PCM/WAV buffer\n");
        return MR_FAILED;
    }
    if (native_audio_open() != MR_SUCCESS) {
        return MR_FAILED;
    }

    SDL_AudioCVT cvt;
    if (SDL_BuildAudioCVT(&cvt, format, (Uint8)channels, freq,
                          native_audio.spec.format, native_audio.spec.channels,
                          native_audio.spec.freq) < 0) {
        printf("native_playSound: SDL_BuildAudioCVT failed: %s\n", SDL_GetError());
        return MR_FAILED;
    }
    cvt.len = (int)dataLen;
    cvt.buf = (Uint8 *)malloc((size_t)cvt.len * (size_t)cvt.len_mult);
    if (!cvt.buf) {
        return MR_FAILED;
    }
    memcpy(cvt.buf, data, dataLen);
    if (SDL_ConvertAudio(&cvt) < 0) {
        printf("native_playSound: SDL_ConvertAudio failed: %s\n", SDL_GetError());
        free(cvt.buf);
        return MR_FAILED;
    }

    SDL_LockAudioDevice(native_audio.device);
    native_audio_clear_locked(&native_audio);
    native_audio.pcm = cvt.buf;
    native_audio.pcm_len = (uint32)cvt.len_cvt;
    native_audio.pcm_pos = 0;
    native_audio.loop = loop ? 1 : 0;
    native_audio.source = AUDIO_SOURCE_PCM;
    SDL_UnlockAudioDevice(native_audio.device);
    return MR_SUCCESS;
}

static int native_audio_play_wav(const void *data, uint32 dataLen, int32 loop) {
    SDL_RWops *rw = SDL_RWFromConstMem(data, (int)dataLen);
    if (!rw) {
        printf("native_playSound: SDL_RWFromConstMem failed: %s\n", SDL_GetError());
        return MR_FAILED;
    }

    SDL_AudioSpec wav_spec;
    Uint8 *wav_buf = NULL;
    Uint32 wav_len = 0;
    if (!SDL_LoadWAV_RW(rw, 1, &wav_spec, &wav_buf, &wav_len)) {
        printf("native_playSound: SDL_LoadWAV_RW failed: %s\n", SDL_GetError());
        return MR_FAILED;
    }
    int ret = native_audio_set_pcm(wav_buf, wav_len, loop, wav_spec.format, wav_spec.freq, wav_spec.channels);
    SDL_FreeWAV(wav_buf);
    return ret;
}

static int midi_add_event(NativeAudioState *s, uint32 tick, uint8 status, uint8 a, uint8 b, uint32 value) {
    if (s->midi_event_count >= MIDI_MAX_EVENTS) {
        printf("native_playSound: MIDI event limit exceeded\n");
        return 0;
    }
    MidiEvent *e = &s->midi_events[s->midi_event_count++];
    e->tick = tick;
    e->value = value;
    e->status = status;
    e->a = a;
    e->b = b;
    if (tick > s->midi_loop_ticks) {
        s->midi_loop_ticks = tick;
    }
    return 1;
}

static int native_audio_parse_midi(NativeAudioState *s, const uint8 *data, uint32 dataLen) {
    if (dataLen < 14 || memcmp(data, "MThd", 4) != 0) {
        printf("native_playSound: MIDI data is not a standard SMF stream\n");
        return MR_FAILED;
    }
    uint32 header_len = read_be32(data + 4);
    if (header_len < 6 || dataLen < 8 + header_len) {
        printf("native_playSound: invalid MIDI header length\n");
        return MR_FAILED;
    }
    uint16 track_count = read_be16(data + 10);
    uint16 division = read_be16(data + 12);
    if (track_count == 0 || track_count > MIDI_MAX_TRACKS || (division & 0x8000)) {
        printf("native_playSound: unsupported MIDI timing or track count\n");
        return MR_FAILED;
    }

    s->midi_event_count = 0;
    s->midi_loop_ticks = 0;
    uint32 note_on_count = 0;
    uint32 default_samples_per_tick = (uint32)(((uint64_t)MIDI_DEFAULT_TEMPO_US * AUDIO_SAMPLE_RATE) /
                                               ((uint64_t)division * 1000000ULL));
    if (!midi_add_event(s, 0, 0xff, 0, 0, default_samples_per_tick ? default_samples_per_tick : 1)) {
        return MR_FAILED;
    }
    uint32 pos = 8 + header_len;
    for (uint16 track = 0; track < track_count; track++) {
        if (pos + 8 > dataLen || memcmp(data + pos, "MTrk", 4) != 0) {
            printf("native_playSound: invalid MIDI track header\n");
            return MR_FAILED;
        }
        uint32 track_len = read_be32(data + pos + 4);
        pos += 8;
        if (track_len > dataLen - pos) {
            printf("native_playSound: truncated MIDI track\n");
            return MR_FAILED;
        }
        uint32 end = pos + track_len;
        uint32 tick = 0;
        uint8 running_status = 0;
        while (pos < end) {
            uint32 delta = 0;
            if (!read_midi_vlq(data, end, &pos, &delta)) {
                printf("native_playSound: invalid MIDI delta time\n");
                return MR_FAILED;
            }
            tick += delta;
            if (tick > s->midi_loop_ticks) {
                s->midi_loop_ticks = tick;
            }

            uint8 b = data[pos];
            uint8 status;
            if (b & 0x80) {
                status = b;
                pos++;
                if (status < 0xf0) {
                    running_status = status;
                }
            } else {
                if (!running_status) {
                    printf("native_playSound: missing MIDI running status\n");
                    return MR_FAILED;
                }
                status = running_status;
            }

            if (status == 0xff) {
                if (pos >= end) return MR_FAILED;
                uint8 meta = data[pos++];
                uint32 meta_len = 0;
                if (!read_midi_vlq(data, end, &pos, &meta_len) || meta_len > end - pos) {
                    printf("native_playSound: invalid MIDI meta event\n");
                    return MR_FAILED;
                }
                if (meta == 0x51 && meta_len == 3) {
                    uint32 tempo_us = ((uint32)data[pos] << 16) | ((uint32)data[pos + 1] << 8) | data[pos + 2];
                    uint32 samples_per_tick = (uint32)(((uint64_t)tempo_us * AUDIO_SAMPLE_RATE) /
                                                       ((uint64_t)division * 1000000ULL));
                    if (!midi_add_event(s, tick, 0xff, 0, 0, samples_per_tick ? samples_per_tick : 1)) {
                        return MR_FAILED;
                    }
                }
                pos += meta_len;
            } else if (status == 0xf0 || status == 0xf7) {
                uint32 sysex_len = 0;
                if (!read_midi_vlq(data, end, &pos, &sysex_len) || sysex_len > end - pos) {
                    printf("native_playSound: invalid MIDI sysex event\n");
                    return MR_FAILED;
                }
                pos += sysex_len;
            } else {
                uint8 type = status & 0xf0;
                uint8 a = 0;
                uint8 c = 0;
                if (pos >= end) return MR_FAILED;
                a = data[pos++];
                if (type != 0xc0 && type != 0xd0) {
                    if (pos >= end) return MR_FAILED;
                    c = data[pos++];
                }
                if (type == 0x80 || type == 0x90 || type == 0xb0 || type == 0xc0) {
                    if (type == 0x90 && c != 0) {
                        note_on_count++;
                    }
                    if (!midi_add_event(s, tick, status, a, c, 0)) {
                        return MR_FAILED;
                    }
                }
            }
        }
        pos = end;
    }

    if (note_on_count == 0) {
        printf("native_playSound: MIDI has no playable note events\n");
        return MR_FAILED;
    }
    qsort(s->midi_events, s->midi_event_count, sizeof(s->midi_events[0]), midi_event_compare);
    s->midi_samples_per_tick = default_samples_per_tick ? default_samples_per_tick : 1;
    return MR_SUCCESS;
}

static int native_audio_play_midi(const void *data, uint32 dataLen, int32 loop) {
    if (native_audio_open() != MR_SUCCESS) {
        return MR_FAILED;
    }
    SDL_LockAudioDevice(native_audio.device);
    native_audio_clear_locked(&native_audio);
    int ret = native_audio_parse_midi(&native_audio, (const uint8 *)data, dataLen);
    if (ret == MR_SUCCESS) {
        native_audio.loop = loop ? 1 : 0;
        midi_reset_playback(&native_audio);
        native_audio.source = AUDIO_SOURCE_MIDI;
    }
    SDL_UnlockAudioDevice(native_audio.device);
    return ret;
}
#endif

static int32 native_playSound(int type, const void *data, uint32 dataLen, int32 loop) {
#if defined(VMRP_SDL_AUDIO)
    /* Mythroad 传入的是完整内存音频流：gxdzc 的“是否开启音乐？”确认路径
     * 实测为 MR_SOUND_MIDI + 标准 SMF；MR_SOUND_PCM 按 mrc_base.h 标注处理为
     * 8KHz/16bit/mono。未实现的压缩格式返回失败，避免旧 stub 那种无声成功。 */
    switch (type) {
        case NATIVE_SOUND_MIDI:
            return native_audio_play_midi(data, dataLen, loop);
        case NATIVE_SOUND_WAV:
            return native_audio_play_wav(data, dataLen, loop);
        case NATIVE_SOUND_PCM:
            return native_audio_set_pcm(data, dataLen, loop, AUDIO_S16SYS, 8000, 1);
        case NATIVE_SOUND_MP3:
        case NATIVE_SOUND_M4A:
        case NATIVE_SOUND_AMR:
        case NATIVE_SOUND_AMR_WB:
            printf("native_playSound: unsupported compressed sound type %d len=%u\n", type, dataLen);
            return MR_FAILED;
        default:
            printf("native_playSound: unknown sound type %d len=%u\n", type, dataLen);
            return MR_FAILED;
    }
#else
    (void)data;
    (void)dataLen;
    (void)loop;
    printf("native_playSound: SDL audio is not enabled for this build, type=%d\n", type);
    return MR_FAILED;
#endif
}

static int32 native_stopSound(int type) {
    (void)type;
#if defined(VMRP_SDL_AUDIO)
    if (native_audio.device) {
        SDL_LockAudioDevice(native_audio.device);
        native_audio_clear_locked(&native_audio);
        SDL_UnlockAudioDevice(native_audio.device);
    }
#endif
    return MR_SUCCESS;
}

static int32 native_startShake(int32 ms) {
    (void)ms;
    return MR_SUCCESS;
}

static int32 native_stopShake(void) {
    return MR_SUCCESS;
}

static int32 native_dialogCreate(const char *title, const char *text, int32 type) {
    (void)title;
    (void)text;
    (void)type;
    return MR_FAILED;
}

static int32 native_dialogRelease(int32 dialog) {
    (void)dialog;
    return MR_FAILED;
}

static int32 native_dialogRefresh(int32 dialog, const char *title, const char *text, int32 type) {
    (void)dialog;
    (void)title;
    (void)text;
    (void)type;
    return MR_FAILED;
}

static int32 native_textCreate(const char *title, const char *text, int32 type) {
    (void)title;
    (void)text;
    (void)type;
    return MR_FAILED;
}

static int32 native_textRelease(int32 text) {
    (void)text;
    return MR_FAILED;
}

static int32 native_textRefresh(int32 handle, const char *title, const char *text) {
    (void)handle;
    (void)title;
    (void)text;
    return MR_FAILED;
}

#if defined(__EMSCRIPTEN__)
#define NATIVE_DSM_FLAGS FLAG_USE_UTF8_FS
#elif defined(_WIN32)
#define NATIVE_DSM_FLAGS FLAG_USE_UTF8_EDIT
#else
#define NATIVE_DSM_FLAGS (FLAG_USE_UTF8_FS | FLAG_USE_UTF8_EDIT)
#endif

static DSM_REQUIRE_FUNCS native_funcs = {
    .test = native_test,
    .log = native_log,
    .exit = native_exit,
    .srand = srand,
    .rand = rand,
    .mem_get = native_mem_get,
    .mem_free = native_mem_free,
    .timerStart = timerStart,
    .timerStop = timerStop,
    .get_uptime_ms = native_get_uptime_ms,
    .getDatetime = getDatetime,
    .sleep = native_sleep,
    .open = my_open,
    .close = my_close,
    .read = my_read,
    .write = my_write,
    .seek = my_seek,
    .info = my_info,
    .remove = my_remove,
    .rename = my_rename,
    .mkDir = my_mkDir,
    .rmDir = my_rmDir,
    .opendir = my_opendir,
    .readdir = native_readdir,
    .closedir = my_closedir,
    .getLen = my_getLen,
    .drawBitmap = guiDrawBitmap,
    .getHostByName = native_getHostByName,
    .initNetwork = native_initNetwork,
    .mr_closeNetwork = my_closeNetwork,
    .mr_socket = my_socket,
    .mr_connect = my_connect,
    .mr_getSocketState = my_getSocketState,
    .mr_closeSocket = my_closeSocket,
    .mr_recv = my_recv,
    .mr_send = my_send,
    .mr_recvfrom = my_recvfrom,
    .mr_sendto = my_sendto,
    .mr_startShake = native_startShake,
    .mr_stopShake = native_stopShake,
    .mr_playSound = native_playSound,
    .mr_stopSound = native_stopSound,
    .mr_dialogCreate = native_dialogCreate,
    .mr_dialogRelease = native_dialogRelease,
    .mr_dialogRefresh = native_dialogRefresh,
    .mr_textCreate = native_textCreate,
    .mr_textRelease = native_textRelease,
    .mr_textRefresh = native_textRefresh,
    .mr_editCreate = editCreate,
    .mr_editRelease = editRelease,
    .mr_editGetText = editGetText,
    .flags = NATIVE_DSM_FLAGS,
    .screen_width = 0,
    .screen_height = 0,
};

DSM_REQUIRE_FUNCS *native_dsm_funcs_get(void) {
    native_uptime_base = (uint64_t)get_uptime_ms();
    native_funcs.screen_width = vmrp_config.screen_width;
    native_funcs.screen_height = vmrp_config.screen_height;
    return &native_funcs;
}

void native_dsm_funcs_destroy(void) {
#if defined(VMRP_SDL_AUDIO)
    if (native_audio.device) {
        SDL_CloseAudioDevice(native_audio.device);
        native_audio.device = 0;
    }
    native_audio_clear_locked(&native_audio);
#endif
#ifdef __linux__
#ifdef __x86_64__
    if (native_mem_base != NULL) {
        uintptr_t p = (uintptr_t)native_mem_base;
        if (p < 0x80000000ULL) {
            munmap(native_mem_base, native_mem_len);
            native_mem_base = NULL;
            native_mem_len = 0;
            return;
        }
    }
#endif
#endif
    free(native_mem_base);
    native_mem_base = NULL;
    native_mem_len = 0;
}

int32_t bridge_dsm_network_cb(uc_engine *uc, uint32_t addr, int32_t p0, uint32_t p1) {
    (void)uc;
    int32 (*cb)(int32, void *) = (int32 (*)(int32, void *))(uintptr_t)addr;
    return cb ? cb(p0, (void *)(uintptr_t)p1) : MR_FAILED;
}
