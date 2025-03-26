#pragma once

#include <stdint.h>


typedef struct {
    uint8_t* data;
    size_t size;
} RAW;


RAW* alloc_raw(size_t size);
void free_raw(RAW* raw);
RAW* load_raw(const char* filename);