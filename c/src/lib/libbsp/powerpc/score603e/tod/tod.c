/*
 * Real Time Clock (Harris ICM7170) for RTEMS 
 *
 *  This part is found on the second generation of this board. 
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */ 

#include <rtems.h>
#include <tod.h>
#include <bsp.h>

/*
 * These values are programed into a register and must not be changed.
 */

#define ICM1770_CRYSTAL_FREQ_32K      0x00
#define ICM1770_CRYSTAL_FREQ_1M       0x01
#define ICM1770_CRYSTAL_FREQ_2M       0x02
#define ICM1770_CRYSTAL_FREQ_4M       0x03

void ICM7170_GetTOD(
  volatile unsigned char *imc1770_regs,
  rtems_unsigned8        icm1770_freq,
  rtems_time_of_day      *rtc_tod
);
void ICM7170_SetTOD(
  volatile unsigned char *imc1770_regs,
  rtems_unsigned8        icm1770_freq,
  rtems_time_of_day      *rtc_tod
);

/*
 *  This code is dependent on the Vista 603e's use of the ICM7170 RTC/NVRAM
 *  and should remain in this file.
 */

void setRealTimeToRTEMS()
{
  rtems_time_of_day rtc_tod;

  ICM7170_GetTOD( SCORE603E_RTC_ADDRESS, SCORE_RTC_FREQUENCY, &rtc_tod );
  rtems_clock_set( &rtc_tod );
}

void setRealTimeFromRTEMS()
{
  rtems_time_of_day rtems_tod;

  rtems_clock_get( RTEMS_CLOCK_GET_TOD, &rtems_tod );
  ICM7170_SetTOD( SCORE603E_RTC_ADDRESS, SCORE_RTC_FREQUENCY, &rtems_tod );
}

int checkRealTime()
{
  rtems_time_of_day rtems_tod;
  rtems_time_of_day rtc_tod;

  ICM7170_GetTOD( SCORE603E_RTC_ADDRESS, SCORE_RTC_FREQUENCY, &rtc_tod );
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

/*
 *  These routines are ICM7170 should be in 
 *  a separate support library.
 * XXX Make static
 */
static int ICM7170_GetField(
  volatile unsigned char *imc1770_regs,
  int                     reg
)
{
  unsigned char x;

  x = imc1770_regs[reg*4];

  return x;
}

static void ICM7170_SetField(
  volatile unsigned char *imc1770_regs,
  int                     reg,
  unsigned char             d
)
{
  imc1770_regs[reg*4] = d;
}

void ICM7170_GetTOD(
  volatile unsigned char *imc1770_regs,
  rtems_unsigned8        icm1770_freq,
  rtems_time_of_day      *rtc_tod
)
{
  int year;
  int usec;
  static rtems_boolean init = TRUE;

  /* Initialize the clock at once prior to reading */
  if (init ) {
    ICM7170_SetField( imc1770_regs,  0x11, (0x0c | icm1770_freq) );
    init = FALSE;
  } 

  /* Latch times */
  /* rtc_tod->ticks  = */
  
  usec = ICM7170_GetField( imc1770_regs, 0x00 );
   
  year = ICM7170_GetField( imc1770_regs, 0x06 );
  if ( year >= 88 )
    year += 1900;
  else
    year += 2000;

  rtc_tod->year   = year;
  rtc_tod->month  = ICM7170_GetField( imc1770_regs, 0x04 );
  rtc_tod->day    = ICM7170_GetField( imc1770_regs, 0x05 );
  rtc_tod->hour   = ICM7170_GetField( imc1770_regs, 0x01 );
  rtc_tod->minute = ICM7170_GetField( imc1770_regs, 0x02 );
  rtc_tod->second = ICM7170_GetField( imc1770_regs, 0x03 );
  rtc_tod->ticks  = ICM7170_GetField( imc1770_regs, 0x00 );
}

void ICM7170_SetTOD(
  volatile unsigned char *imc1770_regs,
  rtems_unsigned8        icm1770_freq,
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
  
  ICM7170_SetField( imc1770_regs, 0x11, (0x04 |icm1770_freq ) );

  ICM7170_SetField( imc1770_regs, 0x06, year );
  ICM7170_SetField( imc1770_regs, 0x04, rtc_tod->month );
  ICM7170_SetField( imc1770_regs, 0x05, rtc_tod->day );
  ICM7170_SetField( imc1770_regs, 0x01, rtc_tod->hour );
  ICM7170_SetField( imc1770_regs, 0x02, rtc_tod->minute );
  ICM7170_SetField( imc1770_regs, 0x03, rtc_tod->second );

  /*
   * I don't know which day of week is 
   *  
   */
  ICM7170_SetField( imc1770_regs, 0x07, 1 );

  ICM7170_SetField( imc1770_regs,  0x11, (0x0c | icm1770_freq) );
}








