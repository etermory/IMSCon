#include <fcntl.h>
#include <io.h>
#include <locale.h>
#include <tchar.h>

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
    _tsetlocale(LC_ALL, _T(""));
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

    PLAY_SRC playlist[] = {
        {"MNT-015B.IMS", "MNT-015B.ISS", "MNT-015B.BNK"},
        {"01-YSTN.IMS", NULL, "01-YSTN.BNK"},
        {"01_YSPEC.IMS", NULL, "01_YSPEC.BNK"},
    };
    play(playlist, sizeof(playlist) / sizeof(PLAY_SRC), audio_spec);

    SDL_CloseAudio();
    free_audio_spec(audio_spec);
    SDL_Quit();

    return 0;
}
