#include "audio_output.h"
#include "config.h"

#include "pico/stdlib.h"
#include "pico/audio_i2s.h"

#define BUFFER_FRAME_COUNT 576
#define BUFFER_COUNT 4

static struct audio_format s_audio_format;
static struct audio_buffer_format s_producer_format;
static struct audio_buffer_pool *s_producer_pool;
static uint32_t s_sample_rate;
static int s_volume_percent = VOLUME_DEFAULT;

void audio_output_init(uint32_t sample_rate) {
    s_sample_rate = sample_rate;

    s_audio_format.format = AUDIO_BUFFER_FORMAT_PCM_S16;
    s_audio_format.sample_freq = sample_rate;
    s_audio_format.channel_count = AUDIO_CHANNELS;

    s_producer_format.format = &s_audio_format;
    s_producer_format.sample_stride = AUDIO_CHANNELS * sizeof(int16_t);

    s_producer_pool = audio_new_producer_pool(&s_producer_format, BUFFER_COUNT, BUFFER_FRAME_COUNT);

    struct audio_i2s_config config = {
        .data_pin = PIN_I2S_DOUT,
        .clock_pin_base = PIN_I2S_BCLK,
        .dma_channel = 0,
        .pio_sm = 0,
    };

    const struct audio_format *actual_format = audio_i2s_setup(&s_audio_format, &config);
    if (!actual_format) {
        panic("audio_output_init: unable to open I2S audio device");
    }

    audio_i2s_connect(s_producer_pool);
    audio_i2s_set_enabled(true);
}

uint32_t audio_output_sample_rate(void) {
    return s_sample_rate;
}

void audio_output_set_volume(int volume_percent) {
    if (volume_percent < VOLUME_MIN) volume_percent = VOLUME_MIN;
    if (volume_percent > VOLUME_MAX) volume_percent = VOLUME_MAX;
    s_volume_percent = volume_percent;
}

int audio_output_get_volume(void) {
    return s_volume_percent;
}

void audio_output_write(const int16_t *interleaved_stereo, uint32_t frame_count) {
    uint32_t vol_mul = (uint32_t)((s_volume_percent * 32768) / VOLUME_MAX);
    if (vol_mul > 32768) vol_mul = 32768;

    uint32_t offset = 0;
    while (offset < frame_count) {
        struct audio_buffer *buffer = take_audio_buffer(s_producer_pool, true);
        uint32_t remaining = frame_count - offset;
        uint32_t n = remaining > buffer->max_sample_count ? buffer->max_sample_count : remaining;

        int16_t *out = (int16_t *)buffer->buffer->bytes;
        const int16_t *in = interleaved_stereo + offset * AUDIO_CHANNELS;
        for (uint32_t i = 0; i < n * AUDIO_CHANNELS; i++) {
            out[i] = (int16_t)(((int32_t)in[i] * (int32_t)vol_mul) >> 15);
        }

        buffer->sample_count = n;
        give_audio_buffer(s_producer_pool, buffer);
        offset += n;
    }
}
