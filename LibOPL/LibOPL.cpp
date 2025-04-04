#include "framework.h"
#include "LibOPL.h"

#include "adlib.h"
#include "outchip.h"


void OPL_SoundWarmInit(void)
{
    SoundWarmInit();
}

void OPL_SetMode(int mode)
{
    SetMode(mode);
}

void OPL_SetVoiceTimbre(int voice, int* paramArray)
{
    SetVoiceTimbre(voice, paramArray);
}

void OPL_SetVoiceVolume(int voice, int volume)
{
    SetVoiceVolume(voice, volume);
}

void OPL_SetVoicePitch(int voice, int pitchBend)
{
    SetVoicePitch(voice, pitchBend);
}

void OPL_NoteOn(int voice, int pitch)
{
    NoteOn(voice, pitch);
}

void OPL_NoteOff(int voice)
{
    NoteOff(voice);
}

void OPL_SndOutput(int addr, int dataVal)
{
    SndOutput(addr, dataVal);
}

void OPL_SndInit(int rate)
{
    SndInit(rate);
}

void OPL_SndUpdateOne(short* buffer, int length)
{
    SndUpdateOne(buffer, length);
}

void OPL_SndShutdown()
{
    SndShutdown();
}

void OPL_SndReset()
{
    SndReset();
}