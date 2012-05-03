/*---------------------------------------------------------------------------------

  Copyright (C) 2005
			Jason Rogers (dovoto)
			Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

---------------------------------------------------------------------------------*/
#ifndef IMAGE_H
#define IMAGE_H

#include <nds/arm9/video.h>

//holds a rgb triplet
 typedef struct
 {
    unsigned char r,g,b;
 }__attribute__ ((packed)) RGB_24;

 //holds a basic image type for loading image files
 typedef struct
 {
    short height,width;
    int bpp;
    unsigned short* palette;

    union
    {
       unsigned char* data8;
       unsigned short* data16;
       unsigned int* data32;
    } image;

 } sImage, *psImage;

#ifdef __cplusplus
extern "C" {
#endif

void image24to16(sImage* img);
void image8to16(sImage* img);
void image8to16trans(sImage* img, u8 transperentColor);
void imageDestroy(sImage* img);
void imageTileData(sImage* img);

#ifdef __cplusplus
}
#endif


#include <nds/arm9/pcx.h>

#endif
