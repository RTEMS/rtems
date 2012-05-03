/* Driver for the Maxim 1375 i2c RTC (TOD only; very simple...) */

/*
 * Authorship
 * ----------
 * This software was created by
 *
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 *      Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 *      under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

/* This driver uses the file-system interface to the i2c bus */

#include <unistd.h> /* write, read, close */

#include <rtems.h>
#include <rtems/rtc.h>
#include <libchip/rtc.h>
#include <libchip/ds1375-rtc.h>

#include <sys/fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <inttypes.h>


#define STATIC static
#undef  DEBUG

/* The RTC driver routines are possibly called during
 * system initialization -- that would be prior to opening
 * the console. At this point it is not safe to use stdio
 * (printf, perror etc.).
 * Our file descriptors may even be 0..2
 */
#define STDIOSAFE(fmt,args...)        \
  do {                                \
    if ( _System_state_Is_up( _System_state_Get() ) ) { \
      fprintf(stderr,fmt,args);   \
    } else {                        \
      printk(fmt,args);           \
    }                               \
  } while (0)


STATIC uint8_t ds1375_bcd2bin(uint8_t x)
{
  uint8_t h = x & 0xf0;

  /* 8*hi + 2*hi + lo */
  return ( h >> 1 ) + ( h >> 3 ) + ( x & 0xf );
}

STATIC uint8_t ds1375_bin2bcd(uint8_t x)
{
  uint8_t h = x/10;

  return ( h << 4 ) + ( x - ( ( h << 3 ) + ( h << 1 ) ) );
}

/*
 * Register Definitions and Access Macros
 *
 * The xxx_REG macros are offsets into the register files
 * The xxx_OFF macros are offsets into a in-memory buffer
 *             starting at the seconds (for the 1375 both,
 *             _REG and _OFF happen to be identical).
 */
#define DS1375_SEC_REG  0x0
#define DS1375_SEC_OFF  (DS1375_SEC_REG-DS1375_SEC_REG)
/* Extract seconds and convert to binary */
#define DS1375_SEC(x)  ds1375_bcd2bin( ((x)[DS1375_SEC_OFF]) & 0x7f )

#define DS1375_MIN_REG  0x1
#define DS1375_MIN_OFF  (DS1375_MIN_REG-DS1375_SEC_REG)
/* Extract minutes and convert to binary */
#define DS1375_MIN(x)  ds1375_bcd2bin( ((x)[DS1375_MIN_OFF]) & 0x7f )

#define DS1375_HR_REG  0x2
#define DS1375_HR_OFF  (DS1375_HR_REG-DS1375_SEC_REG)
#define DS1375_HR_1224  (1<<6)
#define DS1375_HR_AMPM  (1<<5)
/* Are hours in AM/PM representation ?   */
#define DS1375_IS_AMPM(x)  (DS1375_HR_1224 & ((x)[DS1375_HR_OFF]))
/* Are we PM ?                           */
#define DS1375_IS_PM(x)    (DS1375_HR_AMPM & ((x)[DS1375_HR_OFF]))
/* Extract hours (12h mode) and convert to binary */
#define DS1375_HR_12(x)  ds1375_bcd2bin( ((x)[DS1375_HR_OFF]) & 0x1f )
/* Extract hours (24h mode) and convert to binary */
#define DS1375_HR_24(x)  ds1375_bcd2bin( ((x)[DS1375_HR_OFF]) & 0x3f )

#define DS1375_DAY_REG  0x3
#define DS1375_DAY_OFF  (DS1375_DAY_REG-DS1375_SEC_REG)
#define DS1375_DAT_REG  0x4
#define DS1375_DAT_OFF  (DS1375_DAT_REG-DS1375_SEC_REG)
/* Extract date and convert to binary    */
#define DS1375_DAT(x)  ds1375_bcd2bin( ((x)[DS1375_DAT_OFF]) & 0x3f )
#define DS1375_MON_REG  0x5
#define DS1375_MON_OFF  (DS1375_MON_REG-DS1375_SEC_REG)
#define DS1375_MON_CTRY    (1<<7)
/* Is century bit set ?                  */
#define DS1375_IS_CTRY(x)  (((x)[DS1375_MON_OFF]) & DS1375_MON_CTRY)
/* Extract month and convert to binary   */
#define DS1375_MON(x)  ds1375_bcd2bin( ((x)[DS1375_MON_OFF]) & 0x1f )

#define DS1375_YR_REG  0x6
#define DS1375_YR_OFF  (DS1375_YR_REG-DS1375_SEC_REG)
/* Extract year and convert to binary    */
#define DS1375_YR(x)  ds1375_bcd2bin( ((x)[DS1375_YR_OFF]) & 0xff )

