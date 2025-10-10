/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32U5
 *
 * @brief Implementation of the RTC driver for the STM32U5
 */

/*
 * Copyright (c) 2024 embedded brains GmbH & Co. KG
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

#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/start.h>
#include <libchip/rtc.h>
#include <rtems/rtems/clockimpl.h>
#include <rtems/score/todimpl.h>
#include <stm32u5/hal.h>

RTC_HandleTypeDef hrtc  = { 0 };
RTC_TimeTypeDef   sTime = { 0 };
RTC_DateTypeDef   sDate = { 0 };

static void stm32u5_rtc_get_time( rtems_time_of_day *tod )
{
  if ( HAL_RTC_GetTime( &hrtc, &sTime, RTC_FORMAT_BIN ) != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_RTC );
  }

  if ( HAL_RTC_GetDate( &hrtc, &sDate, RTC_FORMAT_BIN ) != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_RTC );
  }

  tod->ticks  = 0;
  tod->second = sTime.Seconds;
  tod->minute = sTime.Minutes;
  tod->hour   = sTime.Hours;
  tod->day    = sDate.Date;
  tod->month  = sDate.Month;
  tod->year   = TOD_BASE_YEAR + sDate.Year;
}

static void stm32u5_rtc_device_initialize( int minor )
{
  (void) minor;

  hrtc.Instance            = RTC;
  hrtc.Init.HourFormat     = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv   = 127;
  hrtc.Init.SynchPrediv    = 255;
  hrtc.Init.OutPut         = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp   = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode        = RTC_BINARY_MIX;
  if ( HAL_RTC_Init( &hrtc ) != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_RTC );
  }
}

static int stm32u5_rtc_device_get_time( int minor, rtems_time_of_day *tod )
{
  (void) minor;

  stm32u5_rtc_get_time( tod );

  return 0;
}

static int stm32u5_rtc_device_set_time(
  int                      minor,
  const rtems_time_of_day *tod
)
{
  (void) minor;

  sTime.Hours   = (uint8_t) tod->hour;
  sTime.Minutes = (uint8_t) tod->minute;
  sTime.Seconds = (uint8_t) tod->second;
  sDate.Date    = (uint8_t) tod->day;
  sDate.Month   = (uint8_t) tod->month;
  sDate.Year    = (uint8_t) tod->year - TOD_BASE_YEAR;

  if ( HAL_RTC_SetTime( &hrtc, &sTime, RTC_FORMAT_BIN ) != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_RTC );
  }

  if ( HAL_RTC_SetDate( &hrtc, &sDate, RTC_FORMAT_BIN ) != HAL_OK ) {
    bsp_fatal( STM32U5_FATAL_RTC );
  }

  return 0;
}

static bool stm32u5_rtc_device_probe( int minor )
{
  (void) minor;

  return true;
}

const rtc_fns stm32u5_rtc_device_ops = {
  .deviceInitialize = stm32u5_rtc_device_initialize,
  .deviceGetTime    = stm32u5_rtc_device_get_time,
  .deviceSetTime    = stm32u5_rtc_device_set_time
};

rtc_tbl RTC_Table[] = {
  { .sDeviceName = "/dev/rtc",
    .deviceType  = RTC_CUSTOM,
    .pDeviceFns  = &stm32u5_rtc_device_ops,
    .deviceProbe = stm32u5_rtc_device_probe }
};

void BSP_START_TEXT_SECTION HAL_RTC_MspInit( RTC_HandleTypeDef *hrtc )
{
  (void) hrtc;

  /* Enable peripheral clocks and battery backup. */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_RTC_CONFIG( RCC_RTCCLKSOURCE_LSE );
  __HAL_RCC_RTC_ENABLE();
  __HAL_RCC_RTCAPB_CLK_ENABLE();
  __HAL_RCC_RTCAPB_CLKAM_ENABLE();
}

size_t RTC_Count = RTEMS_ARRAY_SIZE( RTC_Table );
