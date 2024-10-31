/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup zynq_slcr
 * @brief SLCR support.
 */

/*
 *
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
 * @defgroup zynq_slcr SLCR Support
 * @ingroup arm_zynq
 * @brief SLCR Support
 */

#ifndef LIBBSP_ARM_XILINX_ZYNQ_SLCR_H
#define LIBBSP_ARM_XILINX_ZYNQ_SLCR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Set the mask that allows the FPGA resets to be modified.
 *
 * Bit 0 corresponds to FPGA0_OUT_RST, and bit 3 to FPGA3_OUT_RST. Setting a
 * bit in the mask to 1 allows calls to zynq_slcr_fpga_clk_rst to modify that
 * reset. The default mask is 0xf.
 */
void zynq_slcr_fpga_clk_rst_mask_set(
  uint32_t mask
);

/**
 * @brief Control the FPGA reset values.
 *
 * @param val Bits 0 through 3 correspond to FPGA RST 0 through 3. A bit value
 * of 1 asserts the reset.
 */
void zynq_slcr_fpga_clk_rst(
  uint32_t val
);

/**
 * @brief Control the level shifters between the PS and PL.
 *
 * @param val Acceptable values are ZYNQ_SLCR_LVL_SHFTR_EN_DISABLE,
 * ZYNQ_SLCR_LVL_SHFTR_EN_PS_TO_PL, and ZYNQ_SLCR_LVL_SHFTR_EN_ALL.
 */
void zynq_slcr_level_shifter_enable(
  uint32_t val
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_SLCR_H */
