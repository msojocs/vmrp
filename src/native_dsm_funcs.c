#include "./include/native_dsm_funcs.h"
#include "./include/compat_msvc.h"

#include <errno.h>
#include <math.h>
#include <stdint.h>
#ifdef _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
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
#include "./include/file_lib.h"
#include "./include/native_text_widget.h"
#include "./include/network.h"
#include "./include/utils.h"
#include "./include/skyengine.h"

/* MP3 解码器(vendored 单头文件, 见 third_party/minimp3/README.md)。
 * talkcat 等应用的音效/音乐是 MPEG1 Layer III 流, 之前 native_playSound
 * 对 MR_SOUND_MP3 直接返回 MR_FAILED 导致启动音完全无声。
 * MINIMP3_ONLY_MP3 去掉 Layer I/II 支持以减小体积——MRP 样本均为 Layer III。 */
#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#include "../third_party/minimp3/minimp3.h"

static void *native_mem_base;
static uint32 native_mem_len;
static uint64_t native_uptime_base;
static char readdir_buf[128];

#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNELS 2
#define AUDIO_FRAMES 1024
#define AUDIO_BYTES_PER_FRAME ((int)(sizeof(int16_t) * AUDIO_CHANNELS))
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
    int32 handle;
    uint8 *pcm;
    uint32 pcm_len;
    uint32 pcm_pos;
    int loop;
} NativeAudioChannelVoice;

typedef struct {
#if defined(VMRP_SDL_AUDIO)
    SDL_AudioDeviceID device;
#endif
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
    NativeAudioChannelVoice *channel_voices;
    uint32 channel_voice_count;
    uint32 channel_voice_capacity;
} NativeAudioState;

static NativeAudioState native_audio;
#if !defined(VMRP_SDL_AUDIO) && defined(_MSC_VER)
static CRITICAL_SECTION native_audio_mutex;
static INIT_ONCE native_audio_init_once = INIT_ONCE_STATIC_INIT;
#elif !defined(VMRP_SDL_AUDIO)
static pthread_mutex_t native_audio_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#if !defined(VMRP_SDL_AUDIO) && defined(_MSC_VER)
static BOOL CALLBACK native_audio_init_lock(PINIT_ONCE init_once, PVOID parameter, PVOID *context) {
    (void)init_once;
    (void)parameter;
    (void)context;
    InitializeCriticalSection(&native_audio_mutex);
    return TRUE;
}
#endif

/* 宿主侧 dsm.h 只暴露平台函数表，不带 mrporting.h 里的声音枚举。
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

static void native_test(void) {}

static void native_log(char *msg) {
    puts(msg ? msg : "(null)");
}

static void native_exit(void) {
    puts("mythroad exit.");
    skyengine_request_exit();
}

static int32 native_mem_get(char **mem_base, uint32 *mem_len) {
    if (native_mem_base == NULL) {
        /* 应用可见内存由 --memory/SKYENGINE_MEMORY 配置(1M-16M),历史默认
         * 4MB 改为跟随配置;非 EXT 应用(lua/mini mythroad)拿到的即为
         * 全部运行内存。 */
        uint32 mem_size = SKYENGINE_MEMORY_bytes(skyengine_config.memory_mb);
#ifdef __linux__
#ifdef __x86_64__
        native_mem_base = mmap(NULL, mem_size, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
        if (native_mem_base == MAP_FAILED) native_mem_base = NULL;
#endif
#endif
        if (native_mem_base == NULL) {
            native_mem_base = malloc(mem_size);
        }
        if (native_mem_base == NULL) {
            return MR_FAILED;
        }
        native_mem_len = mem_size;
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
#ifdef _MSC_VER
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
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
    /* native 路径同步初始化,不使用回调;签名由 dsm 函数表 ABI 固定 */
    (void)cb;
    (void)userData;
    return my_initNetwork(NULL, NULL, mode, NULL);
}

static int32 native_getHostByName(const char *ptr, NETWORK_CB cb, void *userData) {
    /* NETWORK_CB(int,void*) 与形参类型 MR_GET_HOST_CB(int32) 原型不同,历史上
     * 直接强转传递;下游 bridge_dsm_network_cb 最终仍按 int32(*)(int32,void*)
     * 即 NETWORK_CB 原型回调,所以这里的强转只是类型转运。经 void(*)(void)
     * 中转仅消除 -Wcast-function-type,行为与原先完全一致 */
    return my_getHostByName(NULL, ptr, (MR_GET_HOST_CB)(void (*)(void))cb, userData);
}

static int32 native_timerStop(void) {
    return timerStop();
}

static uint16 read_be16(const uint8 *p) {
    return (uint16)(((uint16)p[0] << 8) | p[1]);
}

static uint32 read_be32(const uint8 *p) {
    return ((uint32)p[0] << 24) | ((uint32)p[1] << 16) | ((uint32)p[2] << 8) | p[3];
}

static uint16 read_le16(const uint8 *p) {
    return (uint16)(p[0] | ((uint16)p[1] << 8));
}

