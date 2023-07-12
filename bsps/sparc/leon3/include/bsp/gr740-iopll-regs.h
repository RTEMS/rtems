/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsGR740IOPLL
 *
 * @brief This header file defines the GR740 I/O and PLL configuration register
 *   block interface.
 */

/*
 * Copyright (C) 2021, 2023 embedded brains GmbH & Co. KG
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
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/bsp/sparc/leon3/if/gr740-iopll-header */

#ifndef _BSP_GR740_IOPLL_REGS_H
#define _BSP_GR740_IOPLL_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/bsp/sparc/leon3/if/gr740-iopll */

/**
 * @defgroup RTEMSBSPsGR740IOPLL GR740 I/0 and PLL Configuration
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This group contains the GR740 I/0 and PLL Configuration interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSBSPsGR740IOPLLFTMFUNC \
 *   FTMCTRL function enable register (FTMFUNC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_IOPLL_FTMFUNC_FTMEN_SHIFT 0
#define GR740_IOPLL_FTMFUNC_FTMEN_MASK 0x3fffffU
#define GR740_IOPLL_FTMFUNC_FTMEN_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_FTMFUNC_FTMEN_MASK ) >> \
    GR740_IOPLL_FTMFUNC_FTMEN_SHIFT )
#define GR740_IOPLL_FTMFUNC_FTMEN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_FTMFUNC_FTMEN_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_FTMFUNC_FTMEN_SHIFT ) & \
      GR740_IOPLL_FTMFUNC_FTMEN_MASK ) )
#define GR740_IOPLL_FTMFUNC_FTMEN( _val ) \
  ( ( ( _val ) << GR740_IOPLL_FTMFUNC_FTMEN_SHIFT ) & \
    GR740_IOPLL_FTMFUNC_FTMEN_MASK )

/** @} */

/**
 * @defgroup RTEMSBSPsGR740IOPLLALTFUNC \
 *   Alternative function enable register (ALTFUNC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_IOPLL_ALTFUNC_ALTEN_SHIFT 0
#define GR740_IOPLL_ALTFUNC_ALTEN_MASK 0x3fffffU
#define GR740_IOPLL_ALTFUNC_ALTEN_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_ALTFUNC_ALTEN_MASK ) >> \
    GR740_IOPLL_ALTFUNC_ALTEN_SHIFT )
#define GR740_IOPLL_ALTFUNC_ALTEN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_ALTFUNC_ALTEN_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_ALTFUNC_ALTEN_SHIFT ) & \
      GR740_IOPLL_ALTFUNC_ALTEN_MASK ) )
#define GR740_IOPLL_ALTFUNC_ALTEN( _val ) \
  ( ( ( _val ) << GR740_IOPLL_ALTFUNC_ALTEN_SHIFT ) & \
    GR740_IOPLL_ALTFUNC_ALTEN_MASK )

/** @} */

/**
 * @defgroup RTEMSBSPsGR740IOPLLLVDSMCLK \
 *   LVDS and memory clock pad enable register (LVDSMCLK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_IOPLL_LVDSMCLK_SMEM 0x20000U

#define GR740_IOPLL_LVDSMCLK_DMEM 0x10000U

#define GR740_IOPLL_LVDSMCLK_SPWOE_SHIFT 0
#define GR740_IOPLL_LVDSMCLK_SPWOE_MASK 0xffU
#define GR740_IOPLL_LVDSMCLK_SPWOE_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_LVDSMCLK_SPWOE_MASK ) >> \
    GR740_IOPLL_LVDSMCLK_SPWOE_SHIFT )
#define GR740_IOPLL_LVDSMCLK_SPWOE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_LVDSMCLK_SPWOE_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_LVDSMCLK_SPWOE_SHIFT ) & \
      GR740_IOPLL_LVDSMCLK_SPWOE_MASK ) )
#define GR740_IOPLL_LVDSMCLK_SPWOE( _val ) \
  ( ( ( _val ) << GR740_IOPLL_LVDSMCLK_SPWOE_SHIFT ) & \
    GR740_IOPLL_LVDSMCLK_SPWOE_MASK )

/** @} */

