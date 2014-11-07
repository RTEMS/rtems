/**
 * @file
 *
 * @brief GPIO NXP PCA9535 Driver API
 *
 * @ingroup I2CGPIONXPPCA9535
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

#ifndef _DEV_I2C_GPIO_NXP_PCA9539_H
#define _DEV_I2C_GPIO_NXP_PCA9539_H

#include <dev/i2c/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup I2CGPIONXPPCA9535 GPIO NXP PCA9535 Driver
 *
 * @ingroup I2CDevice
 *
 * @brief Driver for NXP PCA9535 16-bit GPIO device.
 *
 * @{
 */

int i2c_dev_register_gpio_nxp_pca9535(
  const char *bus_path,
  const char *dev_path,
  uint16_t address
);

#define GPIO_NXP_PCA9535_GET_INPUT (I2C_DEV_IO_CONTROL + 0)

#define GPIO_NXP_PCA9535_GET_OUTPUT (I2C_DEV_IO_CONTROL + 1)

#define GPIO_NXP_PCA9535_SET_OUTPUT (I2C_DEV_IO_CONTROL + 2)

#define GPIO_NXP_PCA9535_CLEAR_AND_SET_OUTPUT (I2C_DEV_IO_CONTROL + 3)

#define GPIO_NXP_PCA9535_GET_POL_INV (I2C_DEV_IO_CONTROL + 4)

#define GPIO_NXP_PCA9535_SET_POL_INV (I2C_DEV_IO_CONTROL + 5)

#define GPIO_NXP_PCA9535_GET_CONFIG (I2C_DEV_IO_CONTROL + 6)

#define GPIO_NXP_PCA9535_SET_CONFIG (I2C_DEV_IO_CONTROL + 7)

static inline int gpio_nxp_pca9535_get_input(int fd, uint16_t *val)
{
  return ioctl(fd, GPIO_NXP_PCA9535_GET_INPUT, val);
}

static inline int gpio_nxp_pca9535_get_output(int fd, uint16_t *val)
{
  return ioctl(fd, GPIO_NXP_PCA9535_GET_OUTPUT, val);
}

static inline int gpio_nxp_pca9535_set_output(int fd, uint16_t val)
{
  return ioctl(fd, GPIO_NXP_PCA9535_SET_OUTPUT, (void *)(uintptr_t) val);
}

static inline int gpio_nxp_pca9535_clear_and_set_output(
  int fd,
  uint16_t clear,
  uint16_t set
)
{
  uint32_t clear_and_set = ((uint32_t) set << 16) | (uint32_t) clear;

  return ioctl(
    fd,
    GPIO_NXP_PCA9535_CLEAR_AND_SET_OUTPUT,
    (void *)(uintptr_t) clear_and_set
  );
}

static inline int gpio_nxp_pca9535_get_polarity_inversion(
  int fd,
  uint16_t *val
)
{
  return ioctl(fd, GPIO_NXP_PCA9535_GET_POL_INV, val);
}

static inline int gpio_nxp_pca9535_set_polarity_inversion(int fd, uint16_t val)
{
  return ioctl(fd, GPIO_NXP_PCA9535_SET_POL_INV, (void *)(uintptr_t) val);
}

static inline int gpio_nxp_pca9535_get_config(int fd, uint16_t *val)
{
  return ioctl(fd, GPIO_NXP_PCA9535_GET_CONFIG, val);
}

static inline int gpio_nxp_pca9535_set_config(int fd, uint16_t val)
{
  return ioctl(fd, GPIO_NXP_PCA9535_SET_CONFIG, (void *)(uintptr_t) val);
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DEV_I2C_GPIO_NXP_PCA9539_H */
