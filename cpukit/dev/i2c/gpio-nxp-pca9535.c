/**
 * @file
 *
 * @brief GPIO NXP PCA9535 Driver Implementation
 *
 * @ingroup I2CGPIONXPPCA9535
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

#include <dev/i2c/gpio-nxp-pca9535.h>

typedef enum {
  GPIO_NXP_PCA9535_INPUT_PORT_0,
  GPIO_NXP_PCA9535_INPUT_PORT_1,
  GPIO_NXP_PCA9535_OUTPUT_PORT_0,
  GPIO_NXP_PCA9535_OUTPUT_PORT_1,
  GPIO_NXP_PCA9535_POL_INV_PORT_0,
  GPIO_NXP_PCA9535_POL_INV_PORT_1,
  GPIO_NXP_PCA9535_CONF_PORT_0,
  GPIO_NXP_PCA9535_CONF_PORT_1
} gpio_nxp_pca9535_port;

static int gpio_nxp_pca9535_get_reg(
  i2c_dev *dev,
  gpio_nxp_pca9535_port port,
  uint16_t *val
)
{
  uint8_t out[1] = { port };
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
  *val = in[0] | (in[1] << 8);

  return err;
}

static int gpio_nxp_pca9535_set_reg(
  i2c_dev *dev,
  gpio_nxp_pca9535_port port,
  uint16_t val
)
{
  uint8_t out[3] = { port, (uint8_t) val, (uint8_t) (val >> 8) };
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

static int gpio_nxp_pca9535_ioctl(
  i2c_dev *dev,
  ioctl_command_t command,
  void *arg
)
{
  uint16_t v16 = (uint16_t)(uintptr_t) arg;
  uint32_t v32 = (uint32_t)(uintptr_t) arg;
  int err;

  switch (command) {
    case GPIO_NXP_PCA9535_GET_INPUT:
      err = gpio_nxp_pca9535_get_reg(dev, GPIO_NXP_PCA9535_INPUT_PORT_0, arg);
      break;
    case GPIO_NXP_PCA9535_GET_OUTPUT:
      err = gpio_nxp_pca9535_get_reg(dev, GPIO_NXP_PCA9535_OUTPUT_PORT_0, arg);
      break;
    case GPIO_NXP_PCA9535_CLEAR_AND_SET_OUTPUT:
      i2c_bus_obtain(dev->bus);
      err = gpio_nxp_pca9535_get_reg(dev, GPIO_NXP_PCA9535_OUTPUT_PORT_0, &v16);
      if (err == 0) {
        v16 &= ~((uint16_t) v32);
        v16 |= (uint16_t) (v32 >> 16);
        err = gpio_nxp_pca9535_set_reg(dev, GPIO_NXP_PCA9535_OUTPUT_PORT_0, v16);
      }
      i2c_bus_release(dev->bus);
      break;
    case GPIO_NXP_PCA9535_SET_OUTPUT:
      err = gpio_nxp_pca9535_set_reg(dev, GPIO_NXP_PCA9535_OUTPUT_PORT_0, v16);
      break;
    case GPIO_NXP_PCA9535_GET_POL_INV:
      err = gpio_nxp_pca9535_get_reg(dev, GPIO_NXP_PCA9535_POL_INV_PORT_0, arg);
      break;
    case GPIO_NXP_PCA9535_SET_POL_INV:
      err = gpio_nxp_pca9535_set_reg(dev, GPIO_NXP_PCA9535_POL_INV_PORT_0, v16);
      break;
    case GPIO_NXP_PCA9535_GET_CONFIG:
      err = gpio_nxp_pca9535_get_reg(dev, GPIO_NXP_PCA9535_CONF_PORT_0, arg);
      break;
    case GPIO_NXP_PCA9535_SET_CONFIG:
      err = gpio_nxp_pca9535_set_reg(dev, GPIO_NXP_PCA9535_CONF_PORT_0, v16);
      break;
    default:
      err = -ENOTTY;
      break;
  }

  return err;
}

int i2c_dev_register_gpio_nxp_pca9535(
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

  dev->ioctl = gpio_nxp_pca9535_ioctl;

  return i2c_dev_register(dev, dev_path);
}
