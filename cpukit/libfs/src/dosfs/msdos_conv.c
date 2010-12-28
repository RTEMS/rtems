/*
 * Adaptation of NetBSD code for RTEMS by Victor V. Vengerov <vvv@oktet.ru>
 */
/*	$NetBSD: msdosfs_conv.c,v 1.10 1994/12/27 18:36:24 mycroft Exp $	*/
/*
 * Written by Paul Popelka (paulp@uts.amdahl.com)
 *
 * You can do anything you want with this software, just don't say you wrote
 * it, and don't remove this notice.
 *
 * This software is provided "as is".
 *
 * The author supplies this software to be publicly redistributed on the
 * understanding that the author is not responsible for the correct
 * functioning of this software in any circumstances and is not liable for
 * any damages caused by this software.
 *
 * October 1992
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include "msdos.h"

/* #define SECONDSPERDAY (24 * 60 * 60) */
#define SECONDSPERDAY ((uint32_t) 86400)

/*
 * Days in each month in a regular year.
 */
static uint16_t regyear[] = {
	31, 28, 31, 30, 31, 30,
	31, 31, 30, 31, 30, 31
};

/*
 * Days in each month in a leap year.
 */
static uint16_t leapyear[] = {
	31, 29, 31, 30, 31, 30,
	31, 31, 30, 31, 30, 31
};

/*
 * Variables used to remember parts of the last time conversion.  Maybe we
 * can avoid a full conversion.
 */
static uint32_t lasttime;
static uint32_t lastday;
static uint16_t lastddate;
static uint16_t lastdtime;

/*
 * Convert the unix version of time to dos's idea of time to be used in
 * file timestamps. The passed in unix time is assumed to be in GMT.
 */
void
msdos_date_unix2dos(unsigned int t, uint16_t *ddp,
                    uint16_t *dtp)
{
	uint32_t days;
	uint32_t inc;
	uint32_t year;
	uint32_t month;
	uint16_t *months;

	/*
	 * If the time from the last conversion is the same as now, then
	 * skip the computations and use the saved result.
	 */
	if (lasttime != t) {
		lasttime = t;
		lastdtime = (((t % 60) >> 1) << MSDOS_DT_2SECONDS_SHIFT)
		    + (((t / 60) % 60) << MSDOS_DT_MINUTES_SHIFT)
		    + (((t / 3600) % 24) << MSDOS_DT_HOURS_SHIFT);

		/*
		 * If the number of days since 1970 is the same as the last
		 * time we did the computation then skip all this leap year
		 * and month stuff.
		 */
		days = t / (SECONDSPERDAY);
		if (days != lastday) {
			lastday = days;
			for (year = 1970;; year++) {
				inc = year & 0x03 ? 365 : 366;
				if (days < inc)
					break;
				days -= inc;
			}
			months = year & 0x03 ? regyear : leapyear;
			for (month = 0; month < 12; month++) {
				if (days < months[month])
					break;
				days -= months[month];
			}
			lastddate = ((days + 1) << MSDOS_DD_DAY_SHIFT)
			    + ((month + 1) << MSDOS_DD_MONTH_SHIFT);
			/*
			 * Remember dos's idea of time is relative to 1980.
			 * unix's is relative to 1970.  If somehow we get a
			 * time before 1980 then don't give totally crazy
			 * results.
			 */
			if (year > 1980)
				lastddate += (year - 1980) <<
				             MSDOS_DD_YEAR_SHIFT;
		}
	}
	*dtp = lastdtime;
	*ddp = lastddate;
}

/*
 * The number of days between Jan 1, 1970 and Jan 1, 1980. In that
 * interval there were 8 regular years and 2 leap years.
 */
/* #define	DAYSTO1980	((8 * 365) + (2 * 366)) */
#define DAYSTO1980   ((uint32_t) 3652)

static uint16_t lastdosdate;
static uint32_t lastseconds;

