#include <stdio.h>
#include <stdlib.h>

#include "raw.h"

RAW* alloc_raw(size_t size)
{
    uint8_t* data = malloc(size);
    if (data == NULL) {
        return NULL;
    }

    RAW* raw = malloc(sizeof(RAW));
    if (raw == NULL) {
        free(data);
        return NULL;
    }

    raw->data = data;
    raw->size = size;

    return raw;
}

void free_raw(RAW* raw)
{
    free(raw->data);
    free(raw);
}

RAW* load_raw(const char* filename)
{
    RAW* raw = NULL;

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (0 < size && size < 1024 * 1024) {
        raw = alloc_raw(size);
        if (raw != NULL) {
            size_t count = fread(raw->data, size, 1, fp);

            if (count == 0) {
                free_raw(raw);
                raw = NULL;
            }
        }
    }

    fclose(fp);

    return raw;
}