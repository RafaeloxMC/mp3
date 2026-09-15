#include "ui.h"
#include "config.h"

#include <string.h>
#include <stdio.h>

#include "pico/time.h"

#include "st7735.h"
#include "buttons.h"
#include "library.h"
#include "bmp_image.h"
#include "id3_tags.h"
#include "mp3_player.h"
#include "audio_output.h"

typedef enum {
    SCREEN_BROWSE = 0,
    SCREEN_NOW_PLAYING
} screen_t;

#define VISIBLE_ROWS 12
#define ROW_HEIGHT 11
#define LIST_TOP 18

static screen_t s_screen;

static char s_current_path[LIBRARY_PATH_MAX];
static library_dir_t s_dir;
static int s_selected;
static int s_scroll_offset;

static char s_now_playing_path[LIBRARY_PATH_MAX];
static char s_now_playing_dir[LIBRARY_PATH_MAX];
static int s_now_playing_index;
static id3_tags_t s_tags;
static bmp_image_t s_cover;
static bool s_cover_loaded;

static absolute_time_t s_last_time_refresh;
static uint32_t s_last_elapsed_shown;

static bool s_volume_overlay_active;
static absolute_time_t s_volume_overlay_until;

static void truncate_for_width(const char *in, char *out, size_t out_size, int max_chars) {
    size_t n = strlen(in);
    if ((int)n > max_chars) n = (size_t)max_chars;
    if (n >= out_size) n = out_size - 1;
    memcpy(out, in, n);
    out[n] = 0;
}

static void load_directory(const char *path) {
    library_list_dir(path, &s_dir);
    strncpy(s_current_path, path, sizeof(s_current_path) - 1);
    s_selected = 0;
    s_scroll_offset = 0;
}

static void draw_browse_screen(void) {
    st7735_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ST7735_BLACK);

    char header[24];
    truncate_for_width(s_current_path, header, sizeof(header), 20);
    st7735_draw_text(4, 3, header, ST7735_CYAN, ST7735_BLACK, 1);
    st7735_fill_rect(0, 13, SCREEN_WIDTH, 1, ST7735_DARKGRAY);

    if (s_dir.entry_count == 0) {
        st7735_draw_text(4, LIST_TOP, "no files found", ST7735_GRAY, ST7735_BLACK, 1);
        return;
    }

    if (s_selected < s_scroll_offset) {
        s_scroll_offset = s_selected;
    }
    if (s_selected >= s_scroll_offset + VISIBLE_ROWS) {
        s_scroll_offset = s_selected - VISIBLE_ROWS + 1;
    }

    for (int row = 0; row < VISIBLE_ROWS; row++) {
        int idx = s_scroll_offset + row;
        if (idx >= s_dir.entry_count) break;

        library_entry_t *e = &s_dir.entries[idx];
        char line[24];
        char display[22];
        truncate_for_width(e->name, display, sizeof(display), 19);
        if (e->type == ENTRY_DIR) {
            snprintf(line, sizeof(line), "/%s", display);
        } else {
            snprintf(line, sizeof(line), "%s", display);
        }

        bool selected = (idx == s_selected);
        uint16_t fg = selected ? ST7735_BLACK : ST7735_WHITE;
        uint16_t bg = selected ? ST7735_WHITE : ST7735_BLACK;
        int y = LIST_TOP + row * ROW_HEIGHT;
        if (selected) {
            st7735_fill_rect(0, y - 1, SCREEN_WIDTH, ROW_HEIGHT, bg);
        }
        st7735_draw_text(2, y, line, fg, bg, 1);
    }
}

static void draw_volume_overlay(void) {
    int bar_x = 8, bar_y = SCREEN_HEIGHT - 20, bar_w = SCREEN_WIDTH - 16, bar_h = 10;
    st7735_fill_rect(bar_x - 4, bar_y - 12, bar_w + 8, bar_h + 20, ST7735_BLACK);

    char label[16];
    snprintf(label, sizeof(label), "Volume %d", audio_output_get_volume());
    st7735_draw_text(bar_x, bar_y - 10, label, ST7735_YELLOW, ST7735_BLACK, 1);

    st7735_fill_rect(bar_x, bar_y, bar_w, bar_h, ST7735_DARKGRAY);
    int fill_w = (bar_w * audio_output_get_volume()) / VOLUME_MAX;
    if (fill_w > 0) {
        st7735_fill_rect(bar_x, bar_y, fill_w, bar_h, ST7735_GREEN);
    }
}

static int cover_area_top(void) { return 6; }
static int cover_area_size(void) { return 64; }

