/*
costs per 4 pixels: (# = black)

1 bit:
----

4 bits:
#---
##--
--##
####
    
5 bits:
---#
#-##
-###
-#--
--#-
    
6 bits:
#-#-
-##-
###-
#--#
-#-#
##-#
*/

#include <stdio.h>

void LogoPackBits(unsigned char *srcp, unsigned char *destp)
{
	unsigned int destbit = 0;
	unsigned int destvalue = 0;
	unsigned int d = 0;
	for (unsigned int s=0; s<1664; s++)
	{
		if (srcp[s]) destvalue |= 1<<destbit;
		if (++destbit == 8)
		{
			destp[d++] = destvalue;
			destbit = 0;
			destvalue = 0;
		}
	}
}

struct LogoPattern
{
	unsigned int value;
	unsigned int bits;
} logoPatterns[16] =
{
	{0x01, 1},	{0x06, 4},	{0x0A, 5},	{0x04, 4},
	{0x02, 5},	{0x1E, 6},	{0x16, 6},	{0x06, 6},
	{0x06, 5},	{0x1F, 6},	{0x17, 6},	{0x07, 6},
	{0x02, 4},	{0x0E, 5},	{0x07, 5},	{0x00, 4},
};

int LogoCompress(unsigned char *src, unsigned char *dst)
{
	unsigned int data_out = 0;
	unsigned int outbit = 31;
	unsigned int outbitcnt = 0;
	
	for (int i=0; i<212; i++)
	{
		unsigned int data = *src++;
		for (int j=0; j<8; j+=4)
		{
			LogoPattern &lh = logoPatterns[data >> j & 0xF];
			outbitcnt += lh.bits;
			for (int b=lh.bits-1; b>=0; b--)
			{
				data_out |= (lh.value >> b & 1) << outbit;
				if (outbit == 0)
				{
					if (outbitcnt <= 156*8)
					{
						*dst++ = data_out >> 0;
						*dst++ = data_out >> 8;
						*dst++ = data_out >> 16;
						*dst++ = data_out >> 24;
					}
					outbit = 31;
					data_out = 0;
				}
				else
				{
					outbit--;
				}
			}
		}
	}

	if (outbit != 31)
	{
		if (outbitcnt <= 156*8)
		{
			*dst++ = data_out >> 0;
			*dst++ = data_out >> 8;
			*dst++ = data_out >> 16;
			*dst++ = data_out >> 24;
		}
	}

	return 156*8 - outbitcnt;
};

void LogoDiff(unsigned char *srcp, unsigned char *dstp)
{
	unsigned int *intp_dst = (unsigned int *)dstp;
	*intp_dst++ = 0xD0 << 8 | 0x80 | 2;	// header

	unsigned short *shortp_dst = (unsigned short *)intp_dst;
	unsigned short *shortp_src = (unsigned short *)srcp;
	unsigned short prev = 0;
    for (unsigned int i=0; i<0xD0; i+=2)
    {
        *shortp_dst++ = *shortp_src - prev;
        prev = *shortp_src++;
    }
}

int LogoConvert(unsigned char *srcp, unsigned char *dstp, unsigned char white)
{
	// convert to tiles
	unsigned char tiles[1664];
	for (int ty=0; ty<2; ty++)
	{
		for (int y=0; y<8; y++)
		{
			for (int tx=0; tx<13; tx++)
			{
				for (int x=0; x<8; x++)
				{
					tiles[(ty*13 + tx)*64 + y*8 + x] = (*srcp++ == white) ? 0 : 1;
				}
			}
		}
	}

	// bitpack
	unsigned char bitpacked[1664/8];
	LogoPackBits(tiles, bitpacked);

	// diff
	unsigned char diffed[4 + 1664/8];
	LogoDiff(bitpacked, diffed);

	// compress
	int r = LogoCompress(diffed, dstp);
	if (r < 0)
	{
		fprintf(stderr, "Compressed logo is %u bit(s) too big. Please simplify.\n", -r);
		return -1;
	}
	
	return 0;
}
