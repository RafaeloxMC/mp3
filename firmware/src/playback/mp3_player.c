#include "mp3_player.h"
#include "audio_output.h"

#include <string.h>

#include "ff.h"
#include "mp3dec.h"

#define INPUT_BUF_SIZE 1600
#define OUTPUT_BUF_SAMPLES (1152 * 2)
#define MAX_RESYNC_ATTEMPTS 64

static FIL s_file;
static bool s_file_open;
static HMP3Decoder s_decoder;

static uint8_t s_buf[INPUT_BUF_SIZE];
static int s_buf_valid;
static bool s_input_eof;

static int16_t s_pcm[OUTPUT_BUF_SAMPLES];
static int16_t s_resample_buf[OUTPUT_BUF_SAMPLES];

static player_state_t s_state = PLAYER_STOPPED;
static uint64_t s_frames_output;
static uint32_t s_output_rate;
static uint32_t s_bitrate_bps;
static uint32_t s_file_size;

static void skip_id3v2_tag(void) {
    uint8_t header[10];
    UINT read_bytes = 0;
    if (f_read(&s_file, header, sizeof(header), &read_bytes) != FR_OK || read_bytes != sizeof(header)) {
        f_lseek(&s_file, 0);
        return;
    }

    if (header[0] != 'I' || header[1] != 'D' || header[2] != '3') {
        f_lseek(&s_file, 0);
        return;
    }

    uint32_t tag_size = ((uint32_t)(header[6] & 0x7F) << 21) | ((uint32_t)(header[7] & 0x7F) << 14) |
                        ((uint32_t)(header[8] & 0x7F) << 7) | (uint32_t)(header[9] & 0x7F);
    uint8_t flags = header[5];
    FSIZE_t skip_to = (FSIZE_t)10 + tag_size;
    if (flags & 0x10) {
        skip_to += 10;
    }
    f_lseek(&s_file, skip_to);
}

static void refill_buffer(void) {
    if (s_input_eof) return;
    int space = INPUT_BUF_SIZE - s_buf_valid;
    if (space <= 0) return;
    UINT read_bytes = 0;
    f_read(&s_file, s_buf + s_buf_valid, (UINT)space, &read_bytes);
    s_buf_valid += (int)read_bytes;
    if ((int)read_bytes < space) {
        s_input_eof = true;
    }
}

bool player_open(const char *path) {
    player_close();

    if (f_open(&s_file, path, FA_READ) != FR_OK) {
        return false;
    }
    s_file_open = true;
    s_file_size = (uint32_t)f_size(&s_file);

    s_decoder = MP3InitDecoder();
    if (!s_decoder) {
        f_close(&s_file);
        s_file_open = false;
        return false;
    }

    skip_id3v2_tag();

    s_buf_valid = 0;
    s_input_eof = false;
    refill_buffer();

    s_frames_output = 0;
    s_bitrate_bps = 0;
    s_output_rate = audio_output_sample_rate();
    s_state = PLAYER_PLAYING;
    return true;
}

void player_close(void) {
    if (s_decoder) {
        MP3FreeDecoder(s_decoder);
        s_decoder = NULL;
    }
    if (s_file_open) {
        f_close(&s_file);
        s_file_open = false;
    }
    s_state = PLAYER_STOPPED;
}

void player_pause(void) {
    if (s_state == PLAYER_PLAYING) s_state = PLAYER_PAUSED;
}

void player_resume(void) {
    if (s_state == PLAYER_PAUSED) s_state = PLAYER_PLAYING;
}

void player_toggle_pause(void) {
    if (s_state == PLAYER_PLAYING) s_state = PLAYER_PAUSED;
    else if (s_state == PLAYER_PAUSED) s_state = PLAYER_PLAYING;
}

player_state_t player_get_state(void) {
    return s_state;
}

uint32_t player_elapsed_seconds(void) {
    if (s_output_rate == 0) return 0;
    return (uint32_t)(s_frames_output / s_output_rate);
}

uint32_t player_duration_seconds(void) {
    if (s_bitrate_bps == 0) return 0;
    return (uint32_t)(((uint64_t)s_file_size * 8) / s_bitrate_bps);
}

