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

/*
 * Note: This driver implements only the basic RTC functionality of the
 * MCP7940M. It tries not to touch any register fields except for the basic
 * date/time fields in the get/set time functions. That way it should be
 * possible to re-use the driver for similar RTCs by just replacing the
 * initialization function. Suggested method for that: Add a field to the struct
 * mcp7940m_rtc with a function pointer that points to the initialization
 * function.
 *
 * All flags that are considered MCP7940M specific have a MCP7940M in the name.
 *
 * Only 24 hour format is supported. If this driver is the only ones who write
 * the RTC, that shouldn't be a problem.
 *
 * The weekday register is not used. It has a user-defined representation anyway
 * and therefore doesn't really matter.
 */

#include <dev/i2c/i2c.h>
#include <libchip/mcp7940m-rtc.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/todimpl.h>

#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define REG_RTCSEC 0x00u

#define RTCSEC_SECBCD_SHIFT  0u
#define RTCSEC_SECBCD_MASK   (0x7fu << RTCSEC_SECBCD_SHIFT)
#define RTCSEC_SECBCD(x)     (((x) << RTCSEC_SECBCD_SHIFT) & RTCSEC_SECBCD_MASK)
#define RTCSEC_SECBCD_GET(x) (((x) & RTCSEC_SECBCD_MASK) >> RTCSEC_SECBCD_SHIFT)

#define MCP7940M_RTCSEC_ST   (0x01u << 7)

#define REG_RTCMIN 0x01

#define RTCMIN_MINBCD_SHIFT  0u
#define RTCMIN_MINBCD_MASK   (0x7fu << RTCMIN_MINBCD_SHIFT)
#define RTCMIN_MINBCD(x)     (((x) << RTCMIN_MINBCD_SHIFT) & RTCMIN_MINBCD_MASK)
#define RTCMIN_MINBCD_GET(x) (((x) & RTCMIN_MINBCD_MASK) >> RTCMIN_MINBCD_SHIFT)

#define REG_RTCHOUR 0x02

#define RTCHOUR_HRBCD12_SHIFT  0u
#define RTCHOUR_HRBCD12_MASK   (0x1fu << RTCHOUR_HRBCD12_SHIFT)
#define RTCHOUR_HRBCD12(x)     (((x) << RTCHOUR_HRBCD12_SHIFT) & RTCHOUR_HRBCD12_MASK)
#define RTCHOUR_HRBCD12_GET(x) (((x) & RTCHOUR_HRBCD12_MASK) >> RTCHOUR_HRBCD12_SHIFT)

#define RTCHOUR_HRBCD24_SHIFT  0u
#define RTCHOUR_HRBCD24_MASK   (0x3fu << RTCHOUR_HRBCD24_SHIFT)
#define RTCHOUR_HRBCD24(x)     (((x) << RTCHOUR_HRBCD24_SHIFT) & RTCHOUR_HRBCD24_MASK)
#define RTCHOUR_HRBCD24_GET(x) (((x) & RTCHOUR_HRBCD24_MASK) >> RTCHOUR_HRBCD24_SHIFT)

#define RTCHOUR_AMPM           (0x01u << 5)
#define RTCHOUR_1224           (0x01u << 6)

#define REG_RTCWKDAY 0x03

#define RTCWKDAY_WKDAY_SHIFT   0u
#define RTCWKDAY_WKDAY_MASK    (0x7u << RTCWKDAY_WKDAY_SHIFT)
#define RTCWKDAY_WKDAY(x)      (((x) << RTCWKDAY_WKDAY_SHIFT) & RTCWKDAY_WKDAY_MASK)
#define RTCWKDAY_WKDAY_GET(x)  (((x) & RTCWKDAY_WKDAY_MASK) >> RTCWKDAY_WKDAY_SHIFT)

#define REG_RTCDATE 0x04

#define RTCDATE_DATEBCD_SHIFT  0u
#define RTCDATE_DATEBCD_MASK   (0x3fu << RTCDATE_DATEBCD_SHIFT)
#define RTCDATE_DATEBCD(x)     (((x) << RTCDATE_DATEBCD_SHIFT) & RTCDATE_DATEBCD_MASK)
#define RTCDATE_DATEBCD_GET(x) (((x) & RTCDATE_DATEBCD_MASK) >> RTCDATE_DATEBCD_SHIFT)

#define REG_RTCMTH 0x05

