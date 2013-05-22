/**
 * @file
 *
 * @brief MDOS Date Conversion
 * @ingroup libfs_msdos MSDOS FileSystem
 */

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
 * Adaptation of NetBSD code for RTEMS by Victor V. Vengerov <vvv@oktet.ru>
 * $NetBSD: msdosfs_conv.c,v 1.10 1994/12/27 18:36:24 mycroft Exp $
 *
 * October 1992
 *
 * Modifications to support UTF-8 in the file system are
 * Copyright (c) 2013 embedded brains GmbH.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <rtems.h>
#include "msdos.h"

/* #define SECONDSPERDAY (24 * 60 * 60) */
#define SECONDSPERDAY ((uint32_t) 86400)

#define UTF8_MAX_CHAR_SIZE    4
#define UTF8_NULL             0x00
#define UTF8_NULL_SIZE        1
#define UTF8_BLANK            0x20
#define UTF8_BLANK_SIZE       1
#define UTF8_FULL_STOP        0x2e
#define UTF8_FULL_STOP_SIZE   1

#define UTF16_MAX_CHAR_SIZE   4
#define UTF16_NULL            CT_LE_W( 0x0000 )
#define UTF16_NULL_SIZE       2
#define UTF16_BLANK           CT_LE_W( 0x0020 )
#define UTF16_BLANK_SIZE      2
#define UTF16_FULL_STOP       CT_LE_W( 0x002e )
#define UTF16_FULL_STOP_SIZE  2

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


static const uint8_t codepage_valid_char_map[] = {
    0,    0,    0,    0,    0,    0,    0,    0,    /* 00-07 */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 08-0f */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 10-17 */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 18-1f */
    0x20, 0x21, 0,    0x23, 0x24, 0x25, 0x26, 0x27, /* 20-27 */
    0x28, 0x29, 0,    0,    0,    0x2d,  0,    0,   /* 28-2f */
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 30-37 */
    0x38, 0x39, 0,    0,    0,    0,    0,    0,    /* 38-3f */
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* 40-47 */
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* 48-4f */
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* 50-57 */
    0x58, 0x59, 0x5a, 0,    0,    0,    0x5e, 0x5f, /* 58-5f */
    0x60, 0,    0,    0,    0,    0,    0,    0,    /* 60-67 */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 68-6f */
    0,    0,    0,    0,    0,    0,    0,    0,    /* 70-77 */
    0,    0,    0,    0x7b, 0,    0x7d, 0x7e, 0,    /* 78-7f */
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, /* 80-87 */
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, /* 88-8f */
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, /* 90-97 */
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, /* 98-9f */
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, /* a0-a7 */
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, /* a8-af */
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, /* b0-b7 */
    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, /* b8-bf */
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, /* c0-c7 */
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, /* c8-cf */
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, /* d0-d7 */
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, /* d8-df */
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, /* e0-e7 */
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, /* e8-ef */
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, /* f0-f7 */
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  /* f8-ff */
};

static uint16_t
msdos_get_valid_utf16_filename_character (const uint16_t utf16_character)
{
  uint16_t retval    = 0x0000;
  uint16_t char_num  = CF_LE_W( utf16_character );

  if ( char_num <= 0x00ff ) {
    switch ( char_num )
    {
      case 0x002b: /* '+' */
      case 0x002c: /* ',' */
      case 0x002e: /* '.' */
      case 0x003b: /* ';' */
      case 0x003d: /* '=' */
      case 0x005b: /* '[' */
      case 0x005d: /* ']' */
      case 0x0061: /* 'a' */
      case 0x0062: /* 'b' */
      case 0x0063: /* 'c' */
      case 0x0064: /* 'd' */
      case 0x0065: /* 'e' */
      case 0x0066: /* 'f' */
      case 0x0067: /* 'g' */
      case 0x0068: /* 'h' */
      case 0x0069: /* 'i' */
      case 0x006a: /* 'j' */
      case 0x006b: /* 'k' */
      case 0x006c: /* 'l' */
      case 0x006d: /* 'm' */
      case 0x006e: /* 'n' */
      case 0x006f: /* 'o' */
      case 0x0070: /* 'p' */
      case 0x0071: /* 'q' */
      case 0x0072: /* 'r' */
      case 0x0073: /* 's' */
      case 0x0074: /* 't' */
      case 0x0075: /* 'u' */
      case 0x0076: /* 'v' */
      case 0x0077: /* 'w' */
      case 0x0078: /* 'x' */
      case 0x0079: /* 'y' */
      case 0x007a: /* 'z' */
        retval = char_num;
      break;
      default:
        retval = codepage_valid_char_map[char_num];
      break;
    }
  }
  else
    retval = char_num;

  return CT_LE_W( retval );
}

