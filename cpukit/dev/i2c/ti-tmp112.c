/*
 * Copyright (c) 2016-2017 Chris Johns <chrisj@rtems.org>
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <dev/i2c/i2c.h>
#include <dev/i2c/ti-tmp112.h>

/*
 * Registers.
 */
#define TMP_TEMPERATURE (0)
#define TMP_CONFIG      (1)
#define TMP_LOW_TEMP    (2)
#define TMP_HIGH_TEMP   (3)

/*
 * Configuration register.
 */
#define CFG_ONE_SHOT_BASE         (15)
#define CFG_SHUTDOWN_BASE         (8)
#define CFG_EXTENDED_MODE_BASE    (4)

#define CFG_ONE_SHOT              (1 << CFG_ONE_SHOT_BASE)
#define CFG_SHUTDOWN              (1 << CFG_SHUTDOWN_BASE)
#define CFG_EXTENDED_MODE         (1 << CFG_EXTENDED_MODE_BASE)

typedef struct {
  i2c_dev  base;
  uint8_t  pointer;
  uint16_t config_shadow;
} ti_tmp112;

static int
ti_tmp112_reg_write(ti_tmp112* dev, int reg, uint16_t value)
{
  uint8_t out[3];
  i2c_msg msgs[1] = {
    {
      .addr = dev->base.address,
      .flags = 0,
      .len = (uint16_t) sizeof(out),
      .buf = &out[0]
    }
  };
  int err;
  if (dev->pointer == reg) {
    out[0] = (uint8_t) (value >> 8);
    out[1] = (uint8_t) value;
    msgs[0].len = 2;
  }
  else {
    out[0] = reg;
    out[1] = (uint8_t) (value >> 8);
    out[2] = (uint8_t) value;
  }
  err = i2c_bus_transfer(dev->base.bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
  if (err == 0)
    dev->pointer = reg;
  else
    dev->pointer = -1;
  return err;
}

static int
ti_tmp112_reg_read(ti_tmp112* dev, int reg, uint16_t* value)
{
  uint8_t in[2] = { 0, 0 };
  int     err;
  if (dev->pointer == reg) {
    i2c_msg msgs[1] = {
      {
        .addr = dev->base.address,
        .flags = I2C_M_RD,
        .len = (uint16_t) sizeof(in),
        .buf = &in[0]
      }
    };
    err = i2c_bus_transfer(dev->base.bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
  }
  else {
    uint8_t out[1] = { (uint8_t) reg };
    i2c_msg msgs[2] = {
      {
        .addr = dev->base.address,
        .flags = 0,
        .len = (uint16_t) sizeof(out),
        .buf = &out[0]
      }, {
        .addr = dev->base.address,
        .flags = I2C_M_RD,
        .len = (uint16_t) sizeof(in),
        .buf = &in[0]
      }
    };
    err = i2c_bus_transfer(dev->base.bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
    if (err == 0)
      dev->pointer = reg;
    else
      dev->pointer = -1;
  }
  *value = (((uint16_t) in[0]) << 8) | in[1];
  return err;
}

static int
ti_tmp112_set_config(ti_tmp112* dev, uint16_t value)
{
  int err;
  dev->config_shadow = value;
  err = ti_tmp112_reg_write(dev, TMP_CONFIG, dev->config_shadow);
  return err;
}

static int
ti_tmp112_get_temp(ti_tmp112* dev, int* temp, bool raw)
{
  uint16_t value = 0;
  int      err;

  *temp = 0;

  if ((dev->config_shadow & CFG_SHUTDOWN) == CFG_SHUTDOWN) {
    i2c_bus_obtain(dev->base.bus);
    err = ti_tmp112_reg_write(dev,
                           TMP_CONFIG,
                           dev->config_shadow | CFG_ONE_SHOT);
    if (err == 0) {
      uint16_t config = 0;
      while (err == 0 && (config & CFG_ONE_SHOT) == 0)
        err = ti_tmp112_reg_read(dev, TMP_CONFIG, &config);
      err = ti_tmp112_reg_read(dev, TMP_TEMPERATURE, &value);
    }
    i2c_bus_release(dev->base.bus);
  }
  else {
    err = ti_tmp112_reg_read(dev, TMP_TEMPERATURE, &value);
  }

  if (err == 0) {
    if (raw) {
      *temp = (int) value;
    }
    else {
      int      bits = 12;
      uint32_t u;
      if ((dev->config_shadow & CFG_EXTENDED_MODE) != 0)
        bits = 13;
      u = value >> ((sizeof(value) * 8) - bits);
      *temp = (int) (u  & ((1 << bits) - 1));
      if ((u & (1 << (bits - 1))) != 0)
        *temp |= ~((1 << bits) - 1);
      *temp = *temp * 625;
    }
  }

  return err;
}

static int
ti_tmp112_ioctl(i2c_dev* iic_dev, ioctl_command_t command, void* arg)
{
  ti_tmp112* dev = (ti_tmp112*) iic_dev;
  uint16_t   v16;
  int        vint;
  int        err;

  switch (command) {
    case TI_TMP112_GET_TEMP:
      vint = 0;
      err = ti_tmp112_get_temp(dev, &vint, false);
      if (err == 0)
        *((int*) arg) = vint;
      break;
    case TI_TMP112_GET_TEMP_RAW:
      vint = 0;
      err = ti_tmp112_get_temp(dev, &vint, true);
      if (err == 0)
        *((uint16_t*) arg) = (uint16_t) vint;
      break;
    case TI_TMP112_SET_CONFIG:
      v16 = (uint16_t)(uintptr_t) arg;
      err = ti_tmp112_set_config(dev, v16);
      break;
    case TI_TMP112_SET_LOW_TEMP:
      v16 = (uint16_t)(uintptr_t) arg;
      err = ti_tmp112_reg_write(dev, TMP_LOW_TEMP, v16);
      break;
    case TI_TMP112_SET_HIGH_TEMP:
      v16 = (uint16_t)(uintptr_t) arg;
      err = ti_tmp112_reg_write(dev, TMP_HIGH_TEMP, v16);
      break;
    default:
      err = -ENOTTY;
      break;
  }

  return err;
}

int
i2c_dev_register_ti_tmp112(const char* bus_path,
                           const char* dev_path,
                           uint16_t    address)
{
  ti_tmp112* dev;

  dev = (ti_tmp112*)
    i2c_dev_alloc_and_init(sizeof(*dev), bus_path, address);
  if (dev == NULL) {
    return -1;
  }

  dev->base.ioctl = ti_tmp112_ioctl;
  dev->pointer = -1;
  dev->config_shadow = 0x60a0;

  return i2c_dev_register(&dev->base, dev_path);
}
