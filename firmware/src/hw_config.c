#include <string.h>

#include "hw_config.h"
#include "ff.h"
#include "diskio.h"

#include "config.h"

static spi_t spis[] = {
    {
        .hw_inst = SD_SPI_PORT,
        .miso_gpio = PIN_SD_MISO,
        .mosi_gpio = PIN_SD_MOSI,
        .sck_gpio = PIN_SD_SCK,
        .baud_rate = SD_BAUD_RATE_HZ,
    }
};

static sd_card_t sd_cards[] = {
    {
        .pcName = "0:",
        .spi = &spis[0],
        .ss_gpio = PIN_SD_CS,
        .use_card_detect = true,
        .card_detect_gpio = PIN_SD_DET,
        .card_detected_true = SD_DET_PRESENT_VALUE,
    }
};

size_t sd_get_num() { return count_of(sd_cards); }

sd_card_t *sd_get_by_num(size_t num) {
    if (num < sd_get_num()) {
        return &sd_cards[num];
    }
    return NULL;
}

size_t spi_get_num() { return count_of(spis); }

spi_t *spi_get_by_num(size_t num) {
    if (num < spi_get_num()) {
        return &spis[num];
    }
    return NULL;
}