static char
msdos_get_valid_codepage_filename_character (const uint8_t character)
{
  return codepage_valid_char_map[(unsigned int)character];
}

static ssize_t
msdos_filename_process_dot_names (const uint8_t *src_name,
                                  const size_t   src_size,
                                  uint8_t       *dest_name,
                                  const size_t   dest_size)
{
  ssize_t returned_size = 0;
  int     eno           = 0;
  /*
    * The filenames "." and ".." are handled specially, since they
    * don't follow dos filename rules.
    */
   if (    src_name[0] == UTF8_FULL_STOP
        && src_size    == UTF8_FULL_STOP_SIZE) {
     if (dest_size >= UTF8_FULL_STOP_SIZE) {
       dest_name[0]  = UTF8_FULL_STOP;
       returned_size = UTF8_FULL_STOP_SIZE;
     }
     else
       eno = ENAMETOOLONG;
   }
   else if (    eno           == 0
             && src_name[0]   == UTF8_FULL_STOP
             && src_name[1]   == UTF8_FULL_STOP
             && src_size      == ( 2 * UTF8_FULL_STOP_SIZE ) ) {
     if (dest_size >= 2 * UTF8_FULL_STOP_SIZE) {
       dest_name[0]  = UTF8_FULL_STOP;
       dest_name[1]  = UTF8_FULL_STOP;
       returned_size = 2 * UTF8_FULL_STOP_SIZE;
     }
     else
       eno = ENAMETOOLONG;
   }

   if (eno != 0) {
     errno         = eno;
     returned_size = -1;
   }

   return returned_size;
}

static ssize_t
msdos_filename_delete_trailing_dots (const uint8_t *filename_utf8,
                                     const size_t   filename_size)
{
  ssize_t      size_returned = filename_size;
  unsigned int i;

  /*
   * Remove any dots from the end of a file name.
   */
  for ( i = size_returned - UTF8_FULL_STOP_SIZE;
           size_returned >= UTF8_FULL_STOP_SIZE
        && filename_utf8[i] == UTF8_FULL_STOP;) {
    size_returned -= UTF8_FULL_STOP_SIZE;
    i             -= UTF8_FULL_STOP_SIZE;
  }

  return size_returned;
}

ssize_t
msdos_filename_utf8_to_long_name_for_compare (
    rtems_dosfs_convert_control     *converter,
    const uint8_t                   *utf8_name,
    const size_t                     utf8_name_size,
    uint8_t                         *long_name,
    const size_t                     long_name_size)
  {
    ssize_t        returned_size = 0;
    int            eno           = 0;
    size_t         name_size;
    size_t         dest_size     = long_name_size;

    returned_size = msdos_filename_process_dot_names (
      utf8_name,
      utf8_name_size,
      long_name,
      long_name_size);

    if (returned_size == 0) {
      name_size = msdos_filename_delete_trailing_dots (
        &utf8_name[0],
        utf8_name_size);
      if (name_size > 0) {
        eno = (*converter->handler->utf8_normalize_and_fold) (
          converter,
          utf8_name,
          name_size,
          long_name,
          &dest_size);
        if (eno == 0) {
          returned_size = (ssize_t)dest_size;
        }
      } else {
        eno = EINVAL;
      }
    }

    if ( eno != 0 ) {
      errno         = eno;
      returned_size = -1;
    }

    return returned_size;
  }

