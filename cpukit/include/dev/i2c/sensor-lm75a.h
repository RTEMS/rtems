/**
 * @file
 *
 * @brief Temperature Sensor LM75A Driver API
 *
 * @ingroup I2CSensorLM75A
 */

/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
