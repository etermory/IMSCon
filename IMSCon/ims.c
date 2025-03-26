#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ims.h"
#include "raw.h"


IMS* load_ims(const char* filename)
{
    IMS* ims = malloc(sizeof(IMS));
    if (ims == NULL) {
        return NULL;
    }

    RAW* raw = load_raw(filename);
    if (raw == NULL) {
        free(ims);
        return NULL;
    }

    ims->raw = raw;
    uint8_t* cur = raw->data;

    ims->header = (IMS_HEADER*)cur;
    cur = cur + sizeof(IMS_HEADER) + 1;

    ims->song_data = cur;
    cur = cur + ims->header->data_size + 1;

    assert(memcmp(cur - 2, "ww", 2) == 0);

    ims->num_instruments = *(uint16_t*)cur;
    cur = cur + sizeof(uint16_t);

    ims->instruments = (IMS_INSTRUMENT*)cur;
    cur = cur + sizeof(IMS_INSTRUMENT) * (ims->num_instruments);

    assert(cur - raw->data == raw->size);

    return ims;
}

void free_ims(IMS* ims)
{
    free_raw(ims->raw);
    free(ims);
}