ssize_t
msdos_filename_utf8_to_long_name_for_save (
    rtems_dosfs_convert_control     *converter,
    const uint8_t                   *utf8_name,
    const size_t                     utf8_name_size,
    uint16_t                        *long_name,
    const size_t                     long_name_size)
{
    ssize_t      returned_size = 0;
    int          eno           = 0;
    size_t       name_size     = utf8_name_size;
    size_t       name_size_tmp = long_name_size / MSDOS_NAME_LFN_BYTES_PER_CHAR;
    int          i;
    uint16_t     c;
    unsigned int chars_written;

    name_size_tmp = long_name_size;
    name_size = msdos_filename_delete_trailing_dots (
      &utf8_name[0],
      utf8_name_size);
    if (name_size > 0) {
      /*
       * Finally convert from UTF-8 to UTF-16
       */
      eno = (*converter->handler->utf8_to_utf16) (
          converter,
          utf8_name,
          name_size,
          &long_name[0],
          &name_size_tmp);
      if (eno == 0) {
        if (name_size_tmp <= (MSDOS_NAME_MAX_LNF_LEN * MSDOS_NAME_LFN_BYTES_PER_CHAR))
          name_size = name_size_tmp;
        else
          eno = ENAMETOOLONG;
      }

      if ( eno == 0 )
      {
        /*
         * Validate the characters and assign them to the UTF-16 file name
         */
        for ( i = 0;
                 name_size
              && (c = msdos_get_valid_utf16_filename_character ( long_name[i]) );
              ++i ) {
          long_name[i]   = c;
          returned_size += MSDOS_NAME_LFN_BYTES_PER_CHAR;
          name_size     -= MSDOS_NAME_LFN_BYTES_PER_CHAR;
        }
        if ( name_size == UTF16_NULL_SIZE && c == UTF16_NULL ) {
          long_name[i]   = c;
          returned_size += MSDOS_NAME_LFN_BYTES_PER_CHAR;
          name_size     -= MSDOS_NAME_LFN_BYTES_PER_CHAR;
        }
        else if ( name_size != 0 )
          eno = EINVAL;
        chars_written = returned_size / MSDOS_NAME_LFN_BYTES_PER_CHAR;
        if (   long_name [chars_written - 1] != UTF16_NULL
            && (returned_size + UTF16_NULL_SIZE ) <= long_name_size ) {
          long_name[chars_written] = UTF16_NULL;
        }
      }
    }
    else
      eno = EINVAL;

    if ( eno != 0 ) {
      errno         = eno;
      returned_size = -1;
    }

    return returned_size;
  }

/*
 * Remove any dots from the start of a file name.
 */
static void msdos_filename_remove_prepended_dots (const uint8_t **name_utf8,
                                                  size_t         *name_size)
{
  while (    *name_size >= UTF8_FULL_STOP_SIZE
         && **name_utf8 == UTF8_FULL_STOP) {
    *name_utf8  += UTF8_FULL_STOP_SIZE;
    *name_size  -= UTF8_FULL_STOP_SIZE;
  }
}

ssize_t
msdos_filename_utf8_to_short_name_for_compare (
    rtems_dosfs_convert_control     *converter,
    const uint8_t                   *utf8_name,
    const size_t                     utf8_name_size,
    void                            *short_name,
    const size_t                     short_name_size)
{
  ssize_t        returned_size           = 0;
  int            eno                     = 0;
  const uint8_t *name_ptr                = utf8_name;
  char          *dest_ptr                = (char*)short_name;
  size_t         name_size               = utf8_name_size;
  uint8_t        name_normalized_buf[(MSDOS_SHORT_NAME_LEN +1) * MSDOS_NAME_MAX_UTF8_BYTES_PER_CHAR];
  size_t         name_size_tmp           = sizeof(name_normalized_buf);

  returned_size = msdos_filename_process_dot_names (
    utf8_name,
    utf8_name_size,
    short_name,
    short_name_size);

  if (returned_size == 0) {
    msdos_filename_remove_prepended_dots (&name_ptr,
                                          &name_size);
    if (name_size > 0) {
      /*
       * Normalize the name and convert to lower case
       */
      eno = (*converter->handler->utf8_normalize_and_fold) (
        converter,
        name_ptr,
        name_size,
        &name_normalized_buf[0],
        &name_size_tmp);
      name_ptr  = &name_normalized_buf[0];
      name_size = name_size_tmp;
      if ( eno == ENOMEM ) {
        eno = 0;
      }
      if ( eno == 0 ) {
        memcpy (&dest_ptr[0], &name_ptr[0], name_size);
        returned_size = name_size;
      }
    } else
      eno = EINVAL;
  }

  if ( eno != 0 ) {
    errno         = eno;
    returned_size = -1;
  }

  return returned_size;
}

