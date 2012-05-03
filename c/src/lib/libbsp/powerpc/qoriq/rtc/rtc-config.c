/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief RTC configuration.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libchip/rtc.h>

#define RTC_COUNT 1

static void qoriq_rtc_initialize(int minor)
{
  /* FIXME */
}

static int qoriq_rtc_get_time(int minor, rtems_time_of_day *tod)
{
  return -1;
}

static int qoriq_rtc_set_time(int minor, const rtems_time_of_day *tod)
{
  return -1;
}

static bool qoriq_rtc_probe(int minor)
{
  return false;
}

const rtc_fns qoriq_rtc_ops = {
  .deviceInitialize = qoriq_rtc_initialize,
  .deviceGetTime = qoriq_rtc_get_time,
  .deviceSetTime = qoriq_rtc_set_time
};

size_t RTC_Count = RTC_COUNT;

rtems_device_minor_number RTC_Minor = 0;

rtc_tbl	RTC_Table [RTC_COUNT] = {
  {
    .sDeviceName = "/dev/rtc",
    .deviceType = RTC_CUSTOM,
    .pDeviceFns = &qoriq_rtc_ops,
    .deviceProbe = qoriq_rtc_probe,
    .pDeviceParams = NULL,
    .ulCtrlPort1 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL
  }
};
