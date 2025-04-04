#include "framework.h"
#include "LibIMS.h"

#include "music.h"


void IMS_Init(int freq)
{
    ims_init(freq);
}

void IMS_Shutdown()
{
    ims_shutdown();
}

IMS_MUSIC* IMS_PrepareMusic(char ims_path[], char iss_path[], char bnk_path[])
{
    return prepare_music(ims_path, iss_path, bnk_path);
}

void IMS_FreeMusic(IMS_MUSIC* music)
{
    free_music(music);
}

int IMS_GetSample(IMS_MUSIC* music, int freq, int16_t* pcm_buffer, int buffer_len)
{
    return get_sample(music, freq, pcm_buffer, buffer_len);
}