static void draw_cover_area(void) {
    int size = cover_area_size();
    int x = (SCREEN_WIDTH - size) / 2;
    int y = cover_area_top();

    if (s_cover_loaded && s_cover.pixels) {
        int draw_w = s_cover.width > size ? size : s_cover.width;
        int draw_h = s_cover.height > size ? size : s_cover.height;
        int draw_x = x + (size - draw_w) / 2;
        int draw_y = y + (size - draw_h) / 2;
        st7735_fill_rect(x, y, size, size, ST7735_BLACK);
        st7735_draw_image_rgb565(draw_x, draw_y, draw_w, draw_h, s_cover.pixels);
    } else {
        st7735_fill_rect(x, y, size, size, ST7735_DARKGRAY);
        st7735_draw_text(x + 6, y + size / 2 - 4, "no cover", ST7735_GRAY, ST7735_DARKGRAY, 1);
    }
}

static void format_time(uint32_t seconds, char *out, size_t out_size) {
    uint32_t m = seconds / 60;
    uint32_t s = seconds % 60;
    snprintf(out, out_size, "%02u-%02u", (unsigned)m, (unsigned)s);
}

static void draw_now_playing_time(void) {
    int y = cover_area_top() + cover_area_size() + 44;
    char elapsed_str[8];
    char duration_str[8];
    format_time(player_elapsed_seconds(), elapsed_str, sizeof(elapsed_str));
    uint32_t duration = player_duration_seconds();

    char line[24];
    if (duration > 0) {
        format_time(duration, duration_str, sizeof(duration_str));
        snprintf(line, sizeof(line), "%s of %s", elapsed_str, duration_str);
    } else {
        snprintf(line, sizeof(line), "%s", elapsed_str);
    }

    st7735_fill_rect(4, y, SCREEN_WIDTH - 8, 8, ST7735_BLACK);
    st7735_draw_text(4, y, line, ST7735_WHITE, ST7735_BLACK, 1);

    const char *state_str = "playing";
    if (player_get_state() == PLAYER_PAUSED) state_str = "paused";
    else if (player_get_state() == PLAYER_FINISHED) state_str = "finished";
    st7735_fill_rect(4, y + 10, SCREEN_WIDTH - 8, 8, ST7735_BLACK);
    st7735_draw_text(4, y + 10, state_str, ST7735_GRAY, ST7735_BLACK, 1);
}

static void draw_now_playing_screen(void) {
    st7735_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ST7735_BLACK);
    draw_cover_area();

    int text_y = cover_area_top() + cover_area_size() + 6;
    char title[22];
    char artist[22];
    const char *title_src = s_tags.has_tag && s_tags.title[0] ? s_tags.title : s_now_playing_path;
    truncate_for_width(title_src, title, sizeof(title), 19);
    truncate_for_width(s_tags.artist[0] ? s_tags.artist : "unknown artist", artist, sizeof(artist), 19);

    st7735_draw_text(4, text_y, title, ST7735_WHITE, ST7735_BLACK, 1);
    st7735_draw_text(4, text_y + 10, artist, ST7735_CYAN, ST7735_BLACK, 1);

    draw_now_playing_time();
}

static void load_cover_for_path(const char *song_path) {
    bmp_free(&s_cover);
    s_cover_loaded = false;

    char dir_copy[LIBRARY_PATH_MAX];
    strncpy(dir_copy, song_path, sizeof(dir_copy) - 1);
    dir_copy[sizeof(dir_copy) - 1] = 0;
    char *slash = strrchr(dir_copy, '/');
    if (slash) *slash = 0;

    char cover_path[LIBRARY_PATH_MAX];
    if (library_find_cover(dir_copy, cover_path, sizeof(cover_path))) {
        s_cover_loaded = bmp_load(cover_path, &s_cover, COVER_ART_MAX_SIZE);
    }
}

static void start_playing_index(int index) {
    if (index < 0 || index >= s_dir.entry_count) return;
    if (s_dir.entries[index].type != ENTRY_SONG) return;

    char path[LIBRARY_PATH_MAX];
    library_join_path(path, sizeof(path), s_current_path, s_dir.entries[index].name);

    if (!player_open(path)) {
        return;
    }

    strncpy(s_now_playing_path, path, sizeof(s_now_playing_path) - 1);
    strncpy(s_now_playing_dir, s_current_path, sizeof(s_now_playing_dir) - 1);
    s_now_playing_index = index;

    id3_read_tags(path, &s_tags);
    load_cover_for_path(path);

    s_screen = SCREEN_NOW_PLAYING;
    draw_now_playing_screen();
    s_last_elapsed_shown = 0xFFFFFFFF;
}

