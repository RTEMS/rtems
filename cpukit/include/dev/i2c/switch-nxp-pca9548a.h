/**
 * @file
 *
 * @brief Switch NXP PCA9548A Driver API
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

#ifndef _DEV_I2C_SWITCH_NXP_PCA9548A_H
#define _DEV_I2C_SWITCH_NXP_PCA9548A_H

#include <dev/i2c/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup I2CSWITCHNXPPCA9548A Switch NXP PCA9535 Driver
 *
 * @ingroup I2CDevice
 *
 * @brief Driver for NXP PCA9548A 8-channel switch device.
 *
 * @{
 */

int i2c_dev_register_switch_nxp_pca9548a(
  const char *bus_path,
  const char *dev_path,
  uint16_t address
);

#define SWITCH_NXP_PCA9548A_GET_CONTROL (I2C_DEV_IO_CONTROL + 0)

#define SWITCH_NXP_PCA9548A_SET_CONTROL (I2C_DEV_IO_CONTROL + 1)

static inline int switch_nxp_pca9548a_get_control(int fd, uint8_t *val)
{
  return ioctl(fd, SWITCH_NXP_PCA9548A_GET_CONTROL, val);
}

static inline int switch_nxp_pca9548a_set_control(int fd, uint8_t val)
{
  return ioctl(fd, SWITCH_NXP_PCA9548A_SET_CONTROL, (void *)(uintptr_t) val);
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DEV_I2C_SWITCH_NXP_PCA9548A_H */
