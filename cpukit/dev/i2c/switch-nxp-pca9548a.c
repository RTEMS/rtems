/**
 * @file
 *
 * @brief Switch NXP PCA9548A Driver Implementation
 *
 * @ingroup I2CSWITCHNXPPCA9548A
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <dev/i2c/switch-nxp-pca9548a.h>

static int switch_nxp_pca9548a_do_get_control(
  i2c_dev *dev,
  uint8_t *val
)
{
  i2c_msg msg = {
    .addr = dev->address,
    .flags = I2C_M_RD,
    .len = (uint16_t) sizeof(*val),
    .buf = val
  };

  return i2c_bus_transfer(dev->bus, &msg, 1);
}

static int switch_nxp_pca9548a_do_set_control(
  i2c_dev *dev,
  uint8_t val
)
{
  i2c_msg msg = {
    .addr = dev->address,
    .flags = 0,
    .len = (uint16_t) sizeof(val),
    .buf = &val
  };

  return i2c_bus_transfer(dev->bus, &msg, 1);
}

static int switch_nxp_pca9548a_ioctl(
  i2c_dev *dev,
  ioctl_command_t command,
  void *arg
)
{
  uint8_t v8 = (uint8_t)(uintptr_t) arg;
  int err;

  switch (command) {
    case SWITCH_NXP_PCA9548A_GET_CONTROL:
      err = switch_nxp_pca9548a_do_get_control(dev, arg);
      break;
    case SWITCH_NXP_PCA9548A_SET_CONTROL:
      err = switch_nxp_pca9548a_do_set_control(dev, v8);
      break;
    default:
      err = -ENOTTY;
      break;
  }

  return err;
}

int i2c_dev_register_switch_nxp_pca9548a(
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

  dev->ioctl = switch_nxp_pca9548a_ioctl;

  return i2c_dev_register(dev, dev_path);
}