static int find_adjacent_song(int start_index, int direction) {
    int idx = start_index;
    for (int steps = 0; steps < s_dir.entry_count; steps++) {
        idx += direction;
        if (idx < 0 || idx >= s_dir.entry_count) return -1;
        if (s_dir.entries[idx].type == ENTRY_SONG) return idx;
    }
    return -1;
}

static void handle_browse_input(void) {
    if (button_repeated(BUTTON_UP)) {
        if (s_selected > 0) s_selected--;
        draw_browse_screen();
    }
    if (button_repeated(BUTTON_DOWN)) {
        if (s_selected < s_dir.entry_count - 1) s_selected++;
        draw_browse_screen();
    }
    if (button_pressed(BUTTON_SELECT)) {
        if (s_dir.entry_count > 0) {
            library_entry_t *e = &s_dir.entries[s_selected];
            if (e->type == ENTRY_DIR) {
                char path[LIBRARY_PATH_MAX];
                library_join_path(path, sizeof(path), s_current_path, e->name);
                load_directory(path);
                draw_browse_screen();
            } else {
                start_playing_index(s_selected);
            }
        }
    }
    if (button_pressed(BUTTON_BACK)) {
        if (strcmp(s_current_path, library_root_path()) != 0) {
            char parent[LIBRARY_PATH_MAX];
            strncpy(parent, s_current_path, sizeof(parent) - 1);
            parent[sizeof(parent) - 1] = 0;
            char *slash = strrchr(parent, '/');
            if (slash) {
                *slash = 0;
            }
            if (strlen(parent) == 0 || strchr(parent, '/') == NULL) {
                strncpy(parent, library_root_path(), sizeof(parent) - 1);
                parent[sizeof(parent) - 1] = 0;
            }
            load_directory(parent);
            draw_browse_screen();
        }
    }
}

static void handle_now_playing_input(void) {
    if (button_pressed(BUTTON_SELECT)) {
        player_toggle_pause();
        draw_now_playing_time();
    }
    if (button_pressed(BUTTON_BACK)) {
        s_screen = SCREEN_BROWSE;
        draw_browse_screen();
    }
    bool dir_matches = (strcmp(s_current_path, s_now_playing_dir) == 0);

    if (dir_matches && button_pressed(BUTTON_UP)) {
        int idx = find_adjacent_song(s_now_playing_index, -1);
        if (idx >= 0) start_playing_index(idx);
    }
    if (dir_matches && button_pressed(BUTTON_DOWN)) {
        int idx = find_adjacent_song(s_now_playing_index, 1);
        if (idx >= 0) start_playing_index(idx);
    }

    if (dir_matches && player_get_state() == PLAYER_FINISHED) {
        int idx = find_adjacent_song(s_now_playing_index, 1);
        if (idx >= 0) {
            start_playing_index(idx);
        }
    }

    int64_t since_refresh = absolute_time_diff_us(s_last_time_refresh, get_absolute_time());
    if (since_refresh >= 1000000) {
        s_last_time_refresh = get_absolute_time();
        uint32_t elapsed = player_elapsed_seconds();
        if (elapsed != s_last_elapsed_shown) {
            s_last_elapsed_shown = elapsed;
            draw_now_playing_time();
        }
    }
}

void ui_init(void) {
    s_screen = SCREEN_BROWSE;
    s_last_time_refresh = get_absolute_time();
    audio_output_set_volume(VOLUME_DEFAULT);

    if (!library_mount()) {
        st7735_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ST7735_BLACK);
        st7735_draw_text(4, 4, "sd card error", ST7735_RED, ST7735_BLACK, 1);
        return;
    }

    load_directory(library_root_path());
    draw_browse_screen();
}

void ui_update(void) {
    bool volume_changed = false;
    if (button_repeated(BUTTON_LEFT)) {
        audio_output_set_volume(audio_output_get_volume() - 5);
        volume_changed = true;
    }
    if (button_repeated(BUTTON_RIGHT)) {
        audio_output_set_volume(audio_output_get_volume() + 5);
        volume_changed = true;
    }

    if (volume_changed) {
        s_volume_overlay_active = true;
        s_volume_overlay_until = make_timeout_time_ms(1500);
        draw_volume_overlay();
    }

    if (!volume_changed && s_volume_overlay_active) {
        if (absolute_time_diff_us(get_absolute_time(), s_volume_overlay_until) <= 0) {
            s_volume_overlay_active = false;
            if (s_screen == SCREEN_BROWSE) draw_browse_screen();
            else draw_now_playing_screen();
        }
    }

    if (!s_volume_overlay_active) {
        if (s_screen == SCREEN_BROWSE) {
            handle_browse_input();
        } else {
            handle_now_playing_input();
        }
    }

    player_pump();
}
