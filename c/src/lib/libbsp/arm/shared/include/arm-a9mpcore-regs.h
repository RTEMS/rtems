/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_A9MPCORE_REGS_H
#define LIBBSP_ARM_SHARED_ARM_A9MPCORE_REGS_H

#include <bsp/utility.h>

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
  uint32_t reserved_10[12];
  uint32_t fltstart;
  uint32_t fltend;
  uint32_t reserved_48[2];
  uint32_t sac;
  uint32_t snsac;
} a9mpcore_scu;

typedef struct {
} a9mpcore_gic;

typedef struct {
  uint32_t cntr;
  uint32_t reserved_04;
  uint32_t ctrl;
  uint32_t irqst;
  uint32_t cmpval;
  uint32_t reserved_14;
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

#endif /* LIBBSP_ARM_SHARED_ARM_A9MPCORE_REGS_H */
