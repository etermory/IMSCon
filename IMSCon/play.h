#pragma once

#include "music.h"


typedef struct {
    char* ims_path;
    char* iss_path;
    char* bnk_path;
} PLAY_SRC;


SDL_AudioSpec* get_audio_spec();
void free_audio_spec(SDL_AudioSpec* audio_spec);
void play(PLAY_SRC playlist[], int list_cnt, SDL_AudioSpec* audio_spec);