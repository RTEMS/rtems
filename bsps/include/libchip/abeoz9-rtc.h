/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup shared_tod_abeoz9_rtc
 *
 * @brief This file provides the interfaces of @ref shared_tod_abeoz9_rtc.
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#ifndef LIBCHIP_ABEOZ9_RTC_H
#define LIBCHIP_ABEOZ9_RTC_H

#include <libchip/i2c-rtc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup shared_tod_abeoz9_rtc
 *
 * @ingroup shared_tod_i2c_rtc
 *
 * @brief RTC driver for the Abracon EOZ9 series.
 *
 * To use this driver, use the following initialization:
 *
 * Define a context for the RTC somewhere:
 *
 *   static struct abeoz9_rtc rtc_ctx =
 *     ABEOZ9_RTC_INITIALIZER("/dev/i2c-1", 0x56);
 *
 * Then you can use the following for the RTC_Table:
 *
 *   ABEOZ9_RTC_TBL_ENTRY("/dev/rtc", &rtc_ctx)
 *
 * @{
 */

struct abeoz9_rtc {
  struct i2c_rtc_base base;
};

/** Hardware init. Used internally. */
int abeoz9_rtc_hw_init(struct i2c_rtc_base *base);

#define ABEOZ9_RTC_INITIALIZER(i2c_path, i2c_address) { \
    .base = I2C_RTC_INITIALIZER( \
        i2c_path, \
        i2c_address, \
        8, \
        I2C_RTC_ORDER_sec_min_hour_day_wkday_month_year, \
        "abeoz9", \
        abeoz9_rtc_hw_init), \
  }

#define ABEOZ9_RTC_TBL_ENTRY(dev_name, abeoz9_rtc_ctx) \
          I2C_RTC_TBL_ENTRY(dev_name, abeoz9_rtc_ctx)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCHIP_ABEOZ9_RTC_H */
