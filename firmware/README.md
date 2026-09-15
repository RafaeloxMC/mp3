# Firmware

Firmware for the Raspberry Pi Pico based MP3 player, built with the
Raspberry Pi Pico C SDK.

## Features

- Browses folders and MP3 files on the microSD card
- Displays song title, artist and a cover image on the ST7735 LCD
- Adjustable volume from any screen using the left and right buttons
- Plays audio out both AUX jacks at the same time through the onboard codec

## Hardware mapping

See src/config.h for the full pin list. Summary:

- SPI0: microSD card (SCK 2, MOSI 3, MISO 4, CS 5, card detect 22)
- SPI1: ST7735 LCD (SCK 14, MOSI 15, MISO 12, CS 13, DC 20, reset 19, backlight 21)
- I2S: audio codec (BCLK 16, LRCLK 17, DOUT 18)
- Buttons: up 6, down 7, left 8, right 9, select 10, back 11

## Controls

- Up / down: move the selection in the file browser, skip track while playing
- Left / right: adjust volume, from any screen
- Select: open a folder or start a song, toggle play and pause while playing
- Back: go up one folder, or return to the browser from the now playing screen

## Cover art

Place a file named cover.bmp in the same folder as the songs. It must be an
uncompressed 24 bit BMP file, at most 96 by 96 pixels.

## Building

Requires the Raspberry Pi Pico SDK and its toolchain. Set the PICO_SDK_PATH
environment variable to point at a checkout of raspberrypi/pico-sdk.

An internet connection is needed the first time the project is configured,
since CMake fetches raspberrypi/pico-extras and
carlk3/no-OS-FatFS-SD-SPI-RPi-Pico automatically.

    cmake -S . -B build -G Ninja
    cmake --build build

The resulting mp3_player.uf2 file in build/ can be copied to the Pico while
it is in USB bootloader mode.

## Known limitations

- Only 24 bit uncompressed BMP cover art is supported, no JPEG or PNG
- MP3 files at a sample rate other than the fixed output rate are resampled
  with a simple nearest neighbor method
- The on/off switch is fully hardware controlled and is not read by the
  firmware
