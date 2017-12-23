/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_ATSAM_I2C_H
#define LIBBSP_ARM_ATSAM_I2C_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ATSAM_I2C_0_BUS_PATH "/dev/i2c-0"
#define ATSAM_I2C_1_BUS_PATH "/dev/i2c-1"
#define ATSAM_I2C_2_BUS_PATH "/dev/i2c-2"

int atsam_register_i2c_0(void);

int atsam_register_i2c_1(void);

int atsam_register_i2c_2(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_I2C_H */
