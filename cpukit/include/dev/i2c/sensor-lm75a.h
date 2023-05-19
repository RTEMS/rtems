/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Temperature Sensor LM75A Driver API
 *
 * @ingroup I2CSensorLM75A
 */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#ifndef _DEV_I2C_SENSOR_LM75A_H
#define _DEV_I2C_SENSOR_LM75A_H

#include <dev/i2c/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup I2CSensorLM75A Temperature Sensor LM75A Driver
 *
 * @ingroup I2CDevice
 *
 * @brief Driver for NXP or Texas Instruments LM75A temperature sensor.
 *
 * @{
 */

int i2c_dev_register_sensor_lm75a(
  const char *bus_path,
  const char *dev_path,
  uint16_t address
);

typedef enum {
  SENSOR_LM75A_GET_CONF = I2C_DEV_IO_CONTROL,
  SENSOR_LM75A_SET_CONF,
  SENSOR_LM75A_CLEAR_AND_SET_CONF,
  SENSOR_LM75A_GET_TEMP,
  SENSOR_LM75A_GET_TOS,
  SENSOR_LM75A_SET_TOS,
  SENSOR_LM75A_GET_THYST,
  SENSOR_LM75A_SET_THYST
} sensor_lm75a_command;

static inline int sensor_lm75a_get_conf(int fd, uint8_t *val)
{
  return ioctl(fd, SENSOR_LM75A_GET_CONF, val);
}

static inline int sensor_lm75a_set_conf(int fd, uint8_t val)
{
  return ioctl(fd, SENSOR_LM75A_SET_CONF, (void *)(uintptr_t) val);
}

static inline int sensor_lm75a_clear_and_set_conf(
  int fd,
  uint8_t clear,
  uint8_t set
)
{
  uint16_t clear_and_set = (uint16_t) (((uint16_t) set << 8) | clear);

  return ioctl(
    fd,
    SENSOR_LM75A_CLEAR_AND_SET_CONF,
    (void *)(uintptr_t) clear_and_set
  );
}

static inline int sensor_lm75a_get_temp(int fd, int16_t *val)
{
  return ioctl(fd, SENSOR_LM75A_GET_TEMP, val);
}

static inline int sensor_lm75a_get_temp_celsius(int fd, double *celsius)
{
  int rv;
  int16_t val;

  rv = ioctl(fd, SENSOR_LM75A_GET_TEMP, &val);
  *celsius = (((int) val) >> 5) * 0.125;
  return rv;
}

static inline int sensor_lm75a_get_tos(int fd, uint16_t *val)
{
  return ioctl(fd, SENSOR_LM75A_GET_TOS, val);
}

static inline int sensor_lm75a_set_tos(int fd, uint16_t val)
{
  return ioctl(fd, SENSOR_LM75A_SET_TOS, (void *)(uintptr_t) val);
}

static inline int sensor_lm75a_get_thyst(int fd, uint16_t *val)
{
  return ioctl(fd, SENSOR_LM75A_GET_THYST, val);
}

static inline int sensor_lm75a_set_thyst(int fd, uint16_t val)
{
  return ioctl(fd, SENSOR_LM75A_SET_THYST, (void *)(uintptr_t) val);
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DEV_I2C_SENSOR_LM75A_H */
