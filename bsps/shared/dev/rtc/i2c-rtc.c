/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup shared_tod_i2c_rtc
 *
 * @brief This file provides the implementation of @ref shared_tod_i2c_rtc.
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

#include <dev/i2c/i2c.h>
#include <libchip/i2c-rtc.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/todimpl.h>

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define RTCSEC_SECBCD_SHIFT  0u
#define RTCSEC_SECBCD_MASK   (0x7fu << RTCSEC_SECBCD_SHIFT)
#define RTCSEC_SECBCD(x)     (((x) << RTCSEC_SECBCD_SHIFT) & RTCSEC_SECBCD_MASK)
#define RTCSEC_SECBCD_GET(x) (((x) & RTCSEC_SECBCD_MASK) >> RTCSEC_SECBCD_SHIFT)

#define RTCMIN_MINBCD_SHIFT  0u
#define RTCMIN_MINBCD_MASK   (0x7fu << RTCMIN_MINBCD_SHIFT)
#define RTCMIN_MINBCD(x)     (((x) << RTCMIN_MINBCD_SHIFT) & RTCMIN_MINBCD_MASK)
#define RTCMIN_MINBCD_GET(x) (((x) & RTCMIN_MINBCD_MASK) >> RTCMIN_MINBCD_SHIFT)

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

#define RTCWKDAY_WKDAY_SHIFT   0u
#define RTCWKDAY_WKDAY_MASK    (0x7u << RTCWKDAY_WKDAY_SHIFT)
#define RTCWKDAY_WKDAY(x)      (((x) << RTCWKDAY_WKDAY_SHIFT) & RTCWKDAY_WKDAY_MASK)
#define RTCWKDAY_WKDAY_GET(x)  (((x) & RTCWKDAY_WKDAY_MASK) >> RTCWKDAY_WKDAY_SHIFT)

#define RTCDATE_DATEBCD_SHIFT  0u
#define RTCDATE_DATEBCD_MASK   (0x3fu << RTCDATE_DATEBCD_SHIFT)
#define RTCDATE_DATEBCD(x)     (((x) << RTCDATE_DATEBCD_SHIFT) & RTCDATE_DATEBCD_MASK)
#define RTCDATE_DATEBCD_GET(x) (((x) & RTCDATE_DATEBCD_MASK) >> RTCDATE_DATEBCD_SHIFT)

#define RTCMTH_MTHBCD_SHIFT    0u
#define RTCMTH_MTHBCD_MASK     (0x1fu << RTCMTH_MTHBCD_SHIFT)
#define RTCMTH_MTHBCD(x)       (((x) << RTCMTH_MTHBCD_SHIFT) & RTCMTH_MTHBCD_MASK)
#define RTCMTH_MTHBCD_GET(x)   (((x) & RTCMTH_MTHBCD_MASK) >> RTCMTH_MTHBCD_SHIFT)

#define RTCYEAR_YRBCD_SHIFT    0u
#define RTCYEAR_YRBCD_MASK     (0xffu << RTCYEAR_YRBCD_SHIFT)
#define RTCYEAR_YRBCD(x)       (((x) << RTCYEAR_YRBCD_SHIFT) & RTCYEAR_YRBCD_MASK)
#define RTCYEAR_YRBCD_GET(x)   (((x) & RTCYEAR_YRBCD_MASK) >> RTCYEAR_YRBCD_SHIFT)

#define NR_RTC_REGISTERS 0x07u

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

static struct i2c_rtc_base *i2c_rtc_get_context(int minor)
{
  return (struct i2c_rtc_base *) RTC_Table[minor].pDeviceParams;
}