ssize_t
msdos_filename_utf8_to_short_name_for_save (
    rtems_dosfs_convert_control     *converter,
    const uint8_t                   *utf8_name,
    const size_t                     utf8_name_size,
    void                            *short_name,
    const size_t                     short_name_size)
{
  ssize_t        returned_size           = 0;
  int            eno                     = 0;
  const uint8_t *name_ptr                = utf8_name;
  size_t         name_size               = utf8_name_size;
  char          *dest_ptr                = (char*)short_name;
  unsigned int   i;
  char           c;
  size_t         name_size_tmp;
  char           name_to_format_buf[MSDOS_SHORT_NAME_LEN +1];

  returned_size = msdos_filename_process_dot_names (
    utf8_name,
    utf8_name_size,
    short_name,
    short_name_size);

  if (returned_size == 0) {
    msdos_filename_remove_prepended_dots (&name_ptr,
                                          &name_size);

    if (name_size > 0) {
      /*
       * Finally convert from UTF-8 to codepage
       */
      name_size_tmp = sizeof ( name_to_format_buf );
      eno = (*converter->handler->utf8_to_codepage) (
        converter,
        name_ptr,
        name_size,
        &name_to_format_buf[0],
        &name_size_tmp);
      if ( eno != 0 ) {
        /* The UTF-8 name my well be long name, for which we now want to
         * generate the corresponding short name. Under these circumstances
         * eno != 0 likely simply means that the UTF-8 name is longer than 11 characters
         * or that it contains unicode characters which can not be converted to the code page
         * in a reversible way. Non-reversible characters will be represented by question mark
         * characters. Later in this method they will get replaced by underline characters.
         */
        eno = 0;
      }
      name_ptr  = (const uint8_t *)(&name_to_format_buf[0]);
      name_size = name_size_tmp;
      for (i = 0; i < name_size; ++i)
        name_to_format_buf[i] = toupper ( (unsigned char)(name_to_format_buf[i]) );
      /*
       * Validate the characters and assign them to the codepage file name
       */
      if ( name_size > 0 ) {
        /*
         * The first character needs some special treatment
         */
        if ( 0x20 == *name_ptr )
          dest_ptr[0] = '_';
        else if ( 0xE5 == *name_ptr )
          dest_ptr[0] = 0x05;
        else if (0 != (c = msdos_get_valid_codepage_filename_character( *name_ptr ) ) )
          dest_ptr[0] = c;
        else
          dest_ptr[0] = '_';
        ++name_ptr;
        ++returned_size;
        --name_size;
        /*
         * Validate and assign all other characters of the name part
         */
        for (i = 1; i <= 7 && name_size && *name_ptr != '.'; ++i) {
          c = msdos_get_valid_codepage_filename_character ( *name_ptr );
          if (c != 0)
            dest_ptr[i] = c;
          else
            dest_ptr[i] = '_';
          ++name_ptr;
          ++returned_size;
          --name_size;
        }
        /*
         * Strip any further characters up to a '.' or the end of the
         * string.
         */
        if ( *name_ptr == '.' ) {
          ++name_ptr;
          --name_size;
        }

        for (; i < 8; ++i) {
          dest_ptr[i] = ' ';
          ++returned_size;
        }

        /*
         * Copy in the extension part of the name, if any.
         */
        for (; i <= 10 && name_size ; i++) {
          c = msdos_get_valid_codepage_filename_character ( *name_ptr);
          if (c != 0)
            dest_ptr[i] = c;
          else
            dest_ptr[i] = '_';
          ++name_ptr;
          ++returned_size;
          name_size--;
        }
        /*
         * Fill up with blanks. These are DOS's pad characters.
         */
        for ( ; i < short_name_size; ++i ) {
          dest_ptr[i] = ' ';
          ++returned_size;
        }
      }
    }
    else
      eno = EINVAL;
  }

  if ( eno != 0 ) {
    errno = eno;
    return -1;
  }

  return returned_size;
}


