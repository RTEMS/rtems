/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup shared_tod_i2c_rtc
 *
 * @brief This file provides the interfaces of @ref shared_tod_i2c_rtc.
 */

/*
 * Copyright (C) 2023-2024 embedded brains GmbH & Co. KG
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

#ifndef LIBCHIP_I2C_RTC_H
#define LIBCHIP_I2C_RTC_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup shared_tod_i2c_rtc
 *
 * @ingroup shared_tod
 *
 * @brief Generic I2C RTC driver
 *
 * Base functionality for I2C based RTC drivers. An additional, chip specific
 * initialization is necessary.
 *
 * Expects a register block with the following time format:
 *
 * * <base>+off.second:
 *    * Bit 0 to 6: Seconds (BCD encoded)
 *    * Bit 7: Don't care
 * * <base>+off.minute:
 *    * Bit 0 to 6: Minutes (BCD encoded)
 *    * Bit 7: Don't care
 * * <base>+off.hour:
 *    * Bit 0 to 5: Hours (BCD encoded); Bit 5 indicates AM (0) and PM (1) in 12
 *      hour mode
 *    * Bit 6: 0 for 24 hour mode; 1 for 12 hour mode
 * * <base>+off.wkday:
 *    * Bit 0 to 2: Day of Week
 *    * Bit 3 to 7: Don't care
 * * <base>+off.day:
 *    * Bit 0 to 5: Day (BCD encoded)
 *    * Bit 6 and 7: Don't care
 * * <base>+off.month:
 *    * Bit 0 to 4: Month (BCD encoded)
 *    * Bit 5 to 7: Don't care
 * * <base>+off.year:
 *    * Bit 0 to 6: Year (BCD encoded)
 *    * Bit 7: Don't care
 *
 * See abeoz9 or mcp7940m for how to use this base driver.
 *
 * @{
 */

#include <rtems.h>
#include <rtems/thread.h>
#include <libchip/rtc.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern const rtc_fns i2c_rtc_fns;
bool i2c_rtc_probe(int minor);

/** Base context for the driver. */
struct i2c_rtc_base {
  /** Just initialize with RTEMS_MUTEX_INITIALIZER('mcp7940'). */
  rtems_mutex mutex;

  /** The path of the I2C bus device. */
  const char *i2c_bus_path;

  /** I2C address. */
  uint8_t i2c_addr;

  /** Whether the RTC has already been initialized. Used internally. */
  bool initialized;

  /**
   * Hardware initialization function. Will be called once when the RTC is used
   * for the first time. Can return 0 on success or anything else on error. In
   * an error case, it will be called again on the next use.
   */
  int (*hw_init) (struct i2c_rtc_base *ctx);

  /** Offset of the start of the clock register block. */
  size_t clock_offset;

  /** Order of the fields of the date. */
  struct {
    size_t year;
    size_t month;
    size_t wkday;
    size_t day;
    size_t hour;
    size_t min;
    size_t sec;
  } order;
};

/**
 * Read bytes from the I2C RTC. Can be used in derived drivers.
 *
 * Returns 0 on success or anything else on error.
 */
int i2c_rtc_read(struct i2c_rtc_base *ctx, uint8_t addr, uint8_t *buf,
    size_t len);

/**
 * Write bytes to the I2C RTC. Can be used in derived drivers.
 *
 * Returns 0 on success or anything else on error.
 */
int i2c_rtc_write(struct i2c_rtc_base *ctx, uint8_t addr, const uint8_t *buf,
    size_t len);

#define I2C_RTC_ORDER_sec_min_hour_wkday_day_month_year \
  { .sec = 0, .min = 1, .hour = 2, .day = 4, .wkday = 3, .month = 5, .year = 6 }

#define I2C_RTC_ORDER_sec_min_hour_day_wkday_month_year \
  { .sec = 0, .min = 1, .hour = 2, .day = 3, .wkday = 4, .month = 5, .year = 6 }

#define I2C_RTC_INITIALIZER(i2c_path, i2c_address, offset, reg_order, \
    driver_name, hwinit)\
  { \
    .mutex = RTEMS_MUTEX_INITIALIZER(driver_name), \
    .i2c_bus_path = i2c_path, \
    .i2c_addr = i2c_address, \
    .initialized = false, \
    .hw_init = hwinit, \
    .clock_offset = offset, \
    .order = reg_order, \
  }

#define I2C_RTC_TBL_ENTRY(dev_name, i2c_rtc_ctx) \
  { \
    .sDeviceName = dev_name, \
    .deviceType = RTC_CUSTOM, \
    .pDeviceFns = &i2c_rtc_fns, \
    .deviceProbe = i2c_rtc_probe, \
    .pDeviceParams = (void *)i2c_rtc_ctx, \
    .ulCtrlPort1 = 0, \
    .ulDataPort = 0, \
    .getRegister = NULL, \
    .setRegister = NULL, \
  }

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCHIP_I2C_RTC_H */
