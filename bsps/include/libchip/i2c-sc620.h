/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#ifndef I2C_SC620_H
#define I2C_SC620_H

#include <rtems/libi2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief I2C driver for SEMTECH SC620 octal LED driver.
 *
 * A write() must use two character buffer.  The buffer[0] value specifies the
 * register and the buffer[1] value specifies the register data.
 *
 * A read() must use a one character buffer.  The buffer[0] value specifies the
 * register on function entry.  The buffer[0] value contains the register value
 * after a successful operation.
 */
extern rtems_libi2c_drv_t i2c_sc620_driver;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* I2C_SC620_H */
