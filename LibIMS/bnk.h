#pragma once

#include "bnk_types.h"


BNK* load_bnk(const char* filename);
void free_bnk(BNK* bnk);