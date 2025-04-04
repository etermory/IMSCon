#pragma once

#include <stdbool.h>

#include "ims_types.h"
#include "iss_types.h"
#include "bnk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char text[64];
} LYRIC;

typedef struct {
    int ksl;                // Key scaling level
    int multiple;           // Frequency multiplier
    int feedback;           // Feedback [op 0 only, op 1 ignored]
    int attack;             // Attack rate
    int sustain;            // Sustain level
    int eg;                 // Envelope gain (nonzero value is on)
    int decay;              // Decay rate
    int release_rate;       // Release rate
    int total_level;        // Total output level
    int am;                 // Amplitude modulation (Tremolo)
    int vib;                // Frequency Vibrato
    int ksr;                // Key scaling/envelope rate
    int con;                // Connector [op 0 only, op 1 ignored]
} OPLREGS32;


typedef struct {
    int percussive;         // 0: Melodic instrument, 1: Percussive instrument
    int voice_num;          // Voice number (percussive only)
    OPLREGS32 op_modulator; // Register values for the Modulator operator (op 0)
    OPLREGS32 op_carrier;   // Register values for the Carrier operator (op 1)
    int mod_wave_sel;       // Modulator wave select (OPL base register 0xE0)
    int car_wave_sel;       // Carrier wave select (OPL base register 0xE0)
} TIMBRE;


typedef struct {
    IMS* ims;
    char title[30];
    ISS* iss;
    LYRIC* lyrics;
    BNK* bnk;
    TIMBRE* timbres;
    bool is_end;
    int tick;
    int last_iss_rec_pos;
    int tempo;
    int pos;
    int sample_remain_len;
} IMS_MUSIC;

#ifdef __cplusplus
}
#endif