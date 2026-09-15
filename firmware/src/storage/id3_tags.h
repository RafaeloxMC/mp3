#ifndef ID3_TAGS_H
#define ID3_TAGS_H

#include <stdbool.h>

#define ID3_FIELD_MAX 64

typedef struct {
    char title[ID3_FIELD_MAX];
    char artist[ID3_FIELD_MAX];
    char album[ID3_FIELD_MAX];
    bool has_tag;
} id3_tags_t;

bool id3_read_tags(const char *path, id3_tags_t *out);

#endif
