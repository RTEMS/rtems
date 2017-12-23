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

#ifndef LIBBSP_ARM_XILINX_ZYNQ_CADENCE_I2C_H
#define LIBBSP_ARM_XILINX_ZYNQ_CADENCE_I2C_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int i2c_bus_register_cadence(
  const char *bus_path,
  uintptr_t register_base,
  uint32_t input_clock,
  rtems_vector_number irq
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_CADENCE_I2C_H */
