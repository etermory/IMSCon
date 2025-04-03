#include <locale.h>
#include <stdio.h>

#include <fcntl.h>
#include <io.h>

#include <Windows.h>
#include <SDL.h>
#undef main

#include "play.h"


void hide_cursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

int main()
{
    _wsetlocale(LC_ALL, L"");
    if (_setmode(_fileno(stdout), _O_WTEXT) == -1) {
        return 1;
    }
    hide_cursor();

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return 1;
    }

    SDL_AudioSpec* audio_spec = get_audio_spec();
    if (audio_spec == NULL) {
        return 1;
    }

    if (SDL_OpenAudio(audio_spec, NULL) < 0) {
        return 1;
    }

    LIST playlist = get_playlist("playlist.txt");
    if (playlist.head == NULL) {
        return 1;
    }
    play(playlist, audio_spec);
    free_playlist(playlist);

    SDL_CloseAudio();
    free_audio_spec(audio_spec);
    SDL_Quit();

    return 0;
}
