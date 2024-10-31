/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup zynq_slcr
 * @brief SLCR register definitions.
 */

/*
 * Copyright (c) 2017
 *  NSF Center for High-Performance Reconfigurable Computing (CHREC),
 *  University of Pittsburgh.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of CHREC.
 *
 * Author: Patrick Gauvin <gauvin@hcs.ufl.edu>
 */

/**
 * @defgroup zynq_slcr_regs SLCR Register Definitions
 * @ingroup zynq_slcr
 * @brief SLCR Register Definitions
 */

#ifndef LIBBSP_ARM_XILINX_ZYNQ_SLCR_REGS_H
#define LIBBSP_ARM_XILINX_ZYNQ_SLCR_REGS_H

#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ZYNQ_SLCR_BASE_ADDR ( 0xF8000000 )

#define ZYNQ_SLCR_LOCK_OFF ( 0x4 )
#define ZYNQ_SLCR_UNLOCK_OFF ( 0x8 )
#define ZYNQ_SLCR_FPGA_RST_CTRL_OFF ( 0x240 )
#define ZYNQ_SLCR_PSS_IDCODE_OFF ( 0x530 )
#define ZYNQ_SLCR_LVL_SHFTR_EN_OFF ( 0x900 )

#define ZYNQ_SLCR_LOCK_KEY ( 0x767b )
#define ZYNQ_SLCR_UNLOCK_KEY ( 0xdf0d )

/** \brief Get FPGA0_OUT_RST (bit 0) through FPGA3_OUT_RST fields (bit 3). */
#define ZYNQ_SLCR_FPGA_RST_CTRL_FPGA_OUT_RST_GET( reg ) \
  BSP_FLD32GET( reg, 0, 3 )
#define ZYNQ_SLCR_FPGA_RST_CTRL_FPGA_OUT_RST( val ) BSP_FLD32( val, 0, 3 )

/* NOTE: QEMU gives a value of 0 for the pss_idcode. */
#define ZYNQ_SLCR_PSS_IDCODE_DEVICE_GET( reg ) BSP_FLD32GET( reg, 12, 16 )
#define ZYNQ_SLCR_PSS_IDCODE_DEVICE_7z007s ( 0x03 )
#define ZYNQ_SLCR_PSS_IDCODE_DEVICE_7z012s ( 0x1c )
#define ZYNQ_SLCR_PSS_IDCODE_DEVICE_7z014s ( 0x08 )
#define ZYNQ_SLCR_PSS_IDCODE_DEVICE_7z010 ( 0x02 )
#define ZYNQ_SLCR_PSS_IDCODE_DEVICE_7z015 ( 0x1b )
#define ZYNQ_SLCR_PSS_IDCODE_DEVICE_7z020 ( 0x07 )
#define ZYNQ_SLCR_PSS_IDCODE_DEVICE_7z030 ( 0x0c )
#define ZYNQ_SLCR_PSS_IDCODE_DEVICE_7z035 ( 0x12 )
#define ZYNQ_SLCR_PSS_IDCODE_DEVICE_7z045 ( 0x11 )
#define ZYNQ_SLCR_PSS_IDCODE_DEVICE_7z100 ( 0x16 )

#define ZYNQ_SLCR_LVL_SHFTR_EN_DISABLE ( 0 )
#define ZYNQ_SLCR_LVL_SHFTR_EN_PS_TO_PL ( 0xA )
#define ZYNQ_SLCR_LVL_SHFTR_EN_ALL ( 0xF )

static inline void zynq_slcr_write32(
  const uint32_t reg_off,
  const uint32_t val
)
{
  volatile uint32_t *slcr_reg;
  slcr_reg = (volatile uint32_t *)( ZYNQ_SLCR_BASE_ADDR + reg_off );
  *slcr_reg = val;
}

static inline uint32_t zynq_slcr_read32(
  const uint32_t reg_off
)
{
  volatile uint32_t *slcr_reg;
  slcr_reg = (volatile uint32_t *)( ZYNQ_SLCR_BASE_ADDR + reg_off);
  return *slcr_reg;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_SLCR_REGS_H */
