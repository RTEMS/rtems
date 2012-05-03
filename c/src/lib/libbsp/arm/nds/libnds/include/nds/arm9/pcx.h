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

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#ifndef PCX_H
#define PCX_H

typedef struct
{
   char         manufacturer;   //should be 0
   char         version;        //should be 5
   char         encoding;       //should be 1
   char         bitsPerPixel; //should be 8
   short int    xmin,ymin;      //coordinates for top left,bottom right
   short int    xmax,ymax;
   short int    hres;           //resolution
   short int    vres;
   char         palette16[48];  //16 color palette if 16 color image
   char         reserved;       //ignore
   char         colorPlanes;   //ignore
   short int    bytesPerLine;
   short int    paletteYype;   //should be 2
   char         filler[58];     //ignore
}__attribute__ ((packed)) PCXHeader, *pPCXHeader;

#ifdef __cplusplus
extern "C" {
#endif

int loadPCX(unsigned char* pcx, sImage* image);

#ifdef __cplusplus
}
#endif

#endif

