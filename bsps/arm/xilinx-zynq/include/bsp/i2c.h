/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_XILINX_ZYNQ_I2C_H
#define LIBBSP_ARM_XILINX_ZYNQ_I2C_H

#include <bsp/cadence-i2c.h>
#include <bsp/irq.h>
#include <bsp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static inline int zynq_register_i2c_0(void)
{
  return i2c_bus_register_cadence(
    "/dev/i2c-0",
    0xe0004000,
    zynq_clock_cpu_1x(),
    ZYNQ_IRQ_I2C_0
  );
}

static inline int zynq_register_i2c_1(void)
{
  return i2c_bus_register_cadence(
    "/dev/i2c-1",
    0xe0005000,
    zynq_clock_cpu_1x(),
    ZYNQ_IRQ_I2C_1
  );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_I2C_H */
