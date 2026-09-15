#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>

typedef enum {
    BUTTON_UP = 0,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_SELECT,
    BUTTON_BACK,
    BUTTON_COUNT
} button_id_t;

void buttons_init(void);
void buttons_update(void);
bool button_pressed(button_id_t id);
bool button_repeated(button_id_t id);
bool button_is_down(button_id_t id);

#endif