/* CR Register and bit definitions       */
#define DS1375_CR_REG  0xe
#define DS1375_CR_ECLK    (1<<7)
#define DS1375_CR_CLKSEL1  (1<<6)
#define DS1375_CR_CLKSEL0  (1<<5)
#define DS1375_CR_RS2    (1<<4)
#define DS1375_CR_RS1    (1<<3)
#define DS1375_CR_INTCN    (1<<2)
#define DS1375_CR_A2IE    (1<<1)
#define DS1375_CR_A1IE    (1<<0)

#define DS1375_CSR_REG  0xf

/* User SRAM (8 bytes)                   */
#define DS1375_RAM    0x10  /* start of 8 bytes user ram */

/* Access Primitives                     */

STATIC int rd_bytes(
  int      fd,
  uint32_t off,
  uint8_t *buf,
  int      len
)
{
  uint8_t ptr = off;

  return 1 == write( fd, &ptr, 1 ) && len == read( fd, buf, len ) ? 0 : -1;
}

STATIC int wr_bytes(
  int      fd,
  uint32_t off,
  uint8_t *buf,
  int      len
)
{
  uint8_t d[ len + 1 ];

  /* Must not break up writing of the register pointer and
   * the data to-be-written into multiple write() calls
   * because every 'write()' operation sends START and
   * the chip interprets the first byte after START as
   * the register pointer.
   */

  d[0] = off;
  memcpy( d + 1, buf, len );

  return len + 1 == write( fd, d, len + 1 ) ? 0 : -1;
}

/* Helpers                               */

static int getfd(
  int minor
)
{
  return open( (const char *)RTC_Table[minor].ulCtrlPort1, O_RDWR );
}

/* Driver Access Functions               */

STATIC void ds1375_initialize(
  int minor
)
{
  int     fd;
  uint8_t cr;

  if ( ( fd = getfd( minor ) ) >= 0 ) {
    if ( 0 == rd_bytes( fd, DS1375_CR_REG, &cr, 1 ) ) {
      /* make sure clock is enabled */
      if ( ! ( DS1375_CR_ECLK & cr ) ) {
        cr |= DS1375_CR_ECLK;
        wr_bytes( fd, DS1375_CR_REG, &cr, 1 );
      }
    }
    close( fd );
  }

}

STATIC int ds1375_get_time(
  int                minor,
  rtems_time_of_day *time
)
{
  int   rval = -1;
  int   fd;
  uint8_t  buf[DS1375_YR_REG + 1 - DS1375_SEC_REG];

  if ( time && ( ( fd = getfd( minor ) ) >= 0 ) ) {
    if ( 0 == rd_bytes( fd, DS1375_SEC_REG, buf, sizeof(buf) ) ) {
      time->year    =  DS1375_IS_CTRY( buf ) ? 2000 : 1900;
      time->year  +=  DS1375_YR ( buf );
      time->month  =  DS1375_MON( buf );
      time->day    =  DS1375_DAT( buf );  /* DAY is weekday */

      if ( DS1375_IS_AMPM( buf ) ) {
        time->hour   = DS1375_HR_12 ( buf );
        if ( DS1375_IS_PM( buf ) )
          time->hour += 12;
      } else {
        time->hour   = DS1375_HR_24 ( buf );
      }

      time->minute =  DS1375_MIN( buf );
      time->second =  DS1375_SEC( buf );
      time->ticks  = 0;
      rval = 0;
    }
    close( fd );
  }
  return rval;
}

