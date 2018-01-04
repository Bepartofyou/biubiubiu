/*
 * pngdecoder.h
 *
 *  Created on: 2017年12月28日
 *      Author: mj
 */

#ifndef _PNG_DECODER_H_
#define _PNG_DECODER_H_

#include <stdint.h>
#include <stdio.h>
#include <png.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct PNGHandle{
	uint8_t* data;
	uint32_t length;
	uint32_t width;
	uint32_t height;
}PNGHandle;

PNGHandle* mallocPngHandle();
void freePngHandle(PNGHandle*);
int decodePng(const char*, PNGHandle*);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _PNG_DECODER_H_ */
