/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief RTC configuration.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <libchip/rtc.h>
#include <bsp/io.h>

#define LPC176X_RTC_NUMBER 1U

void bsp_rtc_initialize( void );
int bsp_rtc_get_time( rtems_time_of_day *tod );
int bsp_rtc_set_time( const rtems_time_of_day *tod );
bool bsp_rtc_probe( void );

/**
 * @brief Initialize the rtc device.
 */
void bsp_rtc_initialize( void )
{
  /* Enable module power */
  lpc176x_module_enable( LPC176X_MODULE_RTC, LPC176X_MODULE_PCLK_DEFAULT );

  /* Enable the RTC and use external clock */
  RTC_CCR = RTC_CCR_CLKEN | RTC_CCR_CLKSRC;

  /* Disable interrupts */
  RTC_CIIR = 0U;
  RTC_CISS = 0U;
  RTC_AMR = 0xFFU;

  /* Clear interrupts */
  RTC_ILR = RTC_ILR_RTCCIF | RTC_ILR_RTCALF | RTC_ILR_RTSSF;
}

/**
 * @brief Gets the information according to the current time.
 *
 * @param  tod Value to be modified.
 * @return  0
 */
int bsp_rtc_get_time( rtems_time_of_day *tod )
{
  tod->ticks = 0;
  tod->second = RTC_SEC;
  tod->minute = RTC_MIN;
  tod->hour = RTC_HOUR;
  tod->day = RTC_DOM;
  tod->month = RTC_MONTH;
  tod->year = RTC_YEAR;

  return 0;
}

/**
 * @brief Sets the information according to the current time.
 *
 * @param  tod Value to get the new information.
 * @return  0
 */
int bsp_rtc_set_time( const rtems_time_of_day *tod )
{
  RTC_SEC = tod->second;
  RTC_MIN = tod->minute;
  RTC_HOUR = tod->hour;
  RTC_DOM = tod->day;
  RTC_MONTH = tod->month;
  RTC_YEAR = tod->year;

  return 0;
}

/**
 * @brief Used to probe. At the moment is not used.
 *
 * @return true.
 */
bool bsp_rtc_probe( void )
{
  return true;
}

/**
 * @brief Represents the real time clock options.
 */
const rtc_fns lpc176x_rtc_ops = {
  .deviceInitialize = (void *) bsp_rtc_initialize,
  .deviceGetTime = (void *) bsp_rtc_get_time,
  .deviceSetTime = (void *) bsp_rtc_set_time
};

size_t RTC_Count = LPC176X_RTC_NUMBER;

/**
 * @brief Table to describes the rtc device.
 */
rtc_tbl RTC_Table[ LPC176X_RTC_NUMBER ] = {
  {
    .sDeviceName = "/dev/rtc",
    .deviceType = RTC_CUSTOM,
    .pDeviceFns = &lpc176x_rtc_ops,
    .deviceProbe = (void *) bsp_rtc_probe,
    .pDeviceParams = NULL,
    .ulCtrlPort1 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL
  }
};
