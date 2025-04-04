#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)
#pragma pack(1)
typedef struct {
    uint8_t ver_major;          // Major version number
    uint8_t ver_minor;          // Minor version number
    char signature[6];          // "ADLIB-"
    uint16_t num_used;          // Number of instrument records in use
    uint16_t num_instruments;   // Number of instrument records in the file
    uint32_t offset_name;       // Offset where instrument names begin
    uint32_t offset_data;       // Offset where instrument data begins
    uint8_t pad[8];             // Padded with 0x00
} BNK_HEADER;
#pragma pack(pop)

#pragma pack(push, 1)
#pragma pack(1)
typedef struct {
    uint16_t index;             // Index into data section. Calculation: offsetInstr = offsetData + (index*  sizeof(PackedTimbre))
    uint8_t flag;               // 0 if this record is not used, else 1
    char name[9];               // Instrument name - must be NULL-terminated
} BNK_INST_NAME;
#pragma pack(pop)

#pragma pack(push, 1)
#pragma pack(1)
typedef struct {
    uint8_t ksl;                // Key scaling level
    uint8_t multiple;           // Frequency multiplier
    uint8_t feedback;           // Feedback [op 0 only, op 1 ignored]
    uint8_t attack;             // Attack rate
    uint8_t sustain;            // Sustain level
    uint8_t eg;                 // Envelope gain (nonzero value is on)
    uint8_t decay;              // Decay rate
    uint8_t release_rate;       // Release rate
    uint8_t total_level;        // Total output level
    uint8_t am;                 // Amplitude modulation (Tremolo)
    uint8_t vib;                // Frequency Vibrato
    uint8_t ksr;                // Key scaling/envelope rate
    uint8_t con;                // Connector [op 0 only, op 1 ignored]
} OPLREGS;
#pragma pack(pop)

#pragma pack(push, 1)
#pragma pack(1)
typedef struct {
    uint8_t percussive;         // 0: Melodic instrument, 1: Percussive instrument
    uint8_t voice_num;          // Voice number (percussive only)
    OPLREGS op_modulator;       // Register values for the Modulator operator (op 0)
    OPLREGS op_carrier;         // Register values for the Carrier operator (op 1)
    uint8_t mod_wave_sel;       // Modulator wave select (OPL base register 0xE0)
    uint8_t car_wave_sel;       // Carrier wave select (OPL base register 0xE0)
} BNK_INST_RECORD;
#pragma pack(pop)

typedef struct {
    void* raw;
    BNK_HEADER* header;
    BNK_INST_NAME* instruments;
    BNK_INST_RECORD* data;
} BNK;

#ifdef __cplusplus
}
#endif