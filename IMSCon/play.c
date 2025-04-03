#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <conio.h>
#include <io.h>

#include <Windows.h>    // MulDiv
#include <SDL_mixer.h>

#include "han.h"
#include "play.h"


typedef struct {
    IMS_MUSIC* music;
} SDL_AUDIO_USERDATA;


typedef void (*audio_callback)(void* userdata, Uint8* stream, int len);


void mb_to_wc(char mb[], wchar_t wc[])
{
    int len = mbstowcs(NULL, mb, 0);
    mbstowcs(wc, mb, len + 1);
}

void print_title(IMS_MUSIC* music)
{
    char title[30] = { 0, };
    han_conv(0, music->ims->header->tune_name, title);

    wchar_t buf[50] = { 0, };
    mb_to_wc(title, buf);
    wprintf(L"%s\n", buf);
}

void print_lyrics(IMS_MUSIC* music)
{
    static int last_lyric_pos = 0;
    if (music == NULL) {
        last_lyric_pos = 0;
        return;
    }

    ISS_RECORD* iss_rec = music->iss->records + music->last_iss_rec_pos;

    int lyric_pos = iss_rec->lyric_pos;
    if (lyric_pos != last_lyric_pos) {
        last_lyric_pos = lyric_pos;

        wprintf(L"\r");
        for (int i = 0; i < 78; ++i) {
            wprintf(L" ");
        }
        wprintf(L"\r");

        wchar_t buf[100] = { 0, };
        mb_to_wc(music->lyrics[lyric_pos].text, buf);
        wprintf(L"%s\n", buf);
    }

    wprintf(L"\r");
    for (int i = 0; i < 78; ++i) {
        wprintf(L" ");
    }
    wprintf(L"\r");

    for (int i = 0; i < iss_rec->char_pos - 1; ++i) {
        wprintf(L" ");
    }
    wprintf(L"^");
    for (int i = 0; i < iss_rec->char_cnt - 1; ++i) {
        wprintf(L" ");
    }
    wprintf(L"^");
}

void print_sound_level(double* ampl, double sec)
{
    if (sec <= 0) return;

#define NUM_FREQUENCY 23
    const int METER_FREQUENCY[NUM_FREQUENCY] = { 30, 60, 100, 160, 240, 300, 350, 400, 440, 500, 600, 800, 1000, 1500, 2000, 2600, 3000, 4000, 6000, 8000, 10000, 14000, 16000 };
#define MAX_LEVEL 8
    wchar_t LEVELS[MAX_LEVEL + 1] = { L' ', L'⢀', L'⣀', L'⣠', L'⣤', L'⣴', L'⣶', L'⣾', L'⣿' };
    const double max_ampl = 32767.0 * 32767.0;

    wprintf(L"\r");
    for (int i = 0; i < NUM_FREQUENCY; ++i)
    {
        int indice = METER_FREQUENCY[i] * sec;
        int value = (int)(20 * log10(ampl[indice] / max_ampl));
        value = min(100, max(value, 0));
        value = value / 100.0 * MAX_LEVEL;
        wprintf(L"%c ", LEVELS[value]);
    }
}

SDL_AudioSpec* _get_audio_spec(audio_callback callback)
{
    SDL_AUDIO_USERDATA* userdata = malloc(sizeof(SDL_AUDIO_USERDATA));
    if (userdata == NULL) {
        return NULL;
    }

    SDL_AudioSpec* audio_spec = malloc(sizeof(SDL_AudioSpec));
    if (audio_spec == NULL) {
        free(userdata);
        return NULL;
    }

    memset(audio_spec, 0, sizeof(SDL_AudioSpec));
    audio_spec->freq = 22050;
    audio_spec->format = AUDIO_S16SYS;
    audio_spec->channels = 1;
    audio_spec->samples = 512;
    audio_spec->callback = callback;
    audio_spec->userdata = userdata;

    return audio_spec;
}