int i2c_rtc_read(
    struct i2c_rtc_base *ctx,
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

int i2c_rtc_write(
    struct i2c_rtc_base *ctx,
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

static int i2c_rtc_initialize_once(struct i2c_rtc_base *ctx)
{
  int rv;

  if (ctx->initialized || ctx->hw_init == NULL) {
    return 0;
  }

  rv = ctx->hw_init(ctx);

  if (rv == 0) {
    ctx->initialized = true;
  }

  return rv;
}

static int i2c_rtc_get_time(int minor, rtems_time_of_day *time)
{
  int rv = 0;
  uint8_t buf[NR_RTC_REGISTERS];
  struct i2c_rtc_base *ctx = i2c_rtc_get_context(minor);

  if (!_System_state_Is_up(_System_state_Get())) {
      return -1;
  }

  rtems_mutex_lock(&ctx->mutex);

  rv = i2c_rtc_initialize_once(ctx);

  if (rv == 0) {
    rv = i2c_rtc_read(ctx, ctx->clock_offset, buf, sizeof(buf));
  }

  if (rv == 0) {
    unsigned year = bcd_to_bin(RTCYEAR_YRBCD_GET(buf[ctx->order.year])) +
                    (TOD_BASE_YEAR / 100 * 100);
    if (year < TOD_BASE_YEAR) {
      year += 100;
    }
    time->year = year;
    time->month = bcd_to_bin(RTCMTH_MTHBCD_GET(buf[ctx->order.month]));
    time->day = bcd_to_bin(RTCDATE_DATEBCD_GET(buf[ctx->order.day]));
    time->hour = bcd_to_bin(RTCHOUR_HRBCD24_GET(buf[ctx->order.hour]));
    time->minute = bcd_to_bin(RTCMIN_MINBCD_GET(buf[ctx->order.min]));
    time->second = bcd_to_bin(RTCSEC_SECBCD_GET(buf[ctx->order.sec]));
    time->ticks  = 0;
  }

  rtems_mutex_unlock(&ctx->mutex);

  return rv;
}

static int i2c_rtc_set_time(int minor, const rtems_time_of_day *time)
{
  int rv = 0;
  uint8_t buf[NR_RTC_REGISTERS];
  struct i2c_rtc_base *ctx = i2c_rtc_get_context(minor);

  if (!_System_state_Is_up(_System_state_Get())) {
      return -1;
  }

  rtems_mutex_lock(&ctx->mutex);

  rv = i2c_rtc_initialize_once(ctx);

  if (rv == 0) {
    rv = i2c_rtc_read(ctx, ctx->clock_offset, buf, sizeof(buf));
  }

  if (rv == 0) {
    /* Make sure weekday is not 0 (out of range). Otherwise it's not used. */
    if (RTCWKDAY_WKDAY_GET(buf[ctx->order.wkday]) < 1) {
      buf[ctx->order.wkday] &= ~RTCWKDAY_WKDAY_MASK;
      buf[ctx->order.wkday] |= RTCWKDAY_WKDAY(1);
    }

    buf[ctx->order.year] &= ~RTCYEAR_YRBCD_MASK;
    buf[ctx->order.year] |= RTCYEAR_YRBCD(bin_to_bcd(time->year % 100));

    buf[ctx->order.month] &= ~RTCMTH_MTHBCD_MASK;
    buf[ctx->order.month] |= RTCMTH_MTHBCD(bin_to_bcd(time->month));

    buf[ctx->order.day] &= ~RTCDATE_DATEBCD_MASK;
    buf[ctx->order.day] |= RTCDATE_DATEBCD(bin_to_bcd(time->day));

    buf[ctx->order.hour] &= ~(RTCHOUR_HRBCD24_MASK | RTCHOUR_1224);
    buf[ctx->order.hour] |= RTCHOUR_HRBCD24(bin_to_bcd(time->hour));

    buf[ctx->order.min] &= ~RTCMIN_MINBCD_MASK;
    buf[ctx->order.min] |= RTCMIN_MINBCD(bin_to_bcd(time->minute));

    buf[ctx->order.sec] &= ~RTCSEC_SECBCD_MASK;
    buf[ctx->order.sec] |= RTCSEC_SECBCD(bin_to_bcd(time->second));

    rv = i2c_rtc_write(ctx, ctx->clock_offset, buf, sizeof(buf));
  }

  rtems_mutex_unlock(&ctx->mutex);

  return rv;
}

static void i2c_rtc_init(int minor)
{
  (void) minor;
}

bool i2c_rtc_probe(int minor)
{
  (void) minor;

  return true;
}

const rtc_fns i2c_rtc_fns = {
  .deviceInitialize = i2c_rtc_init,
  .deviceGetTime =    i2c_rtc_get_time,
  .deviceSetTime =    i2c_rtc_set_time,
};
