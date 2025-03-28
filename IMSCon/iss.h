#pragma once

#include <stdint.h>


#pragma pack(push, 1)
#pragma pack(1)
typedef struct {
    char head[20];
    char reserved[10];
    char lyricist[30];
    char composer[30];
    char vocalist[30];
    char creator[30];
    uint16_t rec_count;
    uint16_t lyrics_count;
} ISS_HEADER;
#pragma pack(pop)

#pragma pack(push, 1)
#pragma pack(1)
typedef struct {
    uint16_t time;
    uint8_t lyric_pos;
    uint8_t char_pos;
    uint8_t char_cnt;
} ISS_RECORD;
#pragma pack(pop)

#pragma pack(push, 1)
#pragma pack(1)
typedef struct {
    char text[64];
} ISS_LYRIC;
#pragma pack(pop)

typedef struct {
    void* raw;
    ISS_HEADER* header;
    ISS_RECORD* records;
    ISS_LYRIC* lyrics;
} ISS;


ISS* load_iss(const char* filename);
void free_iss(ISS* iss);