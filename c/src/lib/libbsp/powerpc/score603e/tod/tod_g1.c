/*
 * Real Time Clock (SGS-Thomson M48T08/M48T18) for RTEMS 
 *
 *  This part is only found on the first generation board.
 *
 *  Based on MVME162 TOD Driver by:
 *    COPYRIGHT (C) 1997
 *    by Katsutoshi Shibuya - BU Denken Co.,Ltd. - Sapporo - JAPAN
 *    ALL RIGHTS RESERVED 
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */ 

#include <rtems.h>
#include <tod.h>
#include <bsp.h>

/*
 *  These routines are M48T08 and M48T18 dependent and should be in 
 *  a separate support library.
 */

static int M48T08_GetField(
  volatile unsigned char *mk48t08,
  int                     n,
  unsigned char           mask
)
{
  unsigned char x;

  x = mk48t08[n] & mask;
  return ((x >> 4) * 10) + (x & 0x0f);
}

static void M48T08_SetField(
  volatile unsigned char *mk48t08,
  int                     n,
  unsigned char           d
)
{
  mk48t08[n] = ((d / 10) << 4) + (d % 10);
}

static void M48T08_GetTOD(
  volatile unsigned char *mk48t08,
  rtems_time_of_day      *rtc_tod
)
{
  int year;

  mk48t08[0] |= 0x40;     /* Stop read register */

  year = M48T08_GetField( mk48t08, 7, 0xff );
  if ( year >= 88 )
    year += 1900;
  else
    year += 2000;

  rtc_tod->year   = year;
  rtc_tod->month  = M48T08_GetField( mk48t08, 6, 0x1f );
  rtc_tod->day    = M48T08_GetField( mk48t08, 5, 0x3f );
  rtc_tod->hour   = M48T08_GetField( mk48t08, 3, 0x3f );
  rtc_tod->minute = M48T08_GetField( mk48t08, 2, 0x7f );
  rtc_tod->second = M48T08_GetField( mk48t08, 1, 0x7f );
  rtc_tod->ticks  = 0;
  mk48t08[0] &= 0x3f;     /* Release read register */
}

static void M48T08_SetTOD(
  volatile unsigned char *mk48t08,
  rtems_time_of_day      *rtc_tod
)
{
  int year;

  year = rtc_tod->year;

  if ( year >= 2088 )        /* plan ahead :) */
    rtems_fatal_error_occurred( 0xBAD0BAD0 );

  if ( year >= 2000 )
    year -= 2000;
  else
    year -= 1900;

  mk48t08[0] |= 0x80;     /* Stop write register */ 
  M48T08_SetField( mk48t08, 7, year );
  M48T08_SetField( mk48t08, 6, rtc_tod->month );
  M48T08_SetField( mk48t08, 5, rtc_tod->day );
  M48T08_SetField( mk48t08, 4, 1 );      /* I don't know which day of week is */
  M48T08_SetField( mk48t08, 3, rtc_tod->hour );
  M48T08_SetField( mk48t08, 2, rtc_tod->minute );
  M48T08_SetField( mk48t08, 1, rtc_tod->second );
  mk48t08[0] &= 0x3f;     /* Write these parameters */
}

/*
 *  This code is dependent on the Vista 603e's use of the M48T18 RTC/NVRAM
 *  and should remain in this file.
 */

void setRealTimeToRTEMS()
{
  rtems_time_of_day rtc_tod;

  M48T08_GetTOD( SCORE603E_RTC_ADDRESS, &rtc_tod );
  rtems_clock_set( &rtc_tod );
}

void setRealTimeFromRTEMS()
{
  rtems_time_of_day rtems_tod;

  rtems_clock_get( RTEMS_CLOCK_GET_TOD, &rtems_tod );
  M48T08_SetTOD( SCORE603E_RTC_ADDRESS, &rtems_tod );
}

int checkRealTime()
{
  rtems_time_of_day rtems_tod;
  rtems_time_of_day rtc_tod;

  M48T08_GetTOD( SCORE603E_RTC_ADDRESS, &rtc_tod );
  rtems_clock_get( RTEMS_CLOCK_GET_TOD, &rtems_tod );

  if( rtems_tod.year == rtc_tod.year &&
      rtems_tod.month == rtc_tod.month &&
      rtems_tod.day == rtc_tod.day ) {
     return ((rtems_tod.hour   - rtc_tod.hour) * 3600) +
            ((rtems_tod.minute - rtc_tod.minute) * 60) +
             (rtems_tod.second - rtc_tod.second);
  }
  return 9999;
}
