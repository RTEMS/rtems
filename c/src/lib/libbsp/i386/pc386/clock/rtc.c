/*-------------------------------------------------------------------------+
| rtc.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| This file contains the real time clock manipulation package for the
| PC386 board.
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   rtc.c,v 1.4 1995/12/19 20:07:15 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1999.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in the file LICENSE in this distribution or at
| *  http://www.rtems.com/license/LICENSE.
| **************************************************************************
+--------------------------------------------------------------------------*/

#include <string.h>

#include <bsp.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define IO_RTC         0x70  /* RTC                                    */

#define RTC_SEC        0x00  /* seconds                                */
#define RTC_SECALRM    0x01  /* seconds alarm                          */
#define RTC_MIN        0x02  /* minutes                                */
#define RTC_MINALRM    0x03  /* minutes alarm                          */
#define RTC_HRS        0x04  /* hours                                  */
#define RTC_HRSALRM    0x05  /* hours alarm                            */
#define RTC_WDAY       0x06  /* week day                               */
#define RTC_DAY        0x07  /* day of month                           */
#define RTC_MONTH      0x08  /* month of year                          */
#define RTC_YEAR       0x09  /* month of year                          */
#define RTC_STATUSA    0x0a  /* status register A                      */
#define  RTCSA_TUP     0x80  /* time update, don't look now            */

#define RTC_STATUSB    0x0b  /* status register B                      */

#define RTC_INTR       0x0c  /* status register C (R) interrupt source */
#define  RTCIR_UPDATE  0x10  /* update intr                            */
#define  RTCIR_ALARM   0x20  /* alarm intr                             */
#define  RTCIR_PERIOD  0x40  /* periodic intr                          */
#define  RTCIR_INT     0x80  /* interrupt output signal                */

#define RTC_STATUSD    0x0d  /* status register D (R) Lost Power       */
#define  RTCSD_PWR     0x80  /* clock lost power                       */

#define RTC_DIAG       0x0e  /* status register E - bios diagnostic    */
#define RTCDG_BITS     "\020\010clock_battery\007ROM_cksum\006config_unit\005memory_size\004fixed_disk\003invalid_time"

#define RTC_CENTURY    0x32  /* current century - increment in Dec99   */

/*-------------------------------------------------------------------------+
| Auxiliary Functions
+--------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------+
|         Function: bcd
|      Description: Convert 2 digit number to its BCD representation.
| Global Variables: None.
|        Arguments: i - Number to convert.
|          Returns: BCD representation of number.
+--------------------------------------------------------------------------*/
static inline uint8_t
bcd(uint8_t         i)
{
  return ((i / 16) * 10 + (i % 16));
} /* bcd */

#define QUICK_READ  /* Quick read of the RTC: don't return number of seconds. */

#ifndef QUICK_READ

#define SECS_PER_DAY      (24 * 60 * 60)
#define SECS_PER_REG_YEAR (365 * SECS_PER_DAY)

/*-------------------------------------------------------------------------+
|         Function: ytos
|      Description: Convert years to seconds (since 1970).
| Global Variables: None.
|        Arguments: y - year to convert (1970 <= y <= 2100).
|          Returns: number of seconds since 1970.
+--------------------------------------------------------------------------*/
static inline uint32_t
ytos(uint16_t         y)
{                                       /* v NUM LEAP YEARS v */
  return ((y - 1970) * SECS_PER_REG_YEAR + (y - 1970 + 1) / 4 * SECS_PER_DAY);
} /* ytos */

/*-------------------------------------------------------------------------+
|         Function: mtos
|      Description: Convert months to seconds since January.
| Global Variables: None.
|        Arguments: m - month to convert, leap - is this a month of a leap year.
|          Returns: number of seconds since January.
+--------------------------------------------------------------------------*/
static inline uint32_t
mtos(uint8_t         m, bool leap)
{
  static uint16_t         daysMonth[] = { 0, 0, 31,  59,  90, 120, 151, 181,
				               212, 243, 273, 304, 334, 365 };
    /* Days since beginning of year until beginning of month. */

  return ((daysMonth[m] + (leap ? 1 : 0)) * SECS_PER_DAY);
} /* mtos */

#endif /* QUICK_READ */

/*-------------------------------------------------------------------------+
|         Function: rtcin
|      Description: Perform action on RTC and return its result.
| Global Variables: None.
|        Arguments: what - what to write to RTC port (what to do).
|          Returns: result received from RTC port after action performed.
+--------------------------------------------------------------------------*/
static inline uint8_t
rtcin(uint8_t         what)
{
    uint8_t         r;

    outport_byte(IO_RTC,   what);
    inport_byte (IO_RTC+1, r);
    return r;
} /* rtcin */

/*-------------------------------------------------------------------------+
| Functions
+--------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------+
|         Function: init_rtc
|      Description: Initialize real-time clock (RTC).
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
void
init_rtc(void)
{
  uint8_t         s;

  /* initialize brain-dead battery powered clock */
  outport_byte(IO_RTC,   RTC_STATUSA);
  outport_byte(IO_RTC+1, 0x26);
  outport_byte(IO_RTC,   RTC_STATUSB);
  outport_byte(IO_RTC+1, 2);

  outport_byte(IO_RTC,   RTC_DIAG);
  inport_byte (IO_RTC+1, s);
  if (s)
    printk("RTC BIOS diagnostic error %b\n", s);

  /* FIXME: This was last line's original version. How was it supposed to work?
       printf("RTC BIOS diagnostic error %b\n", s, RTCDG_BITS); */
} /* init_rtc */

/*-------------------------------------------------------------------------+
|         Function: rtc_read
|      Description: Read present time from RTC and return it.
| Global Variables: None.
|        Arguments: tod - to return present time in 'rtems_time_of_day' format.
|          Returns: number of seconds from 1970/01/01 corresponding to 'tod'.
+--------------------------------------------------------------------------*/
long int
rtc_read(rtems_time_of_day *tod)
{
  uint8_t          sa;
  uint32_t         sec = 0;

  memset(tod, 0, sizeof *tod); /* zero tod structure */

  /* do we have a realtime clock present? (otherwise we loop below) */
  sa = rtcin(RTC_STATUSA);
  if (sa == 0xff || sa == 0)
    return -1;

  /* ready for a read? */
  while ((sa&RTCSA_TUP) == RTCSA_TUP)
    sa = rtcin(RTC_STATUSA);

  tod->year	= bcd(rtcin(RTC_YEAR)) + 1900;  /* year    */
  if (tod->year < 1970)	tod->year += 100;
  tod->month	= bcd(rtcin(RTC_MONTH));        /* month   */
  tod->day	= bcd(rtcin(RTC_DAY));          /* day     */
  (void)          bcd(rtcin(RTC_WDAY));         /* weekday */
  tod->hour	= bcd(rtcin(RTC_HRS));          /* hour    */
  tod->minute	= bcd(rtcin(RTC_MIN));          /* minutes */
  tod->second	= bcd(rtcin(RTC_SEC));          /* seconds */
  tod->ticks	= 0;

#ifndef QUICK_READ  /* Quick read of the RTC: don't return number of seconds. */
  sec =  ytos(tod->year);
  sec += mtos(tod->month, (tod->year % 4) == 0);
  sec += tod->day * SECS_PER_DAY;
  sec += tod->hour * 60 * 60;                     /* hour    */
  sec += tod->minute * 60;                        /* minutes */
  sec += tod->second;                             /* seconds */
#endif /* QUICK_READ */

  return (long int)sec;
} /* rtc_read */
