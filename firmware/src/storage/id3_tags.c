#include "id3_tags.h"

#include <string.h>

#include "ff.h"

#define MAX_TAG_SCAN_BYTES (128 * 1024)

static uint32_t decode_synchsafe(const uint8_t *p) {
    return ((uint32_t)(p[0] & 0x7F) << 21) | ((uint32_t)(p[1] & 0x7F) << 14) |
           ((uint32_t)(p[2] & 0x7F) << 7) | (uint32_t)(p[3] & 0x7F);
}

static uint32_t decode_plain32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static void store_text(char *dest, size_t dest_size, const uint8_t *data, uint32_t len) {
    if (len == 0) {
        dest[0] = 0;
        return;
    }

    uint8_t encoding = data[0];
    const uint8_t *text = data + 1;
    uint32_t text_len = len - 1;
    size_t out_pos = 0;

    if (encoding == 0 || encoding == 3) {
        for (uint32_t i = 0; i < text_len && out_pos < dest_size - 1; i++) {
            if (text[i] == 0) break;
            dest[out_pos++] = (char)text[i];
        }
    } else if (encoding == 1 || encoding == 2) {
        uint32_t i = 0;
        bool big_endian = true;
        if (encoding == 1 && text_len >= 2) {
            if (text[0] == 0xFF && text[1] == 0xFE) {
                big_endian = false;
                i = 2;
            } else if (text[0] == 0xFE && text[1] == 0xFF) {
                big_endian = true;
                i = 2;
            }
        }
        for (; i + 1 < text_len && out_pos < dest_size - 1; i += 2) {
            uint16_t unit = big_endian ? ((text[i] << 8) | text[i + 1]) : ((text[i + 1] << 8) | text[i]);
            if (unit == 0) break;
            dest[out_pos++] = (unit < 128) ? (char)unit : '?';
        }
    }

    dest[out_pos] = 0;
    while (out_pos > 0 && (unsigned char)dest[out_pos - 1] <= 0x20) {
        dest[--out_pos] = 0;
    }
}

bool id3_read_tags(const char *path, id3_tags_t *out) {
    memset(out, 0, sizeof(*out));

    FIL file;
    if (f_open(&file, path, FA_READ) != FR_OK) {
        return false;
    }

    uint8_t header[10];
    UINT read_bytes = 0;
    if (f_read(&file, header, sizeof(header), &read_bytes) != FR_OK || read_bytes != sizeof(header)) {
        f_close(&file);
        return false;
    }

    if (header[0] != 'I' || header[1] != 'D' || header[2] != '3') {
        f_close(&file);
        return false;
    }

    uint8_t major_version = header[3];
    uint8_t flags = header[5];
    uint32_t tag_size = decode_synchsafe(&header[6]);
    if (tag_size > MAX_TAG_SCAN_BYTES) {
        tag_size = MAX_TAG_SCAN_BYTES;
    }

    uint32_t pos = 0;

    if (flags & 0x40) {
        uint8_t ext_size_bytes[4];
        if (f_read(&file, ext_size_bytes, 4, &read_bytes) != FR_OK || read_bytes != 4) {
            f_close(&file);
            return false;
        }
        uint32_t ext_size = (major_version >= 4) ? decode_synchsafe(ext_size_bytes) : decode_plain32(ext_size_bytes);
        if (ext_size > 4) {
            f_lseek(&file, f_tell(&file) + (ext_size - 4));
        }
        pos += ext_size;
    }

    int frame_header_size = (major_version == 2) ? 6 : 10;
    int frame_id_size = (major_version == 2) ? 3 : 4;

    uint8_t frame_header[10];
    uint8_t field_buf[ID3_FIELD_MAX * 4];

    out->has_tag = true;

    while (pos + (uint32_t)frame_header_size < tag_size) {
        if (f_read(&file, frame_header, (UINT)frame_header_size, &read_bytes) != FR_OK ||
            read_bytes != (UINT)frame_header_size) {
            break;
        }
        pos += frame_header_size;

        if (frame_header[0] == 0) {
            break;
        }

        char frame_id[5] = {0};
        memcpy(frame_id, frame_header, frame_id_size);

        uint32_t frame_size;
        if (major_version == 2) {
            frame_size = ((uint32_t)frame_header[3] << 16) | ((uint32_t)frame_header[4] << 8) | frame_header[5];
        } else if (major_version >= 4) {
            frame_size = decode_synchsafe(&frame_header[4]);
        } else {
            frame_size = decode_plain32(&frame_header[4]);
        }

        if (frame_size == 0 || pos + frame_size > tag_size) {
            break;
        }

        bool is_title = (strcmp(frame_id, "TIT2") == 0) || (strcmp(frame_id, "TT2") == 0);
        bool is_artist = (strcmp(frame_id, "TPE1") == 0) || (strcmp(frame_id, "TP1") == 0);
        bool is_album = (strcmp(frame_id, "TALB") == 0) || (strcmp(frame_id, "TAL") == 0);

        if ((is_title || is_artist || is_album) && frame_size < sizeof(field_buf)) {
            if (f_read(&file, field_buf, frame_size, &read_bytes) == FR_OK && read_bytes == frame_size) {
                if (is_title) {
                    store_text(out->title, sizeof(out->title), field_buf, frame_size);
                } else if (is_artist) {
                    store_text(out->artist, sizeof(out->artist), field_buf, frame_size);
                } else if (is_album) {
                    store_text(out->album, sizeof(out->album), field_buf, frame_size);
                }
            }
        } else {
            f_lseek(&file, f_tell(&file) + frame_size);
        }

        pos += frame_size;
    }

    f_close(&file);
    return true;
}
