/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRSPWROUTER
 *
 * @brief This header file defines the GRSPWROUTER register block interface.
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

/* Generated from spec:/dev/grlib/if/grspwrouter-header */

#ifndef _GRLIB_GRSPWROUTER_REGS_H
#define _GRLIB_GRSPWROUTER_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grspwrouter-portstats */

/**
 * @defgroup RTEMSDeviceGRSPWRouterPortStats SpaceWire Router Port Statistics
 *
 * @ingroup RTEMSDeviceGRSPWROUTER
 *
 * @brief This group contains the SpaceWire Router Port Statistics interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRSPWRouterPortStatsOCHARCNT \
 *   Outgoing character counter, ports > 0 (OCHARCNT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_OCHARCNT_OR 0x80000000U

#define GRSPWROUTER_OCHARCNT_CC_SHIFT 0
#define GRSPWROUTER_OCHARCNT_CC_MASK 0x7fffffffU
#define GRSPWROUTER_OCHARCNT_CC_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_OCHARCNT_CC_MASK ) >> \
    GRSPWROUTER_OCHARCNT_CC_SHIFT )
#define GRSPWROUTER_OCHARCNT_CC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_OCHARCNT_CC_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_OCHARCNT_CC_SHIFT ) & \
      GRSPWROUTER_OCHARCNT_CC_MASK ) )
#define GRSPWROUTER_OCHARCNT_CC( _val ) \
  ( ( ( _val ) << GRSPWROUTER_OCHARCNT_CC_SHIFT ) & \
    GRSPWROUTER_OCHARCNT_CC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWRouterPortStatsICHARCNT \
 *   Incoming character counter, ports > 0 (ICHARCNT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_ICHARCNT_OR 0x80000000U

#define GRSPWROUTER_ICHARCNT_CC_SHIFT 0
#define GRSPWROUTER_ICHARCNT_CC_MASK 0x7fffffffU
#define GRSPWROUTER_ICHARCNT_CC_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_ICHARCNT_CC_MASK ) >> \
    GRSPWROUTER_ICHARCNT_CC_SHIFT )
#define GRSPWROUTER_ICHARCNT_CC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_ICHARCNT_CC_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_ICHARCNT_CC_SHIFT ) & \
      GRSPWROUTER_ICHARCNT_CC_MASK ) )
#define GRSPWROUTER_ICHARCNT_CC( _val ) \
  ( ( ( _val ) << GRSPWROUTER_ICHARCNT_CC_SHIFT ) & \
    GRSPWROUTER_ICHARCNT_CC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWRouterPortStatsOPKTCNT \
 *   Outgoing packet counter, ports > 0 (OPKTCNT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_OPKTCNT_OR 0x80000000U

#define GRSPWROUTER_OPKTCNT_PC_SHIFT 0
#define GRSPWROUTER_OPKTCNT_PC_MASK 0x7fffffffU
#define GRSPWROUTER_OPKTCNT_PC_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_OPKTCNT_PC_MASK ) >> \
    GRSPWROUTER_OPKTCNT_PC_SHIFT )
#define GRSPWROUTER_OPKTCNT_PC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_OPKTCNT_PC_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_OPKTCNT_PC_SHIFT ) & \
      GRSPWROUTER_OPKTCNT_PC_MASK ) )
#define GRSPWROUTER_OPKTCNT_PC( _val ) \
  ( ( ( _val ) << GRSPWROUTER_OPKTCNT_PC_SHIFT ) & \
    GRSPWROUTER_OPKTCNT_PC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWRouterPortStatsIPKTCNT \
 *   Incoming packet counter, ports > 0 (IPKTCNT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_IPKTCNT_OR 0x80000000U

#define GRSPWROUTER_IPKTCNT_PC_SHIFT 0
#define GRSPWROUTER_IPKTCNT_PC_MASK 0x7fffffffU
#define GRSPWROUTER_IPKTCNT_PC_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_IPKTCNT_PC_MASK ) >> \
    GRSPWROUTER_IPKTCNT_PC_SHIFT )
#define GRSPWROUTER_IPKTCNT_PC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_IPKTCNT_PC_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_IPKTCNT_PC_SHIFT ) & \
      GRSPWROUTER_IPKTCNT_PC_MASK ) )
#define GRSPWROUTER_IPKTCNT_PC( _val ) \
  ( ( ( _val ) << GRSPWROUTER_IPKTCNT_PC_SHIFT ) & \
    GRSPWROUTER_IPKTCNT_PC_MASK )

/** @} */

/**
 * @brief This structure defines the SpaceWire Router Port Statistics register
 *   block memory map.
 */
typedef struct grspwrouter_portstats {
  /**
   * @brief See @ref RTEMSDeviceGRSPWRouterPortStatsOCHARCNT.
   */
  uint32_t ocharcnt;

  /**
   * @brief See @ref RTEMSDeviceGRSPWRouterPortStatsICHARCNT.
   */
  uint32_t icharcnt;

  /**
   * @brief See @ref RTEMSDeviceGRSPWRouterPortStatsOPKTCNT.
   */
  uint32_t opktcnt;

  /**
   * @brief See @ref RTEMSDeviceGRSPWRouterPortStatsIPKTCNT.
   */
  uint32_t ipktcnt;
} grspwrouter_portstats;

/** @} */

/* Generated from spec:/dev/grlib/if/grspwrouter */

