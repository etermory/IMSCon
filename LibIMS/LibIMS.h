#ifndef __LIBIMS_H_
#define __LIBOPL_H_

#ifdef LIBIMS_EXPORTS
#define LIBIMS_API __declspec(dllexport)
#else
#define LIBIMS_API __declspec(dllimport)
#endif

#include "music_types.h"

#ifdef __cplusplus
extern "C" {
#endif

    LIBIMS_API void IMS_Init(int freq);
    LIBIMS_API void IMS_Shutdown();
    LIBIMS_API IMS_MUSIC* IMS_PrepareMusic(char ims_path[], char iss_path[], char bnk_path[]);
    LIBIMS_API void IMS_FreeMusic(IMS_MUSIC* music);
    LIBIMS_API int IMS_GetSample(IMS_MUSIC* music, int freq, int16_t* pcm_buffer, int buffer_len);

#ifdef __cplusplus
}
#endif

#endif	//__LIBIMS_H_
