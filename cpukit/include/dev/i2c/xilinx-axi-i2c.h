/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016-2017 Chris Johns <chrisj@rtems.org>  All rights reserved.
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

/*
 * Xilinx AXI IIC Interface v2.0. See PG090.pdf.
 *
 * Note, only master support is provided and no dynamic mode by design.
 *
 * The clock set up is to be handled by the IP integrator. There are too many
 * factors handling this in software.
 */


#ifndef XILINX_AXI_I2C_H
#define XILINX_AXI_I2C_H

#include <dev/i2c/i2c.h>

/*
 * The PL integrator controls the timing. This interface allows software to
 * override those settings. It pays to check the timing with ChipScope.
 *
 * If you set the AXI bus frequency you can use the clock speed ioctl call to
 * change the speed dymanically. The ioctl call overrides the defaults passed
 * in.
 *
 * Set the valid mask to the values that are to be set.
 */
#define XILINX_AIX_I2C_AXI_CLOCK (1 << 0)
#define XILINX_AIX_I2C_TSUSTA    (1 << 1)
#define XILINX_AIX_I2C_TSUSTO    (1 << 2)
#define XILINX_AIX_I2C_THDSTA    (1 << 3)
#define XILINX_AIX_I2C_TSUDAT    (1 << 4)
#define XILINX_AIX_I2C_TBUF      (1 << 5)
#define XILINX_AIX_I2C_THIGH     (1 << 6)
#define XILINX_AIX_I2C_TLOW      (1 << 7)
#define XILINX_AIX_I2C_THDDAT    (1 << 8)
#define XILINX_AIX_I2C_ALL_REGS  (XILINX_AIX_I2C_TSUSTA | \
                                  XILINX_AIX_I2C_TSUSTO | \
                                  XILINX_AIX_I2C_THDSTA | \
                                  XILINX_AIX_I2C_TSUDAT | \
                                  XILINX_AIX_I2C_TBUF   | \
                                  XILINX_AIX_I2C_THIGH  | \
                                  XILINX_AIX_I2C_TLOW   | \
                                  XILINX_AIX_I2C_THDDAT)
typedef struct
{
  uint32_t valid_mask;
  uint32_t AXI_CLOCK;
  uint32_t SCL_INERTIAL_DELAY;
  uint32_t TSUSTA;
  uint32_t TSUSTO;
  uint32_t THDSTA;
  uint32_t TSUDAT;
  uint32_t TBUF;
  uint32_t THIGH;
  uint32_t TLOW;
  uint32_t THDDAT;
} xilinx_aix_i2c_timing;

/*
 * Register the driver.
 *
 * The driver can multipex a number of I2C buses (in master mode only) using
 * the GPO port. The PL designer can use the output pins to select a bus. This
 * is useful if connecting a number of slave devices that have limit selectable
 * addresses.
 *
 * @param bus_path The driver's device path.
 * @param register_base AXI base address.
 * @param irq AXI FPGA interrupt.
 * @param gpio_address Bits 12:15 of a slave address it written to the GPO.
 * @param timing Override the default timing. NULL means no changes.
 */
int i2c_bus_register_xilinx_aix_i2c(const char*                  bus_path,
                                    uintptr_t                    register_base,
                                    rtems_vector_number          irq,
                                    bool                         ten_gpio,
                                    const xilinx_aix_i2c_timing* timing);

#endif
