#pragma once

#include "raw_types.h"


RAW* alloc_raw(size_t size);
void free_raw(RAW* raw);
RAW* load_raw(const char* filename);