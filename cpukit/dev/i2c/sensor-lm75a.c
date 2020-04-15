/**
 * @file
 *
 * @brief Temperature Sensor LM75A Driver Implementation
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dev/i2c/sensor-lm75a.h>

typedef enum {
  SENSOR_LM75A_PTR_TEMP,
  SENSOR_LM75A_PTR_CONF,
  SENSOR_LM75A_PTR_THYST,
  SENSOR_LM75A_PTR_TOS
} sensor_lm75a_ptr;

static int sensor_lm75a_get_reg_8(
  i2c_dev *dev,
  sensor_lm75a_ptr ptr,
  uint8_t *val
)
{
  uint8_t out[1] = { ptr };
  uint8_t in[sizeof(*val)];
  i2c_msg msgs[2] = {
    {
      .addr = dev->address,
      .flags = 0,
      .len = (uint16_t) sizeof(out),
      .buf = &out[0]
    }, {
      .addr = dev->address,
      .flags = I2C_M_RD,
      .len = (uint16_t) sizeof(in),
      .buf = &in[0]
    }
  };
  int err;

  err = i2c_bus_transfer(dev->bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
  *val = in[0];

  return err;
}

static int sensor_lm75a_set_reg_8(
  i2c_dev *dev,
  sensor_lm75a_ptr ptr,
  uint8_t val
)
{
  uint8_t out[2] = { ptr, val };
  i2c_msg msgs[1] = {
    {
      .addr = dev->address,
      .flags = 0,
      .len = (uint16_t) sizeof(out),
      .buf = &out[0]
    }
  };

  return i2c_bus_transfer(dev->bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
}

static int sensor_lm75a_get_reg_16(
  i2c_dev *dev,
  sensor_lm75a_ptr ptr,
  uint16_t *val
)
{
  uint8_t out[1] = { ptr };
  uint8_t in[sizeof(*val)];
  i2c_msg msgs[2] = {
    {
      .addr = dev->address,
      .flags = 0,
      .len = (uint16_t) sizeof(out),
      .buf = &out[0]
    }, {
      .addr = dev->address,
      .flags = I2C_M_RD,
      .len = (uint16_t) sizeof(in),
      .buf = &in[0]
    }
  };
  int err;

  err = i2c_bus_transfer(dev->bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
  *val = (in[0] << 8) | in[1];

  return err;
}

static int sensor_lm75a_set_reg_16(
  i2c_dev *dev,
  sensor_lm75a_ptr ptr,
  uint16_t val
)
{
  uint8_t out[3] = { ptr, (uint8_t) (val >> 8), (uint8_t) val };
  i2c_msg msgs[1] = {
    {
      .addr = dev->address,
      .flags = 0,
      .len = (uint16_t) sizeof(out),
      .buf = &out[0]
    }
  };

  return i2c_bus_transfer(dev->bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
}

static int sensor_lm75a_ioctl(
  i2c_dev *dev,
  ioctl_command_t command,
  void *arg
)
{
  uint8_t v8 = (uint8_t) (uintptr_t) arg;
  uint16_t v16 = (uint16_t) (uintptr_t) arg;
  int err;

  switch (command) {
    case SENSOR_LM75A_GET_CONF:
      err = sensor_lm75a_get_reg_8(dev, SENSOR_LM75A_PTR_CONF, arg);
      break;
    case SENSOR_LM75A_SET_CONF:
      err = sensor_lm75a_set_reg_8(dev, SENSOR_LM75A_PTR_CONF, v8);
      break;
    case SENSOR_LM75A_CLEAR_AND_SET_CONF:
      i2c_bus_obtain(dev->bus);
      err = sensor_lm75a_get_reg_8(dev, SENSOR_LM75A_PTR_CONF, &v8);
      if (err == 0) {
        v8 &= ~((uint8_t) v16);
        v8 |= (uint8_t) (v16 >> 8);
        err = sensor_lm75a_set_reg_8(dev, SENSOR_LM75A_PTR_CONF, v8);
      }
      i2c_bus_release(dev->bus);
      break;
    case SENSOR_LM75A_GET_TEMP:
      err = sensor_lm75a_get_reg_16(dev, SENSOR_LM75A_PTR_TEMP, arg);
      break;
    case SENSOR_LM75A_GET_TOS:
      err = sensor_lm75a_get_reg_16(dev, SENSOR_LM75A_PTR_TOS, arg);
      break;
    case SENSOR_LM75A_SET_TOS:
      err = sensor_lm75a_set_reg_16(dev, SENSOR_LM75A_PTR_TOS, v16);
      break;
    case SENSOR_LM75A_GET_THYST:
      err = sensor_lm75a_get_reg_16(dev, SENSOR_LM75A_PTR_THYST, arg);
      break;
    case SENSOR_LM75A_SET_THYST:
      err = sensor_lm75a_set_reg_16(dev, SENSOR_LM75A_PTR_THYST, v16);
      break;
    default:
      err = -ENOTTY;
      break;
  }

  return err;
}

int i2c_dev_register_sensor_lm75a(
  const char *bus_path,
  const char *dev_path,
  uint16_t address
)
{
  i2c_dev *dev;

  dev = i2c_dev_alloc_and_init(sizeof(*dev), bus_path, address);
  if (dev == NULL) {
    return -1;
  }

  dev->ioctl = sensor_lm75a_ioctl;

  return i2c_dev_register(dev, dev_path);
}
