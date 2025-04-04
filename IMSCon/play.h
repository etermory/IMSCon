#pragma once

#include "list.h"


typedef struct {
    char ims_path[13];
    char iss_path[13];
    char bnk_path[13];
} PLAY_SRC;


SDL_AudioSpec* get_audio_spec();
void free_audio_spec(SDL_AudioSpec* audio_spec);
LIST get_playlist(char* filename);
void free_playlist(LIST playlist);
void play(LIST playlist, SDL_AudioSpec* audio_spec);