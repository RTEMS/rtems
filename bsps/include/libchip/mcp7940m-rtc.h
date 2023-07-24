/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023 embedded brains GmbH & Co. KG
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

#ifndef LIBCHIP_MCP7940M_RTC_H
#define LIBCHIP_MCP7940M_RTC_H

#include <rtems.h>
#include <rtems/thread.h>
#include <libchip/rtc.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern const rtc_fns rtc_mcp7940m_fns;
bool rtc_mcp7940m_probe(int minor);

/*
 * It is expected, that the RTC can be accessed as a raw file. A pointer to a
 * constant string with the name of that device has to be passed to the table
 * initializer.
 *
 * The MCP7940M uses an EEPROM-like interface. So you could for example use the
 * following initialization:
 *
 * Define a context for the RTC somewhere:
 *
 *   static struct mcp7940m_rtc rtc_ctx =
 *     MCP7940M_RTC_INITIALIZER("/dev/i2c-1", 0x6F, true);
 *
 * Then you can use the following for the RTC_Table:
 *
 *   MCP7940M_RTC_TBL_ENTRY("/dev/rtc", &rtc_ctx)
 */

struct mcp7940m_rtc {
  /** Just initialize with RTEMS_MUTEX_INITIALIZER('mcp7940'). */
  rtems_mutex mutex;

  /** The path of the I2C bus device. */
  const char *i2c_bus_path;

  /** I2C address. */
  uint8_t i2c_addr;

  /** True if a crystal should be used. False if an oscillator is connected. */
  bool crystal;

  /** Whether the RTC has already been initialized. Used internally. */
  bool initialized;
};

#define MCP7940M_RTC_INITIALIZER(i2c_path, i2c_address, has_crystal) { \
    .mutex = RTEMS_MUTEX_INITIALIZER("mcp7940m"), \
    .i2c_bus_path = i2c_path, \
    .i2c_addr = i2c_address, \
    .crystal = has_crystal, \
    .initialized = false, \
  }

#define MCP7940M_RTC_TBL_ENTRY(dev_name, mcp7940m_rtc_ctx) \
  { \
    .sDeviceName = dev_name, \
    .deviceType = RTC_CUSTOM, \
    .pDeviceFns = &rtc_mcp7940m_fns, \
    .deviceProbe = rtc_mcp7940m_probe, \
    .pDeviceParams = (void *)mcp7940m_rtc_ctx, \
    .ulCtrlPort1 = 0, \
    .ulDataPort = 0, \
    .getRegister = NULL, \
    .setRegister = NULL, \
  }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCHIP_MCP7940M_RTC_H */