/**
 * @defgroup RTEMSBSPsGR740IOPLLPLLNEWCFG \
 *   PLL new configuration register (PLLNEWCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_IOPLL_PLLNEWCFG_SWTAG_SHIFT 27
#define GR740_IOPLL_PLLNEWCFG_SWTAG_MASK 0x18000000U
#define GR740_IOPLL_PLLNEWCFG_SWTAG_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_PLLNEWCFG_SWTAG_MASK ) >> \
    GR740_IOPLL_PLLNEWCFG_SWTAG_SHIFT )
#define GR740_IOPLL_PLLNEWCFG_SWTAG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_PLLNEWCFG_SWTAG_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_PLLNEWCFG_SWTAG_SHIFT ) & \
      GR740_IOPLL_PLLNEWCFG_SWTAG_MASK ) )
#define GR740_IOPLL_PLLNEWCFG_SWTAG( _val ) \
  ( ( ( _val ) << GR740_IOPLL_PLLNEWCFG_SWTAG_SHIFT ) & \
    GR740_IOPLL_PLLNEWCFG_SWTAG_MASK )

#define GR740_IOPLL_PLLNEWCFG_SPWPLLCFG_SHIFT 18
#define GR740_IOPLL_PLLNEWCFG_SPWPLLCFG_MASK 0x7fc0000U
#define GR740_IOPLL_PLLNEWCFG_SPWPLLCFG_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_PLLNEWCFG_SPWPLLCFG_MASK ) >> \
    GR740_IOPLL_PLLNEWCFG_SPWPLLCFG_SHIFT )
#define GR740_IOPLL_PLLNEWCFG_SPWPLLCFG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_PLLNEWCFG_SPWPLLCFG_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_PLLNEWCFG_SPWPLLCFG_SHIFT ) & \
      GR740_IOPLL_PLLNEWCFG_SPWPLLCFG_MASK ) )
#define GR740_IOPLL_PLLNEWCFG_SPWPLLCFG( _val ) \
  ( ( ( _val ) << GR740_IOPLL_PLLNEWCFG_SPWPLLCFG_SHIFT ) & \
    GR740_IOPLL_PLLNEWCFG_SPWPLLCFG_MASK )

#define GR740_IOPLL_PLLNEWCFG_MEMPLLCFG_SHIFT 9
#define GR740_IOPLL_PLLNEWCFG_MEMPLLCFG_MASK 0x3fe00U
#define GR740_IOPLL_PLLNEWCFG_MEMPLLCFG_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_PLLNEWCFG_MEMPLLCFG_MASK ) >> \
    GR740_IOPLL_PLLNEWCFG_MEMPLLCFG_SHIFT )
#define GR740_IOPLL_PLLNEWCFG_MEMPLLCFG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_PLLNEWCFG_MEMPLLCFG_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_PLLNEWCFG_MEMPLLCFG_SHIFT ) & \
      GR740_IOPLL_PLLNEWCFG_MEMPLLCFG_MASK ) )
#define GR740_IOPLL_PLLNEWCFG_MEMPLLCFG( _val ) \
  ( ( ( _val ) << GR740_IOPLL_PLLNEWCFG_MEMPLLCFG_SHIFT ) & \
    GR740_IOPLL_PLLNEWCFG_MEMPLLCFG_MASK )

#define GR740_IOPLL_PLLNEWCFG_SYSPLLCFG_SHIFT 0
#define GR740_IOPLL_PLLNEWCFG_SYSPLLCFG_MASK 0x1ffU
#define GR740_IOPLL_PLLNEWCFG_SYSPLLCFG_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_PLLNEWCFG_SYSPLLCFG_MASK ) >> \
    GR740_IOPLL_PLLNEWCFG_SYSPLLCFG_SHIFT )
#define GR740_IOPLL_PLLNEWCFG_SYSPLLCFG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_PLLNEWCFG_SYSPLLCFG_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_PLLNEWCFG_SYSPLLCFG_SHIFT ) & \
      GR740_IOPLL_PLLNEWCFG_SYSPLLCFG_MASK ) )
#define GR740_IOPLL_PLLNEWCFG_SYSPLLCFG( _val ) \
  ( ( ( _val ) << GR740_IOPLL_PLLNEWCFG_SYSPLLCFG_SHIFT ) & \
    GR740_IOPLL_PLLNEWCFG_SYSPLLCFG_MASK )

/** @} */

