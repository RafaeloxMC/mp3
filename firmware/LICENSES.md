# Third party code

This firmware vendors the following third party sources under third_party/.

## libhelix-mp3

Location: third_party/libhelix-mp3

Fixed point MP3 decoder from the Helix DNA project, originally by RealNetworks,
as redistributed by the ESP8266Audio project. Licensed under the RealNetworks
Public Source License 1.0 or the RealNetworks Community Source License 1.0,
see the included LICENSE.txt, RPSL.txt and RCSL.txt files.

## Font glyph table

Location: src/display/font5x7.h

Glyph bitmap data derived from the Adafruit-GFX-Library glcdfont table,
Copyright Adafruit Industries, licensed under the BSD license.

## Build scaffolding

pico_sdk_import.cmake and pico_extras_import.cmake are copied unmodified from
the official raspberrypi/pico-sdk and raspberrypi/pico-extras repositories,
both licensed under the BSD 3-Clause license.

The FatFs and SD SPI driver code used at build time comes from
carlk3/no-OS-FatFS-SD-SPI-RPi-Pico, fetched automatically by CMake, licensed
under the Apache License 2.0.
