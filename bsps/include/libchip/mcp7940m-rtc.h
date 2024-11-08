/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup shared_tod_mcp7940m_rtc
 *
 * @brief This file provides the interfaces of @ref shared_tod_mcp7940m_rtc.
 */

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

#include <libchip/i2c-rtc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup shared_tod_mcp7940m_rtc
 *
 * @ingroup shared_tod_i2c_rtc
 *
 * @brief RTC driver for the MCP7940M
 *
 * To use this driver, use the following initialization:
 *
 * Define a context for the RTC somewhere:
 *
 *   static struct mcp7940m_rtc rtc_ctx =
 *     MCP7940M_RTC_INITIALIZER("/dev/i2c-1", 0x6F, true);
 *
 * Then you can use the following for the RTC_Table:
 *
 *   MCP7940M_RTC_TBL_ENTRY("/dev/rtc", &rtc_ctx)
 *
 * @{
 */

struct mcp7940m_rtc {
  struct i2c_rtc_base base;

  /** True if a crystal should be used. False if an oscillator is connected. */
  bool crystal;
};

/** Hardware init. Used internally. */
int mcp7940m_hw_init(struct i2c_rtc_base *base);

#define MCP7940M_RTC_INITIALIZER(i2c_path, i2c_address, has_crystal) { \
    .base = I2C_RTC_INITIALIZER( \
        i2c_path, \
        i2c_address, \
        0, \
        I2C_RTC_ORDER_sec_min_hour_wkday_day_month_year, \
        "mcp7940", \
        mcp7940m_hw_init), \
    .crystal = has_crystal, \
  }

#define MCP7940M_RTC_TBL_ENTRY(dev_name, mcp7940m_rtc_ctx) \
          I2C_RTC_TBL_ENTRY(dev_name, mcp7940m_rtc_ctx)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCHIP_MCP7940M_RTC_H */
