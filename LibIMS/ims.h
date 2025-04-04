#pragma once

#include "ims_types.h"


IMS* load_ims(const char* filename);
void free_ims(IMS* ims);