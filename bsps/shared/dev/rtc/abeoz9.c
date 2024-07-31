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

#include <dev/i2c/i2c.h>
#include <libchip/abeoz9-rtc.h>

#include <stdint.h>

#define REG_ABEOZ9_CONTROL_1 0x00u

#define ABEOZ9_CONTROL_1_WE     (0x1u << 0)
#define ABEOZ9_CONTROL_1_TE     (0x1u << 1)
#define ABEOZ9_CONTROL_1_TAR    (0x1u << 2)
#define ABEOZ9_CONTROL_1_EERE   (0x1u << 3)
#define ABEOZ9_CONTROL_1_SROn   (0x1u << 4)
#define ABEOZ9_CONTROL_1_TD0    (0x1u << 5)
#define ABEOZ9_CONTROL_1_TD1    (0x1u << 6)
#define ABEOZ9_CONTROL_1_ClkInt (0x1u << 7)

#define REG_ABEOZ9_CONTROL_INT 0x01u

#define ABEOZ9_CONTROL_INT_AIE  (0x1u << 0)
#define ABEOZ9_CONTROL_INT_TIE  (0x1u << 1)
#define ABEOZ9_CONTROL_INT_V1IE (0x1u << 2)
#define ABEOZ9_CONTROL_INT_V2IE (0x1u << 3)
#define ABEOZ9_CONTROL_INT_SRIE (0x1u << 4)

#define REG_ABEOZ9_CONTROL_INT_FLAG 0x02u

#define ABEOZ9_CONTROL_INT_FLAG_AF   (0x1u << 0)
#define ABEOZ9_CONTROL_INT_FLAG_TF   (0x1u << 1)
#define ABEOZ9_CONTROL_INT_FLAG_V1IF (0x1u << 2)
#define ABEOZ9_CONTROL_INT_FLAG_V2IF (0x1u << 3)
#define ABEOZ9_CONTROL_INT_FLAG_SRF  (0x1u << 4)

#define REG_ABEOZ9_CONTROL_STATUS 0x03u

#define ABEOZ9_CONTROL_STATUS_V1F    (0x1u << 2)
#define ABEOZ9_CONTROL_STATUS_V2F    (0x1u << 3)
#define ABEOZ9_CONTROL_STATUS_SR     (0x1u << 4)
#define ABEOZ9_CONTROL_STATUS_PON    (0x1u << 5)
#define ABEOZ9_CONTROL_STATUS_EEBusy (0x1u << 7)

#define REG_ABEOZ9_CONTROL_RESET 0x04u

#define ABEOZ9_CONTROL_RESET_SysR (0x1u << 1)

#define REG_ABEOZ9_CLOCK_SEC 0x08u
#define REG_ABEOZ9_CLOCK_MIN 0x09u
#define REG_ABEOZ9_CLOCK_HOUR 0x0au
#define REG_ABEOZ9_CLOCK_WKDAY 0x0bu
#define REG_ABEOZ9_CLOCK_DATE 0x0cu
#define REG_ABEOZ9_CLOCK_MTH 0x0du
#define REG_ABEOZ9_CLOCK_YEAR 0x0eu
#define CLOCK_LEN (REG_ABEOZ9_CLOCK_YEAR + 1 - REG_ABEOZ9_CLOCK_SEC)

#define REG_ABEOZ9_ALARM_SEC 0x10u
#define REG_ABEOZ9_ALARM_MIN 0x11u
#define REG_ABEOZ9_ALARM_HOUR 0x12u
#define REG_ABEOZ9_ALARM_WKDAY 0x13u
#define REG_ABEOZ9_ALARM_DATE 0x14u
#define REG_ABEOZ9_ALARM_MTH 0x15u
#define REG_ABEOZ9_ALARM_YEAR 0x16u
#define ALARM_LEN (REG_ABEOZ9_ALARM_YEAR + 1 - REG_ABEOZ9_ALARM_SEC)

int abeoz9_rtc_hw_init(struct i2c_rtc_base *base)
{
  uint8_t reg;
  ssize_t rv;
  struct abeoz9_rtc *ctx =
      RTEMS_CONTAINER_OF(base, struct abeoz9_rtc, base);

  /*
   * Check PON bit. If that is set, we are starting with an uninitialized chip.
   */
  rv = i2c_rtc_read(&ctx->base, REG_ABEOZ9_CONTROL_STATUS, &reg, 1);
  if (rv == 0) {
    if ((reg & (ABEOZ9_CONTROL_STATUS_PON | ABEOZ9_CONTROL_STATUS_SR)) != 0) {
      /*
       * First power up after a reset or a self recovery reset. Init state is
       * like follows (from the data sheet):
       *
       * - CLKOUT is selected at CLKOUT pin, default frequency is 32.768 kHz
       *   defined in register EEPROM Control
       * - Timer and Timer Auto-Reload mode are disabled; Timer Source Clock
       *   frequency is set to 32Hz
       * - Self Recovery function is enabled
       * - Automatic EEPROM Refresh every hour is enabled
       * - 24 hour mode is selected, no Alarm is set
       * - All Interrupts are disabled
       * - At Power-On Reset, "PON" Flag is set = "1" and has to be cleared by
       *   writing = "0"
       * - At Self-Recovery Reset or System Reset, "SR" Flag is set = "1" and
       *   has to be cleared by writing = "0".
       * - Values in the clock page are undefined.
       *
       * That's a quite sensible default. Only a well defined value for the time
       * and the PON / SR flags have to be set.
       */
      uint8_t clock[CLOCK_LEN] = {0, 0, 0, 1, 1, 1, 1};

      rv = i2c_rtc_write(&ctx->base, REG_ABEOZ9_CLOCK_SEC, clock, CLOCK_LEN);

      if (rv == 0) {
        reg &= ~(ABEOZ9_CONTROL_STATUS_PON | ABEOZ9_CONTROL_STATUS_SR);
        rv = i2c_rtc_write(&ctx->base, REG_ABEOZ9_CONTROL_STATUS, &reg, 1);
      }
    } else {
      /*
       * RTC already active. Disable all alarms and similar functionality.
       * Otherwise leave it in default state.
       */
      uint8_t alarm[ALARM_LEN] = {0, 0, 0, 0, 0, 0, 0};

      reg = 0;
      rv = i2c_rtc_write(&ctx->base, REG_ABEOZ9_CONTROL_INT, &reg, 1);

      reg = 0;
      rv = i2c_rtc_write(&ctx->base, REG_ABEOZ9_ALARM_SEC, alarm, ALARM_LEN);
    }
  }

  return rv;
}