#define RTCMTH_MTHBCD_SHIFT    0u
#define RTCMTH_MTHBCD_MASK     (0x1fu << RTCMTH_MTHBCD_SHIFT)
#define RTCMTH_MTHBCD(x)       (((x) << RTCMTH_MTHBCD_SHIFT) & RTCMTH_MTHBCD_MASK)
#define RTCMTH_MTHBCD_GET(x)   (((x) & RTCMTH_MTHBCD_MASK) >> RTCMTH_MTHBCD_SHIFT)

#define MCP7940M_RTCMTH_LPYR   (0x01u << 5)

#define REG_RTCYEAR 0x06

#define RTCYEAR_YRBCD_SHIFT    0u
#define RTCYEAR_YRBCD_MASK     (0xffu << RTCYEAR_YRBCD_SHIFT)
#define RTCYEAR_YRBCD(x)       (((x) << RTCYEAR_YRBCD_SHIFT) & RTCYEAR_YRBCD_MASK)
#define RTCYEAR_YRBCD_GET(x)   (((x) & RTCYEAR_YRBCD_MASK) >> RTCYEAR_YRBCD_SHIFT)

#define REG_MCP7940M_CONTROL 0x07

#define MCP7940M_CONTROL_OUT     (0x1u << 7)
#define MCP7940M_CONTROL_SQWEN   (0x1u << 6)
#define MCP7940M_CONTROL_ALM1EN  (0x1u << 5)
#define MCP7940M_CONTROL_ALM0EN  (0x1u << 4)
#define MCP7940M_CONTROL_EXTOSC  (0x1u << 3)
#define MCP7940M_CONTROL_CRSTRIM (0x1u << 2)
#define MCP7940M_CONTROL_SQWFS1  (0x1u << 1)
#define MCP7940M_CONTROL_SQWFS0  (0x1u << 0)

static inline uint8_t bcd_to_bin(uint8_t bcd)
{
  uint8_t bin;
  bin = bcd & 0x0f;
  bin += ((bcd >> 4) & 0x0f) * 10;
  return bin;
}

static inline uint8_t bin_to_bcd(uint8_t bin)
{
  uint8_t bcd;
  bcd = bin % 10;
  bcd |= (bin / 10) << 4;
  return bcd;
}

static struct mcp7940m_rtc *mcp7940m_get_context(int minor)
{
  return (struct mcp7940m_rtc *) RTC_Table[minor].pDeviceParams;
}

static int mcp7940m_i2c_read(
    struct mcp7940m_rtc *ctx,
    uint8_t addr,
    uint8_t *buf,
    size_t len
)
{
  int fd;
  int rv;
  struct i2c_msg msgs[] = {{
    .addr = ctx->i2c_addr,
    .flags = 0,
    .buf = &addr,
    .len = 1,
  }, {
    .addr = ctx->i2c_addr,
    .flags = I2C_M_RD,
    .buf = buf,
    .len = len,
  }};
  struct i2c_rdwr_ioctl_data payload = {
    .msgs = msgs,
    .nmsgs = sizeof(msgs)/sizeof(msgs[0]),
  };

  fd = open(ctx->i2c_bus_path, O_RDWR);
  if (fd < 0) {
    return fd;
  }

  rv = ioctl(fd, I2C_RDWR, &payload);

  close(fd);

  return rv;
}

static int mcp7940m_i2c_write(
    struct mcp7940m_rtc *ctx,
    uint8_t addr,
    const uint8_t *buf,
    size_t len
)
{
  int fd;
  int rv;
  uint8_t writebuf[len + 1];
  struct i2c_msg msgs[] = {{
    .addr = ctx->i2c_addr,
    .flags = 0,
    .buf = writebuf,
    .len = len + 1,
  }};
  struct i2c_rdwr_ioctl_data payload = {
    .msgs = msgs,
    .nmsgs = sizeof(msgs)/sizeof(msgs[0]),
  };

  writebuf[0] = addr;
  memcpy(&writebuf[1], buf, len);

  fd = open(ctx->i2c_bus_path, O_RDWR);
  if (fd < 0) {
    return fd;
  }

  rv = ioctl(fd, I2C_RDWR, &payload);

  close(fd);

  return rv;
}

static int mcp7940m_initialize_once(struct mcp7940m_rtc *ctx)
{
  uint8_t reg;
  ssize_t rv;

  if (ctx->initialized) {
    return 0;
  }

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
  rv = mcp7940m_i2c_write(ctx, REG_MCP7940M_CONTROL, &reg, 1);

  if (rv == 0 && ctx->crystal) {
    rv = mcp7940m_i2c_read(ctx, REG_RTCSEC, &reg, 1);
    if (rv == 0 && (reg & MCP7940M_RTCSEC_ST) == 0) {
      reg |= MCP7940M_RTCSEC_ST;
      rv = mcp7940m_i2c_write(ctx, REG_RTCSEC, &reg, 1);
    }
  }

  ctx->initialized = true;

  return rv;
}

