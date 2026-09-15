#ifndef MP3_PLAYER_H
#define MP3_PLAYER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    PLAYER_STOPPED = 0,
    PLAYER_PLAYING,
    PLAYER_PAUSED,
    PLAYER_FINISHED
} player_state_t;

bool player_open(const char *path);
void player_close(void);
void player_pump(void);
void player_pause(void);
void player_resume(void);
void player_toggle_pause(void);
player_state_t player_get_state(void);
uint32_t player_elapsed_seconds(void);
uint32_t player_duration_seconds(void);

#endif
