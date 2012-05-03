/*---------------------------------------------------------------------------------

	Copyright (C) 2005
		Michael Noland (joat)
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
#include <nds/jtypes.h>
#include <nds/arm9/image.h>
#include <nds/dma.h>


#include <malloc.h>

//---------------------------------------------------------------------------------
void image24to16(sImage* img) {
//---------------------------------------------------------------------------------

	int x;
	int y;

	u16* temp = (u16*)malloc(img->height*img->width*2);

	for(y=0;y<img->height;y++)
	{
		for(x=0;x<img->width;x++)
			temp[x+y*img->width]=(1<<15)|RGB15(img->image.data8[x*3+y*img->width*3]>>3, \
			img->image.data8[x*3+y*img->width*3+1]>>3, img->image.data8[x*3+y*img->width*3+2]>>3);
	}

	free(img->image.data8);

	img->bpp=16;
	img->image.data16 = temp;
}

//---------------------------------------------------------------------------------
void image8to16(sImage* img) {
//---------------------------------------------------------------------------------
	int i;

	u16* temp = (u16*)malloc(img->height*img->width*2);

	for(i = 0; i < img->height * img->width; i++)
		temp[i] = img->palette[img->image.data8[i]] | (1<<15);

	free (img->image.data8);
	free (img->palette);

	img->bpp = 16;
	img->image.data16 = temp;
}

//---------------------------------------------------------------------------------
void image8to16trans(sImage* img, u8 transparentColor) {
//---------------------------------------------------------------------------------
	int i;
	u8 c;

	u16* temp = (u16*)malloc(img->height*img->width*2);

	for(i = 0; i < img->height * img->width; i++) {

		c = img->image.data8[i];

		if(c != transparentColor)
			temp[i] = img->palette[c] | (1<<15);
		else
			temp[i] = img->palette[c];
	}

	free (img->image.data8);
	free (img->palette);

	img->bpp = 16;
	img->image.data16 = temp;
}
//---------------------------------------------------------------------------------
void imageTileData(sImage* img) {
//---------------------------------------------------------------------------------
	u32* temp;

	int ix, iy, tx, ty;

	int th, tw;

	int i = 0;

	//can only tile 8 bit data that is a multiple of 8 in dimention
	if(img->bpp != 8 || (img->height & 3) != 0 || (img->width & 3) != 0) return;

	th = img->height >> 3;
	tw = img->width >> 3;

	//buffer to hold data
	temp = (u32*)malloc(img->height * img->width);

	for(ty = 0; ty < th; ty++)
		for(tx = 0; tx < tw; tx++)
			for(iy = 0; iy < 8; iy++)
				for(ix = 0; ix < 2; ix++)
					temp[i++] = img->image.data32[ix + tx * 2 + (iy + ty * 8) * tw * 2 ];

	free(img->image.data32);

	img->image.data32 = temp;
}

//---------------------------------------------------------------------------------
void imageDestroy(sImage* img) {
//---------------------------------------------------------------------------------
	if(img->image.data8) free (img->image.data8);
	if(img->palette && img->bpp == 8) free (img->palette);
}
