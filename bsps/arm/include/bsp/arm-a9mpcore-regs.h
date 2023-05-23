/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup A9MPCoreSupport
 *
 * @brief This header file provides the interfaces of the @ref A9MPCoreSupport.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_SHARED_ARM_A9MPCORE_REGS_H
#define LIBBSP_ARM_SHARED_ARM_A9MPCORE_REGS_H

#include <bsp/utility.h>

/**
 * @defgroup A9MPCoreSupport Cortex-A9 MPCore Support
 *
 * @ingroup RTEMSBSPsARMShared
 *
 * @brief This group contains support interfaces for the Cortex-A9 MPCore.
 *
 * @{
 */

typedef struct {
  uint32_t ctrl;
#define A9MPCORE_SCU_CTRL_SCU_EN BSP_BIT32(0)
#define A9MPCORE_SCU_CTRL_ADDR_FLT_EN BSP_BIT32(1)
#define A9MPCORE_SCU_CTRL_RAM_PAR_EN BSP_BIT32(2)
#define A9MPCORE_SCU_CTRL_SCU_SPEC_LINE_FILL_EN BSP_BIT32(3)
#define A9MPCORE_SCU_CTRL_FORCE_PORT_0_EN BSP_BIT32(4)
#define A9MPCORE_SCU_CTRL_SCU_STANDBY_EN BSP_BIT32(5)
#define A9MPCORE_SCU_CTRL_IC_STANDBY_EN BSP_BIT32(6)
  uint32_t cfg;
#define A9MPCORE_SCU_CFG_CPU_COUNT(val) BSP_FLD32(val, 0, 1)
#define A9MPCORE_SCU_CFG_CPU_COUNT_GET(reg) BSP_FLD32GET(reg, 0, 1)
#define A9MPCORE_SCU_CFG_CPU_COUNT_SET(reg, val) BSP_FLD32SET(reg, val, 0, 1)
#define A9MPCORE_SCU_CFG_SMP_MODE(val) BSP_FLD32(val, 4, 7)
#define A9MPCORE_SCU_CFG_SMP_MODE_GET(reg) BSP_FLD32GET(reg, 4, 7)
#define A9MPCORE_SCU_CFG_SMP_MODE_SET(reg, val) BSP_FLD32SET(reg, val, 4, 7)
#define A9MPCORE_SCU_CFG_TAG_RAM_SIZE(val) BSP_FLD32(val, 8, 15)
#define A9MPCORE_SCU_CFG_TAG_RAM_SIZE_GET(reg) BSP_FLD32GET(reg, 8, 15)
#define A9MPCORE_SCU_CFG_TAG_RAM_SIZE_SET(reg, val) BSP_FLD32SET(reg, val, 8, 15)
  uint32_t pwrst;
  uint32_t invss;
#define A9MPCORE_SCU_INVSS_CPU0(ways) BSP_FLD32(val, 0, 3)
#define A9MPCORE_SCU_INVSS_CPU0_GET(reg) /* Write only register */
#define A9MPCORE_SCU_INVSS_CPU0_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
#define A9MPCORE_SCU_INVSS_CPU1(ways) BSP_FLD32(val, 4, 7)
#define A9MPCORE_SCU_INVSS_CPU1_GET(reg) /* Write only register */
#define A9MPCORE_SCU_INVSS_CPU1_SET(reg, val) BSP_FLD32SET(reg, val, 4, 7)
#define A9MPCORE_SCU_INVSS_CPU2(ways) BSP_FLD32(val, 8, 11)
#define A9MPCORE_SCU_INVSS_CPU2_GET(reg) /* Write only register */
#define A9MPCORE_SCU_INVSS_CPU2_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define A9MPCORE_SCU_INVSS_CPU3(ways) BSP_FLD32(val, 12, 15)
#define A9MPCORE_SCU_INVSS_CPU3_GET(reg) /* Write only register */
#define A9MPCORE_SCU_INVSS_CPU3_SET(reg, val) BSP_FLD32SET(reg, val, 12, 15)
  uint32_t reserved_09[8];
  uint32_t diagn_ctrl;
#define A9MPCORE_SCU_DIAGN_CTRL_MIGRATORY_BIT_DISABLE BSP_BIT32(0)
  uint32_t reserved_10[3];
  uint32_t fltstart;
  uint32_t fltend;
  uint32_t reserved_48[2];
  uint32_t sac;
  uint32_t snsac;
} a9mpcore_scu;

typedef struct {
} a9mpcore_gic;

typedef struct {
  uint32_t cntrlower;
  uint32_t cntrupper;
#define A9MPCORE_GT_CTRL_PRESCALER(val) BSP_FLD32(val, 8, 15)
#define A9MPCORE_GT_CTRL_PRESCALER_GET(reg) BSP_FLD32GET(reg, 8, 15)
#define A9MPCORE_GT_CTRL_PRESCALER_SET(reg, val) BSP_FLD32SET(reg, val, 8, 15)
#define A9MPCORE_GT_CTRL_AUTOINC_EN BSP_BIT32(3)
#define A9MPCORE_GT_CTRL_IRQ_EN BSP_BIT32(2)
#define A9MPCORE_GT_CTRL_COMP_EN BSP_BIT32(1)
#define A9MPCORE_GT_CTRL_TMR_EN BSP_BIT32(0)
  uint32_t ctrl;
#define A9MPCORE_GT_IRQST_EFLG BSP_BIT32(0)
  uint32_t irqst;
  uint32_t cmpvallower;
  uint32_t cmpvalupper;
  uint32_t autoinc;
} a9mpcore_gt;

typedef struct {
  uint32_t load;
  uint32_t cntr;
  uint32_t ctrl;
#define A9MPCORE_PT_CTRL_PRESCALER(val) BSP_FLD32(val, 8, 15)
#define A9MPCORE_PT_CTRL_PRESCALER_GET(reg) BSP_FLD32GET(reg, 8, 15)
#define A9MPCORE_PT_CTRL_PRESCALER_SET(reg, val) BSP_FLD32SET(reg, val, 8, 15)
#define A9MPCORE_PT_CTRL_IRQ_EN BSP_BIT32(2)
#define A9MPCORE_PT_CTRL_AUTO_RLD BSP_BIT32(1)
#define A9MPCORE_PT_CTRL_TMR_EN BSP_BIT32(0)
  uint32_t irqst;
#define A9MPCORE_PT_IRQST_EFLG BSP_BIT32(0)
} a9mpcore_pt;

typedef struct {
  uint32_t load;
  uint32_t cntr;
  uint32_t ctrl;
  uint32_t irqst;
  uint32_t rstst;
  uint32_t dis;
} a9mpcore_pw;

typedef struct {
} a9mpcore_idist;

typedef struct {
  a9mpcore_scu scu;
  uint32_t reserved_58[42];
  a9mpcore_gic gic;
  uint32_t reserved_100[64];
  a9mpcore_gt gt;
  uint32_t reserved_21c[249];
  a9mpcore_pt pt;
  uint32_t reserved_610[4];
  a9mpcore_pw pw;
  uint32_t reserved_638[626];
  a9mpcore_idist idist;
} a9mpcore;

/** @} */

#endif /* LIBBSP_ARM_SHARED_ARM_A9MPCORE_REGS_H */
