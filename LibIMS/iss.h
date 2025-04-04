#pragma once

#include "iss_types.h"


ISS* load_iss(const char* filename);
void free_iss(ISS* iss);