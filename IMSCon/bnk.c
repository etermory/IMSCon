#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "bnk.h"
#include "raw.h"


BNK* load_bnk(const char* filename)
{
    BNK* bnk = malloc(sizeof(BNK));
    if (bnk == NULL) {
        return NULL;
    }

    RAW* raw = load_raw(filename);
    if (raw == NULL) {
        free(bnk);
        return NULL;
    }

    bnk->raw = raw;
    bnk->header = (BNK_HEADER*)raw->data;

    assert(memcmp(bnk->header->signature, "ADLIB-", 6) == 0);

    bnk->instruments = (BNK_INST_NAME*)(raw->data + bnk->header->offset_name);
    bnk->data = (BNK_INST_RECORD*)(raw->data + bnk->header->offset_data);

    return bnk;
}

void free_bnk(BNK* bnk)
{
    free_raw(bnk->raw);
    free(bnk);
}