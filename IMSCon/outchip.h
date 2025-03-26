#ifndef __OUTCHIP_H_
#define __OUTCHIP_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void *ym3812p;

void SndOutput(int addr, int dataVal);

#ifdef __cplusplus
}
#endif

#endif	//__OUTCHIP_H_

