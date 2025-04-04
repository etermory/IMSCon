#include <assert.h>
#include <stdlib.h>

#include "iss.h"
#include "raw.h"


ISS* load_iss(const char* filename)
{
    ISS* iss = malloc(sizeof(ISS));
    if (iss == NULL) {
        return NULL;
    }

    RAW* raw = load_raw(filename);
    if (raw == NULL) {
        free(iss);
        return NULL;
    }

    iss->raw = raw;
    uint8_t* cur = raw->data;

    iss->header = (ISS_HEADER*)cur;
    cur = cur + sizeof(ISS_HEADER);

    iss->records = (ISS_RECORD*)cur;
    cur = cur + sizeof(ISS_RECORD) * iss->header->rec_count;

    iss->lyrics = (ISS_LYRIC*)cur;
    cur = cur + sizeof(ISS_LYRIC) * iss->header->lyrics_count;

    assert(cur - raw->data == raw->size);

    return iss;
}

void free_iss(ISS* iss)
{
    free_raw(iss->raw);
    free(iss);
}