/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016-2017 Chris Johns <chrisj@rtems.org>
 * All rights reserved.
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
 * TI TMP112
 *   http://www.ti.com/product/TMP112
 */

#ifndef TI_TMP112_H
#define TI_TMP112_H

#include <dev/i2c/i2c.h>

/*
 * Confirguration.
 */
#define TI_TMP112_CR_0_25Hz      (0 << 6)
#define TI_TMP112_CR_1Hz         (1 << 6)
#define TI_TMP112_CR_4Hz         (2 << 6)    /* default */
#define TI_TMP112_CR_8Hz         (3 << 6)
#define TI_TMP112_EM_NORMAL      (0 << 4)    /* default */
#define TI_TMP112_EM_EXTENDED    (1 << 4)
#define TI_TMP112_SD_ON          (0 << 8)    /* default */
#define TI_TMP112_SD_SHUTDOWN    (1 << 8)
#define TI_TMP112_TM_COMPARATOR  (0 << 9)    /* default */
#define TI_TMP112_TM_INTERRUPT   (1 << 9)
#define TI_TMP112_POL_ALERT_LOW  (0 << 10)   /* default */
#define TI_TMP112_POL_ALERT_HIGH (1 << 10)
#define TI_TMP112_FQL_1          (0 << 11)   /* default */
#define TI_TMP112_FQL_2          (1 << 11)
#define TI_TMP112_FQL_4          (2 << 11)
#define TI_TMP112_FQL_6          (3 << 11)

/*
 * IO control interface.
 */
#define TI_TMP112_GET_TEMP      (I2C_DEV_IO_CONTROL + 0)
#define TI_TMP112_GET_TEMP_RAW  (I2C_DEV_IO_CONTROL + 1)
#define TI_TMP112_SET_CONFIG    (I2C_DEV_IO_CONTROL + 2)
#define TI_TMP112_SET_LOW_TEMP  (I2C_DEV_IO_CONTROL + 3)
#define TI_TMP112_SET_HIGH_TEMP (I2C_DEV_IO_CONTROL + 4)

/*
 * Register the device.
 */
int i2c_dev_register_ti_tmp112(const char* bus_path,
                               const char* dev_path,
                               uint16_t    address);

/*
 * Get the temperature in degrees C x 10000. To print you would so:
 *
 *   printf("Temperature is %3d.%04d\n", temp / 10000, temp % 10000);
 *
 * If the device is shutdown a single conversion is made waiting for the
 * conversion to complete.
 */
static inline int
ti_tmp112_get_temperature(int fd, int* temp)
{
  return ioctl(fd, TI_TMP112_GET_TEMP, temp);
}

/*
 * Get the temperature as the raw register value.
 *
 * If the device is shutdown a single conversion is made waiting for the
 * conversion to complete.
 */
static inline int
ti_tmp112_get_temperature_raw(int fd, unsigned int* temp)
{
  return ioctl(fd, TI_TMP112_GET_TEMP_RAW, temp);
}

/*
 * Set the configuration.
 */
static inline int
ti_tmp112_adc_set_config(int fd, uint16_t config)
{
  return ioctl(fd, TI_TMP112_SET_CONFIG, (void *)(uintptr_t) config);
}

/*
 * Set the low temperature.
 */
static inline int
ti_tmp112_set_low_temperator(int fd, uint16_t temp)
{
  return ioctl(fd, TI_TMP112_SET_LOW_TEMP, (void *)(uintptr_t) temp);
}

/*
 * Set the high temperature.
 */
static inline int
ti_tmp112_adc_set_high_threshold(int fd, uint16_t level)
{
  return ioctl(fd, TI_TMP112_SET_HIGH_TEMP, (void *)(uintptr_t) level);
}

#endif
