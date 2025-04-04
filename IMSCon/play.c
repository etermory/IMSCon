#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <conio.h>  // _getch
#include <io.h>     // _access

#include <SDL_mixer.h>

#include "LibIMS.h"

#include "fft.h"

#include "play.h"


#define AUDIO_FREQ          22050
#define AUDIO_CHANNELS      1
#define SAMPLE_UNIT_SIZE    2       // audio_spec.format = AUDIO_S16SYS
#define AUDIO_SAMPLES       512


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
    wchar_t buf[50] = { 0, };
    mb_to_wc(music->title, buf);
    wprintf(L"TITLE: %s\n", buf);
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
    audio_spec->freq = AUDIO_FREQ;
    audio_spec->format = AUDIO_S16SYS;
    audio_spec->channels = AUDIO_CHANNELS;
    audio_spec->samples = AUDIO_SAMPLES;
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

void get_fft_ampl(int16_t* pcm_buffer, double* ampl, int len)
{
    if (len == 0) return;

    double real_in[AUDIO_SAMPLES], real_out[AUDIO_SAMPLES], im_out[AUDIO_SAMPLES];

    for (int i = 0; i < len; ++i) {
        real_in[i] = (double)(pcm_buffer[i]);
    }

    FFT_Compute(len, real_in, NULL, real_out, im_out, false);
    FFT_Norm(len / 2, real_out, im_out, ampl);
}

void _play(NODE* head, SDL_AUDIO_USERDATA* userdata)
{
    IMS_Init(AUDIO_FREQ);

    wprintf(L"(음악이 끝났거나 다음 곡으로 넘어가려면 아무 키나 누르십시오...)\n");

    NODE* cur = head;
    while (cur != NULL) {
        PLAY_SRC* src = (PLAY_SRC*)cur->data;
        wprintf(L"IMS: %S\n", src->ims_path);
        wprintf(L"BNK: %S\n", src->bnk_path);

        IMS_MUSIC* music = IMS_PrepareMusic(src->ims_path, src->iss_path, src->bnk_path);
        if (music != NULL) {
            ((SDL_AUDIO_USERDATA*)userdata)->music = music;

            SDL_PauseAudio(0);
            (void)_getch();
            SDL_PauseAudio(1);

            while (SDL_GetAudioStatus() != SDL_AUDIO_PAUSED);
            IMS_FreeMusic(music);
        }

        wprintf(L"\n");

        cur = cur->next;
    }

    IMS_Shutdown();
}

void _callback(void* userdata, Uint8* stream, int len)
{
    IMS_MUSIC* music = ((SDL_AUDIO_USERDATA*)userdata)->music;

    if (!music->is_end || 0 < music->sample_remain_len) {
        if (music->tick == 0) {
            print_title(music);
            print_lyrics(NULL);
        }

        int16_t pcm_buffer[AUDIO_SAMPLES];
        int sample_size = len / SAMPLE_UNIT_SIZE; // len == samples(512) * format(2) * channels(1)

        int remain = IMS_GetSample(music, AUDIO_FREQ, pcm_buffer, sample_size) * SAMPLE_UNIT_SIZE;

        SDL_MixAudio(stream, pcm_buffer, len - remain, SDL_MIX_MAXVOLUME);

        if (music->iss != NULL) {
            print_lyrics(music);
        }
        else {
            double ampl[AUDIO_SAMPLES];
            get_fft_ampl(pcm_buffer, ampl, sample_size);
            print_sound_level(ampl, sample_size / (double)AUDIO_FREQ);
        }
    }
    else {
        SDL_PauseAudio(1);
        wprintf(L"끝!\n");
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