/*
 * Convert from dos' idea of time to unix'. This will probably only be
 * called from the stat(), and fstat() system calls and so probably need
 * not be too efficient.
 */
unsigned int
msdos_date_dos2unix(unsigned int dd, unsigned int dt)
{
	uint32_t seconds;
	uint32_t m, month;
	uint32_t y, year;
	uint32_t days;
	uint16_t *months;

	seconds = 2 * ((dt & MSDOS_DT_2SECONDS_MASK) >> MSDOS_DT_2SECONDS_SHIFT)
	    + ((dt & MSDOS_DT_MINUTES_MASK) >> MSDOS_DT_MINUTES_SHIFT) * 60
	    + ((dt & MSDOS_DT_HOURS_MASK) >> MSDOS_DT_HOURS_SHIFT) * 3600;
	/*
	 * If the year, month, and day from the last conversion are the
	 * same then use the saved value.
	 */
	if (lastdosdate != dd) {
		lastdosdate = dd;
		days = 0;
		year = (dd & MSDOS_DD_YEAR_MASK) >> MSDOS_DD_YEAR_SHIFT;
		for (y = 0; y < year; y++)
			days += y & 0x03 ? 365 : 366;
		months = year & 0x03 ? regyear : leapyear;
		/*
		 * Prevent going from 0 to 0xffffffff in the following
		 * loop.
		 */
		month = (dd & MSDOS_DD_MONTH_MASK) >> MSDOS_DD_MONTH_SHIFT;
		if (month == 0) {
			month = 1;
		}
		for (m = 0; m < month - 1; m++)
			days += months[m];
		days += ((dd & MSDOS_DD_DAY_MASK) >> MSDOS_DD_DAY_SHIFT) - 1;
		lastseconds = (days + DAYSTO1980) * SECONDSPERDAY;
	}
	return seconds + lastseconds;
}

