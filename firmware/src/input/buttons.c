#include "buttons.h"
#include "config.h"

#include "pico/stdlib.h"
#include "pico/time.h"

typedef struct {
    uint gpio;
    bool stable_down;
    bool raw_down;
    bool pressed_event;
    bool repeat_event;
    absolute_time_t last_change;
    absolute_time_t down_since;
    absolute_time_t last_repeat;
} button_state_t;

static button_state_t s_buttons[BUTTON_COUNT];

static void button_setup(button_id_t id, uint gpio) {
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_up(gpio);
    s_buttons[id].gpio = gpio;
    s_buttons[id].stable_down = false;
    s_buttons[id].raw_down = false;
}

void buttons_init(void) {
    button_setup(BUTTON_UP, PIN_BTN_UP);
    button_setup(BUTTON_DOWN, PIN_BTN_DOWN);
    button_setup(BUTTON_LEFT, PIN_BTN_LEFT);
    button_setup(BUTTON_RIGHT, PIN_BTN_RIGHT);
    button_setup(BUTTON_SELECT, PIN_BTN_SELECT);
    button_setup(BUTTON_BACK, PIN_BTN_BACK);
}

void buttons_update(void) {
    absolute_time_t now = get_absolute_time();
    for (int i = 0; i < BUTTON_COUNT; i++) {
        button_state_t *b = &s_buttons[i];
        b->pressed_event = false;
        b->repeat_event = false;

        bool raw_down = !gpio_get(b->gpio);
        if (raw_down != b->raw_down) {
            b->raw_down = raw_down;
            b->last_change = now;
        }

        if (absolute_time_diff_us(b->last_change, now) >= (BUTTON_DEBOUNCE_MS * 1000)) {
            if (b->raw_down != b->stable_down) {
                b->stable_down = b->raw_down;
                if (b->stable_down) {
                    b->pressed_event = true;
                    b->down_since = now;
                    b->last_repeat = now;
                }
            } else if (b->stable_down) {
                int64_t held_us = absolute_time_diff_us(b->down_since, now);
                if (held_us >= (BUTTON_REPEAT_START_MS * 1000)) {
                    int64_t since_repeat_us = absolute_time_diff_us(b->last_repeat, now);
                    if (since_repeat_us >= (BUTTON_REPEAT_INTERVAL_MS * 1000)) {
                        b->repeat_event = true;
                        b->last_repeat = now;
                    }
                }
            }
        }
    }
}

bool button_pressed(button_id_t id) {
    return s_buttons[id].pressed_event;
}

bool button_repeated(button_id_t id) {
    return s_buttons[id].pressed_event || s_buttons[id].repeat_event;
}

bool button_is_down(button_id_t id) {
    return s_buttons[id].stable_down;
}