/**
 * @defgroup RTEMSBSPsGR740IOPLLPLLRECFG \
 *   PLL reconfigure command register (PLLRECFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_IOPLL_PLLRECFG_RECONF_SHIFT 0
#define GR740_IOPLL_PLLRECFG_RECONF_MASK 0x7U
#define GR740_IOPLL_PLLRECFG_RECONF_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_PLLRECFG_RECONF_MASK ) >> \
    GR740_IOPLL_PLLRECFG_RECONF_SHIFT )
#define GR740_IOPLL_PLLRECFG_RECONF_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_PLLRECFG_RECONF_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_PLLRECFG_RECONF_SHIFT ) & \
      GR740_IOPLL_PLLRECFG_RECONF_MASK ) )
#define GR740_IOPLL_PLLRECFG_RECONF( _val ) \
  ( ( ( _val ) << GR740_IOPLL_PLLRECFG_RECONF_SHIFT ) & \
    GR740_IOPLL_PLLRECFG_RECONF_MASK )

/** @} */

/**
 * @defgroup RTEMSBSPsGR740IOPLLPLLCURCFG \
 *   PLL current configuration register (PLLCURCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_IOPLL_PLLCURCFG_SWTAG_SHIFT 27
#define GR740_IOPLL_PLLCURCFG_SWTAG_MASK 0x18000000U
#define GR740_IOPLL_PLLCURCFG_SWTAG_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_PLLCURCFG_SWTAG_MASK ) >> \
    GR740_IOPLL_PLLCURCFG_SWTAG_SHIFT )
#define GR740_IOPLL_PLLCURCFG_SWTAG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_PLLCURCFG_SWTAG_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_PLLCURCFG_SWTAG_SHIFT ) & \
      GR740_IOPLL_PLLCURCFG_SWTAG_MASK ) )
#define GR740_IOPLL_PLLCURCFG_SWTAG( _val ) \
  ( ( ( _val ) << GR740_IOPLL_PLLCURCFG_SWTAG_SHIFT ) & \
    GR740_IOPLL_PLLCURCFG_SWTAG_MASK )

#define GR740_IOPLL_PLLCURCFG_SPWPLLCFG_SHIFT 18
#define GR740_IOPLL_PLLCURCFG_SPWPLLCFG_MASK 0x7fc0000U
#define GR740_IOPLL_PLLCURCFG_SPWPLLCFG_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_PLLCURCFG_SPWPLLCFG_MASK ) >> \
    GR740_IOPLL_PLLCURCFG_SPWPLLCFG_SHIFT )
#define GR740_IOPLL_PLLCURCFG_SPWPLLCFG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_PLLCURCFG_SPWPLLCFG_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_PLLCURCFG_SPWPLLCFG_SHIFT ) & \
      GR740_IOPLL_PLLCURCFG_SPWPLLCFG_MASK ) )
#define GR740_IOPLL_PLLCURCFG_SPWPLLCFG( _val ) \
  ( ( ( _val ) << GR740_IOPLL_PLLCURCFG_SPWPLLCFG_SHIFT ) & \
    GR740_IOPLL_PLLCURCFG_SPWPLLCFG_MASK )

#define GR740_IOPLL_PLLCURCFG_MEMPLLCFG_SHIFT 9
#define GR740_IOPLL_PLLCURCFG_MEMPLLCFG_MASK 0x3fe00U
#define GR740_IOPLL_PLLCURCFG_MEMPLLCFG_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_PLLCURCFG_MEMPLLCFG_MASK ) >> \
    GR740_IOPLL_PLLCURCFG_MEMPLLCFG_SHIFT )
#define GR740_IOPLL_PLLCURCFG_MEMPLLCFG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_PLLCURCFG_MEMPLLCFG_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_PLLCURCFG_MEMPLLCFG_SHIFT ) & \
      GR740_IOPLL_PLLCURCFG_MEMPLLCFG_MASK ) )
#define GR740_IOPLL_PLLCURCFG_MEMPLLCFG( _val ) \
  ( ( ( _val ) << GR740_IOPLL_PLLCURCFG_MEMPLLCFG_SHIFT ) & \
    GR740_IOPLL_PLLCURCFG_MEMPLLCFG_MASK )

#define GR740_IOPLL_PLLCURCFG_SYSPLLCFG_SHIFT 0
#define GR740_IOPLL_PLLCURCFG_SYSPLLCFG_MASK 0x1ffU
#define GR740_IOPLL_PLLCURCFG_SYSPLLCFG_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_PLLCURCFG_SYSPLLCFG_MASK ) >> \
    GR740_IOPLL_PLLCURCFG_SYSPLLCFG_SHIFT )
#define GR740_IOPLL_PLLCURCFG_SYSPLLCFG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_PLLCURCFG_SYSPLLCFG_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_PLLCURCFG_SYSPLLCFG_SHIFT ) & \
      GR740_IOPLL_PLLCURCFG_SYSPLLCFG_MASK ) )
#define GR740_IOPLL_PLLCURCFG_SYSPLLCFG( _val ) \
  ( ( ( _val ) << GR740_IOPLL_PLLCURCFG_SYSPLLCFG_SHIFT ) & \
    GR740_IOPLL_PLLCURCFG_SYSPLLCFG_MASK )

/** @} */