static const uint8_t msdos_map[] = {
    0,    0,    0,    0,    0,    0,    0,    0,    /* 00-07 */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 08-0f */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 10-17 */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 18-1f */
    0,    '!',  0,    '#',  '$',  '%',  '&',  '\'', /* 20-27 */
    '(',  ')',  0,    '+',  0,    '-',  0,    0,    /* 28-2f */
    '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  /* 30-37 */
    '8',  '9',  0,    0,    0,    0,    0,    0,    /* 38-3f */
    '@',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  /* 40-47 */
    'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',  /* 48-4f */
    'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  /* 50-57 */
    'X',  'Y',  'Z',  0,    0,    0,    '^',  '_',  /* 58-5f */
    '`',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  /* 60-67 */
    'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',  /* 68-6f */
    'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  /* 70-77 */
    'X',  'Y',  'Z',  '{',  0,    '}',  '~',  0,    /* 78-7f */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 80-87 */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 88-8f */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 90-97 */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 98-9f */
    0,    0xad, 0xbd, 0x9c, 0xcf, 0xbe, 0xdd, 0xf5, /* a0-a7 */
    0xf9, 0xb8, 0xa6, 0xae, 0xaa, 0xf0, 0xa9, 0xee, /* a8-af */
    0xf8, 0xf1, 0xfd, 0xfc, 0xef, 0xe6, 0xf4, 0xfa, /* b0-b7 */
    0xf7, 0xfb, 0xa7, 0xaf, 0xac, 0xab, 0xf3, 0xa8, /* b8-bf */
    0xb7, 0xb5, 0xb6, 0xc7, 0x8e, 0x8f, 0x92, 0x80, /* c0-c7 */
    0xd4, 0x90, 0xd2, 0xd3, 0xde, 0xd6, 0xd7, 0xd8, /* c8-cf */
    0xd1, 0xa5, 0xe3, 0xe0, 0xe2, 0xe5, 0x99, 0x9e, /* d0-d7 */
    0x9d, 0xeb, 0xe9, 0xea, 0x9a, 0xed, 0xe8, 0xe1, /* d8-df */
    0xb7, 0xb5, 0xb6, 0xc7, 0x8e, 0x8f, 0x92, 0x80, /* e0-e7 */
    0xd4, 0x90, 0xd2, 0xd3, 0xde, 0xd6, 0xd7, 0xd8, /* e8-ef */
    0xd1, 0xa5, 0xe3, 0xe0, 0xe2, 0xe5, 0x99, 0xf6, /* f0-f7 */
    0x9d, 0xeb, 0xe9, 0xea, 0x9a, 0xed, 0xe8, 0x98, /* f8-ff */
#if OLD_TABLE
/* 00 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 08 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 10 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 18 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 20 */ 0x00, 0x21, 0x00, 0x23, 0x24, 0x25, 0x26, 0x27, /*  !"#$%&' */
/* 28 */ 0x28, 0x29, 0x00, 0x00, 0x00, 0x2D, 0x2E, 0x00, /* ()*+,-./ */
/* 30 */ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 01234567 */
/* 38 */ 0x38, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 89:;<=>? */
/* 40 */ 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* @ABCDEFG */
/* 48 */ 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, /* HIJKLMNO */
/* 50 */ 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* PQRSTUVW */
/* 58 */ 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x00, 0x5E, 0x5F, /* XYZ[\]^_ */
/* 60 */ 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* `abcdefg */
/* 68 */ 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, /* hijklmno */
/* 70 */ 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* pqrstuvw */
/* 78 */ 0x58, 0x59, 0x5A, 0x5B, 0x7C, 0x00, 0x7E, 0x00, /* xyz{|}~  */
/* 80 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 88 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 90 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 98 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* A0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* A8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* B0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* B8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* C0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* C8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* D0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* D8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* E0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* E8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* F0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* F8 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#endif
};
/*
 * Convert a unix filename to a DOS filename. Return -1 if wrong name is
 * supplied.
 */
int
msdos_filename_unix2dos(const char *un, int unlen, char *dn)
{
	int i;
	uint8_t c;

	/*
	 * Fill the dos filename string with blanks. These are DOS's pad
	 * characters.
	 */
	for (i = 0; i <= 10; i++)
		dn[i] = ' ';

	/*
	 * The filenames "." and ".." are handled specially, since they
	 * don't follow dos filename rules.
	 */
	if (un[0] == '.' && unlen == 1) {
		dn[0] = '.';
		return 0;
	}
	if (un[0] == '.' && un[1] == '.' && unlen == 2) {
		dn[0] = '.';
		dn[1] = '.';
		return 0;
	}

  /*
   * Remove any dots from the start of a file name.
   */
	while (unlen && (*un == '.')) {
		un++;
		unlen--;
	}

	/*
	 * Copy the unix filename into the dos filename string upto the end
	 * of string, a '.', or 8 characters. Whichever happens first stops
	 * us. This forms the name portion of the dos filename. Fold to
	 * upper case.
	 */
	for (i = 0; i <= 7 && unlen && (c = *un) && c != '.'; i++) {
    if (msdos_map[c] == 0)
      break;
		dn[i] = msdos_map[c];
		un++;
		unlen--;
	}

	/*
	 * Strip any further characters up to a '.' or the end of the
	 * string.
	 */
	while (unlen && (c = *un)) {
		un++;
		unlen--;
		/* Make sure we've skipped over the dot before stopping. */
		if (c == '.')
			break;
	}

	/*
	 * Copy in the extension part of the name, if any. Force to upper
	 * case. Note that the extension is allowed to contain '.'s.
	 * Filenames in this form are probably inaccessable under dos.
	 */
	for (i = 8; i <= 10 && unlen && (c = *un); i++) {
    if (msdos_map[c] == 0)
      break;
    dn[i] = msdos_map[c];
		un++;
		unlen--;
	}
	return 0;
}
