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

/*
 * Only 24 hour format is supported. If this driver is the only ones who write
 * the RTC, that shouldn't be a problem.
 *
 * The weekday register is not used. It has a user-defined representation anyway
 * and therefore doesn't really matter.
 */

#include <libchip/mcp7940m-rtc.h>

#define REG_MCP7940M_RTCSEC 0x00u
#define REG_MCP7940M_RTCMIN 0x01u
#define REG_MCP7940M_RTCHOUR 0x02u
#define REG_MCP7940M_RTCWKDAY 0x03u
#define REG_MCP7940M_RTCDATE 0x04u
#define REG_MCP7940M_RTCMTH 0x05u
#define REG_MCP7940M_RTCYEAR 0x06u

#define MCP7940M_RTCSEC_ST   (0x01u << 7)
#define MCP7940M_RTCMTH_LPYR   (0x01u << 5)

#define REG_MCP7940M_CONTROL 0x07

#define MCP7940M_CONTROL_OUT     (0x1u << 7)
#define MCP7940M_CONTROL_SQWEN   (0x1u << 6)
#define MCP7940M_CONTROL_ALM1EN  (0x1u << 5)
#define MCP7940M_CONTROL_ALM0EN  (0x1u << 4)
#define MCP7940M_CONTROL_EXTOSC  (0x1u << 3)
#define MCP7940M_CONTROL_CRSTRIM (0x1u << 2)
#define MCP7940M_CONTROL_SQWFS1  (0x1u << 1)
#define MCP7940M_CONTROL_SQWFS0  (0x1u << 0)

int mcp7940m_hw_init(struct i2c_rtc_base *base)
{
  uint8_t reg;
  ssize_t rv;
  struct mcp7940m_rtc *ctx =
      RTEMS_CONTAINER_OF(base, struct mcp7940m_rtc, base);

  /*
   * Make sure that all alarms and outputs are disabled. Enable or disable
   * oscillator.
   *
   * This makes sure that we can start with an uninitialized device that has a
   * random value in the control register.
   */
  reg = 0;
  if (!ctx->crystal) {
    reg |= MCP7940M_CONTROL_EXTOSC;
  }
  rv = i2c_rtc_write(&ctx->base, REG_MCP7940M_CONTROL, &reg, 1);

  if (rv == 0 && ctx->crystal) {
    rv = i2c_rtc_read(&ctx->base, REG_MCP7940M_RTCSEC, &reg, 1);
    if (rv == 0 && (reg & MCP7940M_RTCSEC_ST) == 0) {
      reg |= MCP7940M_RTCSEC_ST;
      rv = i2c_rtc_write(&ctx->base, REG_MCP7940M_RTCSEC, &reg, 1);
    }
  }

  return rv;
}