STATIC int ds1375_set_time(
  int                      minor,
  const rtems_time_of_day *time
)
{
  int       rval = -1;
  int       fd   = -1;
  time_t    secs;
  struct tm tm;
  uint8_t   buf[DS1375_YR_REG + 1 - DS1375_SEC_REG];
  uint8_t   cr = 0xff;

  /*
   * The clock hardware maintains the day-of-week as a separate counter
   * so we must compute it ourselves (rtems_time_of_day doesn't come
   * with a day of week).
   */
  secs = _TOD_To_seconds( time );
  /* we're only interested in tm_wday... */
  gmtime_r( &secs, &tm );

  buf[DS1375_SEC_OFF] = ds1375_bin2bcd( time->second );
  buf[DS1375_MIN_OFF] = ds1375_bin2bcd( time->minute );
  /* bin2bcd(hour) implicitly selects 24h mode since ms-bit is clear */
  buf[DS1375_HR_OFF]  = ds1375_bin2bcd( time->hour   );
  buf[DS1375_DAY_OFF] = tm.tm_wday + 1;
  buf[DS1375_DAT_OFF] = ds1375_bin2bcd( time->day    );
  buf[DS1375_MON_OFF] = ds1375_bin2bcd( time->month  );

  if ( time->year >= 2000 ) {
    buf[DS1375_YR_OFF]   = ds1375_bin2bcd( time->year - 2000 );
    buf[DS1375_MON_OFF] |= DS1375_MON_CTRY;
  } else {
    buf[DS1375_YR_OFF]   = ds1375_bin2bcd( time->year - 1900 );
  }

  /*
   * Stop clock; update registers and restart. This is slightly
   * slower than just writing everyting but if we did that we
   * could get inconsistent registers if this routine would not
   * complete in less than 1s (says the datasheet) and we don't
   * know if we are going to be pre-empted for some time...
   */
  if ( ( fd = getfd( minor ) ) < 0 ) {
    goto cleanup;
  }

  if ( rd_bytes( fd, DS1375_CR_REG, &cr, 1 ) )
    goto cleanup;

  cr &= ~DS1375_CR_ECLK;

  /* This stops the clock */
  if ( wr_bytes( fd, DS1375_CR_REG, &cr, 1 ) )
    goto cleanup;

  /* write new contents */
  if ( wr_bytes( fd, DS1375_SEC_REG, buf, sizeof(buf) ) )
    goto cleanup;

  rval = 0;

cleanup:
  if ( fd >= 0 ) {
    if ( ! ( DS1375_CR_ECLK & cr ) ) {
      /* start clock; this handles some cases of failure
       * after stopping the clock by restarting it again
       */
      cr |= DS1375_CR_ECLK;
      if ( wr_bytes( fd, DS1375_CR_REG, &cr, 1 ) )
        rval = -1;
    }
    close( fd );
  }
  return rval;
}

/* Debugging / Testing                   */

#ifdef DEBUG

/* Don't forget to set "TZ" when using these test routines */

/* What is rtems_time_of_day good for ? Why not use std types ? */

uint32_t
ds1375_get_time_tst()
{
rtems_time_of_day rtod;
time_t            secs;

  ds1375_get_time( 0, &rtod );
  secs = _TOD_To_seconds( &rtod );
  printf( "%s\n", ctime( &secs ) );
  return secs;
}

int
ds1375_set_time_tst( const char *datstr, rtems_time_of_day *prt )
{
struct tm         tm;
time_t            secs;
rtems_time_of_day rt;

  if ( !datstr )
    return -1;

  if ( ! strptime( datstr, "%Y-%m-%d/%T", &tm ) )
    return -2;

  if ( ! prt )
    prt = &rt;

  secs = mktime( &tm );

  /* convert to UTC */
  gmtime_r( &secs, &tm );

  printf("Y: %"PRIu32" ",  (prt->year    = tm.tm_year + 1900) );
  printf("M: %"PRIu32" ",  (prt->month   = tm.tm_mon  +    1) );
  printf("D: %"PRIu32" ",  (prt->day     = tm.tm_mday       ) );
  printf("h: %"PRIu32" ",  (prt->hour    = tm.tm_hour       ) );
  printf("m: %"PRIu32" ",  (prt->minute  = tm.tm_min        ) );
  printf("s: %"PRIu32"\n", (prt->second  = tm.tm_sec        ) );
  prt->ticks = 0;

  return ( prt == &rt ) ? ds1375_set_time( 0, &rt ) : 0;
}

#endif


uint32_t
rtc_ds1375_get_register( uintptr_t port, uint8_t reg )
{
int      fd;
uint8_t  v;
uint32_t rval = -1;

  if ( ( fd = open( (const char*)port, O_RDWR ) ) >= 0 ) {

    if ( 0 == rd_bytes( fd, reg, &v, 1 ) ) {
      rval = v;
    }
    close( fd );
  }

  return rval;
}

void
rtc_ds1375_set_register( uintptr_t port, uint8_t reg, uint32_t value )
{
int     fd;
uint8_t v = value;

  if ( ( fd = open( (const char*)port, O_RDWR ) ) >= 0 ) {
    wr_bytes( fd, reg, &v, 1 );
    close( fd );
  }

}

bool rtc_ds1375_device_probe(
 int minor
)
{
  int fd;

  if ( ( fd = getfd( minor ) ) < 0 ) {
    STDIOSAFE( "ds1375_probe (open): %s\n", strerror( errno ) );
    return false;
  }

  /* Try to set file pointer */
  if ( 0 != wr_bytes( fd, DS1375_SEC_REG, 0, 0 ) ) {
    STDIOSAFE( "ds1375_probe (wr_bytes): %s\n", strerror( errno ) );
    close( fd );
    return false;
  }

  if ( close( fd ) ) {
    STDIOSAFE( "ds1375_probe (close): %s\n", strerror( errno ) );
    return false;
  }

  return true;
}

rtc_fns rtc_ds1375_fns = {
  .deviceInitialize = ds1375_initialize,
  .deviceGetTime =    ds1375_get_time,
  .deviceSetTime =    ds1375_set_time,
};
