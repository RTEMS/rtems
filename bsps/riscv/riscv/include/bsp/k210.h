/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup k210_regs
 *
 * @brief k210 RISC-V CPU defines.
 */

/*
 * Copyright (c) 2022 Alan Cudmore
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
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

#ifndef LIBBSP_RISCV_RISCV_K210_H
#define LIBBSP_RISCV_RISCV_K210_H

#include <bspopts.h>
#include <stdint.h>
#include <bsp/utility.h>

/**
 * @defgroup K210 RISC V Register Definitions
 *
 * @ingroup RTEMSBSPsriscv
 *
 * @brief Register Definitions
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* For PLL / Clocks */
#define K210_SYSCTL_BASE     0x50440000

#define PLL_CLK_R(n)         (n & 0x00000F)
#define PLL_CLK_F(n)         ((n & 0x0003F0) >> 4)
#define PLL_CLK_OD(n)        ((n & 0x003C00) >> 10)
#define CLKSEL0_ACLK_SEL(n)  (n & 0x00000001)

typedef struct {
  uint32_t git_id;
  uint32_t clk_freq;
  uint32_t pll0;
  uint32_t pll1;
  uint32_t pll2;
  uint32_t resv5;
  uint32_t pll_lock;
  uint32_t rom_error;
  uint32_t clk_sel0;
  uint32_t clk_sel1;
  uint32_t clk_en_cent;
  uint32_t clk_en_peri;
  uint32_t soft_reset;
  uint32_t peri_reset;
  uint32_t clk_th0;
  uint32_t clk_th1;
  uint32_t clk_th2;
  uint32_t clk_th3;
  uint32_t clk_th4;
  uint32_t clk_th5;
  uint32_t clk_th6;
  uint32_t misc;
  uint32_t peri;
  uint32_t spi_sleep;
  uint32_t reset_status;
  uint32_t dma_sel0;
  uint32_t dma_sel1;
  uint32_t power_sel;
  uint32_t resv28;
  uint32_t resv29;
  uint32_t resv30;
  uint32_t resv31;
} k210_sysctl_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LIBBSP_RISCV_RISCV_K210_H */
