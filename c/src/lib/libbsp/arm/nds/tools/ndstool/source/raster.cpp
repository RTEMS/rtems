#include "ndstool.h"
#include "raster.h"


int CRaster::LoadBMP(char *filename)
{
	// open file
	FILE *f = fopen(filename, "rb");
	if (!f) { fprintf(stderr, "Could not open '%s'.\n", filename); return -1; }

	// load bitmap fileheader & infoheader
	BITMAPFILEHEADER bmfh;
	fread((char*)&bmfh, 1, sizeof(bmfh), f);
	BITMAPINFO bmi;
	fread((char*)&bmi, 1, sizeof(bmi), f);
	BITMAPINFOHEADER &bmih = bmi.bmiHeader;
	memcpy(palette, bmi.bmiColors, sizeof(palette));

	// check filetype signature
	if ((bmfh.bfType[0] != 'B') || (bmfh.bfType[1] != 'M')) { fprintf(stderr, "Not a bitmap file.\n"); return -2; }
	if (bmih.biBitCount > 8) { fprintf(stderr, "Bitmap must have a palette.\n"); return -3; }	

	// assign some short variables:
	width = bmih.biWidth;
	height = (bmih.biHeight > 0) ? (int)bmih.biHeight : -(int)bmih.biHeight; // absoulte value
	int pitch = width * bmih.biBitCount / 8;
	pitch += (4 - pitch%4) % 4;

	// load raster
	unsigned int pixelsPerByte = 8 / bmih.biBitCount;
	unsigned int biBitCount_mask = (1<<bmih.biBitCount)-1;
	raster = new unsigned char [width * height];
	for (unsigned int y=0; y<height; y++)
	{
		if (bmih.biHeight > 0)		// if height is positive the bmp is bottom-up, read it reversed
			fseek(f, bmfh.bfOffBits + pitch*(height-1-y), SEEK_SET);
		else
			fseek(f, bmfh.bfOffBits + pitch*y, SEEK_SET);
		for (unsigned int x=0; x<width; x+=pixelsPerByte)
		{
			unsigned char data = fgetc(f);
			unsigned int shift = 8;
			for (unsigned int p=0; p<pixelsPerByte; p++)
			{
				shift -= bmih.biBitCount;
				(*this)[y][x+p] = data >> shift & biBitCount_mask;
			}
		}
	}

	fclose(f);

	return 0;
}
