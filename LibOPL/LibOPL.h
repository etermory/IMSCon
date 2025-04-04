#ifndef __LIBOPL_H_
#define __LIBOPL_H_

#ifdef LIBOPL_EXPORTS
#define LIBOPL_API __declspec(dllexport)
#else
#define LIBOPL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

    LIBOPL_API void OPL_SoundWarmInit(void);
    LIBOPL_API void OPL_SetMode(int mode);
    LIBOPL_API void OPL_SetVoiceTimbre(int voice, int* paramArray);
    LIBOPL_API void OPL_SetVoiceVolume(int voice, int volume);
    LIBOPL_API void OPL_SetVoicePitch(int voice, int pitchBend);
    LIBOPL_API void OPL_NoteOn(int voice, int pitch);
    LIBOPL_API void OPL_NoteOff(int voice);

    LIBOPL_API void OPL_SndOutput(int addr, int dataVal);

    LIBOPL_API void OPL_SndInit(int rate);
    LIBOPL_API void OPL_SndUpdateOne(short* buffer, int length);
    LIBOPL_API void OPL_SndShutdown();
    LIBOPL_API void OPL_SndReset();

#ifdef __cplusplus
}
#endif

#endif	//__LIBOPL_H_
