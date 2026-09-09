/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME2100
 *
 * @brief Register the mpc8240 I2C bus and its EEPROM devices with libi2c
 */

/*
 * Copyright (C) 2026 UChicago Argonne LLC,
 * as operator of Argonne National Laboratory
 * Author: Vijay Banerjee <vijay@rtems.org>
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

#include <bsp.h>
#include <bsp/mpc8240_i2c_busdrv.h>
#include <libchip/i2c-2b-eeprom.h>
#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/libi2c.h>

int BSP_i2c_initialize(void) {
  int busno;

  /* Note: Application will need
   * #define CONFIGURE_APPLICATION_NEEDS_LIBI2C_DRIVER
   */
  if (!rtems_libi2c_is_initialized) {
    printk("libi2c not initialized; skipping i2c device registration\n");
    return -1;
  }

  busno = rtems_libi2c_register_bus(BSP_I2C_BUS0_NAME, BSP_I2C_BUS_DESCRIPTOR);
  if (busno < 0) {
    printk("Registering mpc8240 i2c bus driver failed\n");
    return -1;
  }

  if (rtems_libi2c_register_drv(BSP_I2C_VPD_EEPROM_NAME,
                                i2c_2b_eeprom_ro_driver_descriptor, busno,
                                BSP_VPD_I2C_ADDR) < 0) {
    printk("Registering i2c VPD eeprom driver failed\n");
    return -1;
  }

  return 0;
}
