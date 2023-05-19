/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief RTC configuration.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
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
