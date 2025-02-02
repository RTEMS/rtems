/**
 * @file
 *
 * @brief Switch NXP PCA9548A Driver Implementation
 *
 * @ingroup I2CSWITCHNXPPCA9548A
 */

/*
 * Copyright (C) 2014 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
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