void free_audio_spec(SDL_AudioSpec* audio_spec)
{
    free(audio_spec->userdata);
    free(audio_spec);
}

PLAY_SRC* get_play_src(char* ims_filename)
{
    char filename[13];
    strcpy(filename, ims_filename);

    PLAY_SRC* src = malloc(sizeof(PLAY_SRC));
    if (src == NULL) {
        return NULL;
    }

    strcpy(src->ims_path, filename);

    strcpy(strpbrk(filename, ".") + 1, "ISS");
    if (_access(filename, 0) == 0) {
        strcpy(src->iss_path, filename);
    }

    strcpy(strpbrk(filename, ".") + 1, "BNK");
    if (_access(filename, 0) == 0) {
        strcpy(src->bnk_path, filename);
    } else {
        strcpy(src->bnk_path, "STANDARD.BNK");
    }

    return src;
}

LIST get_playlist(char* filename)
{
    LIST playlist = { NULL };

    if (_access(filename, 0) != 0) {
        return playlist;
    }

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        return playlist;
    }

    NODE* prev = NULL;
    char ims_filename[14];
    while (fgets(ims_filename, 14, fp)) {
        *strpbrk(ims_filename, "\n") = 0;

        PLAY_SRC* src = get_play_src(ims_filename);
        if (src == NULL) {
            return playlist;
        }

        NODE* node = malloc(sizeof(NODE));
        if (node == NULL) {
            free(src);
            return playlist;
        }

        node->data = src;
        node->next = NULL;

        if (playlist.head == NULL) {
            playlist.head = node;
        }

        if (prev == NULL) {
            prev = playlist.head;
        }
        else {
            prev = prev->next = node;
        }
    }
    fclose(fp);

    return playlist;
}

void free_playlist(LIST playlist)
{
    NODE* cur = playlist.head;
    while (cur) {
        free(cur->data);
        cur = cur->next;
    }
}

void _play(NODE* head, SDL_AUDIO_USERDATA* userdata)
{
    ims_init();

    NODE* cur = head;
    while (cur != NULL) {
        PLAY_SRC* src = (PLAY_SRC*)cur->data;

        IMS_MUSIC* music = prepare_music(src->ims_path, src->iss_path, src->bnk_path);
        if (music != NULL) {
            ((SDL_AUDIO_USERDATA*)userdata)->music = music;

            SDL_PauseAudio(0);
            (void)_getch();
            SDL_PauseAudio(1);

            while (SDL_GetAudioStatus() != SDL_AUDIO_PAUSED);
            free_music(music);
        }

        cur = cur->next;
    }

    ims_shutdown();
}

void _callback(void* userdata, Uint8* stream, int len)
{
    IMS_MUSIC* music = ((SDL_AUDIO_USERDATA*)userdata)->music;

    if (!music->is_end || 0 < music->sample_remain_len) {
        if (music->tick == 0) {
            wprintf(L"\n(음악이 끝났거나 다음 곡으로 넘어가려면 아무 키나 누르십시오...)\n");
            print_title(music);
            print_lyrics(NULL);
        }

#define MAX_SAMPLES 512
        int16_t pcm_buffer[MAX_SAMPLES];
        int remain = get_sample(music, pcm_buffer, len / 2, MulDiv); // len == samples(512) * format(2) * channels(1)

        SDL_MixAudio(stream, pcm_buffer, len - remain * 2, SDL_MIX_MAXVOLUME);

        if (music->iss != NULL) {
            print_lyrics(music);
        }
        else {
            double ampl[512];
            get_fft_ampl(pcm_buffer, ampl, len / 2);
            print_sound_level(ampl, len / 2 / 22050.0);
        }
    }
    else {
        SDL_PauseAudio(1);
        wprintf(L"끝!");
    }
}

SDL_AudioSpec* get_audio_spec()
{
    return _get_audio_spec(_callback);
}

void play(LIST playlist, SDL_AudioSpec* audio_spec)
{
    _play(playlist.head, audio_spec->userdata);
}