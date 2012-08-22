/**
 * @file
 *
 * @ingroup beagle
 *
 * @brief RTC configuration.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libchip/rtc.h>

#include <bsp.h>
#include <bsp/beagle.h>

#define BEAGLE_RTC_COUNT 1U

#define BEAGLE_RTC_COUNTER_DELTA 0xfffffffeU

#define BEAGLE_RTC_KEY 0xb5c13f27U

#define BEAGLE_RTC_CTRL_FORCE_ONSW (1U << 7)
#define BEAGLE_RTC_CTRL_STOP (1U << 6)
#define BEAGLE_RTC_CTRL_RESET (1U << 4)
#define BEAGLE_RTC_CTRL_MATCH_1_ONSW (1U << 3)
#define BEAGLE_RTC_CTRL_MATCH_0_ONSW (1U << 2)
#define BEAGLE_RTC_CTRL_MATCH_1_INTR (1U << 1)
#define BEAGLE_RTC_CTRL_MATCH_0_INTR (1U << 0)

static void beagle_rtc_set( uint32_t val ) {
  unsigned i = beagle_arm_clk() / BEAGLE_OSCILLATOR_RTC;

  //beagle.rtc.ctrl |= BEAGLE_RTC_CTRL_STOP;
  //beagle.rtc.ucount = val;
  //beagle.rtc.dcount = BEAGLE_RTC_COUNTER_DELTA - val;
  //beagle.rtc.ctrl &= ~BEAGLE_RTC_CTRL_STOP;

  /* It needs some time before we can read the values back */
  while( i != 0 ) {
    __asm__ volatile( "nop" );
    --i;
  }
}

static void beagle_rtc_reset( void ) {
  //beagle.rtc.ctrl = BEAGLE_RTC_CTRL_RESET;
  //beagle.rtc.ctrl = 0;
  //beagle.rtc.key = BEAGLE_RTC_KEY;
  beagle_rtc_set( 0 );
}

static void beagle_rtc_initialize( int minor ) {
  uint32_t up_first = 0;
  uint32_t up_second = 0;
  uint32_t down_first = 0;
  uint32_t down_second = 0;

  //if( beagle.rtc.key != BEAGLE_RTC_KEY ) {
  //  beagle_rtc_reset();
  //}

  do {
    //up_first = beagle.rtc.ucount;
    //down_first = beagle.rtc.dcount;
    //up_second = beagle.rtc.ucount;
    //down_second = beagle.rtc.dcount;
  } while( up_first != up_second || down_first != down_second );

  if( up_first + down_first != BEAGLE_RTC_COUNTER_DELTA ) {
    beagle_rtc_reset();
  }
}

static int beagle_rtc_get_time( int minor, rtems_time_of_day *tod ) {
  struct timeval now = {
    //.tv_sec = beagle.rtc.ucount,
    .tv_usec = 0
  };
  struct tm time;

  gmtime_r( &now.tv_sec, &time );

  tod->year   = time.tm_year + 1900;
  tod->month  = time.tm_mon + 1;
  tod->day    = time.tm_mday;
  tod->hour   = time.tm_hour;
  tod->minute = time.tm_min;
  tod->second = time.tm_sec;
  tod->ticks  = 0;

  return RTEMS_SUCCESSFUL;
}

static int beagle_rtc_set_time( int minor, const rtems_time_of_day *tod ) {
  beagle_rtc_set( _TOD_To_seconds( tod ) );

  return 0;
}

static bool beagle_rtc_probe( int minor ) {
  return true;
}

const rtc_fns beagle_rtc_ops = {
  .deviceInitialize = beagle_rtc_initialize,
  .deviceGetTime = beagle_rtc_get_time,
  .deviceSetTime = beagle_rtc_set_time
};

size_t RTC_Count = BEAGLE_RTC_COUNT;

rtems_device_minor_number RTC_Minor = 0;

rtc_tbl RTC_Table [BEAGLE_RTC_COUNT] = {
  {
    .sDeviceName = "/dev/rtc",
    .deviceType = RTC_CUSTOM,
    .pDeviceFns = &beagle_rtc_ops,
    .deviceProbe = beagle_rtc_probe,
    .pDeviceParams = NULL,
    .ulCtrlPort1 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL
  }
};
