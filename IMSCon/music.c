#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "han.h"
#include "music.h"

#include "adlib.h"
#include "fmopl.h"
#include "outchip.h"

#include "fft.h"


const int _freq = 22050;


int _proc_music(IMS_MUSIC* music)
{
    static uint8_t st_code;
    int voice, note, volume, inst_id;
    uint16_t pitch_bend;

    int delay = 0;
    do {
        uint8_t code = music->ims->song_data[music->pos];

        if (code < 0x80) {
            code = st_code;
        }
        else {
            st_code = code;
            music->pos++;
        }

        voice = code & 0x0F;

        switch (code & 0xF0) {
        case 0x80:	// note1
            note = music->ims->song_data[music->pos];
            volume = music->ims->song_data[music->pos + 1];

            NoteOff(voice);
            SetVoiceVolume(voice, volume);
            NoteOn(voice, note);
            music->pos += 2;
            break;

        case 0x90:	// note2
            note = music->ims->song_data[music->pos];
            volume = music->ims->song_data[music->pos + 1];

            NoteOff(voice);
            if (volume) {
                SetVoiceVolume(voice, volume);
                NoteOn(voice, note);
            }
            music->pos += 2;
            break;

        case 0xA0:	// vol
            volume = music->ims->song_data[music->pos];
            SetVoiceVolume(voice, volume);
            music->pos++;
            break;

        case 0xC0:	// ins
            inst_id = music->ims->song_data[music->pos];
            assert(0 <= inst_id && inst_id < music->bnk->header->num_instruments);
            SetVoiceTimbre(voice, &music->timbres[inst_id].op_modulator);
            music->pos++;
            break;

        case 0xE0:	// pit
            pitch_bend = *(uint16_t*)(music->ims->song_data + music->pos) / 2;
            SetVoicePitch(voice, pitch_bend);
            music->pos += 2;
            break;

        case 0xF0:	// tem
            music->pos += 2;
            music->tempo = music->ims->header->basic_tempo * (music->ims->song_data[music->pos] + music->ims->song_data[music->pos + 1] / 128.f);
            music->pos += 3;
            break;
        }

        while (1) {
            code = music->ims->song_data[music->pos];
            music->pos++;

            if (code != 0xF8) {
                delay += code;
                break;
            }

            delay += 240;
        }

        if (music->ims->song_data[music->pos] == 0xFC) {
            music->is_end = true;
            break;
        }
    } while (delay == 0);

    return delay;
}

void _proc_lyrics(IMS_MUSIC* music)
{
    if (music->iss != NULL) {
        int time_end = music->tick / 8;
        for (int i = music->last_iss_rec_pos + 1; i < music->iss->header->rec_count; ++i) {
            if (time_end <= music->iss->records[i].time) {
                music->last_iss_rec_pos = i - 1;
                break;
            }
        }
    }
}

int get_sample(IMS_MUSIC* music, int16_t* pcm_buffer, int buffer_len, muldiv_func muldiv)
{
    int sample_len = 0;

    while (0 < buffer_len) {
        if (0 < music->sample_remain_len) {
            sample_len = music->sample_remain_len;
            music->sample_remain_len = 0;
        }
        else if (!music->is_end) {
            int tempo = music->tempo;
            int delay = _proc_music(music);
            sample_len = muldiv(_freq * 60, delay, tempo * music->ims->header->tick_beat);
            music->tick += delay;

            _proc_lyrics(music);
        }

        if (sample_len == 0) {
            break;
        }

        if (buffer_len < sample_len) {
            music->sample_remain_len = sample_len - buffer_len;
            sample_len = buffer_len;
        }

        YM3812UpdateOne(ym3812p, pcm_buffer, sample_len);

        pcm_buffer = pcm_buffer + sample_len;
        buffer_len = buffer_len - sample_len;
    }

    return buffer_len; // remain
}

void get_fft_ampl(int16_t* pcm_buffer, double* ampl, int len)
{
    if (len == 0) return;

#define MAX_SAMPLES 512
    double real_in[MAX_SAMPLES], real_out[MAX_SAMPLES], im_out[MAX_SAMPLES];

    for (int i = 0; i < len; ++i) {
        real_in[i] = (double)(pcm_buffer[i]);
    }

    FFT_Compute(len, real_in, NULL, real_out, im_out, false);
    FFT_Norm(len / 2, real_out, im_out, ampl);
}