/**
 * @defgroup RTEMSBSPsGR740IOPLLDRVSTR1 \
 *   Drive strength configuration register 1 (DRVSTR1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_IOPLL_DRVSTR1_S9_SHIFT 18
#define GR740_IOPLL_DRVSTR1_S9_MASK 0xc0000U
#define GR740_IOPLL_DRVSTR1_S9_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR1_S9_MASK ) >> \
    GR740_IOPLL_DRVSTR1_S9_SHIFT )
#define GR740_IOPLL_DRVSTR1_S9_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR1_S9_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S9_SHIFT ) & \
      GR740_IOPLL_DRVSTR1_S9_MASK ) )
#define GR740_IOPLL_DRVSTR1_S9( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S9_SHIFT ) & \
    GR740_IOPLL_DRVSTR1_S9_MASK )

#define GR740_IOPLL_DRVSTR1_S8_SHIFT 16
#define GR740_IOPLL_DRVSTR1_S8_MASK 0x30000U
#define GR740_IOPLL_DRVSTR1_S8_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR1_S8_MASK ) >> \
    GR740_IOPLL_DRVSTR1_S8_SHIFT )
#define GR740_IOPLL_DRVSTR1_S8_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR1_S8_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S8_SHIFT ) & \
      GR740_IOPLL_DRVSTR1_S8_MASK ) )
#define GR740_IOPLL_DRVSTR1_S8( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S8_SHIFT ) & \
    GR740_IOPLL_DRVSTR1_S8_MASK )

#define GR740_IOPLL_DRVSTR1_S7_SHIFT 14
#define GR740_IOPLL_DRVSTR1_S7_MASK 0xc000U
#define GR740_IOPLL_DRVSTR1_S7_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR1_S7_MASK ) >> \
    GR740_IOPLL_DRVSTR1_S7_SHIFT )
#define GR740_IOPLL_DRVSTR1_S7_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR1_S7_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S7_SHIFT ) & \
      GR740_IOPLL_DRVSTR1_S7_MASK ) )
#define GR740_IOPLL_DRVSTR1_S7( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S7_SHIFT ) & \
    GR740_IOPLL_DRVSTR1_S7_MASK )

#define GR740_IOPLL_DRVSTR1_S6_SHIFT 12
#define GR740_IOPLL_DRVSTR1_S6_MASK 0x3000U
#define GR740_IOPLL_DRVSTR1_S6_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR1_S6_MASK ) >> \
    GR740_IOPLL_DRVSTR1_S6_SHIFT )
#define GR740_IOPLL_DRVSTR1_S6_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR1_S6_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S6_SHIFT ) & \
      GR740_IOPLL_DRVSTR1_S6_MASK ) )
#define GR740_IOPLL_DRVSTR1_S6( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S6_SHIFT ) & \
    GR740_IOPLL_DRVSTR1_S6_MASK )

#define GR740_IOPLL_DRVSTR1_S5_SHIFT 10
#define GR740_IOPLL_DRVSTR1_S5_MASK 0xc00U
#define GR740_IOPLL_DRVSTR1_S5_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR1_S5_MASK ) >> \
    GR740_IOPLL_DRVSTR1_S5_SHIFT )
#define GR740_IOPLL_DRVSTR1_S5_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR1_S5_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S5_SHIFT ) & \
      GR740_IOPLL_DRVSTR1_S5_MASK ) )
#define GR740_IOPLL_DRVSTR1_S5( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S5_SHIFT ) & \
    GR740_IOPLL_DRVSTR1_S5_MASK )

#define GR740_IOPLL_DRVSTR1_S4_SHIFT 8
#define GR740_IOPLL_DRVSTR1_S4_MASK 0x300U
#define GR740_IOPLL_DRVSTR1_S4_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR1_S4_MASK ) >> \
    GR740_IOPLL_DRVSTR1_S4_SHIFT )
#define GR740_IOPLL_DRVSTR1_S4_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR1_S4_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S4_SHIFT ) & \
      GR740_IOPLL_DRVSTR1_S4_MASK ) )
#define GR740_IOPLL_DRVSTR1_S4( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S4_SHIFT ) & \
    GR740_IOPLL_DRVSTR1_S4_MASK )

#define GR740_IOPLL_DRVSTR1_S3_SHIFT 6
#define GR740_IOPLL_DRVSTR1_S3_MASK 0xc0U
#define GR740_IOPLL_DRVSTR1_S3_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR1_S3_MASK ) >> \
    GR740_IOPLL_DRVSTR1_S3_SHIFT )
#define GR740_IOPLL_DRVSTR1_S3_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR1_S3_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S3_SHIFT ) & \
      GR740_IOPLL_DRVSTR1_S3_MASK ) )
#define GR740_IOPLL_DRVSTR1_S3( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S3_SHIFT ) & \
    GR740_IOPLL_DRVSTR1_S3_MASK )

#define GR740_IOPLL_DRVSTR1_S2_SHIFT 4
#define GR740_IOPLL_DRVSTR1_S2_MASK 0x30U
#define GR740_IOPLL_DRVSTR1_S2_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR1_S2_MASK ) >> \
    GR740_IOPLL_DRVSTR1_S2_SHIFT )
#define GR740_IOPLL_DRVSTR1_S2_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR1_S2_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S2_SHIFT ) & \
      GR740_IOPLL_DRVSTR1_S2_MASK ) )
#define GR740_IOPLL_DRVSTR1_S2( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S2_SHIFT ) & \
    GR740_IOPLL_DRVSTR1_S2_MASK )

#define GR740_IOPLL_DRVSTR1_S1_SHIFT 2
#define GR740_IOPLL_DRVSTR1_S1_MASK 0xcU
#define GR740_IOPLL_DRVSTR1_S1_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR1_S1_MASK ) >> \
    GR740_IOPLL_DRVSTR1_S1_SHIFT )
#define GR740_IOPLL_DRVSTR1_S1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR1_S1_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S1_SHIFT ) & \
      GR740_IOPLL_DRVSTR1_S1_MASK ) )
#define GR740_IOPLL_DRVSTR1_S1( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S1_SHIFT ) & \
    GR740_IOPLL_DRVSTR1_S1_MASK )

#define GR740_IOPLL_DRVSTR1_S0_SHIFT 0
#define GR740_IOPLL_DRVSTR1_S0_MASK 0x3U
#define GR740_IOPLL_DRVSTR1_S0_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR1_S0_MASK ) >> \
    GR740_IOPLL_DRVSTR1_S0_SHIFT )
#define GR740_IOPLL_DRVSTR1_S0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR1_S0_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S0_SHIFT ) & \
      GR740_IOPLL_DRVSTR1_S0_MASK ) )
#define GR740_IOPLL_DRVSTR1_S0( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR1_S0_SHIFT ) & \
    GR740_IOPLL_DRVSTR1_S0_MASK )

/** @} */

