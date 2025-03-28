#include <stdio.h>

#include <Windows.h>    // MulDiv
#include <SDL_mixer.h>

#include "han.h"
#include "play.h"


typedef struct {
    IMS_MUSIC* music;
} SDL_AUDIO_USERDATA;


typedef void (*audio_callback)(void* userdata, Uint8* stream, int len);


void print_title(IMS_MUSIC* music)
{
    char title[30] = { 0, };
    han_conv(0, music->ims->header->tune_name, title);
    printf("%s\n", title);
}

void print_lyrics(IMS_MUSIC* music)
{
    static int last_lyric_pos = 0;

    ISS_RECORD* iss_rec = music->iss->records + music->last_iss_rec_pos;

    int lyric_pos = iss_rec->lyric_pos;
    if (lyric_pos != last_lyric_pos) {
        last_lyric_pos = lyric_pos;

        printf("\r");
        for (int i = 0; i < 78; ++i) {
            printf(" ");
        }
        printf("\r");
        printf("%s\n", music->lyrics[lyric_pos].text);
    }

    printf("\r");
    for (int i = 0; i < 78; ++i) {
        printf(" ");
    }
    printf("\r");

    for (int i = 0; i < iss_rec->char_pos - 1; ++i) {
        printf(" ");
    }
    printf("^");
    for (int i = 0; i < iss_rec->char_cnt - 1; ++i) {
        printf(" ");
    }
    printf("^");
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

void _play(PLAY_SRC playlist[], int list_cnt, SDL_AUDIO_USERDATA* userdata)
{
    ims_init();

    for (int i = 0; i < list_cnt; ++i) {
        IMS_MUSIC* music = prepare_music(playlist[i].ims_path, playlist[i].iss_path, playlist[i].bnk_path);
        if (music != NULL) {
            ((SDL_AUDIO_USERDATA*)userdata)->music = music;

            SDL_PauseAudio(0);
            printf("(음악이 끝났거나, 다음곡을 ");
            system("PAUSE");
            SDL_PauseAudio(1);

            free_music(music);
        }
    }

    ims_shutdown();
}

void callback(void* userdata, Uint8* stream, int len)
{
    IMS_MUSIC* music = ((SDL_AUDIO_USERDATA*)userdata)->music;

    if (!music->is_end || 0 < music->sample_remain_len) {
        if (music->tick == 0) {
            printf(")\n");  // PAUSE 문구가 출력된 후
            print_title(music);
        }

        uint8_t pcm_buffer[512 * 2]; // len == samples(512) * format(2) * channels(1)
        int remain = get_sample(music, pcm_buffer, len / 2, MulDiv);
        SDL_MixAudio(stream, pcm_buffer, len - remain * 2, 128);

        if (music->iss != NULL) {
            print_lyrics(music);
        }
    }
    else {
        SDL_PauseAudio(1);
        printf("Done!");
    }
}

SDL_AudioSpec* get_audio_spec()
{
    return _get_audio_spec(callback);
}

void play(PLAY_SRC playlist[], int list_cnt, SDL_AudioSpec* audio_spec)
{
    _play(playlist, list_cnt, audio_spec->userdata);
}