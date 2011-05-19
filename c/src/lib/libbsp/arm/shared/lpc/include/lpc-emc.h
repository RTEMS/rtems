/**
 * @file
 *
 * @ingroup lpc_emc
 *
 * @brief EMC support API.
 */

/*
 * Copyright (c) 2010-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_LPC_EMC_H
#define LIBBSP_ARM_SHARED_LPC_EMC_H

#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc_emc EMC Support
 *
 * @ingroup lpc24xx
 * @ingroup lpc32xx
 *
 * @brief EMC Support
 *
 * @{
 */

/**
 * @name EMC Control Register (EMCControl)
 *
 * @{
 */

#define EMC_CTRL_E BSP_BIT32(0)
#define EMC_CTRL_M BSP_BIT32(0)
#define EMC_CTRL_L BSP_BIT32(2)

/** @} */

/**
 * @name EMC Dynamic Memory Control Register (EMCDynamicControl)
 *
 * @{
 */

#define EMC_DYN_CTRL_CE BSP_BIT32(0)
#define EMC_DYN_CTRL_CS BSP_BIT32(1)
#define EMC_DYN_CTRL_SR BSP_BIT32(2)
#define EMC_DYN_CTRL_SRMCC BSP_BIT32(3)
#define EMC_DYN_CTRL_IMCC BSP_BIT32(4)
#define EMC_DYN_CTRL_MCC BSP_BIT32(5)
#define EMC_DYN_CTRL_I_MASK BSP_MSK32(7, 8)
#define EMC_DYN_CTRL_I_NORMAL BSP_FLD32(0x0, 7, 8)
#define EMC_DYN_CTRL_I_MODE BSP_FLD32(0x1, 7, 8)
#define EMC_DYN_CTRL_I_PALL BSP_FLD32(0x2, 7, 8)
#define EMC_DYN_CTRL_I_NOP BSP_FLD32(0x3, 7, 8)
#define EMC_DYN_CTRL_DP BSP_BIT32(13)

/** @} */

/**
 * @name EMC Dynamic Memory Read Configuration Register (EMCDynamicReadConfig)
 *
 * @{
 */

#define EMC_DYN_READ_CONFIG_SDR_STRAT(val) BSP_FLD32(val, 0, 1)
#define EMC_DYN_READ_CONFIG_SDR_POL_POS BSP_BIT32(4)
#define EMC_DYN_READ_CONFIG_DDR_STRAT(val) BSP_FLD32(val, 8, 9)
#define EMC_DYN_READ_CONFIG_DDR_POL_POS BSP_BIT32(12)

/** @} */

/**
 * @name EMC Dynamic Memory Configuration N Register (EMCDynamicConfigN)
 *
 * @{
 */

#define EMC_DYN_CFG_MD_LPC24XX(val) BSP_FLD32(val, 3, 4)
#define EMC_DYN_CFG_MD_LPC32XX(val) BSP_FLD32(val, 0, 2)
#define EMC_DYN_CFG_AM(val) BSP_FLD32(val, 7, 14)
#define EMC_DYN_CFG_B BSP_BIT32(19)
#define EMC_DYN_CFG_P BSP_BIT32(20)

/** @} */

/**
 * @name EMC Dynamic Memory RAS and CAS Delay N Register (EMCDynamicRasCasN)
 *
 * @{
 */

#define EMC_DYN_RASCAS_RAS(val) BSP_FLD32(val, 0, 3)
#define EMC_DYN_RASCAS_CAS(val, half) BSP_FLD32(((val) << 1) | (half), 7, 10)

/** @} */

#define EMC_DYN_CHIP_COUNT 4

#define EMC_STATIC_CHIP_COUNT 4

typedef struct {
  uint32_t config;
  uint32_t rascas;
  uint32_t reserved_0 [6];
} lpc_emc_dynamic;

typedef struct {
  uint32_t config;
  uint32_t waitwen;
  uint32_t waitoen;
  uint32_t waitrd;
  uint32_t waitpage;
  uint32_t waitwr;
  uint32_t waitturn;
  uint32_t reserved_0 [1];
} lpc_emc_static;

typedef struct {
  uint32_t control;
  uint32_t status;
  uint32_t config;
  uint32_t reserved_0 [5];
  uint32_t dynamiccontrol;
  uint32_t dynamicrefresh;
  uint32_t dynamicreadconfig;
  uint32_t reserved_1;
  uint32_t dynamictrp;
  uint32_t dynamictras;
  uint32_t dynamictsrex;
  uint32_t dynamictapr;
  uint32_t dynamictdal;
  uint32_t dynamictwr;
  uint32_t dynamictrc;
  uint32_t dynamictrfc;
  uint32_t dynamictxsr;
  uint32_t dynamictrrd;
  uint32_t dynamictmrd;
  uint32_t dynamictcdlr;
  uint32_t reserved_3 [8];
  uint32_t staticextendedwait;
  uint32_t reserved_4 [31];
  lpc_emc_dynamic dynamic [EMC_DYN_CHIP_COUNT];
  uint32_t reserved_5 [32];
  lpc_emc_static emcstatic [EMC_STATIC_CHIP_COUNT];
} lpc_emc;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_LPC_EMC_H */