static uint32 read_le32(const uint8 *p) {
    return (uint32)p[0] | ((uint32)p[1] << 8) | ((uint32)p[2] << 16) | ((uint32)p[3] << 24);
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

static int16_t audio_float_to_s16(float sample) {
    if (sample > 1.0f) sample = 1.0f;
    if (sample < -1.0f) sample = -1.0f;
    return (int16_t)(sample * 32767.0f);
}

static void midi_render(NativeAudioState *s, int16_t *stream, int frames) {
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
        stream[i * 2] = audio_float_to_s16(left);
        stream[i * 2 + 1] = audio_float_to_s16(right);

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

static void native_audio_clear_legacy_locked(NativeAudioState *s) {
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

static void native_audio_channel_remove_locked(NativeAudioState *s,
                                               uint32 index) {
    if (index >= s->channel_voice_count) return;
    free(s->channel_voices[index].pcm);
    if (index + 1 < s->channel_voice_count) {
        memmove(&s->channel_voices[index], &s->channel_voices[index + 1],
                (size_t)(s->channel_voice_count - index - 1) *
                    sizeof(s->channel_voices[0]));
    }
    s->channel_voice_count--;
}

static void native_audio_clear_channels_locked(NativeAudioState *s) {
    for (uint32 i = 0; i < s->channel_voice_count; i++) {
        free(s->channel_voices[i].pcm);
    }
    free(s->channel_voices);
    s->channel_voices = NULL;
    s->channel_voice_count = 0;
    s->channel_voice_capacity = 0;
}

static void native_audio_clear_all_locked(NativeAudioState *s) {
    native_audio_clear_legacy_locked(s);
    native_audio_clear_channels_locked(s);
}

static void native_audio_lock(void) {
#if defined(VMRP_SDL_AUDIO)
    if (native_audio.device) {
        SDL_LockAudioDevice(native_audio.device);
    }
#elif defined(_MSC_VER)
    InitOnceExecuteOnce(&native_audio_init_once, native_audio_init_lock, NULL, NULL);
    EnterCriticalSection(&native_audio_mutex);
#elif !defined(_MSC_VER)
    pthread_mutex_lock(&native_audio_mutex);
#endif
}

static void native_audio_unlock(void) {
#if defined(VMRP_SDL_AUDIO)
    if (native_audio.device) {
        SDL_UnlockAudioDevice(native_audio.device);
    }
#elif defined(_MSC_VER)
    LeaveCriticalSection(&native_audio_mutex);
#elif !defined(_MSC_VER)
    pthread_mutex_unlock(&native_audio_mutex);
#endif
}

static int16_t native_audio_saturating_add_s16(int16_t a, int16_t b) {
    int32_t mixed = (int32_t)a + (int32_t)b;
    if (mixed > INT16_MAX) return INT16_MAX;
    if (mixed < INT16_MIN) return INT16_MIN;
    return (int16_t)mixed;
}

static void native_audio_mix_channels_locked(NativeAudioState *s,
                                             int16_t *stream, int frames) {
    uint32 vi = 0;
    while (vi < s->channel_voice_count) {
        NativeAudioChannelVoice *voice = &s->channel_voices[vi];
        int finished = 0;
        for (int frame = 0; frame < frames; frame++) {
            if (voice->pcm_pos >= voice->pcm_len) {
                if (voice->loop) {
                    voice->pcm_pos = 0;
                } else {
                    finished = 1;
                    break;
                }
            }

            const int16_t *src = (const int16_t *)(voice->pcm + voice->pcm_pos);
            stream[frame * AUDIO_CHANNELS] = native_audio_saturating_add_s16(
                stream[frame * AUDIO_CHANNELS], src[0]);
            stream[frame * AUDIO_CHANNELS + 1] = native_audio_saturating_add_s16(
                stream[frame * AUDIO_CHANNELS + 1], src[1]);
            voice->pcm_pos += AUDIO_BYTES_PER_FRAME;
        }

        if (!voice->loop && voice->pcm_pos >= voice->pcm_len) {
            finished = 1;
        }
        if (finished) {
            native_audio_channel_remove_locked(s, vi);
        } else {
            vi++;
        }
    }
}

static int native_audio_render_bytes(NativeAudioState *s, uint8 *stream, int len) {
    if (!stream || len <= 0) return 0;
    memset(stream, 0, (size_t)len);

    int frames = len / AUDIO_BYTES_PER_FRAME;
    if (frames <= 0) return 0;

    NativeAudioSource source = s->source;
    int had_audio = source != AUDIO_SOURCE_NONE || s->channel_voice_count != 0;
    if (source == AUDIO_SOURCE_PCM) {
        uint32 need = (uint32)(frames * AUDIO_BYTES_PER_FRAME);
        uint8 *dst = stream;
        while (need > 0 && s->pcm_len > 0) {
            uint32 avail = s->pcm_len - s->pcm_pos;
            uint32 copy = (avail < need) ? avail : need;
            memcpy(dst, s->pcm + s->pcm_pos, copy);
            dst += copy;
            need -= copy;
            s->pcm_pos += copy;
            if (s->pcm_pos >= s->pcm_len) {
                if (s->loop) {
                    s->pcm_pos = 0;
                } else {
                    native_audio_clear_legacy_locked(s);
                    break;
                }
            }
        }
    } else if (source == AUDIO_SOURCE_MIDI) {
        midi_render(s, (int16_t *)stream, frames);
    }

    native_audio_mix_channels_locked(s, (int16_t *)stream, frames);

    return had_audio ? frames : 0;
}

#if defined(VMRP_SDL_AUDIO)
static void native_audio_callback(void *userdata, Uint8 *stream, int len) {
    native_audio_render_bytes((NativeAudioState *)userdata, stream, len);
}
#endif

static int native_audio_start_output(void) {
#if defined(VMRP_SDL_AUDIO)
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
    native_audio.device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
    if (!native_audio.device) {
        printf("native_playSound: SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        return MR_FAILED;
    }
    SDL_PauseAudioDevice(native_audio.device, 0);
#endif
    return MR_SUCCESS;
}

static int native_audio_take_pcm(uint8 *pcm, uint32 pcm_len, int32 loop) {
    if (!pcm || pcm_len == 0) {
        free(pcm);
        return MR_FAILED;
    }
    if (native_audio_start_output() != MR_SUCCESS) {
        free(pcm);
        return MR_FAILED;
    }

    native_audio_lock();
    native_audio_clear_legacy_locked(&native_audio);
    native_audio.pcm = pcm;
    native_audio.pcm_len = pcm_len;
    native_audio.pcm_pos = 0;
    native_audio.loop = loop ? 1 : 0;
    native_audio.source = AUDIO_SOURCE_PCM;
    native_audio_unlock();
    return MR_SUCCESS;
}

static int native_audio_pcm_format_supported(int format, int bits_per_sample) {
    if (format == 1) {
        return bits_per_sample == 8 || bits_per_sample == 16 ||
               bits_per_sample == 24 || bits_per_sample == 32;
    }
    if (format == 3) {
        return bits_per_sample == 32;
    }
    return 0;
}

static int16_t native_audio_read_sample_s16(const uint8 *p, int format, int bits_per_sample) {
    if (format == 3 && bits_per_sample == 32) {
        float value = 0.0f;
        memcpy(&value, p, sizeof(value));
        return audio_float_to_s16(value);
    }

    switch (bits_per_sample) {
        case 8:
            return (int16_t)(((int)p[0] - 128) << 8);
        case 16:
            return (int16_t)read_le16(p);
        case 24: {
            int32_t value = (int32_t)p[0] | ((int32_t)p[1] << 8) | ((int32_t)p[2] << 16);
            if (value & 0x800000) value |= ~0xffffff;
            return (int16_t)(value >> 8);
        }
        case 32:
            return (int16_t)((int32_t)read_le32(p) >> 16);
        default:
            return 0;
    }
}

static int native_audio_convert_pcm(const void *data, uint32 dataLen,
                                    int freq, int channels,
                                    int bits_per_sample, int format,
                                    int block_align, uint8 **out_pcm,
                                    uint32 *out_pcm_len) {
    if (out_pcm) *out_pcm = NULL;
    if (out_pcm_len) *out_pcm_len = 0;
    if (!data || dataLen == 0 || freq <= 0 || channels <= 0 || channels > 8 ||
        !out_pcm || !out_pcm_len ||
        !native_audio_pcm_format_supported(format, bits_per_sample)) {
        printf("native_playSound: unsupported PCM/WAV format fmt=%d hz=%d ch=%d bits=%d len=%u\n",
               format, freq, channels, bits_per_sample, dataLen);
        return MR_FAILED;
    }

    int bytes_per_sample = (bits_per_sample + 7) / 8;
    if (block_align <= 0) {
        block_align = bytes_per_sample * channels;
    }
    if (block_align < bytes_per_sample * channels) {
        printf("native_playSound: invalid PCM block align %d\n", block_align);
        return MR_FAILED;
    }

    uint32 src_frames = dataLen / (uint32)block_align;
    if (src_frames == 0) {
        printf("native_playSound: empty PCM/WAV frame buffer\n");
        return MR_FAILED;
    }

    uint64_t out_frames64 = ((uint64_t)src_frames * AUDIO_SAMPLE_RATE + (uint32)freq - 1) / (uint32)freq;
    if (out_frames64 == 0 || out_frames64 > UINT32_MAX / AUDIO_BYTES_PER_FRAME) {
        printf("native_playSound: converted PCM is too large\n");
        return MR_FAILED;
    }

    uint32 out_frames = (uint32)out_frames64;
    uint32 out_len = out_frames * AUDIO_BYTES_PER_FRAME;
    uint8 *out = (uint8 *)malloc(out_len);
    if (!out) return MR_FAILED;

    const uint8 *src = (const uint8 *)data;
    int16_t *dst = (int16_t *)out;
    for (uint32 i = 0; i < out_frames; i++) {
        uint64_t src_index64 = ((uint64_t)i * (uint32)freq) / AUDIO_SAMPLE_RATE;
        if (src_index64 >= src_frames) src_index64 = src_frames - 1;
        const uint8 *frame = src + (uint32)src_index64 * (uint32)block_align;
        int16_t left = native_audio_read_sample_s16(frame, format, bits_per_sample);
        int16_t right = channels > 1
            ? native_audio_read_sample_s16(frame + bytes_per_sample, format, bits_per_sample)
            : left;
        dst[i * AUDIO_CHANNELS] = left;
        dst[i * AUDIO_CHANNELS + 1] = right;
    }

    *out_pcm = out;
    *out_pcm_len = out_len;
    return MR_SUCCESS;
}

static int native_audio_set_pcm(const void *data, uint32 dataLen, int32 loop,
                                int freq, int channels, int bits_per_sample,
                                int format, int block_align) {
    uint8 *pcm = NULL;
    uint32 pcm_len = 0;
    int ret = native_audio_convert_pcm(data, dataLen, freq, channels,
                                       bits_per_sample, format, block_align,
                                       &pcm, &pcm_len);
    if (ret != MR_SUCCESS) return ret;
    return native_audio_take_pcm(pcm, pcm_len, loop);
}

typedef struct {
    int format;
    int channels;
    int sample_rate;
    int bits_per_sample;
    int block_align;
    int samples_per_block;
} NativeWavFormat;

static const int native_ima_index_table[16] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
};

static const int native_ima_step_table[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31,
    34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143,
    157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494,
    544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552,
    1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026,
    4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442,
    11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623,
    27086, 29794, 32767
};

static int16_t native_ima_decode_nibble(int nibble, int *predictor,
                                        int *step_index) {
    int step = native_ima_step_table[*step_index];
    int diff = step >> 3;
    if (nibble & 1) diff += step >> 2;
    if (nibble & 2) diff += step >> 1;
    if (nibble & 4) diff += step;
    if (nibble & 8) {
        *predictor -= diff;
    } else {
        *predictor += diff;
    }
    if (*predictor > INT16_MAX) *predictor = INT16_MAX;
    if (*predictor < INT16_MIN) *predictor = INT16_MIN;

    *step_index += native_ima_index_table[nibble & 0x0f];
    if (*step_index < 0) *step_index = 0;
    if (*step_index > 88) *step_index = 88;
    return (int16_t)*predictor;
}

static void native_audio_write_s16le(uint8 *dst, int16_t sample) {
    uint16_t value = (uint16_t)sample;
    dst[0] = (uint8)(value & 0xffu);
    dst[1] = (uint8)(value >> 8);
}

static int native_audio_decode_ima_adpcm(const uint8 *data, uint32 data_len,
                                         const NativeWavFormat *fmt,
                                         uint8 **out_pcm,
                                         uint32 *out_pcm_len) {
    if (out_pcm) *out_pcm = NULL;
    if (out_pcm_len) *out_pcm_len = 0;
    if (!data || !fmt || !out_pcm || !out_pcm_len ||
        (fmt->channels != 1 && fmt->channels != 2) ||
        fmt->bits_per_sample != 4 || fmt->block_align <= fmt->channels * 4 ||
        fmt->samples_per_block <= 1) {
        printf("native_playSound: invalid IMA ADPCM format ch=%d bits=%d align=%d samples=%d\n",
               fmt ? fmt->channels : 0, fmt ? fmt->bits_per_sample : 0,
               fmt ? fmt->block_align : 0,
               fmt ? fmt->samples_per_block : 0);
        return MR_FAILED;
    }

    uint32 block_align = (uint32)fmt->block_align;
    uint32 header_bytes = (uint32)fmt->channels * 4u;
    uint32 payload_bytes = block_align - header_bytes;
    if ((fmt->channels == 2 && (payload_bytes & 7u) != 0) ||
        payload_bytes > (UINT32_MAX - 1u) / 2u) {
        printf("native_playSound: invalid IMA ADPCM block layout align=%u ch=%d\n",
               block_align, fmt->channels);
        return MR_FAILED;
    }

    uint32 expected_samples = 1u +
        (payload_bytes * 2u) / (uint32)fmt->channels;
    if ((uint32)fmt->samples_per_block != expected_samples) {
        printf("native_playSound: invalid IMA ADPCM samples/block got=%d expected=%u\n",
               fmt->samples_per_block, expected_samples);
        return MR_FAILED;
    }
    if (data_len == 0 || data_len % block_align != 0) {
        printf("native_playSound: truncated IMA ADPCM block data len=%u align=%u\n",
               data_len, block_align);
        return MR_FAILED;
    }

    uint32 block_count = data_len / block_align;
    uint64_t sample_values = (uint64_t)block_count * expected_samples *
                             (uint32)fmt->channels;
    if (sample_values == 0 || sample_values > UINT32_MAX / sizeof(int16_t)) {
        printf("native_playSound: decoded IMA ADPCM is too large\n");
        return MR_FAILED;
    }

    uint32 decoded_len = (uint32)sample_values * (uint32)sizeof(int16_t);
    uint8 *decoded = (uint8 *)malloc(decoded_len);
    if (!decoded) return MR_FAILED;

    for (uint32 bi = 0; bi < block_count; bi++) {
        const uint8 *block = data + bi * block_align;
        uint32 base_frame = bi * expected_samples;
        int predictor[2] = {0, 0};
        int step_index[2] = {0, 0};
        for (int channel = 0; channel < fmt->channels; channel++) {
            const uint8 *header = block + channel * 4;
            predictor[channel] = (int16_t)read_le16(header);
            step_index[channel] = header[2];
            if (step_index[channel] > 88 || header[3] != 0) {
                free(decoded);
                printf("native_playSound: invalid IMA ADPCM block header index=%d reserved=%u\n",
                       step_index[channel], (unsigned)header[3]);
                return MR_FAILED;
            }
            uint32 sample_index = (base_frame * (uint32)fmt->channels) +
                                  (uint32)channel;
            native_audio_write_s16le(decoded + sample_index * 2u,
                                     (int16_t)predictor[channel]);
        }

        const uint8 *encoded = block + header_bytes;
        if (fmt->channels == 1) {
            uint32 frame = 1;
            for (uint32 i = 0; i < payload_bytes; i++) {
                uint8 packed = encoded[i];
                int16_t low = native_ima_decode_nibble(
                    packed & 0x0f, &predictor[0], &step_index[0]);
                native_audio_write_s16le(
                    decoded + (base_frame + frame++) * 2u, low);
                int16_t high = native_ima_decode_nibble(
                    packed >> 4, &predictor[0], &step_index[0]);
                native_audio_write_s16le(
                    decoded + (base_frame + frame++) * 2u, high);
            }
        } else {
            uint32 frame = 1;
            uint32 encoded_pos = 0;
            while (encoded_pos < payload_bytes) {
                for (int channel = 0; channel < 2; channel++) {
                    for (uint32 i = 0; i < 4; i++) {
                        uint8 packed = encoded[encoded_pos++];
                        int16_t low = native_ima_decode_nibble(
                            packed & 0x0f, &predictor[channel],
                            &step_index[channel]);
                        uint32 low_index = ((base_frame + frame + i * 2u) * 2u) +
                                           (uint32)channel;
                        native_audio_write_s16le(decoded + low_index * 2u, low);
                        int16_t high = native_ima_decode_nibble(
                            packed >> 4, &predictor[channel],
                            &step_index[channel]);
                        native_audio_write_s16le(
                            decoded + (low_index + 2u) * 2u, high);
                    }
                }
                frame += 8;
            }
        }
    }

    *out_pcm = decoded;
    *out_pcm_len = decoded_len;
    return MR_SUCCESS;
}

static int native_audio_parse_wav(const void *data, uint32 dataLen,
                                  NativeWavFormat *fmt,
                                  const uint8 **audio_data,
                                  uint32 *audio_len) {
    const uint8 *bytes = (const uint8 *)data;
    if (!fmt || !audio_data || !audio_len) return MR_FAILED;
    memset(fmt, 0, sizeof(*fmt));
    *audio_data = NULL;
    *audio_len = 0;
    if (!bytes || dataLen < 12 || memcmp(bytes, "RIFF", 4) != 0 ||
        memcmp(bytes + 8, "WAVE", 4) != 0) {
        printf("native_playSound: WAV data is not RIFF/WAVE\n");
        return MR_FAILED;
    }

    uint32 riff_size = read_le32(bytes + 4);
    if (riff_size < 4 || riff_size > dataLen - 8u) {
        printf("native_playSound: truncated WAV RIFF container\n");
        return MR_FAILED;
    }
    uint32 riff_end = riff_size + 8u;
    int have_fmt = 0;

    uint32 pos = 12;
    while (pos < riff_end) {
        if (riff_end - pos < 8u) {
            printf("native_playSound: truncated WAV chunk header\n");
            return MR_FAILED;
        }
        const uint8 *chunk = bytes + pos;
        uint32 chunk_len = read_le32(chunk + 4);
        pos += 8;
        if (chunk_len > riff_end - pos) {
            printf("native_playSound: truncated WAV chunk\n");
            return MR_FAILED;
        }

        if (memcmp(chunk, "fmt ", 4) == 0) {
            if (have_fmt || chunk_len < 16) {
                printf("native_playSound: invalid WAV fmt chunk\n");
                return MR_FAILED;
            }
            fmt->format = (int)read_le16(bytes + pos);
            fmt->channels = (int)read_le16(bytes + pos + 2);
            uint32 sample_rate = read_le32(bytes + pos + 4);
            if (sample_rate > INT32_MAX) {
                printf("native_playSound: WAV sample rate is too large\n");
                return MR_FAILED;
            }
            fmt->sample_rate = (int)sample_rate;
            fmt->block_align = (int)read_le16(bytes + pos + 12);
            fmt->bits_per_sample = (int)read_le16(bytes + pos + 14);
            if (fmt->format == 0x11) {
                if (chunk_len < 20) {
                    printf("native_playSound: missing IMA ADPCM fmt extension\n");
                    return MR_FAILED;
                }
                uint32 extra_size = read_le16(bytes + pos + 16);
                if (extra_size < 2 || extra_size > chunk_len - 18u) {
                    printf("native_playSound: invalid IMA ADPCM fmt extension\n");
                    return MR_FAILED;
                }
                fmt->samples_per_block = (int)read_le16(bytes + pos + 18);
            }
            have_fmt = 1;
        } else if (memcmp(chunk, "data", 4) == 0 && !*audio_data) {
            *audio_data = bytes + pos;
            *audio_len = chunk_len;
        }

        pos += chunk_len;
        if (chunk_len & 1u) {
            if (pos >= riff_end) {
                printf("native_playSound: missing WAV chunk padding\n");
                return MR_FAILED;
            }
            pos++;
        }
    }

    if (!have_fmt || !*audio_data || *audio_len == 0 || !fmt->format ||
        !fmt->channels || !fmt->sample_rate || !fmt->bits_per_sample) {
        printf("native_playSound: missing WAV fmt/data chunk\n");
        return MR_FAILED;
    }
    return MR_SUCCESS;
}

static int native_audio_decode_wav(const void *data, uint32 dataLen,
                                   uint8 **out_pcm, uint32 *out_pcm_len) {
    NativeWavFormat fmt;
    const uint8 *audio_data = NULL;
    uint32 audio_len = 0;
    int ret = native_audio_parse_wav(data, dataLen, &fmt, &audio_data,
                                     &audio_len);
    if (ret != MR_SUCCESS) return ret;

    if (fmt.format == 0x11) {
        uint8 *decoded = NULL;
        uint32 decoded_len = 0;
        ret = native_audio_decode_ima_adpcm(audio_data, audio_len, &fmt,
                                            &decoded, &decoded_len);
        if (ret != MR_SUCCESS) return ret;
        ret = native_audio_convert_pcm(
            decoded, decoded_len, fmt.sample_rate, fmt.channels, 16, 1,
            fmt.channels * (int)sizeof(int16_t), out_pcm, out_pcm_len);
        free(decoded);
        return ret;
    }

    return native_audio_convert_pcm(audio_data, audio_len, fmt.sample_rate,
                                    fmt.channels, fmt.bits_per_sample,
                                    fmt.format, fmt.block_align, out_pcm,
                                    out_pcm_len);
}

static int native_audio_play_wav(const void *data, uint32 dataLen, int32 loop) {
    uint8 *pcm = NULL;
    uint32 pcm_len = 0;
    int ret = native_audio_decode_wav(data, dataLen, &pcm, &pcm_len);
    if (ret != MR_SUCCESS) return ret;
    return native_audio_take_pcm(pcm, pcm_len, loop);
}

/* 用 minimp3 把 MR_SOUND_MP3 整段解码为宿主端 S16 PCM, 再走
 * native_audio_set_pcm 统一重采样到 44.1KHz/stereo。MRP 音效通常只有
 * 几秒(talkcat 音效 32KHz mono 约 1 秒), 全量解码的内存开销可以接受;
 * 以首帧的采样率/声道数为准, 中途参数变化的畸形流按解码到该处截断处理,
 * 避免把不同采样率的帧硬拼进同一个 PCM 缓冲造成变调。 */
static int native_audio_play_mp3(const void *data, uint32 dataLen, int32 loop) {
    if (!data || dataLen == 0) {
        return MR_FAILED;
    }

    /* mp3dec_t 约 6.5KB, native_playSound 在主线程串行调用, 静态实例即可 */
    static mp3dec_t mp3_decoder;
    mp3dec_init(&mp3_decoder);

    const uint8 *src = (const uint8 *)data;
    uint32 pos = 0;
    int hz = 0;
    int channels = 0;
    uint32 pcm_samples = 0;   /* 每声道样本数 */
    uint32 pcm_capacity = 0;  /* 每声道样本容量 */
    int16_t *pcm = NULL;
    int16_t frame_pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];

    while (pos < dataLen) {
        mp3dec_frame_info_t info;
        int samples = mp3dec_decode_frame(&mp3_decoder, src + pos, (int)(dataLen - pos),
                                          frame_pcm, &info);
        if (info.frame_bytes <= 0) {
            break; /* 剩余数据里已找不到合法帧(尾部 ID3/垃圾字节) */
        }
        pos += (uint32)info.frame_bytes;
        if (samples <= 0) {
            continue; /* 合法帧但无输出(解码器预热帧), 继续扫描 */
        }
        if (hz == 0) {
            hz = info.hz;
            channels = info.channels;
        } else if (info.hz != hz || info.channels != channels) {
            printf("native_playSound: MP3 stream params changed (%dHz/%dch -> %dHz/%dch), truncated\n",
                   hz, channels, info.hz, info.channels);
            break;
        }

        if (pcm_samples + (uint32)samples > pcm_capacity) {
            uint32 new_capacity = pcm_capacity ? pcm_capacity * 2 : 64 * 1024;
            while (new_capacity < pcm_samples + (uint32)samples) {
                new_capacity *= 2;
            }
            int16_t *grown = (int16_t *)realloc(pcm, (size_t)new_capacity * (uint32)channels * sizeof(int16_t));
            if (!grown) {
                free(pcm);
                return MR_FAILED;
            }
            pcm = grown;
            pcm_capacity = new_capacity;
        }
        memcpy(pcm + (size_t)pcm_samples * (uint32)channels, frame_pcm,
               (size_t)samples * (uint32)channels * sizeof(int16_t));
        pcm_samples += (uint32)samples;
    }

    if (!pcm || pcm_samples == 0 || hz <= 0 || channels <= 0) {
        free(pcm);
        printf("native_playSound: MP3 stream has no decodable frame len=%u\n", dataLen);
        return MR_FAILED;
    }

    /* 解码结果是交织 S16(format=1/bits=16), 交给统一的 PCM 重采样入口。
     * 成功日志用于 e2e 从 stdout 判断 MP3 播放链路是否被触发(无声回归排查)。 */
    int ret = native_audio_set_pcm(pcm, pcm_samples * (uint32)channels * (uint32)sizeof(int16_t),
                                   loop, hz, channels, 16, 1, channels * (int)sizeof(int16_t));
    free(pcm); /* native_audio_set_pcm 内部拷贝转换, 源缓冲这里释放 */
    if (ret == MR_SUCCESS) {
        printf("native_playSound: mp3 len=%u -> %uHz/%dch samples=%u loop=%d\n",
               dataLen, (uint32)hz, channels, pcm_samples, (int)loop);
    }
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

            if (pos >= end) {
                printf("native_playSound: missing MIDI event status\n");
                return MR_FAILED;
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
    if (native_audio_start_output() != MR_SUCCESS) {
        return MR_FAILED;
    }
    native_audio_lock();
    native_audio_clear_legacy_locked(&native_audio);
    int ret = native_audio_parse_midi(&native_audio, (const uint8 *)data, dataLen);
    if (ret == MR_SUCCESS) {
        native_audio.loop = loop ? 1 : 0;
        midi_reset_playback(&native_audio);
        native_audio.source = AUDIO_SOURCE_MIDI;
    }
    native_audio_unlock();
    return ret;
}

static int native_audio_take_channel_pcm(int32 handle, uint8 *pcm,
                                         uint32 pcm_len, int32 loop) {
    if (handle <= 0 || !pcm || pcm_len == 0 ||
        pcm_len % AUDIO_BYTES_PER_FRAME != 0) {
        free(pcm);
        return MR_FAILED;
    }
    if (native_audio_start_output() != MR_SUCCESS) {
        free(pcm);
        return MR_FAILED;
    }

    native_audio_lock();
    uint32 index = 0;
    while (index < native_audio.channel_voice_count &&
           native_audio.channel_voices[index].handle != handle) {
        index++;
    }
    if (index == native_audio.channel_voice_count) {
        if (native_audio.channel_voice_count ==
            native_audio.channel_voice_capacity) {
            uint32 new_capacity = native_audio.channel_voice_capacity
                ? native_audio.channel_voice_capacity * 2u : 4u;
            if (new_capacity < native_audio.channel_voice_capacity ||
                (size_t)new_capacity >
                    SIZE_MAX / sizeof(native_audio.channel_voices[0])) {
                native_audio_unlock();
                free(pcm);
                return MR_FAILED;
            }
            NativeAudioChannelVoice *grown =
                (NativeAudioChannelVoice *)realloc(
                    native_audio.channel_voices,
                    (size_t)new_capacity *
                        sizeof(native_audio.channel_voices[0]));
            if (!grown) {
                native_audio_unlock();
                free(pcm);
                return MR_FAILED;
            }
            native_audio.channel_voices = grown;
            native_audio.channel_voice_capacity = new_capacity;
        }
        native_audio.channel_voice_count++;
    } else {
        free(native_audio.channel_voices[index].pcm);
    }

    NativeAudioChannelVoice *voice = &native_audio.channel_voices[index];
    voice->handle = handle;
    voice->pcm = pcm;
    voice->pcm_len = pcm_len;
    voice->pcm_pos = 0;
    voice->loop = loop ? 1 : 0;
    native_audio_unlock();
    return MR_SUCCESS;
}

static int32 native_playSoundChannel(int32 handle, int type,
                                     const void *data, uint32 dataLen,
                                     int32 loop) {
    uint8 *pcm = NULL;
    uint32 pcm_len = 0;
    int ret;

    /* 当前多通道设备 ABI 传入完整 WAVE 流；解码结果由 native voice 独立
     * 持有，DSM 可以在 PLAY 返回后继续按自己的 OPEN/CLOSE 生命周期管理
     * 原压缩缓冲。其它媒体类型不能冒充裸 PCM，否则会掩盖设备编码错误。 */
    if (type != NATIVE_SOUND_WAV) {
        printf("native_playSoundChannel: unsupported sound type %d len=%u\n",
               type, dataLen);
        return MR_FAILED;
    }
    ret = native_audio_decode_wav(data, dataLen, &pcm, &pcm_len);
    if (ret != MR_SUCCESS) return ret;
    return native_audio_take_channel_pcm(handle, pcm, pcm_len, loop);
}

static int32 native_stopSoundChannel(int32 handle) {
    if (handle <= 0) return MR_FAILED;
    native_audio_lock();
    for (uint32 i = 0; i < native_audio.channel_voice_count; i++) {
        if (native_audio.channel_voices[i].handle == handle) {
            native_audio_channel_remove_locked(&native_audio, i);
            break;
        }
    }
    native_audio_unlock();
    /* DSM owns handle validity. A valid non-looping clip may already have
     * drained from the mixer, so stopping an absent positive voice is
     * intentionally idempotent. */
    return MR_SUCCESS;
}

static int32 native_playSound(int type, const void *data, uint32 dataLen, int32 loop) {
    /* Mythroad 传入的是完整内存音频流：gxdzc 的“是否开启音乐？”确认路径
     * 实测为 MR_SOUND_MIDI + 标准 SMF；MR_SOUND_PCM 按 mrc_base.h 标注处理为
     * 8KHz/16bit/mono。Flutter 共享库通过 skyengine_api_audio_render_s16le()
     * 拉取统一的 44.1KHz/S16/stereo PCM，SDL 入口则用同一渲染器回调播放。 */
    switch (type) {
        case NATIVE_SOUND_MIDI:
            return native_audio_play_midi(data, dataLen, loop);
        case NATIVE_SOUND_WAV:
            return native_audio_play_wav(data, dataLen, loop);
        case NATIVE_SOUND_PCM:
            return native_audio_set_pcm(data, dataLen, loop, 8000, 1, 16, 1, 2);
        case NATIVE_SOUND_MP3:
            /* talkcat 启动音/交互音效是 MP3(经 mr_platEx MR_MEDIA_* 或
             * mr_playSound 直接传入), 用 minimp3 解码后统一走 PCM 渲染 */
            return native_audio_play_mp3(data, dataLen, loop);
        case NATIVE_SOUND_M4A:
        case NATIVE_SOUND_AMR:
        case NATIVE_SOUND_AMR_WB:
            printf("native_playSound: unsupported compressed sound type %d len=%u\n", type, dataLen);
            return MR_FAILED;
        default:
            printf("native_playSound: unknown sound type %d len=%u\n", type, dataLen);
            return MR_FAILED;
    }
}

static int32 native_stopSound(int type) {
    (void)type;
    native_audio_lock();
    native_audio_clear_legacy_locked(&native_audio);
    native_audio_unlock();
    return MR_SUCCESS;
}

/* 震动马达(SKYENGINE 手册 mr_startShake.md):委托前端 bridge 实现,
 * 契约返回 MR_SUCCESS。 */
static int32 native_startShake(int32 ms) {
    guiStartShake(ms);
    return MR_SUCCESS;
}

static int32 native_stopShake(void) {
    guiStopShake();
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

/* 平台文本框:委托 native_text_widget(黑底绿字全屏文本页,软键转
 * MR_DIALOG_EVENT)。此前直接返回 MR_FAILED 会让"先注册模态状态、后调
 * mr_textCreate 且失败不回滚"的应用(如 gtdgdq 帮助页)死等对话框事件,
 * 表现为按键全部失效。验证:test/e2e/gtdgdq/menu.test.ts。 */
static int32 native_textCreate(const char *title, const char *text, int32 type) {
    return native_text_widget_create(title, text, type);
}

static int32 native_textRelease(int32 text) {
    return native_text_widget_release(text);
}

static int32 native_textRefresh(int32 handle, const char *title, const char *text) {
    return native_text_widget_refresh(handle, title, text);
}

static void native_drawBitmap(uint16 *bmp, int16 x, int16 y, uint16 w, uint16 h) {
    guiDrawBitmap(bmp, x, y, w, h);
}

static const char *native_editGetText(int32 edit) {
    return editGetText(edit);
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
    .timerStop = native_timerStop,
    .get_uptime_ms = native_get_uptime_ms,
    .getDatetime = getDatetime,
    .sleep = native_sleep,
    .open = my_open,
    .close = my_close,
    .read = my_read,
    .write = my_write,
    .seek = my_seek,
    .tell = my_tell,
    .info = my_info,
    .remove = my_remove,
    .rename = my_rename,
    .mkDir = my_mkDir,
    .rmDir = my_rmDir,
    .opendir = my_opendir,
    .readdir = native_readdir,
    .closedir = my_closedir,
    .getLen = my_getLen,
    .drawBitmap = native_drawBitmap,
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
    .mr_editGetText = native_editGetText,
    .mr_playSoundChannel = native_playSoundChannel,
    .mr_stopSoundChannel = native_stopSoundChannel,
    .flags = NATIVE_DSM_FLAGS,
    .screen_width = 0,
    .screen_height = 0,
};

DSM_REQUIRE_FUNCS *native_dsm_funcs_get(void) {
    native_uptime_base = (uint64_t)get_uptime_ms();
    native_funcs.screen_width = skyengine_config.screen_width;
    native_funcs.screen_height = skyengine_config.screen_height;
    return &native_funcs;
}

int native_audio_sample_rate(void) {
    return AUDIO_SAMPLE_RATE;
}

int native_audio_channels(void) {
    return AUDIO_CHANNELS;
}

int native_audio_is_active(void) {
    native_audio_lock();
    int active = native_audio.source != AUDIO_SOURCE_NONE ||
                 native_audio.channel_voice_count != 0;
    native_audio_unlock();
    return active;
}

int native_audio_render_s16le(void *buffer, int frames) {
    if (!buffer || frames <= 0) {
        return 0;
    }
    native_audio_lock();
    int rendered = native_audio_render_bytes(&native_audio, (uint8 *)buffer, frames * AUDIO_BYTES_PER_FRAME);
    native_audio_unlock();
    return rendered;
}

void native_audio_stop(void) {
    native_audio_lock();
    native_audio_clear_all_locked(&native_audio);
    native_audio_unlock();
}

void native_dsm_funcs_destroy(void) {
#if defined(VMRP_SDL_AUDIO)
    if (native_audio.device) {
        SDL_CloseAudioDevice(native_audio.device);
        native_audio.device = 0;
    }
#endif
    native_audio_lock();
    native_audio_clear_all_locked(&native_audio);
    native_audio_unlock();
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