/**
 * @defgroup RTEMSBSPsGR740IOPLLDRVSTR2 \
 *   Drive strength configuration register 2 (DRVSTR2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_IOPLL_DRVSTR2_S19_SHIFT 18
#define GR740_IOPLL_DRVSTR2_S19_MASK 0xc0000U
#define GR740_IOPLL_DRVSTR2_S19_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR2_S19_MASK ) >> \
    GR740_IOPLL_DRVSTR2_S19_SHIFT )
#define GR740_IOPLL_DRVSTR2_S19_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR2_S19_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S19_SHIFT ) & \
      GR740_IOPLL_DRVSTR2_S19_MASK ) )
#define GR740_IOPLL_DRVSTR2_S19( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S19_SHIFT ) & \
    GR740_IOPLL_DRVSTR2_S19_MASK )

#define GR740_IOPLL_DRVSTR2_S18_SHIFT 16
#define GR740_IOPLL_DRVSTR2_S18_MASK 0x30000U
#define GR740_IOPLL_DRVSTR2_S18_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR2_S18_MASK ) >> \
    GR740_IOPLL_DRVSTR2_S18_SHIFT )
#define GR740_IOPLL_DRVSTR2_S18_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR2_S18_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S18_SHIFT ) & \
      GR740_IOPLL_DRVSTR2_S18_MASK ) )
#define GR740_IOPLL_DRVSTR2_S18( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S18_SHIFT ) & \
    GR740_IOPLL_DRVSTR2_S18_MASK )

#define GR740_IOPLL_DRVSTR2_S17_SHIFT 14
#define GR740_IOPLL_DRVSTR2_S17_MASK 0xc000U
#define GR740_IOPLL_DRVSTR2_S17_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR2_S17_MASK ) >> \
    GR740_IOPLL_DRVSTR2_S17_SHIFT )
#define GR740_IOPLL_DRVSTR2_S17_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR2_S17_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S17_SHIFT ) & \
      GR740_IOPLL_DRVSTR2_S17_MASK ) )
#define GR740_IOPLL_DRVSTR2_S17( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S17_SHIFT ) & \
    GR740_IOPLL_DRVSTR2_S17_MASK )

#define GR740_IOPLL_DRVSTR2_S16_SHIFT 12
#define GR740_IOPLL_DRVSTR2_S16_MASK 0x3000U
#define GR740_IOPLL_DRVSTR2_S16_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR2_S16_MASK ) >> \
    GR740_IOPLL_DRVSTR2_S16_SHIFT )
#define GR740_IOPLL_DRVSTR2_S16_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR2_S16_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S16_SHIFT ) & \
      GR740_IOPLL_DRVSTR2_S16_MASK ) )
#define GR740_IOPLL_DRVSTR2_S16( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S16_SHIFT ) & \
    GR740_IOPLL_DRVSTR2_S16_MASK )

#define GR740_IOPLL_DRVSTR2_S15_SHIFT 10
#define GR740_IOPLL_DRVSTR2_S15_MASK 0xc00U
#define GR740_IOPLL_DRVSTR2_S15_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR2_S15_MASK ) >> \
    GR740_IOPLL_DRVSTR2_S15_SHIFT )
#define GR740_IOPLL_DRVSTR2_S15_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR2_S15_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S15_SHIFT ) & \
      GR740_IOPLL_DRVSTR2_S15_MASK ) )
#define GR740_IOPLL_DRVSTR2_S15( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S15_SHIFT ) & \
    GR740_IOPLL_DRVSTR2_S15_MASK )

#define GR740_IOPLL_DRVSTR2_S14_SHIFT 8
#define GR740_IOPLL_DRVSTR2_S14_MASK 0x300U
#define GR740_IOPLL_DRVSTR2_S14_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR2_S14_MASK ) >> \
    GR740_IOPLL_DRVSTR2_S14_SHIFT )
#define GR740_IOPLL_DRVSTR2_S14_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR2_S14_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S14_SHIFT ) & \
      GR740_IOPLL_DRVSTR2_S14_MASK ) )
#define GR740_IOPLL_DRVSTR2_S14( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S14_SHIFT ) & \
    GR740_IOPLL_DRVSTR2_S14_MASK )

#define GR740_IOPLL_DRVSTR2_S13_SHIFT 6
#define GR740_IOPLL_DRVSTR2_S13_MASK 0xc0U
#define GR740_IOPLL_DRVSTR2_S13_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR2_S13_MASK ) >> \
    GR740_IOPLL_DRVSTR2_S13_SHIFT )
#define GR740_IOPLL_DRVSTR2_S13_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR2_S13_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S13_SHIFT ) & \
      GR740_IOPLL_DRVSTR2_S13_MASK ) )
#define GR740_IOPLL_DRVSTR2_S13( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S13_SHIFT ) & \
    GR740_IOPLL_DRVSTR2_S13_MASK )

#define GR740_IOPLL_DRVSTR2_S12_SHIFT 4
#define GR740_IOPLL_DRVSTR2_S12_MASK 0x30U
#define GR740_IOPLL_DRVSTR2_S12_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR2_S12_MASK ) >> \
    GR740_IOPLL_DRVSTR2_S12_SHIFT )
#define GR740_IOPLL_DRVSTR2_S12_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR2_S12_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S12_SHIFT ) & \
      GR740_IOPLL_DRVSTR2_S12_MASK ) )
#define GR740_IOPLL_DRVSTR2_S12( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S12_SHIFT ) & \
    GR740_IOPLL_DRVSTR2_S12_MASK )

#define GR740_IOPLL_DRVSTR2_S11_SHIFT 2
#define GR740_IOPLL_DRVSTR2_S11_MASK 0xcU
#define GR740_IOPLL_DRVSTR2_S11_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR2_S11_MASK ) >> \
    GR740_IOPLL_DRVSTR2_S11_SHIFT )
#define GR740_IOPLL_DRVSTR2_S11_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR2_S11_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S11_SHIFT ) & \
      GR740_IOPLL_DRVSTR2_S11_MASK ) )
#define GR740_IOPLL_DRVSTR2_S11( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S11_SHIFT ) & \
    GR740_IOPLL_DRVSTR2_S11_MASK )

#define GR740_IOPLL_DRVSTR2_S10_SHIFT 0
#define GR740_IOPLL_DRVSTR2_S10_MASK 0x3U
#define GR740_IOPLL_DRVSTR2_S10_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_DRVSTR2_S10_MASK ) >> \
    GR740_IOPLL_DRVSTR2_S10_SHIFT )
#define GR740_IOPLL_DRVSTR2_S10_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_DRVSTR2_S10_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S10_SHIFT ) & \
      GR740_IOPLL_DRVSTR2_S10_MASK ) )
#define GR740_IOPLL_DRVSTR2_S10( _val ) \
  ( ( ( _val ) << GR740_IOPLL_DRVSTR2_S10_SHIFT ) & \
    GR740_IOPLL_DRVSTR2_S10_MASK )

/** @} */

