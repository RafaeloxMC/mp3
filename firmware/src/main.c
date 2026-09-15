#include "pico/stdlib.h"

#include "config.h"
#include "st7735.h"
#include "buttons.h"
#include "audio_output.h"
#include "ui.h"

int main(void) {
    stdio_init_all();

    buttons_init();
    st7735_init();
    st7735_set_backlight(true);

    audio_output_init(44100);

    ui_init();

    while (true) {
        buttons_update();
        ui_update();
    }

    return 0;
}
