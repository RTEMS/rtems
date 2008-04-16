#pragma pack(2)

typedef struct tagRGBQUAD
{
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER
{
	unsigned_int biSize;
	signed_int biWidth;
	signed_int biHeight;
	unsigned_short biPlanes;
	unsigned_short biBitCount;
	unsigned_int biCompression;
	unsigned_int biSizeImage;
	signed_int biXPelsPerMeter;
	signed_int biYPelsPerMeter;
	unsigned_int biClrUsed;
	unsigned_int biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[256];
} BITMAPINFO, *PBITMAPINFO;

typedef struct tagBITMAPFILEHEADER
{
	unsigned char bfType[2];
	unsigned_int bfSize;
	unsigned_short bfReserved1;
	unsigned_short bfReserved2;
	unsigned_int bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

#pragma pack()


struct CRaster
{
	unsigned int width, height;
	unsigned char *raster;
	RGBQUAD palette[256];

	int LoadBMP(char *filename);

	unsigned char * operator [] (unsigned int y)
	{
		return raster + y*width;
	}
};