/**
 * @defgroup RTEMSBSPsGR740IOPLLLOCKDOWN \
 *   Configuration lockdown register (LOCKDOWN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_IOPLL_LOCKDOWN_PERMANENT_SHIFT 16
#define GR740_IOPLL_LOCKDOWN_PERMANENT_MASK 0xff0000U
#define GR740_IOPLL_LOCKDOWN_PERMANENT_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_LOCKDOWN_PERMANENT_MASK ) >> \
    GR740_IOPLL_LOCKDOWN_PERMANENT_SHIFT )
#define GR740_IOPLL_LOCKDOWN_PERMANENT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_LOCKDOWN_PERMANENT_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_LOCKDOWN_PERMANENT_SHIFT ) & \
      GR740_IOPLL_LOCKDOWN_PERMANENT_MASK ) )
#define GR740_IOPLL_LOCKDOWN_PERMANENT( _val ) \
  ( ( ( _val ) << GR740_IOPLL_LOCKDOWN_PERMANENT_SHIFT ) & \
    GR740_IOPLL_LOCKDOWN_PERMANENT_MASK )

#define GR740_IOPLL_LOCKDOWN_REVOCABLE_SHIFT 0
#define GR740_IOPLL_LOCKDOWN_REVOCABLE_MASK 0xffU
#define GR740_IOPLL_LOCKDOWN_REVOCABLE_GET( _reg ) \
  ( ( ( _reg ) & GR740_IOPLL_LOCKDOWN_REVOCABLE_MASK ) >> \
    GR740_IOPLL_LOCKDOWN_REVOCABLE_SHIFT )
#define GR740_IOPLL_LOCKDOWN_REVOCABLE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_IOPLL_LOCKDOWN_REVOCABLE_MASK ) | \
    ( ( ( _val ) << GR740_IOPLL_LOCKDOWN_REVOCABLE_SHIFT ) & \
      GR740_IOPLL_LOCKDOWN_REVOCABLE_MASK ) )
#define GR740_IOPLL_LOCKDOWN_REVOCABLE( _val ) \
  ( ( ( _val ) << GR740_IOPLL_LOCKDOWN_REVOCABLE_SHIFT ) & \
    GR740_IOPLL_LOCKDOWN_REVOCABLE_MASK )

/** @} */

