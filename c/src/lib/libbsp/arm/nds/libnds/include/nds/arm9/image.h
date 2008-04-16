/*---------------------------------------------------------------------------------
	$Id$


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

	$Log$
	Revision 1.5  2007/01/19 14:45:59  wntrmute
	name anonymous structs and unions for -std=c99
	
	Revision 1.4  2005/10/11 05:05:26  dovoto
	Added imageTileData(sImage* img) to allow loading of pcx as sprite data.
	Updated pcx.c to set image bit per pixel field
	
	Revision 1.3  2005/08/30 17:56:21  wntrmute
	add video.h
	
	Revision 1.2  2005/08/11 14:53:00  desktopman
	Added image24to16(sImage* img)

	Revision 1.1  2005/07/27 02:20:52  wntrmute
	add image functions


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