/**
 * @defgroup RTEMSDeviceGRSPWROUTER SpaceWire Router
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the SpaceWire Router interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERRTPMAP \
 *   Routing table port mapping, addresses 1-31 and 32-255 (RTPMAP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_RTPMAP_PE_SHIFT 1
#define GRSPWROUTER_RTPMAP_PE_MASK 0xfffffffeU
#define GRSPWROUTER_RTPMAP_PE_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_RTPMAP_PE_MASK ) >> \
    GRSPWROUTER_RTPMAP_PE_SHIFT )
#define GRSPWROUTER_RTPMAP_PE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_RTPMAP_PE_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_RTPMAP_PE_SHIFT ) & \
      GRSPWROUTER_RTPMAP_PE_MASK ) )
#define GRSPWROUTER_RTPMAP_PE( _val ) \
  ( ( ( _val ) << GRSPWROUTER_RTPMAP_PE_SHIFT ) & \
    GRSPWROUTER_RTPMAP_PE_MASK )

#define GRSPWROUTER_RTPMAP_PD 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERRTACTRL \
 *   Routing table address control, addresses 1-31 and 32-255 (RTACTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_RTACTRL_SR 0x8U

#define GRSPWROUTER_RTACTRL_EN 0x4U

#define GRSPWROUTER_RTACTRL_PR 0x2U

#define GRSPWROUTER_RTACTRL_HD 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPCTRLCFG \
 *   Port control, port 0 (configuration port) (PCTRLCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PCTRLCFG_PL 0x20000U

#define GRSPWROUTER_PCTRLCFG_TS 0x10000U

#define GRSPWROUTER_PCTRLCFG_TR 0x200U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPCTRL Port control, ports > 0 (PCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PCTRL_RD_SHIFT 24
#define GRSPWROUTER_PCTRL_RD_MASK 0xff000000U
#define GRSPWROUTER_PCTRL_RD_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PCTRL_RD_MASK ) >> \
    GRSPWROUTER_PCTRL_RD_SHIFT )
#define GRSPWROUTER_PCTRL_RD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PCTRL_RD_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PCTRL_RD_SHIFT ) & \
      GRSPWROUTER_PCTRL_RD_MASK ) )
#define GRSPWROUTER_PCTRL_RD( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PCTRL_RD_SHIFT ) & \
    GRSPWROUTER_PCTRL_RD_MASK )

#define GRSPWROUTER_PCTRL_RES_SHIFT 22
#define GRSPWROUTER_PCTRL_RES_MASK 0xc00000U
#define GRSPWROUTER_PCTRL_RES_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PCTRL_RES_MASK ) >> \
    GRSPWROUTER_PCTRL_RES_SHIFT )
#define GRSPWROUTER_PCTRL_RES_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PCTRL_RES_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PCTRL_RES_SHIFT ) & \
      GRSPWROUTER_PCTRL_RES_MASK ) )
#define GRSPWROUTER_PCTRL_RES( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PCTRL_RES_SHIFT ) & \
    GRSPWROUTER_PCTRL_RES_MASK )

#define GRSPWROUTER_PCTRL_ST 0x200000U

#define GRSPWROUTER_PCTRL_SR 0x100000U

#define GRSPWROUTER_PCTRL_AD 0x80000U

#define GRSPWROUTER_PCTRL_LR 0x40000U

#define GRSPWROUTER_PCTRL_PL 0x20000U

#define GRSPWROUTER_PCTRL_TS 0x10000U

#define GRSPWROUTER_PCTRL_IC 0x8000U

#define GRSPWROUTER_PCTRL_ET 0x4000U

#define GRSPWROUTER_PCTRL_NF 0x2000U

#define GRSPWROUTER_PCTRL_PS 0x1000U

#define GRSPWROUTER_PCTRL_BE 0x800U

#define GRSPWROUTER_PCTRL_DI 0x400U

#define GRSPWROUTER_PCTRL_TR 0x200U

#define GRSPWROUTER_PCTRL_PR 0x100U

#define GRSPWROUTER_PCTRL_TF 0x80U

#define GRSPWROUTER_PCTRL_RS 0x40U

#define GRSPWROUTER_PCTRL_TE 0x20U

#define GRSPWROUTER_PCTRL_R 0x10U

#define GRSPWROUTER_PCTRL_CE 0x8U

#define GRSPWROUTER_PCTRL_AS 0x4U

#define GRSPWROUTER_PCTRL_LS 0x2U

#define GRSPWROUTER_PCTRL_LD 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPSTSCFG \
 *   Port status, port 0 (configuration port) (PSTSCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PSTSCFG_EO 0x80000000U

#define GRSPWROUTER_PSTSCFG_EE 0x40000000U

#define GRSPWROUTER_PSTSCFG_PL 0x20000000U

#define GRSPWROUTER_PSTSCFG_TT 0x10000000U

#define GRSPWROUTER_PSTSCFG_PT 0x8000000U

#define GRSPWROUTER_PSTSCFG_HC 0x4000000U

#define GRSPWROUTER_PSTSCFG_PI 0x2000000U

#define GRSPWROUTER_PSTSCFG_CE 0x1000000U

#define GRSPWROUTER_PSTSCFG_EC_SHIFT 20
#define GRSPWROUTER_PSTSCFG_EC_MASK 0xf00000U
#define GRSPWROUTER_PSTSCFG_EC_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PSTSCFG_EC_MASK ) >> \
    GRSPWROUTER_PSTSCFG_EC_SHIFT )
#define GRSPWROUTER_PSTSCFG_EC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PSTSCFG_EC_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PSTSCFG_EC_SHIFT ) & \
      GRSPWROUTER_PSTSCFG_EC_MASK ) )
#define GRSPWROUTER_PSTSCFG_EC( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PSTSCFG_EC_SHIFT ) & \
    GRSPWROUTER_PSTSCFG_EC_MASK )

#define GRSPWROUTER_PSTSCFG_R 0x80000U

#define GRSPWROUTER_PSTSCFG_TS 0x40000U

#define GRSPWROUTER_PSTSCFG_ME 0x20000U

#define GRSPWROUTER_PSTSCFG_IP_SHIFT 7
#define GRSPWROUTER_PSTSCFG_IP_MASK 0xf80U
#define GRSPWROUTER_PSTSCFG_IP_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PSTSCFG_IP_MASK ) >> \
    GRSPWROUTER_PSTSCFG_IP_SHIFT )
#define GRSPWROUTER_PSTSCFG_IP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PSTSCFG_IP_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PSTSCFG_IP_SHIFT ) & \
      GRSPWROUTER_PSTSCFG_IP_MASK ) )
#define GRSPWROUTER_PSTSCFG_IP( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PSTSCFG_IP_SHIFT ) & \
    GRSPWROUTER_PSTSCFG_IP_MASK )

#define GRSPWROUTER_PSTSCFG_RES_SHIFT 5
#define GRSPWROUTER_PSTSCFG_RES_MASK 0x60U
#define GRSPWROUTER_PSTSCFG_RES_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PSTSCFG_RES_MASK ) >> \
    GRSPWROUTER_PSTSCFG_RES_SHIFT )
#define GRSPWROUTER_PSTSCFG_RES_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PSTSCFG_RES_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PSTSCFG_RES_SHIFT ) & \
      GRSPWROUTER_PSTSCFG_RES_MASK ) )
#define GRSPWROUTER_PSTSCFG_RES( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PSTSCFG_RES_SHIFT ) & \
    GRSPWROUTER_PSTSCFG_RES_MASK )

#define GRSPWROUTER_PSTSCFG_CP 0x10U

#define GRSPWROUTER_PSTSCFG_PC_SHIFT 0
#define GRSPWROUTER_PSTSCFG_PC_MASK 0xfU
#define GRSPWROUTER_PSTSCFG_PC_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PSTSCFG_PC_MASK ) >> \
    GRSPWROUTER_PSTSCFG_PC_SHIFT )
#define GRSPWROUTER_PSTSCFG_PC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PSTSCFG_PC_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PSTSCFG_PC_SHIFT ) & \
      GRSPWROUTER_PSTSCFG_PC_MASK ) )
#define GRSPWROUTER_PSTSCFG_PC( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PSTSCFG_PC_SHIFT ) & \
    GRSPWROUTER_PSTSCFG_PC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPSTS Port status, ports > 0 (PSTS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PSTS_PT_SHIFT 30
#define GRSPWROUTER_PSTS_PT_MASK 0xc0000000U
#define GRSPWROUTER_PSTS_PT_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PSTS_PT_MASK ) >> \
    GRSPWROUTER_PSTS_PT_SHIFT )
#define GRSPWROUTER_PSTS_PT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PSTS_PT_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PSTS_PT_SHIFT ) & \
      GRSPWROUTER_PSTS_PT_MASK ) )
#define GRSPWROUTER_PSTS_PT( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PSTS_PT_SHIFT ) & \
    GRSPWROUTER_PSTS_PT_MASK )

#define GRSPWROUTER_PSTS_PL 0x20000000U

#define GRSPWROUTER_PSTS_TT 0x10000000U

#define GRSPWROUTER_PSTS_RS 0x8000000U

#define GRSPWROUTER_PSTS_SR 0x4000000U

#define GRSPWROUTER_PSTS_LR 0x400000U

#define GRSPWROUTER_PSTS_SP 0x200000U

#define GRSPWROUTER_PSTS_AC 0x100000U

#define GRSPWROUTER_PSTS_AP 0x80000U

#define GRSPWROUTER_PSTS_TS 0x40000U

#define GRSPWROUTER_PSTS_ME 0x20000U

#define GRSPWROUTER_PSTS_TF 0x10000U

#define GRSPWROUTER_PSTS_RE 0x8000U

#define GRSPWROUTER_PSTS_LS_SHIFT 12
#define GRSPWROUTER_PSTS_LS_MASK 0x7000U
#define GRSPWROUTER_PSTS_LS_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PSTS_LS_MASK ) >> \
    GRSPWROUTER_PSTS_LS_SHIFT )
#define GRSPWROUTER_PSTS_LS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PSTS_LS_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PSTS_LS_SHIFT ) & \
      GRSPWROUTER_PSTS_LS_MASK ) )
#define GRSPWROUTER_PSTS_LS( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PSTS_LS_SHIFT ) & \
    GRSPWROUTER_PSTS_LS_MASK )

#define GRSPWROUTER_PSTS_IP_SHIFT 7
#define GRSPWROUTER_PSTS_IP_MASK 0xf80U
#define GRSPWROUTER_PSTS_IP_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PSTS_IP_MASK ) >> \
    GRSPWROUTER_PSTS_IP_SHIFT )
#define GRSPWROUTER_PSTS_IP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PSTS_IP_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PSTS_IP_SHIFT ) & \
      GRSPWROUTER_PSTS_IP_MASK ) )
#define GRSPWROUTER_PSTS_IP( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PSTS_IP_SHIFT ) & \
    GRSPWROUTER_PSTS_IP_MASK )

#define GRSPWROUTER_PSTS_PR 0x40U

#define GRSPWROUTER_PSTS_PB 0x20U

#define GRSPWROUTER_PSTS_IA 0x10U

#define GRSPWROUTER_PSTS_CE 0x8U

#define GRSPWROUTER_PSTS_ER 0x4U

#define GRSPWROUTER_PSTS_DE 0x2U

#define GRSPWROUTER_PSTS_PE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPTIMER Port timer reload (PTIMER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PTIMER_RL_SHIFT 0
#define GRSPWROUTER_PTIMER_RL_MASK 0x3ffU
#define GRSPWROUTER_PTIMER_RL_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PTIMER_RL_MASK ) >> \
    GRSPWROUTER_PTIMER_RL_SHIFT )
#define GRSPWROUTER_PTIMER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PTIMER_RL_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PTIMER_RL_SHIFT ) & \
      GRSPWROUTER_PTIMER_RL_MASK ) )
#define GRSPWROUTER_PTIMER_RL( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PTIMER_RL_SHIFT ) & \
    GRSPWROUTER_PTIMER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPCTRL2CFG \
 *   Port control 2, port 0 (configuration port) (PCTRL2CFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PCTRL2CFG_SM_SHIFT 24
#define GRSPWROUTER_PCTRL2CFG_SM_MASK 0xff000000U
#define GRSPWROUTER_PCTRL2CFG_SM_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PCTRL2CFG_SM_MASK ) >> \
    GRSPWROUTER_PCTRL2CFG_SM_SHIFT )
#define GRSPWROUTER_PCTRL2CFG_SM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PCTRL2CFG_SM_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PCTRL2CFG_SM_SHIFT ) & \
      GRSPWROUTER_PCTRL2CFG_SM_MASK ) )
#define GRSPWROUTER_PCTRL2CFG_SM( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PCTRL2CFG_SM_SHIFT ) & \
    GRSPWROUTER_PCTRL2CFG_SM_MASK )

#define GRSPWROUTER_PCTRL2CFG_SV_SHIFT 16
#define GRSPWROUTER_PCTRL2CFG_SV_MASK 0xff0000U
#define GRSPWROUTER_PCTRL2CFG_SV_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PCTRL2CFG_SV_MASK ) >> \
    GRSPWROUTER_PCTRL2CFG_SV_SHIFT )
#define GRSPWROUTER_PCTRL2CFG_SV_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PCTRL2CFG_SV_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PCTRL2CFG_SV_SHIFT ) & \
      GRSPWROUTER_PCTRL2CFG_SV_MASK ) )
#define GRSPWROUTER_PCTRL2CFG_SV( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PCTRL2CFG_SV_SHIFT ) & \
    GRSPWROUTER_PCTRL2CFG_SV_MASK )

#define GRSPWROUTER_PCTRL2CFG_OR 0x8000U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPCTRL2 Port control 2, ports > 0 (PCTRL2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PCTRL2_SM_SHIFT 24
#define GRSPWROUTER_PCTRL2_SM_MASK 0xff000000U
#define GRSPWROUTER_PCTRL2_SM_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PCTRL2_SM_MASK ) >> \
    GRSPWROUTER_PCTRL2_SM_SHIFT )
#define GRSPWROUTER_PCTRL2_SM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PCTRL2_SM_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PCTRL2_SM_SHIFT ) & \
      GRSPWROUTER_PCTRL2_SM_MASK ) )
#define GRSPWROUTER_PCTRL2_SM( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PCTRL2_SM_SHIFT ) & \
    GRSPWROUTER_PCTRL2_SM_MASK )

#define GRSPWROUTER_PCTRL2_SV_SHIFT 16
#define GRSPWROUTER_PCTRL2_SV_MASK 0xff0000U
#define GRSPWROUTER_PCTRL2_SV_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PCTRL2_SV_MASK ) >> \
    GRSPWROUTER_PCTRL2_SV_SHIFT )
#define GRSPWROUTER_PCTRL2_SV_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PCTRL2_SV_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PCTRL2_SV_SHIFT ) & \
      GRSPWROUTER_PCTRL2_SV_MASK ) )
#define GRSPWROUTER_PCTRL2_SV( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PCTRL2_SV_SHIFT ) & \
    GRSPWROUTER_PCTRL2_SV_MASK )

#define GRSPWROUTER_PCTRL2_OR 0x8000U

#define GRSPWROUTER_PCTRL2_UR 0x4000U

#define GRSPWROUTER_PCTRL2_R 0x2000U

#define GRSPWROUTER_PCTRL2_AT 0x1000U

#define GRSPWROUTER_PCTRL2_AR 0x800U

#define GRSPWROUTER_PCTRL2_IT 0x400U

#define GRSPWROUTER_PCTRL2_IR 0x200U

#define GRSPWROUTER_PCTRL2_SD_SHIFT 1
#define GRSPWROUTER_PCTRL2_SD_MASK 0x3eU
#define GRSPWROUTER_PCTRL2_SD_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PCTRL2_SD_MASK ) >> \
    GRSPWROUTER_PCTRL2_SD_SHIFT )
#define GRSPWROUTER_PCTRL2_SD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PCTRL2_SD_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PCTRL2_SD_SHIFT ) & \
      GRSPWROUTER_PCTRL2_SD_MASK ) )
#define GRSPWROUTER_PCTRL2_SD( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PCTRL2_SD_SHIFT ) & \
    GRSPWROUTER_PCTRL2_SD_MASK )

#define GRSPWROUTER_PCTRL2_SC 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERRTRCFG \
 *   Router configuration / status (RTRCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_RTRCFG_SP_SHIFT 27
#define GRSPWROUTER_RTRCFG_SP_MASK 0xf8000000U
#define GRSPWROUTER_RTRCFG_SP_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_RTRCFG_SP_MASK ) >> \
    GRSPWROUTER_RTRCFG_SP_SHIFT )
#define GRSPWROUTER_RTRCFG_SP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_RTRCFG_SP_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_RTRCFG_SP_SHIFT ) & \
      GRSPWROUTER_RTRCFG_SP_MASK ) )
#define GRSPWROUTER_RTRCFG_SP( _val ) \
  ( ( ( _val ) << GRSPWROUTER_RTRCFG_SP_SHIFT ) & \
    GRSPWROUTER_RTRCFG_SP_MASK )

#define GRSPWROUTER_RTRCFG_AP_SHIFT 22
#define GRSPWROUTER_RTRCFG_AP_MASK 0x7c00000U
#define GRSPWROUTER_RTRCFG_AP_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_RTRCFG_AP_MASK ) >> \
    GRSPWROUTER_RTRCFG_AP_SHIFT )
#define GRSPWROUTER_RTRCFG_AP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_RTRCFG_AP_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_RTRCFG_AP_SHIFT ) & \
      GRSPWROUTER_RTRCFG_AP_MASK ) )
#define GRSPWROUTER_RTRCFG_AP( _val ) \
  ( ( ( _val ) << GRSPWROUTER_RTRCFG_AP_SHIFT ) & \
    GRSPWROUTER_RTRCFG_AP_MASK )

#define GRSPWROUTER_RTRCFG_FP_SHIFT 17
#define GRSPWROUTER_RTRCFG_FP_MASK 0x3e0000U
#define GRSPWROUTER_RTRCFG_FP_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_RTRCFG_FP_MASK ) >> \
    GRSPWROUTER_RTRCFG_FP_SHIFT )
#define GRSPWROUTER_RTRCFG_FP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_RTRCFG_FP_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_RTRCFG_FP_SHIFT ) & \
      GRSPWROUTER_RTRCFG_FP_MASK ) )
#define GRSPWROUTER_RTRCFG_FP( _val ) \
  ( ( ( _val ) << GRSPWROUTER_RTRCFG_FP_SHIFT ) & \
    GRSPWROUTER_RTRCFG_FP_MASK )

#define GRSPWROUTER_RTRCFG_R 0x10000U

#define GRSPWROUTER_RTRCFG_SR 0x8000U

#define GRSPWROUTER_RTRCFG_PE 0x4000U

#define GRSPWROUTER_RTRCFG_IC 0x2000U

#define GRSPWROUTER_RTRCFG_IS 0x1000U

#define GRSPWROUTER_RTRCFG_IP 0x800U

#define GRSPWROUTER_RTRCFG_AI 0x400U

#define GRSPWROUTER_RTRCFG_AT 0x200U

#define GRSPWROUTER_RTRCFG_IE 0x100U

#define GRSPWROUTER_RTRCFG_RE 0x80U

#define GRSPWROUTER_RTRCFG_EE 0x40U

#define GRSPWROUTER_RTRCFG_R 0x20U

#define GRSPWROUTER_RTRCFG_SA 0x10U

#define GRSPWROUTER_RTRCFG_TF 0x8U

#define GRSPWROUTER_RTRCFG_RM 0x4U

#define GRSPWROUTER_RTRCFG_TA 0x2U

#define GRSPWROUTER_RTRCFG_PP 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERTC Time-code (TC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_TC_RE 0x200U

#define GRSPWROUTER_TC_EN 0x100U

#define GRSPWROUTER_TC_CF_SHIFT 6
#define GRSPWROUTER_TC_CF_MASK 0xc0U
#define GRSPWROUTER_TC_CF_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_TC_CF_MASK ) >> \
    GRSPWROUTER_TC_CF_SHIFT )
#define GRSPWROUTER_TC_CF_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_TC_CF_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_TC_CF_SHIFT ) & \
      GRSPWROUTER_TC_CF_MASK ) )
#define GRSPWROUTER_TC_CF( _val ) \
  ( ( ( _val ) << GRSPWROUTER_TC_CF_SHIFT ) & \
    GRSPWROUTER_TC_CF_MASK )

#define GRSPWROUTER_TC_TC_SHIFT 0
#define GRSPWROUTER_TC_TC_MASK 0x3fU
#define GRSPWROUTER_TC_TC_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_TC_TC_MASK ) >> \
    GRSPWROUTER_TC_TC_SHIFT )
#define GRSPWROUTER_TC_TC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_TC_TC_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_TC_TC_SHIFT ) & \
      GRSPWROUTER_TC_TC_MASK ) )
#define GRSPWROUTER_TC_TC( _val ) \
  ( ( ( _val ) << GRSPWROUTER_TC_TC_SHIFT ) & \
    GRSPWROUTER_TC_TC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERVER Version / instance ID (VER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_VER_MA_SHIFT 24
#define GRSPWROUTER_VER_MA_MASK 0xff000000U
#define GRSPWROUTER_VER_MA_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_VER_MA_MASK ) >> \
    GRSPWROUTER_VER_MA_SHIFT )
#define GRSPWROUTER_VER_MA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_VER_MA_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_VER_MA_SHIFT ) & \
      GRSPWROUTER_VER_MA_MASK ) )
#define GRSPWROUTER_VER_MA( _val ) \
  ( ( ( _val ) << GRSPWROUTER_VER_MA_SHIFT ) & \
    GRSPWROUTER_VER_MA_MASK )

#define GRSPWROUTER_VER_MI_SHIFT 16
#define GRSPWROUTER_VER_MI_MASK 0xff0000U
#define GRSPWROUTER_VER_MI_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_VER_MI_MASK ) >> \
    GRSPWROUTER_VER_MI_SHIFT )
#define GRSPWROUTER_VER_MI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_VER_MI_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_VER_MI_SHIFT ) & \
      GRSPWROUTER_VER_MI_MASK ) )
#define GRSPWROUTER_VER_MI( _val ) \
  ( ( ( _val ) << GRSPWROUTER_VER_MI_SHIFT ) & \
    GRSPWROUTER_VER_MI_MASK )

#define GRSPWROUTER_VER_PA_SHIFT 8
#define GRSPWROUTER_VER_PA_MASK 0xff00U
#define GRSPWROUTER_VER_PA_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_VER_PA_MASK ) >> \
    GRSPWROUTER_VER_PA_SHIFT )
#define GRSPWROUTER_VER_PA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_VER_PA_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_VER_PA_SHIFT ) & \
      GRSPWROUTER_VER_PA_MASK ) )
#define GRSPWROUTER_VER_PA( _val ) \
  ( ( ( _val ) << GRSPWROUTER_VER_PA_SHIFT ) & \
    GRSPWROUTER_VER_PA_MASK )

#define GRSPWROUTER_VER_ID_SHIFT 0
#define GRSPWROUTER_VER_ID_MASK 0xffU
#define GRSPWROUTER_VER_ID_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_VER_ID_MASK ) >> \
    GRSPWROUTER_VER_ID_SHIFT )
#define GRSPWROUTER_VER_ID_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_VER_ID_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_VER_ID_SHIFT ) & \
      GRSPWROUTER_VER_ID_MASK ) )
#define GRSPWROUTER_VER_ID( _val ) \
  ( ( ( _val ) << GRSPWROUTER_VER_ID_SHIFT ) & \
    GRSPWROUTER_VER_ID_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERIDIV Initialization divisor (IDIV)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_IDIV_ID_SHIFT 0
#define GRSPWROUTER_IDIV_ID_MASK 0xffU
#define GRSPWROUTER_IDIV_ID_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_IDIV_ID_MASK ) >> \
    GRSPWROUTER_IDIV_ID_SHIFT )
#define GRSPWROUTER_IDIV_ID_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_IDIV_ID_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_IDIV_ID_SHIFT ) & \
      GRSPWROUTER_IDIV_ID_MASK ) )
#define GRSPWROUTER_IDIV_ID( _val ) \
  ( ( ( _val ) << GRSPWROUTER_IDIV_ID_SHIFT ) & \
    GRSPWROUTER_IDIV_ID_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERCFGWE \
 *   Configuration port write enable (CFGWE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_CFGWE_WE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPRESCALER Timer prescaler reload (PRESCALER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PRESCALER_RL_SHIFT 0
#define GRSPWROUTER_PRESCALER_RL_MASK 0xffffffffU
#define GRSPWROUTER_PRESCALER_RL_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PRESCALER_RL_MASK ) >> \
    GRSPWROUTER_PRESCALER_RL_SHIFT )
#define GRSPWROUTER_PRESCALER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PRESCALER_RL_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PRESCALER_RL_SHIFT ) & \
      GRSPWROUTER_PRESCALER_RL_MASK ) )
#define GRSPWROUTER_PRESCALER_RL( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PRESCALER_RL_SHIFT ) & \
    GRSPWROUTER_PRESCALER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERIMASK Interrupt mask (IMASK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_IMASK_PE 0x400U

#define GRSPWROUTER_IMASK_SR 0x200U

#define GRSPWROUTER_IMASK_RS 0x100U

#define GRSPWROUTER_IMASK_TT 0x80U

#define GRSPWROUTER_IMASK_PL 0x40U

#define GRSPWROUTER_IMASK_TS 0x20U

#define GRSPWROUTER_IMASK_AC 0x10U

#define GRSPWROUTER_IMASK_RE 0x8U

#define GRSPWROUTER_IMASK_IA 0x4U

#define GRSPWROUTER_IMASK_LE 0x2U

#define GRSPWROUTER_IMASK_ME 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERIPMASK Interrupt port mask (IPMASK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_IPMASK_IE_SHIFT 0
#define GRSPWROUTER_IPMASK_IE_MASK 0xffffffffU
#define GRSPWROUTER_IPMASK_IE_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_IPMASK_IE_MASK ) >> \
    GRSPWROUTER_IPMASK_IE_SHIFT )
#define GRSPWROUTER_IPMASK_IE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_IPMASK_IE_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_IPMASK_IE_SHIFT ) & \
      GRSPWROUTER_IPMASK_IE_MASK ) )
#define GRSPWROUTER_IPMASK_IE( _val ) \
  ( ( ( _val ) << GRSPWROUTER_IPMASK_IE_SHIFT ) & \
    GRSPWROUTER_IPMASK_IE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPIP Port interrupt pending (PIP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PIP_IP_SHIFT 0
#define GRSPWROUTER_PIP_IP_MASK 0xffffffffU
#define GRSPWROUTER_PIP_IP_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PIP_IP_MASK ) >> \
    GRSPWROUTER_PIP_IP_SHIFT )
#define GRSPWROUTER_PIP_IP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PIP_IP_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PIP_IP_SHIFT ) & \
      GRSPWROUTER_PIP_IP_MASK ) )
#define GRSPWROUTER_PIP_IP( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PIP_IP_SHIFT ) & \
    GRSPWROUTER_PIP_IP_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERICODEGEN \
 *   Interrupt code generation (ICODEGEN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_ICODEGEN_HI 0x200000U

#define GRSPWROUTER_ICODEGEN_UA 0x100000U

#define GRSPWROUTER_ICODEGEN_AH 0x80000U

#define GRSPWROUTER_ICODEGEN_IT 0x40000U

#define GRSPWROUTER_ICODEGEN_TE 0x1U

#define GRSPWROUTER_ICODEGEN_EN 0x20000U

#define GRSPWROUTER_ICODEGEN_IN_SHIFT 6
#define GRSPWROUTER_ICODEGEN_IN_MASK 0xffc0U
#define GRSPWROUTER_ICODEGEN_IN_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_ICODEGEN_IN_MASK ) >> \
    GRSPWROUTER_ICODEGEN_IN_SHIFT )
#define GRSPWROUTER_ICODEGEN_IN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_ICODEGEN_IN_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_ICODEGEN_IN_SHIFT ) & \
      GRSPWROUTER_ICODEGEN_IN_MASK ) )
#define GRSPWROUTER_ICODEGEN_IN( _val ) \
  ( ( ( _val ) << GRSPWROUTER_ICODEGEN_IN_SHIFT ) & \
    GRSPWROUTER_ICODEGEN_IN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERISR0 \
 *   Interrupt code distribution ISR register, interrupt 0-31 (ISR0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_ISR0_IB_SHIFT 0
#define GRSPWROUTER_ISR0_IB_MASK 0xffffffffU
#define GRSPWROUTER_ISR0_IB_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_ISR0_IB_MASK ) >> \
    GRSPWROUTER_ISR0_IB_SHIFT )
#define GRSPWROUTER_ISR0_IB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_ISR0_IB_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_ISR0_IB_SHIFT ) & \
      GRSPWROUTER_ISR0_IB_MASK ) )
#define GRSPWROUTER_ISR0_IB( _val ) \
  ( ( ( _val ) << GRSPWROUTER_ISR0_IB_SHIFT ) & \
    GRSPWROUTER_ISR0_IB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERISR1 \
 *   Interrupt code distribution ISR register, interrupt 32-63 (ISR1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_ISR1_IB_SHIFT 0
#define GRSPWROUTER_ISR1_IB_MASK 0xffffffffU
#define GRSPWROUTER_ISR1_IB_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_ISR1_IB_MASK ) >> \
    GRSPWROUTER_ISR1_IB_SHIFT )
#define GRSPWROUTER_ISR1_IB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_ISR1_IB_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_ISR1_IB_SHIFT ) & \
      GRSPWROUTER_ISR1_IB_MASK ) )
#define GRSPWROUTER_ISR1_IB( _val ) \
  ( ( ( _val ) << GRSPWROUTER_ISR1_IB_SHIFT ) & \
    GRSPWROUTER_ISR1_IB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERISRTIMER \
 *   Interrupt code distribution ISR timer reload (ISRTIMER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_ISRTIMER_RL_SHIFT 0
#define GRSPWROUTER_ISRTIMER_RL_MASK 0xffffffffU
#define GRSPWROUTER_ISRTIMER_RL_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_ISRTIMER_RL_MASK ) >> \
    GRSPWROUTER_ISRTIMER_RL_SHIFT )
#define GRSPWROUTER_ISRTIMER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_ISRTIMER_RL_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_ISRTIMER_RL_SHIFT ) & \
      GRSPWROUTER_ISRTIMER_RL_MASK ) )
#define GRSPWROUTER_ISRTIMER_RL( _val ) \
  ( ( ( _val ) << GRSPWROUTER_ISRTIMER_RL_SHIFT ) & \
    GRSPWROUTER_ISRTIMER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAITIMER \
 *   Interrupt code distribution ACK-to-INT timer reload (AITIMER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_AITIMER_RL_SHIFT 0
#define GRSPWROUTER_AITIMER_RL_MASK 0xffffffffU
#define GRSPWROUTER_AITIMER_RL_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_AITIMER_RL_MASK ) >> \
    GRSPWROUTER_AITIMER_RL_SHIFT )
#define GRSPWROUTER_AITIMER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_AITIMER_RL_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_AITIMER_RL_SHIFT ) & \
      GRSPWROUTER_AITIMER_RL_MASK ) )
#define GRSPWROUTER_AITIMER_RL( _val ) \
  ( ( ( _val ) << GRSPWROUTER_AITIMER_RL_SHIFT ) & \
    GRSPWROUTER_AITIMER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERISRCTIMER \
 *   Interrupt code distribution ISR change timer reload (ISRCTIMER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_ISRCTIMER_RL_SHIFT 0
#define GRSPWROUTER_ISRCTIMER_RL_MASK 0x1fU
#define GRSPWROUTER_ISRCTIMER_RL_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_ISRCTIMER_RL_MASK ) >> \
    GRSPWROUTER_ISRCTIMER_RL_SHIFT )
#define GRSPWROUTER_ISRCTIMER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_ISRCTIMER_RL_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_ISRCTIMER_RL_SHIFT ) & \
      GRSPWROUTER_ISRCTIMER_RL_MASK ) )
#define GRSPWROUTER_ISRCTIMER_RL( _val ) \
  ( ( ( _val ) << GRSPWROUTER_ISRCTIMER_RL_SHIFT ) & \
    GRSPWROUTER_ISRCTIMER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERLRUNSTAT Link running status (LRUNSTAT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_LRUNSTAT_LR_SHIFT 1
#define GRSPWROUTER_LRUNSTAT_LR_MASK 0xfffffffeU
#define GRSPWROUTER_LRUNSTAT_LR_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_LRUNSTAT_LR_MASK ) >> \
    GRSPWROUTER_LRUNSTAT_LR_SHIFT )
#define GRSPWROUTER_LRUNSTAT_LR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_LRUNSTAT_LR_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_LRUNSTAT_LR_SHIFT ) & \
      GRSPWROUTER_LRUNSTAT_LR_MASK ) )
#define GRSPWROUTER_LRUNSTAT_LR( _val ) \
  ( ( ( _val ) << GRSPWROUTER_LRUNSTAT_LR_SHIFT ) & \
    GRSPWROUTER_LRUNSTAT_LR_MASK )

#define GRSPWROUTER_LRUNSTAT_R 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERCAP Capability (CAP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_CAP_AF_SHIFT 24
#define GRSPWROUTER_CAP_AF_MASK 0x3000000U
#define GRSPWROUTER_CAP_AF_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_CAP_AF_MASK ) >> \
    GRSPWROUTER_CAP_AF_SHIFT )
#define GRSPWROUTER_CAP_AF_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_CAP_AF_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_CAP_AF_SHIFT ) & \
      GRSPWROUTER_CAP_AF_MASK ) )
#define GRSPWROUTER_CAP_AF( _val ) \
  ( ( ( _val ) << GRSPWROUTER_CAP_AF_SHIFT ) & \
    GRSPWROUTER_CAP_AF_MASK )

#define GRSPWROUTER_CAP_R 0x800000U

#define GRSPWROUTER_CAP_PF_SHIFT 20
#define GRSPWROUTER_CAP_PF_MASK 0x700000U
#define GRSPWROUTER_CAP_PF_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_CAP_PF_MASK ) >> \
    GRSPWROUTER_CAP_PF_SHIFT )
#define GRSPWROUTER_CAP_PF_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_CAP_PF_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_CAP_PF_SHIFT ) & \
      GRSPWROUTER_CAP_PF_MASK ) )
#define GRSPWROUTER_CAP_PF( _val ) \
  ( ( ( _val ) << GRSPWROUTER_CAP_PF_SHIFT ) & \
    GRSPWROUTER_CAP_PF_MASK )

#define GRSPWROUTER_CAP_R 0x80000U

#define GRSPWROUTER_CAP_RM_SHIFT 16
#define GRSPWROUTER_CAP_RM_MASK 0x70000U
#define GRSPWROUTER_CAP_RM_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_CAP_RM_MASK ) >> \
    GRSPWROUTER_CAP_RM_SHIFT )
#define GRSPWROUTER_CAP_RM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_CAP_RM_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_CAP_RM_SHIFT ) & \
      GRSPWROUTER_CAP_RM_MASK ) )
#define GRSPWROUTER_CAP_RM( _val ) \
  ( ( ( _val ) << GRSPWROUTER_CAP_RM_SHIFT ) & \
    GRSPWROUTER_CAP_RM_MASK )

#define GRSPWROUTER_CAP_R 0x8000U

#define GRSPWROUTER_CAP_AA 0x4000U

#define GRSPWROUTER_CAP_AX 0x2000U

#define GRSPWROUTER_CAP_DP 0x1000U

#define GRSPWROUTER_CAP_ID 0x800U

#define GRSPWROUTER_CAP_SD 0x400U

#define GRSPWROUTER_CAP_PC_SHIFT 5
#define GRSPWROUTER_CAP_PC_MASK 0x3e0U
#define GRSPWROUTER_CAP_PC_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_CAP_PC_MASK ) >> \
    GRSPWROUTER_CAP_PC_SHIFT )
#define GRSPWROUTER_CAP_PC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_CAP_PC_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_CAP_PC_SHIFT ) & \
      GRSPWROUTER_CAP_PC_MASK ) )
#define GRSPWROUTER_CAP_PC( _val ) \
  ( ( ( _val ) << GRSPWROUTER_CAP_PC_SHIFT ) & \
    GRSPWROUTER_CAP_PC_MASK )

#define GRSPWROUTER_CAP_CC_SHIFT 0
#define GRSPWROUTER_CAP_CC_MASK 0x1fU
#define GRSPWROUTER_CAP_CC_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_CAP_CC_MASK ) >> \
    GRSPWROUTER_CAP_CC_SHIFT )
#define GRSPWROUTER_CAP_CC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_CAP_CC_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_CAP_CC_SHIFT ) & \
      GRSPWROUTER_CAP_CC_MASK ) )
#define GRSPWROUTER_CAP_CC( _val ) \
  ( ( ( _val ) << GRSPWROUTER_CAP_CC_SHIFT ) & \
    GRSPWROUTER_CAP_CC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPNPVEND \
 *   SpaceWire Plug-and-Play - Device Vendor and Product ID (PNPVEND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PNPVEND_VI_SHIFT 16
#define GRSPWROUTER_PNPVEND_VI_MASK 0xffff0000U
#define GRSPWROUTER_PNPVEND_VI_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PNPVEND_VI_MASK ) >> \
    GRSPWROUTER_PNPVEND_VI_SHIFT )
#define GRSPWROUTER_PNPVEND_VI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PNPVEND_VI_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PNPVEND_VI_SHIFT ) & \
      GRSPWROUTER_PNPVEND_VI_MASK ) )
#define GRSPWROUTER_PNPVEND_VI( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PNPVEND_VI_SHIFT ) & \
    GRSPWROUTER_PNPVEND_VI_MASK )

#define GRSPWROUTER_PNPVEND_PI_SHIFT 0
#define GRSPWROUTER_PNPVEND_PI_MASK 0x3ffffffU
#define GRSPWROUTER_PNPVEND_PI_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PNPVEND_PI_MASK ) >> \
    GRSPWROUTER_PNPVEND_PI_SHIFT )
#define GRSPWROUTER_PNPVEND_PI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PNPVEND_PI_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PNPVEND_PI_SHIFT ) & \
      GRSPWROUTER_PNPVEND_PI_MASK ) )
#define GRSPWROUTER_PNPVEND_PI( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PNPVEND_PI_SHIFT ) & \
    GRSPWROUTER_PNPVEND_PI_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPNPUVEND \
 *   SpaceWire Plug-and-Play - Unit Vendor and Product ID (PNPUVEND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PNPUVEND_VI_SHIFT 16
#define GRSPWROUTER_PNPUVEND_VI_MASK 0xffff0000U
#define GRSPWROUTER_PNPUVEND_VI_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PNPUVEND_VI_MASK ) >> \
    GRSPWROUTER_PNPUVEND_VI_SHIFT )
#define GRSPWROUTER_PNPUVEND_VI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PNPUVEND_VI_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PNPUVEND_VI_SHIFT ) & \
      GRSPWROUTER_PNPUVEND_VI_MASK ) )
#define GRSPWROUTER_PNPUVEND_VI( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PNPUVEND_VI_SHIFT ) & \
    GRSPWROUTER_PNPUVEND_VI_MASK )

#define GRSPWROUTER_PNPUVEND_PI_SHIFT 0
#define GRSPWROUTER_PNPUVEND_PI_MASK 0x3ffffffU
#define GRSPWROUTER_PNPUVEND_PI_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PNPUVEND_PI_MASK ) >> \
    GRSPWROUTER_PNPUVEND_PI_SHIFT )
#define GRSPWROUTER_PNPUVEND_PI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PNPUVEND_PI_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PNPUVEND_PI_SHIFT ) & \
      GRSPWROUTER_PNPUVEND_PI_MASK ) )
#define GRSPWROUTER_PNPUVEND_PI( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PNPUVEND_PI_SHIFT ) & \
    GRSPWROUTER_PNPUVEND_PI_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPNPUSN \
 *   SpaceWire Plug-and-Play - Unit Serial Number (PNPUSN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PNPUSN_SN_SHIFT 0
#define GRSPWROUTER_PNPUSN_SN_MASK 0xffffffffU
#define GRSPWROUTER_PNPUSN_SN_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PNPUSN_SN_MASK ) >> \
    GRSPWROUTER_PNPUSN_SN_SHIFT )
#define GRSPWROUTER_PNPUSN_SN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PNPUSN_SN_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PNPUSN_SN_SHIFT ) & \
      GRSPWROUTER_PNPUSN_SN_MASK ) )
#define GRSPWROUTER_PNPUSN_SN( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PNPUSN_SN_SHIFT ) & \
    GRSPWROUTER_PNPUSN_SN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERPNPNETDISC \
 *   SpaceWire Plug-and-Play - Port network discovery enable (PNPNETDISC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_PNPNETDISC_ND_SHIFT 0
#define GRSPWROUTER_PNPNETDISC_ND_MASK 0xffffffffU
#define GRSPWROUTER_PNPNETDISC_ND_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_PNPNETDISC_ND_MASK ) >> \
    GRSPWROUTER_PNPNETDISC_ND_SHIFT )
#define GRSPWROUTER_PNPNETDISC_ND_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_PNPNETDISC_ND_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_PNPNETDISC_ND_SHIFT ) & \
      GRSPWROUTER_PNPNETDISC_ND_MASK ) )
#define GRSPWROUTER_PNPNETDISC_ND( _val ) \
  ( ( ( _val ) << GRSPWROUTER_PNPNETDISC_ND_SHIFT ) & \
    GRSPWROUTER_PNPNETDISC_ND_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERMAXPLEN \
 *   Maximum packet length, ports > 0 (MAXPLEN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_MAXPLEN_ML_SHIFT 0
#define GRSPWROUTER_MAXPLEN_ML_MASK 0x1ffffffU
#define GRSPWROUTER_MAXPLEN_ML_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_MAXPLEN_ML_MASK ) >> \
    GRSPWROUTER_MAXPLEN_ML_SHIFT )
#define GRSPWROUTER_MAXPLEN_ML_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_MAXPLEN_ML_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_MAXPLEN_ML_SHIFT ) & \
      GRSPWROUTER_MAXPLEN_ML_MASK ) )
#define GRSPWROUTER_MAXPLEN_ML( _val ) \
  ( ( ( _val ) << GRSPWROUTER_MAXPLEN_ML_SHIFT ) & \
    GRSPWROUTER_MAXPLEN_ML_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERCREDCNT \
 *   Credit counter, SpaceWire ports (CREDCNT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_CREDCNT_OC_SHIFT 6
#define GRSPWROUTER_CREDCNT_OC_MASK 0xfc0U
#define GRSPWROUTER_CREDCNT_OC_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_CREDCNT_OC_MASK ) >> \
    GRSPWROUTER_CREDCNT_OC_SHIFT )
#define GRSPWROUTER_CREDCNT_OC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_CREDCNT_OC_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_CREDCNT_OC_SHIFT ) & \
      GRSPWROUTER_CREDCNT_OC_MASK ) )
#define GRSPWROUTER_CREDCNT_OC( _val ) \
  ( ( ( _val ) << GRSPWROUTER_CREDCNT_OC_SHIFT ) & \
    GRSPWROUTER_CREDCNT_OC_MASK )

#define GRSPWROUTER_CREDCNT_IC_SHIFT 0
#define GRSPWROUTER_CREDCNT_IC_MASK 0x3fU
#define GRSPWROUTER_CREDCNT_IC_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_CREDCNT_IC_MASK ) >> \
    GRSPWROUTER_CREDCNT_IC_SHIFT )
#define GRSPWROUTER_CREDCNT_IC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_CREDCNT_IC_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_CREDCNT_IC_SHIFT ) & \
      GRSPWROUTER_CREDCNT_IC_MASK ) )
#define GRSPWROUTER_CREDCNT_IC( _val ) \
  ( ( ( _val ) << GRSPWROUTER_CREDCNT_IC_SHIFT ) & \
    GRSPWROUTER_CREDCNT_IC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERGPO \
 *   General purpose out, bits 0-31, 32-63, 64-95, and 96-127 (GPO)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_GPO_GPO_SHIFT 0
#define GRSPWROUTER_GPO_GPO_MASK 0xffffffffU
#define GRSPWROUTER_GPO_GPO_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_GPO_GPO_MASK ) >> \
    GRSPWROUTER_GPO_GPO_SHIFT )
#define GRSPWROUTER_GPO_GPO_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_GPO_GPO_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_GPO_GPO_SHIFT ) & \
      GRSPWROUTER_GPO_GPO_MASK ) )
#define GRSPWROUTER_GPO_GPO( _val ) \
  ( ( ( _val ) << GRSPWROUTER_GPO_GPO_SHIFT ) & \
    GRSPWROUTER_GPO_GPO_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERGPI \
 *   General purpose in, bits 0-31, 32-63, 64-95, and 96-127 (GPI)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_GPI_GPI_SHIFT 0
#define GRSPWROUTER_GPI_GPI_MASK 0xffffffffU
#define GRSPWROUTER_GPI_GPI_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_GPI_GPI_MASK ) >> \
    GRSPWROUTER_GPI_GPI_SHIFT )
#define GRSPWROUTER_GPI_GPI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_GPI_GPI_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_GPI_GPI_SHIFT ) & \
      GRSPWROUTER_GPI_GPI_MASK ) )
#define GRSPWROUTER_GPI_GPI( _val ) \
  ( ( ( _val ) << GRSPWROUTER_GPI_GPI_SHIFT ) & \
    GRSPWROUTER_GPI_GPI_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERRTCOMB \
 *   Routing table, combined port mapping and address control, addresses 1-255 (RTCOMB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_RTCOMB_SR 0x80000000U

#define GRSPWROUTER_RTCOMB_EN 0x40000000U

#define GRSPWROUTER_RTCOMB_PR 0x20000000U

#define GRSPWROUTER_RTCOMB_HD 0x10000000U

#define GRSPWROUTER_RTCOMB_PE_SHIFT 1
#define GRSPWROUTER_RTCOMB_PE_MASK 0xffffffeU
#define GRSPWROUTER_RTCOMB_PE_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_RTCOMB_PE_MASK ) >> \
    GRSPWROUTER_RTCOMB_PE_SHIFT )
#define GRSPWROUTER_RTCOMB_PE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_RTCOMB_PE_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_RTCOMB_PE_SHIFT ) & \
      GRSPWROUTER_RTCOMB_PE_MASK ) )
#define GRSPWROUTER_RTCOMB_PE( _val ) \
  ( ( ( _val ) << GRSPWROUTER_RTCOMB_PE_SHIFT ) & \
    GRSPWROUTER_RTCOMB_PE_MASK )

#define GRSPWROUTER_RTCOMB_PD 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAPBAREA APB address area (APBAREA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPWROUTER_APBAREA_APB_SHIFT 0
#define GRSPWROUTER_APBAREA_APB_MASK 0xffffffffU
#define GRSPWROUTER_APBAREA_APB_GET( _reg ) \
  ( ( ( _reg ) & GRSPWROUTER_APBAREA_APB_MASK ) >> \
    GRSPWROUTER_APBAREA_APB_SHIFT )
#define GRSPWROUTER_APBAREA_APB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPWROUTER_APBAREA_APB_MASK ) | \
    ( ( ( _val ) << GRSPWROUTER_APBAREA_APB_SHIFT ) & \
      GRSPWROUTER_APBAREA_APB_MASK ) )
#define GRSPWROUTER_APBAREA_APB( _val ) \
  ( ( ( _val ) << GRSPWROUTER_APBAREA_APB_SHIFT ) & \
    GRSPWROUTER_APBAREA_APB_MASK )

/** @} */

