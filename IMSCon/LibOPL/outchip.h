#ifndef __OUTCHIP_H_
#define __OUTCHIP_H_

#ifdef __cplusplus
extern "C" {
#endif

void SndOutput(int addr, int dataVal);

void SndInit(int rate);
void SndUpdateOne(short *buffer, int length);
void SndShutdown();
void SndReset();

#ifdef __cplusplus
}
#endif

#endif	//__OUTCHIP_H_

