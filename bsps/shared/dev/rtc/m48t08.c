/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file interfaces with the real-time clock found in
 *  a Mostek M48T08 or M48T18 or compatibles.
 *
 *  Year 2K Notes:
 *
 *  This chip only uses a two digit field to store the year.  This
 *  code uses the RTEMS Epoch as a pivot year.  This lets us map the
 *  two digit year field as follows:
 *
 *    + two digit years 0-87 are mapped to 2000-2087.
 *    + two digit years 88-99 are mapped to 1988-1999.
 *
 *  This is less than the time span supported by RTEMS.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems.h>
#include <libchip/rtc.h>
#include <libchip/m48t08.h>

/*
 *  Control register bits
 */

#define M48T08_CONTROL_WRITE  0x80
#define M48T08_CONTROL_READ   0x40
#define M48T08_CONTROL_SIGN   0x20

/*
 *  m48t08_initialize
 */

static void m48t08_initialize(
  int minor
)
{
}

/*
 *  m48t08_get_time
 */

#define From_BCD( _x ) ((((_x) >> 4) * 10) + ((_x) & 0x0F))
#define To_BCD( _x )   ((((_x) / 10) << 4) + ((_x) % 10))

static int m48t08_get_time(
  int                minor,
  rtems_time_of_day *time
)
{
  uint32_t       m48t08;
  getRegister_f  getReg;
  setRegister_f  setReg;
  uint8_t        controlReg;
  uint32_t       value1;
  uint32_t       value2;

  m48t08 = RTC_Table[ minor ].ulCtrlPort1;
  getReg = RTC_Table[ minor ].getRegister;
  setReg = RTC_Table[ minor ].setRegister;

  /*
   *  Put the RTC into read mode
   */

  controlReg = (*getReg)( m48t08, M48T08_CONTROL );
  (*setReg)( m48t08, M48T08_CONTROL, controlReg | M48T08_CONTROL_READ );

  value1 = (*getReg)( m48t08, M48T08_YEAR );
  value2 = From_BCD( value1 );
  if ( value2 < 88 )
    time->year = 2000 + value2;
  else
    time->year = 1900 + value2;

  value1 = (*getReg)( m48t08, M48T08_MONTH );
  time->month = From_BCD( value1 );

  value1 = (*getReg)( m48t08, M48T08_DATE );
  time->day = From_BCD( value1 );

  value1 = (*getReg)( m48t08, M48T08_HOUR );
  time->hour = From_BCD( value1 );

  value1 = (*getReg)( m48t08, M48T08_MINUTE );
  time->minute = From_BCD( value1 );

  value1 = (*getReg)( m48t08, M48T08_SECOND );
  time->second = From_BCD( value1 );

  time->ticks  = 0;

  /*
   *  Put the RTC back into normal mode.
   */

  (*setReg)( m48t08, M48T08_CONTROL, controlReg );

  return 0;
}

/*
 *  m48t08_set_time
 */

static int m48t08_set_time(
  int                minor,
  const rtems_time_of_day *time
)
{
  uint32_t       m48t08;
  getRegister_f  getReg;
  setRegister_f  setReg;
  uint8_t        controlReg;

  m48t08 = RTC_Table[ minor ].ulCtrlPort1;
  getReg = RTC_Table[ minor ].getRegister;
  setReg = RTC_Table[ minor ].setRegister;

  /*
   *  Put the RTC into read mode
   */

  controlReg = (*getReg)( m48t08, M48T08_CONTROL );
  (*setReg)( m48t08, M48T08_CONTROL, controlReg | M48T08_CONTROL_WRITE );

  if ( time->year >= 2088 )
    rtems_fatal_error_occurred( RTEMS_INVALID_NUMBER );

  (*setReg)( m48t08, M48T08_YEAR,    To_BCD(time->year % 100) );
  (*setReg)( m48t08, M48T08_MONTH,   To_BCD(time->month) );
  (*setReg)( m48t08, M48T08_DATE,    To_BCD(time->day) );
  (*setReg)( m48t08, M48T08_HOUR,    To_BCD(time->hour) );
  (*setReg)( m48t08, M48T08_MINUTE,  To_BCD(time->minute) );
  (*setReg)( m48t08, M48T08_SECOND,  To_BCD(time->second) );

  /*
   *  Put the RTC back into normal mode.
   */

  (*setReg)( m48t08, M48T08_CONTROL, controlReg );

  return 0;
}

/*
 *  Driver function table
 */

rtc_fns m48t08_fns = {
  m48t08_initialize,
  m48t08_get_time,
  m48t08_set_time
};
