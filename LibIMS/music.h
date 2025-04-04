#pragma once

#include "music_types.h"

#ifdef __cplusplus
extern "C" {
#endif

    void ims_init(int freq);
    void ims_shutdown();
    IMS_MUSIC* prepare_music(char ims_path[], char iss_path[], char bnk_path[]);
    void free_music(IMS_MUSIC* music);
    int get_sample(IMS_MUSIC* music, int freq, int16_t* pcm_buffer, int buffer_len);

#ifdef __cplusplus
}
#endif