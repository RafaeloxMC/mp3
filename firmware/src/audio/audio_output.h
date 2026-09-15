#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <stdint.h>
#include <stddef.h>

void audio_output_init(uint32_t sample_rate);
uint32_t audio_output_sample_rate(void);
void audio_output_set_volume(int volume_percent);
int audio_output_get_volume(void);
void audio_output_write(const int16_t *interleaved_stereo, uint32_t frame_count);

#endif