static int resample_to_output(const int16_t *pcm, int frames, int channels, int in_rate) {
    if (frames <= 0) return 0;

    if (channels == 1) {
        if ((uint32_t)in_rate == s_output_rate) {
            int count = frames;
            if (count > OUTPUT_BUF_SAMPLES / 2) count = OUTPUT_BUF_SAMPLES / 2;
            for (int i = 0; i < count; i++) {
                s_resample_buf[i * 2] = pcm[i];
                s_resample_buf[i * 2 + 1] = pcm[i];
            }
            return count;
        }
        int out_frames = (int)((int64_t)frames * (int64_t)s_output_rate / in_rate);
        if (out_frames > OUTPUT_BUF_SAMPLES / 2) out_frames = OUTPUT_BUF_SAMPLES / 2;
        for (int i = 0; i < out_frames; i++) {
            int src = (int)((int64_t)i * in_rate / (int64_t)s_output_rate);
            if (src >= frames) src = frames - 1;
            s_resample_buf[i * 2] = pcm[src];
            s_resample_buf[i * 2 + 1] = pcm[src];
        }
        return out_frames;
    }

    if ((uint32_t)in_rate == s_output_rate) {
        int count = frames;
        if (count > OUTPUT_BUF_SAMPLES / 2) count = OUTPUT_BUF_SAMPLES / 2;
        memcpy(s_resample_buf, pcm, (size_t)count * 2 * sizeof(int16_t));
        return count;
    }

    int out_frames = (int)((int64_t)frames * (int64_t)s_output_rate / in_rate);
    if (out_frames > OUTPUT_BUF_SAMPLES / 2) out_frames = OUTPUT_BUF_SAMPLES / 2;
    for (int i = 0; i < out_frames; i++) {
        int src = (int)((int64_t)i * in_rate / (int64_t)s_output_rate);
        if (src >= frames) src = frames - 1;
        s_resample_buf[i * 2] = pcm[src * 2];
        s_resample_buf[i * 2 + 1] = pcm[src * 2 + 1];
    }
    return out_frames;
}

void player_pump(void) {
    if (s_state != PLAYER_PLAYING) return;

    for (int attempt = 0; attempt < MAX_RESYNC_ATTEMPTS; attempt++) {
        if (!s_input_eof && s_buf_valid < INPUT_BUF_SIZE) {
            refill_buffer();
        }

        if (s_buf_valid <= 0) {
            s_state = PLAYER_FINISHED;
            return;
        }

        int sync = MP3FindSyncWord(s_buf, s_buf_valid);
        if (sync < 0) {
            if (s_input_eof) {
                s_state = PLAYER_FINISHED;
                return;
            }
            s_buf_valid = 0;
            continue;
        }
        if (sync > 0) {
            memmove(s_buf, s_buf + sync, (size_t)(s_buf_valid - sync));
            s_buf_valid -= sync;
        }

        unsigned char *inbuf = s_buf;
        int bytes_left = s_buf_valid;
        int ret = MP3Decode(s_decoder, &inbuf, &bytes_left, s_pcm, 0);

        int consumed = s_buf_valid - bytes_left;
        if (consumed <= 0) consumed = 1;
        if (consumed > s_buf_valid) consumed = s_buf_valid;
        memmove(s_buf, s_buf + consumed, (size_t)(s_buf_valid - consumed));
        s_buf_valid -= consumed;

        if (ret != 0) {
            continue;
        }

        MP3FrameInfo info;
        MP3GetLastFrameInfo(s_decoder, &info);
        if (info.bitrate > 0) {
            s_bitrate_bps = (uint32_t)info.bitrate;
        }

        int frames_per_channel = info.nChans > 0 ? info.outputSamps / info.nChans : 0;
        int out_frames = resample_to_output(s_pcm, frames_per_channel, info.nChans, info.samprate);
        if (out_frames > 0) {
            audio_output_write(s_resample_buf, (uint32_t)out_frames);
            s_frames_output += out_frames;
        }
        return;
    }

    s_state = PLAYER_FINISHED;
}