void ims_init()
{
    ym3812p = YM3812Init(3579545L, _freq);
    SoundWarmInit();
}

void ims_shutdown()
{
    YM3812Shutdown(ym3812p);
}

void _copy_timbre_params(int* timbres, uint8_t* bnk_inst_record)
{
    for (int i = 0; i < 30; ++i) {
        timbres[i] = bnk_inst_record[i];
    }
}

TIMBRE* _create_timbre_params(IMS* ims, BNK* bnk)
{
    TIMBRE* timbres = malloc(sizeof(TIMBRE) * ims->num_instruments);
    if (timbres == NULL) {
        return NULL;
    }

    for (int i = 0; i < ims->num_instruments; ++i) {
        for (int j = 0; j < bnk->header->num_instruments; ++j) {
            if (_stricmp(ims->instruments[i].name, bnk->instruments[j].name) == 0) {
                _copy_timbre_params(timbres + i, bnk->data + j);
                break;
            }
        }
    }

    return timbres;
}

LYRIC* _convert_lyrics(ISS* iss)
{
    LYRIC* lyrics = malloc(sizeof(LYRIC) * iss->header->lyrics_count);
    if (lyrics == NULL) {
        return NULL;
    }

    for (int i = 0; i < iss->header->lyrics_count; ++i) {
        han_conv(0, iss->lyrics[i].text, lyrics[i].text);
    }

    return lyrics;
}

IMS_MUSIC* _load_music(char* ims_path, char* iss_path, char* bnk_path)
{
    IMS* ims = load_ims(ims_path);
    if (ims == NULL) {
        return NULL;
    }

    ISS* iss = NULL;
    LYRIC* lyrics = NULL;
    if (iss_path != NULL) {
        iss = load_iss(iss_path);
        if (iss != NULL) {
            lyrics = _convert_lyrics(iss);
        }
    }

    BNK* bnk = load_bnk(bnk_path);
    if (bnk == NULL) {
        free_ims(ims);
        if (iss != NULL) {
            free_iss(iss);
            if (lyrics != NULL) {
                free(lyrics);
            }
        }
        return NULL;
    }

    TIMBRE* timbres = _create_timbre_params(ims, bnk);
    if (timbres == NULL) {
        free_ims(ims);
        if (iss != NULL) {
            free_iss(iss);
            if (lyrics != NULL) {
                free(lyrics);
            }
        }
        free_bnk(bnk);
        return NULL;
    }

    IMS_MUSIC* music = malloc(sizeof(IMS_MUSIC));
    if (music == NULL) {
        free_ims(ims);
        if (iss != NULL) {
            free_iss(iss);
            if (lyrics != NULL) {
                free(lyrics);
            }
        }
        free_bnk(bnk);
        free(timbres);
        return NULL;
    }

    memset(music, 0, sizeof(IMS_MUSIC));
    music->ims = ims;
    music->iss = iss;
    music->lyrics = lyrics;
    music->bnk = bnk;
    music->timbres = timbres;
    music->tempo = ims->header->basic_tempo;

    return music;
}

void free_music(IMS_MUSIC* music)
{
    free_ims(music->ims);
    if (music->iss != NULL) {
        free_iss(music->iss);
        if (music->lyrics != NULL) {
            free(music->lyrics);
        }
    }
    free_bnk(music->bnk);
    free(music->timbres);
    free(music);
}

IMS_MUSIC* prepare_music(char* ims_path, char* iss_path, char* bnk_path)
{
    IMS_MUSIC* music = _load_music(ims_path, iss_path, bnk_path);
    if (music == NULL) {
        return NULL;
    }

    YM3812ResetChip(ym3812p);
    SndOutput(1, 0x20);
    SetMode(music->ims->header->sound_mode);
    int volumes = 9 + music->ims->header->sound_mode * 2;
    for (int i = 0; i < volumes; ++i) {
        NoteOff(i);
        SetVoiceVolume(i, 0);
    }

    return music;
}