/**
 * @brief This structure defines the GR740 I/0 and PLL Configuration register
 *   block memory map.
 */
typedef struct gr740_iopll {
  /**
   * @brief See @ref RTEMSBSPsGR740IOPLLFTMFUNC.
   */
  uint32_t ftmfunc;

  /**
   * @brief See @ref RTEMSBSPsGR740IOPLLALTFUNC.
   */
  uint32_t altfunc;

  /**
   * @brief See @ref RTEMSBSPsGR740IOPLLLVDSMCLK.
   */
  uint32_t lvdsmclk;

  /**
   * @brief See @ref RTEMSBSPsGR740IOPLLPLLNEWCFG.
   */
  uint32_t pllnewcfg;

  /**
   * @brief See @ref RTEMSBSPsGR740IOPLLPLLRECFG.
   */
  uint32_t pllrecfg;

  /**
   * @brief See @ref RTEMSBSPsGR740IOPLLPLLCURCFG.
   */
  uint32_t pllcurcfg;

  /**
   * @brief See @ref RTEMSBSPsGR740IOPLLDRVSTR1.
   */
  uint32_t drvstr1;

  /**
   * @brief See @ref RTEMSBSPsGR740IOPLLDRVSTR2.
   */
  uint32_t drvstr2;

  /**
   * @brief See @ref RTEMSBSPsGR740IOPLLLOCKDOWN.
   */
  uint32_t lockdown;
} gr740_iopll;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _BSP_GR740_IOPLL_REGS_H */
