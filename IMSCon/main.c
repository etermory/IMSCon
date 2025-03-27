#include <Windows.h>

#include <stdio.h>

#include <SDL.h>
#undef main
#include <SDL_mixer.h>

#include "music.h"
#include "han.h"


typedef struct {
    char* ims_path;
    char* bnk_path;
} SRC_PAIR;


typedef struct {
    IMS_MUSIC* music;
} SDL_AUDIO_USERDATA;


typedef void (*audio_callback)(void* userdata, Uint8* stream, int len);


SDL_AudioSpec* get_audio_spec(audio_callback callback)
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

free_audio_spec(SDL_AudioSpec* audio_spec)
{
    free(audio_spec->userdata);
    free(audio_spec);
}

void play(SRC_PAIR playlist[], int list_cnt, SDL_AUDIO_USERDATA* userdata)
{
    ims_init();

    for (int i = 0; i < list_cnt; ++i) {
        IMS_MUSIC* music = prepare_music(playlist[i].ims_path, playlist[i].bnk_path);
        if (music != NULL) {
            char title[30] = { 0, };
            han_conv(0, music->ims->header->tune_name, title);
            printf("%s\n", title);

            ((SDL_AUDIO_USERDATA*)userdata)->music = music;

            SDL_PauseAudio(0);
            printf("À½¾ÇÀÌ ³¡³µ°Å³ª, ´ÙÀ½°îÀ» ");
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
        uint8_t pcm_buffer[512 * 2];    // len == 512 * 2
        int remain = get_sample(music, pcm_buffer, len / 2, MulDiv);
        SDL_MixAudio(stream, pcm_buffer, len - remain * 2, 128);
    }
    else {
        SDL_PauseAudio(1);
        printf("Done!");
    }
}

int main()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return 1;
    }

    SDL_AudioSpec* audio_spec = get_audio_spec(callback);
    if (audio_spec == NULL) {
        return 1;
    }

    if (SDL_OpenAudio(audio_spec, NULL) < 0) {
        return 1;
    }

    SRC_PAIR playlist[] = {
        {"MNT-015B.IMS", "MNT-015B.BNK"},
        {"01-YSTN.IMS", "01-YSTN.BNK"},
        {"01_YSPEC.IMS", "01_YSPEC.BNK"},
    };
    play(playlist, sizeof(playlist) / sizeof(SRC_PAIR), audio_spec->userdata);

    SDL_CloseAudio();
    free_audio_spec(audio_spec);
    SDL_Quit();

    return 0;
}
