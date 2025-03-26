#pragma once

#include <stdint.h>


#pragma pack(push, 1)
#pragma pack(1)
typedef struct {
    uint8_t major_version;      // Major version number (usually 1)
    uint8_t minor_version;      // 1 byteMinor version number (usually 0)
    int32_t tune_id;            // Song identification number
    char tune_name[30];         // Song title, null terminated
    uint8_t tick_beat;          // Number of ticks per beat
    uint8_t beat_measure;       // Number of beats per measure
    int32_t total_tick;         // Length of song, in ticks
    int32_t data_size;          // Length of MIDI data block, in bytes
    int32_t num_command;        // Total number of MIDI events in song, including final 0xFC "stop"
    uint8_t src_tick_beat;      // ?
    uint8_t filler[7];          // Padding, set to zero
    uint8_t sound_mode;         // OPL rhythm mode: 0=melodic (off), 1=percussive (on)
    uint8_t pitchbend_range;    // Pitchbend range (1-12)
    uint16_t basic_tempo;       // Song tempo, in beats-per-minute
    uint8_t filler2[8];         // Padding, set to zero
} IMS_HEADER;
#pragma pack(pop)

#pragma pack(push, 1)
#pragma pack(1)
typedef struct {
    char name[9];
} IMS_INSTRUMENT;
#pragma pack(pop)

typedef struct {
    void* raw;
    IMS_HEADER* header;
    uint8_t* song_data;
    uint16_t num_instruments;
    IMS_INSTRUMENT* instruments;
} IMS;


IMS* load_ims(const char* filename);
void free_ims(IMS* ims);