#pragma pack(1)

struct Banner
{
	unsigned_short version;
	unsigned_short crc;
	unsigned char reserved[28];
	unsigned char tile_data[4][4][8][4];
	unsigned_short palette[16];
	unsigned_short title[6][128];		// max. 3 lines. seperated by linefeed character
};

#pragma pack()

extern const char *bannerLanguages[];

int InsertTitleString(char *String, FILE *file);
unsigned short CalcBannerCRC(Banner &banner);
void IconFromBMP();