/**
 * @brief This structure defines the SpaceWire Router register block memory
 *   map.
 */
typedef struct grspwrouter {
  uint32_t reserved_0_4;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERRTPMAP.
   */
  uint32_t rtpmap[ 255 ];

  uint32_t reserved_400_404;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERRTACTRL.
   */
  uint32_t rtactrl[ 255 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPCTRLCFG.
   */
  uint32_t pctrlcfg;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPCTRL.
   */
  uint32_t pctrl[ 31 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPSTSCFG.
   */
  uint32_t pstscfg;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPSTS.
   */
  uint32_t psts[ 31 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPTIMER.
   */
  uint32_t ptimer[ 32 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPCTRL2CFG.
   */
  uint32_t pctrl2cfg;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPCTRL2.
   */
  uint32_t pctrl2[ 31 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERRTRCFG.
   */
  uint32_t rtrcfg;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERTC.
   */
  uint32_t tc;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERVER.
   */
  uint32_t ver;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERIDIV.
   */
  uint32_t idiv;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERCFGWE.
   */
  uint32_t cfgwe;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPRESCALER.
   */
  uint32_t prescaler;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERIMASK.
   */
  uint32_t imask;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERIPMASK.
   */
  uint32_t ipmask;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPIP.
   */
  uint32_t pip;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERICODEGEN.
   */
  uint32_t icodegen;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERISR0.
   */
  uint32_t isr0;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERISR1.
   */
  uint32_t isr1;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERISRTIMER.
   */
  uint32_t isrtimer;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAITIMER.
   */
  uint32_t aitimer;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERISRCTIMER.
   */
  uint32_t isrctimer;

  uint32_t reserved_a3c_a40;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERLRUNSTAT.
   */
  uint32_t lrunstat;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERCAP.
   */
  uint32_t cap;

  uint32_t reserved_a48_a50[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPNPVEND.
   */
  uint32_t pnpvend;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPNPUVEND.
   */
  uint32_t pnpuvend;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPNPUSN.
   */
  uint32_t pnpusn;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERPNPNETDISC.
   */
  uint32_t pnpnetdisc;

  uint32_t reserved_a60_c10[ 108 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWRouterPortStats.
   */
  grspwrouter_portstats portstats[ 31 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERMAXPLEN.
   */
  uint32_t maxplen[ 32 ];

  uint32_t reserved_e80_e84;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERCREDCNT.
   */
  uint32_t credcnt[ 31 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERGPO.
   */
  uint32_t gpo[ 4 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERGPI.
   */
  uint32_t gpi[ 4 ];

  uint32_t reserved_f20_1004[ 57 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERRTCOMB.
   */
  uint32_t rtcomb[ 255 ];

  uint32_t reserved_1400_2000[ 768 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAPBAREA.
   */
  uint32_t apbarea[ 1024 ];
} grspwrouter;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRSPWROUTER_REGS_H */
