#include "library.h"

#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdio.h>

#include "ff.h"
#include "hw_config.h"

static const char *ROOT_PATH = "0:";

const char *library_root_path(void) {
    return ROOT_PATH;
}

bool library_mount(void) {
    sd_init_driver();
    sd_card_t *sd = sd_get_by_num(0);
    if (!sd) {
        return false;
    }
    FRESULT fr = f_mount(&sd->fatfs, sd->pcName, 1);
    return fr == FR_OK;
}

static bool has_mp3_extension(const char *name) {
    size_t len = strlen(name);
    if (len < 4) return false;
    const char *ext = name + len - 4;
    return (tolower((unsigned char)ext[0]) == '.') &&
           (tolower((unsigned char)ext[1]) == 'm') &&
           (tolower((unsigned char)ext[2]) == 'p') &&
           (tolower((unsigned char)ext[3]) == '3');
}

static int name_compare(const void *a, const void *b) {
    const library_entry_t *ea = (const library_entry_t *)a;
    const library_entry_t *eb = (const library_entry_t *)b;
    if (ea->type != eb->type) {
        return (ea->type == ENTRY_DIR) ? -1 : 1;
    }
    return strcasecmp(ea->name, eb->name);
}

static void sort_entries(library_dir_t *out) {
    for (int i = 1; i < out->entry_count; i++) {
        library_entry_t key = out->entries[i];
        int j = i - 1;
        while (j >= 0 && name_compare(&out->entries[j], &key) > 0) {
            out->entries[j + 1] = out->entries[j];
            j--;
        }
        out->entries[j + 1] = key;
    }
}

bool library_list_dir(const char *path, library_dir_t *out) {
    memset(out, 0, sizeof(*out));
    strncpy(out->path, path, sizeof(out->path) - 1);

    DIR dir;
    if (f_opendir(&dir, path) != FR_OK) {
        return false;
    }

    FILINFO info;
    while (out->entry_count < LIBRARY_MAX_ENTRIES) {
        if (f_readdir(&dir, &info) != FR_OK || info.fname[0] == 0) {
            break;
        }
        if (info.fattrib & (AM_HID | AM_SYS)) {
            continue;
        }
        if (info.fattrib & AM_DIR) {
            library_entry_t *e = &out->entries[out->entry_count++];
            e->type = ENTRY_DIR;
            strncpy(e->name, info.fname, sizeof(e->name) - 1);
        } else if (has_mp3_extension(info.fname)) {
            library_entry_t *e = &out->entries[out->entry_count++];
            e->type = ENTRY_SONG;
            strncpy(e->name, info.fname, sizeof(e->name) - 1);
        }
    }

    f_closedir(&dir);
    sort_entries(out);
    return true;
}

void library_join_path(char *dest, size_t dest_size, const char *dir, const char *name) {
    if (strcmp(dir, ROOT_PATH) == 0) {
        snprintf(dest, dest_size, "%s/%s", ROOT_PATH, name);
    } else {
        snprintf(dest, dest_size, "%s/%s", dir, name);
    }
}

bool library_find_cover(const char *dir_path, char *out_path, size_t out_size) {
    static const char *candidates[] = {"cover.bmp", "Cover.bmp", "COVER.BMP"};
    for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++) {
        char candidate[LIBRARY_PATH_MAX];
        library_join_path(candidate, sizeof(candidate), dir_path, candidates[i]);
        FILINFO info;
        if (f_stat(candidate, &info) == FR_OK) {
            strncpy(out_path, candidate, out_size - 1);
            out_path[out_size - 1] = 0;
            return true;
        }
    }
    return false;
}
