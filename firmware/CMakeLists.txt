cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)

if(NOT DEFINED PICO_EXTRAS_FETCH_FROM_GIT)
    set(PICO_EXTRAS_FETCH_FROM_GIT 1)
endif()
include(pico_extras_import.cmake)

project(mp3_player C CXX ASM)

pico_sdk_init()

include(FetchContent)
FetchContent_Declare(
    no_os_fatfs_sd_spi
    GIT_REPOSITORY https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico
    GIT_TAG master
)
FetchContent_GetProperties(no_os_fatfs_sd_spi)
if(NOT no_os_fatfs_sd_spi_POPULATED)
    FetchContent_Populate(no_os_fatfs_sd_spi)
endif()
add_subdirectory(${no_os_fatfs_sd_spi_SOURCE_DIR}/FatFs_SPI ${CMAKE_BINARY_DIR}/fatfs_spi)

set(LIBHELIX_MP3_DIR third_party/libhelix-mp3)
set(LIBHELIX_MP3_SOURCES
    ${LIBHELIX_MP3_DIR}/bitstream.c
    ${LIBHELIX_MP3_DIR}/buffers.c
    ${LIBHELIX_MP3_DIR}/dct32.c
    ${LIBHELIX_MP3_DIR}/dequant.c
    ${LIBHELIX_MP3_DIR}/dqchan.c
    ${LIBHELIX_MP3_DIR}/huffman.c
    ${LIBHELIX_MP3_DIR}/hufftabs.c
    ${LIBHELIX_MP3_DIR}/imdct.c
    ${LIBHELIX_MP3_DIR}/mp3dec.c
    ${LIBHELIX_MP3_DIR}/mp3tabs.c
    ${LIBHELIX_MP3_DIR}/polyphase.c
    ${LIBHELIX_MP3_DIR}/scalfact.c
    ${LIBHELIX_MP3_DIR}/stproc.c
    ${LIBHELIX_MP3_DIR}/subband.c
    ${LIBHELIX_MP3_DIR}/trigtabs.c
)
set_source_files_properties(${LIBHELIX_MP3_SOURCES} PROPERTIES COMPILE_DEFINITIONS "ARDUINO=100")

add_executable(mp3_player
    src/main.c
    src/hw_config.c
    src/input/buttons.c
    src/display/st7735.c
    src/audio/audio_output.c
    src/storage/library.c
    src/storage/bmp_image.c
    src/storage/id3_tags.c
    src/playback/mp3_player.c
    src/ui/ui.c
    ${LIBHELIX_MP3_SOURCES}
)

target_include_directories(mp3_player PRIVATE
    src
    src/display
    src/input
    src/audio
    src/storage
    src/playback
    src/ui
    ${LIBHELIX_MP3_DIR}
)

target_link_libraries(mp3_player PRIVATE
    pico_stdlib
    hardware_spi
    hardware_dma
    hardware_pio
    pico_audio_i2s
    FatFs_SPI
)

pico_set_program_name(mp3_player "mp3_player")
pico_set_program_version(mp3_player "0.1")

pico_enable_stdio_uart(mp3_player 1)
pico_enable_stdio_usb(mp3_player 1)

pico_add_extra_outputs(mp3_player)