static int mcp7940m_get_time(int minor, rtems_time_of_day *time)
{
  int rv = 0;
  uint8_t buf[REG_RTCYEAR + 1];
  struct mcp7940m_rtc *ctx = mcp7940m_get_context(minor);

  if (!_System_state_Is_up(_System_state_Get())) {
      return -1;
  }

  rtems_mutex_lock(&ctx->mutex);

  rv = mcp7940m_initialize_once(ctx);

  if (rv == 0) {
    rv = mcp7940m_i2c_read(ctx, REG_RTCSEC, buf, sizeof(buf));
  }

  if (rv == 0) {
    unsigned year = bcd_to_bin(RTCYEAR_YRBCD_GET(buf[REG_RTCYEAR])) +
                    (TOD_BASE_YEAR / 100 * 100);
    if (year < TOD_BASE_YEAR) {
      year += 100;
    }
    time->year = year;
    time->month = bcd_to_bin(RTCMTH_MTHBCD_GET(buf[REG_RTCMTH]));
    time->day = bcd_to_bin(RTCDATE_DATEBCD_GET(buf[REG_RTCDATE]));
    time->hour = bcd_to_bin(RTCHOUR_HRBCD24_GET(buf[REG_RTCHOUR]));
    time->minute = bcd_to_bin(RTCMIN_MINBCD_GET(buf[REG_RTCMIN]));
    time->second = bcd_to_bin(RTCSEC_SECBCD_GET(buf[REG_RTCSEC]));
    time->ticks  = 0;
  }

  rtems_mutex_unlock(&ctx->mutex);

  return rv;
}

static int mcp7940m_set_time(int minor, const rtems_time_of_day *time)
{
  int rv = 0;
  uint8_t buf[REG_RTCYEAR + 1];
  struct mcp7940m_rtc *ctx = mcp7940m_get_context(minor);

  if (!_System_state_Is_up(_System_state_Get())) {
      return -1;
  }

  rtems_mutex_lock(&ctx->mutex);

  rv = mcp7940m_initialize_once(ctx);

  if (rv == 0) {
    rv = mcp7940m_i2c_read(ctx, REG_RTCSEC, buf, sizeof(buf));
  }

  if (rv == 0) {
    /* Make sure weekday is not 0 (out of range). Otherwise it's not used. */
    if (RTCWKDAY_WKDAY_GET(buf[REG_RTCWKDAY]) < 1) {
      buf[REG_RTCWKDAY] &= ~RTCWKDAY_WKDAY_MASK;
      buf[REG_RTCWKDAY] |= RTCWKDAY_WKDAY(1);
    }

    buf[REG_RTCYEAR] &= ~RTCYEAR_YRBCD_MASK;
    buf[REG_RTCYEAR] |= RTCYEAR_YRBCD(bin_to_bcd(time->year % 100));

    buf[REG_RTCMTH] &= ~RTCMTH_MTHBCD_MASK;
    buf[REG_RTCMTH] |= RTCMTH_MTHBCD(bin_to_bcd(time->month));

    buf[REG_RTCDATE] &= ~RTCDATE_DATEBCD_MASK;
    buf[REG_RTCDATE] |= RTCDATE_DATEBCD(bin_to_bcd(time->day));

    buf[REG_RTCHOUR] &= ~(RTCHOUR_HRBCD24_MASK | RTCHOUR_1224);
    buf[REG_RTCHOUR] |= RTCHOUR_HRBCD24(bin_to_bcd(time->hour));

    buf[REG_RTCMIN] &= ~RTCMIN_MINBCD_MASK;
    buf[REG_RTCMIN] |= RTCMIN_MINBCD(bin_to_bcd(time->minute));

    buf[REG_RTCSEC] &= ~RTCSEC_SECBCD_MASK;
    buf[REG_RTCSEC] |= RTCSEC_SECBCD(bin_to_bcd(time->second));

    rv = mcp7940m_i2c_write(ctx, REG_RTCSEC, buf, sizeof(buf));
  }

  rtems_mutex_unlock(&ctx->mutex);

  return rv;
}

static void mcp7940m_init(int minor)
{
  (void) minor;
}

bool rtc_mcp7940m_probe(int minor)
{
  return true;
}

const rtc_fns rtc_mcp7940m_fns = {
  .deviceInitialize = mcp7940m_init,
  .deviceGetTime =    mcp7940m_get_time,
  .deviceSetTime =    mcp7940m_set_time,
};
