#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdbool.h>
#include <stddef.h>

#define LIBRARY_MAX_ENTRIES 96
#define LIBRARY_NAME_MAX 96
#define LIBRARY_PATH_MAX 256

typedef enum {
    ENTRY_DIR = 0,
    ENTRY_SONG = 1
} entry_type_t;

typedef struct {
    entry_type_t type;
    char name[LIBRARY_NAME_MAX];
} library_entry_t;

typedef struct {
    char path[LIBRARY_PATH_MAX];
    library_entry_t entries[LIBRARY_MAX_ENTRIES];
    int entry_count;
} library_dir_t;

bool library_mount(void);
bool library_list_dir(const char *path, library_dir_t *out);
void library_join_path(char *dest, size_t dest_size, const char *dir, const char *name);
bool library_find_cover(const char *dir_path, char *out_path, size_t out_size);
const char *library_root_path(void);

#endif
