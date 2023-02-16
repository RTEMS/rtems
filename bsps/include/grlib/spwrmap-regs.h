/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBSPWMAP
 *
 * @brief This header file defines the SPWRMAP register block interface.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/* Generated from spec:/dev/grlib/if/spwrmap-header */

#ifndef _GRLIB_SPWRMAP_REGS_H
#define _GRLIB_SPWRMAP_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/spwrmap */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAP \
 *   SpaceWire Remote Memory Access Protocol (RMAP)
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the SpaceWire Remote Memory Access Protocol
 *   (RMAP) interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPRTPMAP \
 *   Routing table port mapping, addresses 1-12 and 32-255 (RTPMAP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_RTPMAP_PE_SHIFT 1
#define SPWRMAP_RTPMAP_PE_MASK 0x1ffeU
#define SPWRMAP_RTPMAP_PE_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_RTPMAP_PE_MASK ) >> \
    SPWRMAP_RTPMAP_PE_SHIFT )
#define SPWRMAP_RTPMAP_PE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_RTPMAP_PE_MASK ) | \
    ( ( ( _val ) << SPWRMAP_RTPMAP_PE_SHIFT ) & \
      SPWRMAP_RTPMAP_PE_MASK ) )
#define SPWRMAP_RTPMAP_PE( _val ) \
  ( ( ( _val ) << SPWRMAP_RTPMAP_PE_SHIFT ) & \
    SPWRMAP_RTPMAP_PE_MASK )

#define SPWRMAP_RTPMAP_PD 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPRTACTRL \
 *   Routing table address control, addresses 1-12 and 32-255 (RTACTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_RTACTRL_SR 0x8U

#define SPWRMAP_RTACTRL_EN 0x4U

#define SPWRMAP_RTACTRL_PR 0x2U

#define SPWRMAP_RTACTRL_HD 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPCTRLCFG \
 *   Port control, port 0 (configuration port) (PCTRLCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PCTRLCFG_PL 0x20000U

#define SPWRMAP_PCTRLCFG_TS 0x10000U

#define SPWRMAP_PCTRLCFG_TR 0x200U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPCTRL \
 *   Port control, ports 1-12 (SpaceWire ports and AMBA ports) (PCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PCTRL_RD_SHIFT 24
#define SPWRMAP_PCTRL_RD_MASK 0xff000000U
#define SPWRMAP_PCTRL_RD_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PCTRL_RD_MASK ) >> \
    SPWRMAP_PCTRL_RD_SHIFT )
#define SPWRMAP_PCTRL_RD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PCTRL_RD_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PCTRL_RD_SHIFT ) & \
      SPWRMAP_PCTRL_RD_MASK ) )
#define SPWRMAP_PCTRL_RD( _val ) \
  ( ( ( _val ) << SPWRMAP_PCTRL_RD_SHIFT ) & \
    SPWRMAP_PCTRL_RD_MASK )

#define SPWRMAP_PCTRL_ST 0x200000U

#define SPWRMAP_PCTRL_SR 0x100000U

#define SPWRMAP_PCTRL_AD 0x80000U

#define SPWRMAP_PCTRL_LR 0x40000U

#define SPWRMAP_PCTRL_PL 0x20000U

#define SPWRMAP_PCTRL_TS 0x10000U

#define SPWRMAP_PCTRL_IC 0x8000U

#define SPWRMAP_PCTRL_ET 0x4000U

#define SPWRMAP_PCTRL_DI 0x400U

#define SPWRMAP_PCTRL_TR 0x200U

#define SPWRMAP_PCTRL_PR 0x100U

#define SPWRMAP_PCTRL_TF 0x80U

#define SPWRMAP_PCTRL_RS 0x40U

#define SPWRMAP_PCTRL_TE 0x20U

#define SPWRMAP_PCTRL_CE 0x8U

#define SPWRMAP_PCTRL_AS 0x4U

#define SPWRMAP_PCTRL_LS 0x2U

#define SPWRMAP_PCTRL_LD 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPSTSCFG \
 *   Port status, port 0 (configuration port) (PSTSCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PSTSCFG_EO 0x80000000U

#define SPWRMAP_PSTSCFG_EE 0x40000000U

#define SPWRMAP_PSTSCFG_PL 0x20000000U

#define SPWRMAP_PSTSCFG_TT 0x10000000U

#define SPWRMAP_PSTSCFG_PT 0x8000000U

#define SPWRMAP_PSTSCFG_HC 0x4000000U

#define SPWRMAP_PSTSCFG_PI 0x2000000U

#define SPWRMAP_PSTSCFG_CE 0x1000000U

#define SPWRMAP_PSTSCFG_EC_SHIFT 20
#define SPWRMAP_PSTSCFG_EC_MASK 0xf00000U
#define SPWRMAP_PSTSCFG_EC_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PSTSCFG_EC_MASK ) >> \
    SPWRMAP_PSTSCFG_EC_SHIFT )
#define SPWRMAP_PSTSCFG_EC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PSTSCFG_EC_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PSTSCFG_EC_SHIFT ) & \
      SPWRMAP_PSTSCFG_EC_MASK ) )
#define SPWRMAP_PSTSCFG_EC( _val ) \
  ( ( ( _val ) << SPWRMAP_PSTSCFG_EC_SHIFT ) & \
    SPWRMAP_PSTSCFG_EC_MASK )

#define SPWRMAP_PSTSCFG_TS 0x40000U

#define SPWRMAP_PSTSCFG_ME 0x20000U

#define SPWRMAP_PSTSCFG_IP_SHIFT 7
#define SPWRMAP_PSTSCFG_IP_MASK 0xf80U
#define SPWRMAP_PSTSCFG_IP_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PSTSCFG_IP_MASK ) >> \
    SPWRMAP_PSTSCFG_IP_SHIFT )
#define SPWRMAP_PSTSCFG_IP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PSTSCFG_IP_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PSTSCFG_IP_SHIFT ) & \
      SPWRMAP_PSTSCFG_IP_MASK ) )
#define SPWRMAP_PSTSCFG_IP( _val ) \
  ( ( ( _val ) << SPWRMAP_PSTSCFG_IP_SHIFT ) & \
    SPWRMAP_PSTSCFG_IP_MASK )

#define SPWRMAP_PSTSCFG_CP 0x10U

#define SPWRMAP_PSTSCFG_PC_SHIFT 0
#define SPWRMAP_PSTSCFG_PC_MASK 0xfU
#define SPWRMAP_PSTSCFG_PC_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PSTSCFG_PC_MASK ) >> \
    SPWRMAP_PSTSCFG_PC_SHIFT )
#define SPWRMAP_PSTSCFG_PC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PSTSCFG_PC_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PSTSCFG_PC_SHIFT ) & \
      SPWRMAP_PSTSCFG_PC_MASK ) )
#define SPWRMAP_PSTSCFG_PC( _val ) \
  ( ( ( _val ) << SPWRMAP_PSTSCFG_PC_SHIFT ) & \
    SPWRMAP_PSTSCFG_PC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPSTS \
 *   Port status, ports 1-12 (SpaceWire ports and AMBA ports) (PSTS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PSTS_PT_SHIFT 30
#define SPWRMAP_PSTS_PT_MASK 0xc0000000U
#define SPWRMAP_PSTS_PT_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PSTS_PT_MASK ) >> \
    SPWRMAP_PSTS_PT_SHIFT )
#define SPWRMAP_PSTS_PT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PSTS_PT_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PSTS_PT_SHIFT ) & \
      SPWRMAP_PSTS_PT_MASK ) )
#define SPWRMAP_PSTS_PT( _val ) \
  ( ( ( _val ) << SPWRMAP_PSTS_PT_SHIFT ) & \
    SPWRMAP_PSTS_PT_MASK )

#define SPWRMAP_PSTS_PL 0x20000000U

#define SPWRMAP_PSTS_TT 0x10000000U

#define SPWRMAP_PSTS_RS 0x8000000U

#define SPWRMAP_PSTS_SR 0x4000000U

#define SPWRMAP_PSTS_LR 0x400000U

#define SPWRMAP_PSTS_SP 0x200000U

#define SPWRMAP_PSTS_AC 0x100000U

#define SPWRMAP_PSTS_TS 0x40000U

#define SPWRMAP_PSTS_ME 0x20000U

#define SPWRMAP_PSTS_TF 0x10000U

#define SPWRMAP_PSTS_RE 0x8000U

#define SPWRMAP_PSTS_LS_SHIFT 12
#define SPWRMAP_PSTS_LS_MASK 0x7000U
#define SPWRMAP_PSTS_LS_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PSTS_LS_MASK ) >> \
    SPWRMAP_PSTS_LS_SHIFT )
#define SPWRMAP_PSTS_LS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PSTS_LS_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PSTS_LS_SHIFT ) & \
      SPWRMAP_PSTS_LS_MASK ) )
#define SPWRMAP_PSTS_LS( _val ) \
  ( ( ( _val ) << SPWRMAP_PSTS_LS_SHIFT ) & \
    SPWRMAP_PSTS_LS_MASK )

#define SPWRMAP_PSTS_IP_SHIFT 7
#define SPWRMAP_PSTS_IP_MASK 0xf80U
#define SPWRMAP_PSTS_IP_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PSTS_IP_MASK ) >> \
    SPWRMAP_PSTS_IP_SHIFT )
#define SPWRMAP_PSTS_IP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PSTS_IP_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PSTS_IP_SHIFT ) & \
      SPWRMAP_PSTS_IP_MASK ) )
#define SPWRMAP_PSTS_IP( _val ) \
  ( ( ( _val ) << SPWRMAP_PSTS_IP_SHIFT ) & \
    SPWRMAP_PSTS_IP_MASK )

#define SPWRMAP_PSTS_PR 0x40U

#define SPWRMAP_PSTS_PB 0x20U

#define SPWRMAP_PSTS_IA 0x10U

#define SPWRMAP_PSTS_CE 0x8U

#define SPWRMAP_PSTS_ER 0x4U

#define SPWRMAP_PSTS_DE 0x2U

#define SPWRMAP_PSTS_PE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPTIMER \
 *   Port timer reload, ports 0-12 (PTIMER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PTIMER_RL_SHIFT 0
#define SPWRMAP_PTIMER_RL_MASK 0xffffU
#define SPWRMAP_PTIMER_RL_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PTIMER_RL_MASK ) >> \
    SPWRMAP_PTIMER_RL_SHIFT )
#define SPWRMAP_PTIMER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PTIMER_RL_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PTIMER_RL_SHIFT ) & \
      SPWRMAP_PTIMER_RL_MASK ) )
#define SPWRMAP_PTIMER_RL( _val ) \
  ( ( ( _val ) << SPWRMAP_PTIMER_RL_SHIFT ) & \
    SPWRMAP_PTIMER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPCTRL2CFG \
 *   Port control 2, port 0 (configuration port) (PCTRL2CFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PCTRL2CFG_SM_SHIFT 24
#define SPWRMAP_PCTRL2CFG_SM_MASK 0xff000000U
#define SPWRMAP_PCTRL2CFG_SM_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PCTRL2CFG_SM_MASK ) >> \
    SPWRMAP_PCTRL2CFG_SM_SHIFT )
#define SPWRMAP_PCTRL2CFG_SM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PCTRL2CFG_SM_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PCTRL2CFG_SM_SHIFT ) & \
      SPWRMAP_PCTRL2CFG_SM_MASK ) )
#define SPWRMAP_PCTRL2CFG_SM( _val ) \
  ( ( ( _val ) << SPWRMAP_PCTRL2CFG_SM_SHIFT ) & \
    SPWRMAP_PCTRL2CFG_SM_MASK )

#define SPWRMAP_PCTRL2CFG_SV_SHIFT 16
#define SPWRMAP_PCTRL2CFG_SV_MASK 0xff0000U
#define SPWRMAP_PCTRL2CFG_SV_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PCTRL2CFG_SV_MASK ) >> \
    SPWRMAP_PCTRL2CFG_SV_SHIFT )
#define SPWRMAP_PCTRL2CFG_SV_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PCTRL2CFG_SV_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PCTRL2CFG_SV_SHIFT ) & \
      SPWRMAP_PCTRL2CFG_SV_MASK ) )
#define SPWRMAP_PCTRL2CFG_SV( _val ) \
  ( ( ( _val ) << SPWRMAP_PCTRL2CFG_SV_SHIFT ) & \
    SPWRMAP_PCTRL2CFG_SV_MASK )

#define SPWRMAP_PCTRL2CFG_OR 0x8000U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPCTRL2 \
 *   Port control 2, ports 1-12 (SpaceWire ports and AMBA ports) (PCTRL2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PCTRL2_SM_SHIFT 24
#define SPWRMAP_PCTRL2_SM_MASK 0xff000000U
#define SPWRMAP_PCTRL2_SM_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PCTRL2_SM_MASK ) >> \
    SPWRMAP_PCTRL2_SM_SHIFT )
#define SPWRMAP_PCTRL2_SM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PCTRL2_SM_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PCTRL2_SM_SHIFT ) & \
      SPWRMAP_PCTRL2_SM_MASK ) )
#define SPWRMAP_PCTRL2_SM( _val ) \
  ( ( ( _val ) << SPWRMAP_PCTRL2_SM_SHIFT ) & \
    SPWRMAP_PCTRL2_SM_MASK )

#define SPWRMAP_PCTRL2_SV_SHIFT 16
#define SPWRMAP_PCTRL2_SV_MASK 0xff0000U
#define SPWRMAP_PCTRL2_SV_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PCTRL2_SV_MASK ) >> \
    SPWRMAP_PCTRL2_SV_SHIFT )
#define SPWRMAP_PCTRL2_SV_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PCTRL2_SV_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PCTRL2_SV_SHIFT ) & \
      SPWRMAP_PCTRL2_SV_MASK ) )
#define SPWRMAP_PCTRL2_SV( _val ) \
  ( ( ( _val ) << SPWRMAP_PCTRL2_SV_SHIFT ) & \
    SPWRMAP_PCTRL2_SV_MASK )

#define SPWRMAP_PCTRL2_OR 0x8000U

#define SPWRMAP_PCTRL2_UR 0x4000U

#define SPWRMAP_PCTRL2_AT 0x1000U

#define SPWRMAP_PCTRL2_AR 0x800U

#define SPWRMAP_PCTRL2_IT 0x400U

#define SPWRMAP_PCTRL2_IR 0x200U

#define SPWRMAP_PCTRL2_SD_SHIFT 1
#define SPWRMAP_PCTRL2_SD_MASK 0x3eU
#define SPWRMAP_PCTRL2_SD_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PCTRL2_SD_MASK ) >> \
    SPWRMAP_PCTRL2_SD_SHIFT )
#define SPWRMAP_PCTRL2_SD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PCTRL2_SD_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PCTRL2_SD_SHIFT ) & \
      SPWRMAP_PCTRL2_SD_MASK ) )
#define SPWRMAP_PCTRL2_SD( _val ) \
  ( ( ( _val ) << SPWRMAP_PCTRL2_SD_SHIFT ) & \
    SPWRMAP_PCTRL2_SD_MASK )

#define SPWRMAP_PCTRL2_SC 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPRTRCFG \
 *   Router configuration / status (RTRCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_RTRCFG_SP_SHIFT 27
#define SPWRMAP_RTRCFG_SP_MASK 0xf8000000U
#define SPWRMAP_RTRCFG_SP_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_RTRCFG_SP_MASK ) >> \
    SPWRMAP_RTRCFG_SP_SHIFT )
#define SPWRMAP_RTRCFG_SP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_RTRCFG_SP_MASK ) | \
    ( ( ( _val ) << SPWRMAP_RTRCFG_SP_SHIFT ) & \
      SPWRMAP_RTRCFG_SP_MASK ) )
#define SPWRMAP_RTRCFG_SP( _val ) \
  ( ( ( _val ) << SPWRMAP_RTRCFG_SP_SHIFT ) & \
    SPWRMAP_RTRCFG_SP_MASK )

#define SPWRMAP_RTRCFG_AP_SHIFT 22
#define SPWRMAP_RTRCFG_AP_MASK 0x7c00000U
#define SPWRMAP_RTRCFG_AP_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_RTRCFG_AP_MASK ) >> \
    SPWRMAP_RTRCFG_AP_SHIFT )
#define SPWRMAP_RTRCFG_AP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_RTRCFG_AP_MASK ) | \
    ( ( ( _val ) << SPWRMAP_RTRCFG_AP_SHIFT ) & \
      SPWRMAP_RTRCFG_AP_MASK ) )
#define SPWRMAP_RTRCFG_AP( _val ) \
  ( ( ( _val ) << SPWRMAP_RTRCFG_AP_SHIFT ) & \
    SPWRMAP_RTRCFG_AP_MASK )

#define SPWRMAP_RTRCFG_SR 0x8000U

#define SPWRMAP_RTRCFG_PE 0x4000U

#define SPWRMAP_RTRCFG_IC 0x2000U

#define SPWRMAP_RTRCFG_IS 0x1000U

#define SPWRMAP_RTRCFG_IP 0x800U

#define SPWRMAP_RTRCFG_AI 0x400U

#define SPWRMAP_RTRCFG_AT 0x200U

#define SPWRMAP_RTRCFG_IE 0x100U

#define SPWRMAP_RTRCFG_RE 0x80U

#define SPWRMAP_RTRCFG_EE 0x40U

#define SPWRMAP_RTRCFG_SA 0x10U

#define SPWRMAP_RTRCFG_TF 0x8U

#define SPWRMAP_RTRCFG_ME 0x4U

#define SPWRMAP_RTRCFG_TA 0x2U

#define SPWRMAP_RTRCFG_PP 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPTC Time-code (TC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_TC_RE 0x200U

#define SPWRMAP_TC_EN 0x100U

#define SPWRMAP_TC_CF_SHIFT 6
#define SPWRMAP_TC_CF_MASK 0xc0U
#define SPWRMAP_TC_CF_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_TC_CF_MASK ) >> \
    SPWRMAP_TC_CF_SHIFT )
#define SPWRMAP_TC_CF_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_TC_CF_MASK ) | \
    ( ( ( _val ) << SPWRMAP_TC_CF_SHIFT ) & \
      SPWRMAP_TC_CF_MASK ) )
#define SPWRMAP_TC_CF( _val ) \
  ( ( ( _val ) << SPWRMAP_TC_CF_SHIFT ) & \
    SPWRMAP_TC_CF_MASK )

#define SPWRMAP_TC_TC_SHIFT 0
#define SPWRMAP_TC_TC_MASK 0x3fU
#define SPWRMAP_TC_TC_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_TC_TC_MASK ) >> \
    SPWRMAP_TC_TC_SHIFT )
#define SPWRMAP_TC_TC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_TC_TC_MASK ) | \
    ( ( ( _val ) << SPWRMAP_TC_TC_SHIFT ) & \
      SPWRMAP_TC_TC_MASK ) )
#define SPWRMAP_TC_TC( _val ) \
  ( ( ( _val ) << SPWRMAP_TC_TC_SHIFT ) & \
    SPWRMAP_TC_TC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPVER Version / instance ID (VER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_VER_MA_SHIFT 24
#define SPWRMAP_VER_MA_MASK 0xff000000U
#define SPWRMAP_VER_MA_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_VER_MA_MASK ) >> \
    SPWRMAP_VER_MA_SHIFT )
#define SPWRMAP_VER_MA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_VER_MA_MASK ) | \
    ( ( ( _val ) << SPWRMAP_VER_MA_SHIFT ) & \
      SPWRMAP_VER_MA_MASK ) )
#define SPWRMAP_VER_MA( _val ) \
  ( ( ( _val ) << SPWRMAP_VER_MA_SHIFT ) & \
    SPWRMAP_VER_MA_MASK )

#define SPWRMAP_VER_MI_SHIFT 16
#define SPWRMAP_VER_MI_MASK 0xff0000U
#define SPWRMAP_VER_MI_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_VER_MI_MASK ) >> \
    SPWRMAP_VER_MI_SHIFT )
#define SPWRMAP_VER_MI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_VER_MI_MASK ) | \
    ( ( ( _val ) << SPWRMAP_VER_MI_SHIFT ) & \
      SPWRMAP_VER_MI_MASK ) )
#define SPWRMAP_VER_MI( _val ) \
  ( ( ( _val ) << SPWRMAP_VER_MI_SHIFT ) & \
    SPWRMAP_VER_MI_MASK )

#define SPWRMAP_VER_PA_SHIFT 8
#define SPWRMAP_VER_PA_MASK 0xff00U
#define SPWRMAP_VER_PA_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_VER_PA_MASK ) >> \
    SPWRMAP_VER_PA_SHIFT )
#define SPWRMAP_VER_PA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_VER_PA_MASK ) | \
    ( ( ( _val ) << SPWRMAP_VER_PA_SHIFT ) & \
      SPWRMAP_VER_PA_MASK ) )
#define SPWRMAP_VER_PA( _val ) \
  ( ( ( _val ) << SPWRMAP_VER_PA_SHIFT ) & \
    SPWRMAP_VER_PA_MASK )

#define SPWRMAP_VER_ID_SHIFT 0
#define SPWRMAP_VER_ID_MASK 0xffU
#define SPWRMAP_VER_ID_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_VER_ID_MASK ) >> \
    SPWRMAP_VER_ID_SHIFT )
#define SPWRMAP_VER_ID_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_VER_ID_MASK ) | \
    ( ( ( _val ) << SPWRMAP_VER_ID_SHIFT ) & \
      SPWRMAP_VER_ID_MASK ) )
#define SPWRMAP_VER_ID( _val ) \
  ( ( ( _val ) << SPWRMAP_VER_ID_SHIFT ) & \
    SPWRMAP_VER_ID_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPIDIV Initialization divisor (IDIV)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_IDIV_ID_SHIFT 0
#define SPWRMAP_IDIV_ID_MASK 0xffU
#define SPWRMAP_IDIV_ID_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_IDIV_ID_MASK ) >> \
    SPWRMAP_IDIV_ID_SHIFT )
#define SPWRMAP_IDIV_ID_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_IDIV_ID_MASK ) | \
    ( ( ( _val ) << SPWRMAP_IDIV_ID_SHIFT ) & \
      SPWRMAP_IDIV_ID_MASK ) )
#define SPWRMAP_IDIV_ID( _val ) \
  ( ( ( _val ) << SPWRMAP_IDIV_ID_SHIFT ) & \
    SPWRMAP_IDIV_ID_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPCFGWE \
 *   Configuration port write enable (CFGWE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_CFGWE_WE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPRESCALER Timer prescaler reload (PRESCALER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PRESCALER_RL_SHIFT 0
#define SPWRMAP_PRESCALER_RL_MASK 0xffffU
#define SPWRMAP_PRESCALER_RL_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PRESCALER_RL_MASK ) >> \
    SPWRMAP_PRESCALER_RL_SHIFT )
#define SPWRMAP_PRESCALER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PRESCALER_RL_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PRESCALER_RL_SHIFT ) & \
      SPWRMAP_PRESCALER_RL_MASK ) )
#define SPWRMAP_PRESCALER_RL( _val ) \
  ( ( ( _val ) << SPWRMAP_PRESCALER_RL_SHIFT ) & \
    SPWRMAP_PRESCALER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPIMASK Interrupt mask (IMASK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_IMASK_PE 0x400U

#define SPWRMAP_IMASK_SR 0x200U

#define SPWRMAP_IMASK_RS 0x100U

#define SPWRMAP_IMASK_TT 0x80U

#define SPWRMAP_IMASK_PL 0x40U

#define SPWRMAP_IMASK_TS 0x20U

#define SPWRMAP_IMASK_AC 0x10U

#define SPWRMAP_IMASK_RE 0x8U

#define SPWRMAP_IMASK_IA 0x4U

#define SPWRMAP_IMASK_LE 0x2U

#define SPWRMAP_IMASK_ME 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPIPMASK Interrupt port mask (IPMASK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_IPMASK_IE_SHIFT 0
#define SPWRMAP_IPMASK_IE_MASK 0xffffffffU
#define SPWRMAP_IPMASK_IE_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_IPMASK_IE_MASK ) >> \
    SPWRMAP_IPMASK_IE_SHIFT )
#define SPWRMAP_IPMASK_IE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_IPMASK_IE_MASK ) | \
    ( ( ( _val ) << SPWRMAP_IPMASK_IE_SHIFT ) & \
      SPWRMAP_IPMASK_IE_MASK ) )
#define SPWRMAP_IPMASK_IE( _val ) \
  ( ( ( _val ) << SPWRMAP_IPMASK_IE_SHIFT ) & \
    SPWRMAP_IPMASK_IE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPIP Port interrupt pending (PIP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PIP_IP_SHIFT 0
#define SPWRMAP_PIP_IP_MASK 0xffffffffU
#define SPWRMAP_PIP_IP_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PIP_IP_MASK ) >> \
    SPWRMAP_PIP_IP_SHIFT )
#define SPWRMAP_PIP_IP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PIP_IP_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PIP_IP_SHIFT ) & \
      SPWRMAP_PIP_IP_MASK ) )
#define SPWRMAP_PIP_IP( _val ) \
  ( ( ( _val ) << SPWRMAP_PIP_IP_SHIFT ) & \
    SPWRMAP_PIP_IP_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPICODEGEN \
 *   Interrupt code generation (ICODEGEN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_ICODEGEN_HI 0x200000U

#define SPWRMAP_ICODEGEN_UA 0x100000U

#define SPWRMAP_ICODEGEN_AH 0x80000U

#define SPWRMAP_ICODEGEN_IT 0x40000U

#define SPWRMAP_ICODEGEN_TE 0x20000U

#define SPWRMAP_ICODEGEN_EN 0x10000U

#define SPWRMAP_ICODEGEN_IN_SHIFT 0
#define SPWRMAP_ICODEGEN_IN_MASK 0x3fU
#define SPWRMAP_ICODEGEN_IN_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_ICODEGEN_IN_MASK ) >> \
    SPWRMAP_ICODEGEN_IN_SHIFT )
#define SPWRMAP_ICODEGEN_IN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_ICODEGEN_IN_MASK ) | \
    ( ( ( _val ) << SPWRMAP_ICODEGEN_IN_SHIFT ) & \
      SPWRMAP_ICODEGEN_IN_MASK ) )
#define SPWRMAP_ICODEGEN_IN( _val ) \
  ( ( ( _val ) << SPWRMAP_ICODEGEN_IN_SHIFT ) & \
    SPWRMAP_ICODEGEN_IN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPISR0 \
 *   Interrupt code distribution ISR register, interrupt 0-31 (ISR0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_ISR0_IB_SHIFT 0
#define SPWRMAP_ISR0_IB_MASK 0xffffffffU
#define SPWRMAP_ISR0_IB_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_ISR0_IB_MASK ) >> \
    SPWRMAP_ISR0_IB_SHIFT )
#define SPWRMAP_ISR0_IB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_ISR0_IB_MASK ) | \
    ( ( ( _val ) << SPWRMAP_ISR0_IB_SHIFT ) & \
      SPWRMAP_ISR0_IB_MASK ) )
#define SPWRMAP_ISR0_IB( _val ) \
  ( ( ( _val ) << SPWRMAP_ISR0_IB_SHIFT ) & \
    SPWRMAP_ISR0_IB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPISR1 \
 *   Interrupt code distribution ISR register, interrupt 32-63 (ISR1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_ISR1_IB_SHIFT 0
#define SPWRMAP_ISR1_IB_MASK 0xffffffffU
#define SPWRMAP_ISR1_IB_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_ISR1_IB_MASK ) >> \
    SPWRMAP_ISR1_IB_SHIFT )
#define SPWRMAP_ISR1_IB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_ISR1_IB_MASK ) | \
    ( ( ( _val ) << SPWRMAP_ISR1_IB_SHIFT ) & \
      SPWRMAP_ISR1_IB_MASK ) )
#define SPWRMAP_ISR1_IB( _val ) \
  ( ( ( _val ) << SPWRMAP_ISR1_IB_SHIFT ) & \
    SPWRMAP_ISR1_IB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPISRTIMER \
 *   Interrupt code distribution ISR timer reload (ISRTIMER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_ISRTIMER_RL_SHIFT 0
#define SPWRMAP_ISRTIMER_RL_MASK 0xffffU
#define SPWRMAP_ISRTIMER_RL_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_ISRTIMER_RL_MASK ) >> \
    SPWRMAP_ISRTIMER_RL_SHIFT )
#define SPWRMAP_ISRTIMER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_ISRTIMER_RL_MASK ) | \
    ( ( ( _val ) << SPWRMAP_ISRTIMER_RL_SHIFT ) & \
      SPWRMAP_ISRTIMER_RL_MASK ) )
#define SPWRMAP_ISRTIMER_RL( _val ) \
  ( ( ( _val ) << SPWRMAP_ISRTIMER_RL_SHIFT ) & \
    SPWRMAP_ISRTIMER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPAITIMER \
 *   Interrupt code distribution ACK-to-INT timer reload (AITIMER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_AITIMER_RL_SHIFT 0
#define SPWRMAP_AITIMER_RL_MASK 0xffffU
#define SPWRMAP_AITIMER_RL_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_AITIMER_RL_MASK ) >> \
    SPWRMAP_AITIMER_RL_SHIFT )
#define SPWRMAP_AITIMER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_AITIMER_RL_MASK ) | \
    ( ( ( _val ) << SPWRMAP_AITIMER_RL_SHIFT ) & \
      SPWRMAP_AITIMER_RL_MASK ) )
#define SPWRMAP_AITIMER_RL( _val ) \
  ( ( ( _val ) << SPWRMAP_AITIMER_RL_SHIFT ) & \
    SPWRMAP_AITIMER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPISRCTIMER \
 *   Interrupt code distribution ISR change timer reload (ISRCTIMER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_ISRCTIMER_RL_SHIFT 0
#define SPWRMAP_ISRCTIMER_RL_MASK 0x1fU
#define SPWRMAP_ISRCTIMER_RL_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_ISRCTIMER_RL_MASK ) >> \
    SPWRMAP_ISRCTIMER_RL_SHIFT )
#define SPWRMAP_ISRCTIMER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_ISRCTIMER_RL_MASK ) | \
    ( ( ( _val ) << SPWRMAP_ISRCTIMER_RL_SHIFT ) & \
      SPWRMAP_ISRCTIMER_RL_MASK ) )
#define SPWRMAP_ISRCTIMER_RL( _val ) \
  ( ( ( _val ) << SPWRMAP_ISRCTIMER_RL_SHIFT ) & \
    SPWRMAP_ISRCTIMER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPLRUNSTAT Link running status (LRUNSTAT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_LRUNSTAT_LR_SHIFT 1
#define SPWRMAP_LRUNSTAT_LR_MASK 0xfffffffeU
#define SPWRMAP_LRUNSTAT_LR_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_LRUNSTAT_LR_MASK ) >> \
    SPWRMAP_LRUNSTAT_LR_SHIFT )
#define SPWRMAP_LRUNSTAT_LR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_LRUNSTAT_LR_MASK ) | \
    ( ( ( _val ) << SPWRMAP_LRUNSTAT_LR_SHIFT ) & \
      SPWRMAP_LRUNSTAT_LR_MASK ) )
#define SPWRMAP_LRUNSTAT_LR( _val ) \
  ( ( ( _val ) << SPWRMAP_LRUNSTAT_LR_SHIFT ) & \
    SPWRMAP_LRUNSTAT_LR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPCAP Capability (CAP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_CAP_AF_SHIFT 24
#define SPWRMAP_CAP_AF_MASK 0x3000000U
#define SPWRMAP_CAP_AF_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_CAP_AF_MASK ) >> \
    SPWRMAP_CAP_AF_SHIFT )
#define SPWRMAP_CAP_AF_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_CAP_AF_MASK ) | \
    ( ( ( _val ) << SPWRMAP_CAP_AF_SHIFT ) & \
      SPWRMAP_CAP_AF_MASK ) )
#define SPWRMAP_CAP_AF( _val ) \
  ( ( ( _val ) << SPWRMAP_CAP_AF_SHIFT ) & \
    SPWRMAP_CAP_AF_MASK )

#define SPWRMAP_CAP_PF_SHIFT 20
#define SPWRMAP_CAP_PF_MASK 0x700000U
#define SPWRMAP_CAP_PF_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_CAP_PF_MASK ) >> \
    SPWRMAP_CAP_PF_SHIFT )
#define SPWRMAP_CAP_PF_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_CAP_PF_MASK ) | \
    ( ( ( _val ) << SPWRMAP_CAP_PF_SHIFT ) & \
      SPWRMAP_CAP_PF_MASK ) )
#define SPWRMAP_CAP_PF( _val ) \
  ( ( ( _val ) << SPWRMAP_CAP_PF_SHIFT ) & \
    SPWRMAP_CAP_PF_MASK )

#define SPWRMAP_CAP_RM_SHIFT 16
#define SPWRMAP_CAP_RM_MASK 0x70000U
#define SPWRMAP_CAP_RM_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_CAP_RM_MASK ) >> \
    SPWRMAP_CAP_RM_SHIFT )
#define SPWRMAP_CAP_RM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_CAP_RM_MASK ) | \
    ( ( ( _val ) << SPWRMAP_CAP_RM_SHIFT ) & \
      SPWRMAP_CAP_RM_MASK ) )
#define SPWRMAP_CAP_RM( _val ) \
  ( ( ( _val ) << SPWRMAP_CAP_RM_SHIFT ) & \
    SPWRMAP_CAP_RM_MASK )

#define SPWRMAP_CAP_AS 0x4000U

#define SPWRMAP_CAP_AX 0x2000U

#define SPWRMAP_CAP_DP 0x1000U

#define SPWRMAP_CAP_ID 0x800U

#define SPWRMAP_CAP_SD 0x400U

#define SPWRMAP_CAP_PC_SHIFT 5
#define SPWRMAP_CAP_PC_MASK 0x3e0U
#define SPWRMAP_CAP_PC_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_CAP_PC_MASK ) >> \
    SPWRMAP_CAP_PC_SHIFT )
#define SPWRMAP_CAP_PC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_CAP_PC_MASK ) | \
    ( ( ( _val ) << SPWRMAP_CAP_PC_SHIFT ) & \
      SPWRMAP_CAP_PC_MASK ) )
#define SPWRMAP_CAP_PC( _val ) \
  ( ( ( _val ) << SPWRMAP_CAP_PC_SHIFT ) & \
    SPWRMAP_CAP_PC_MASK )

#define SPWRMAP_CAP_CC_SHIFT 0
#define SPWRMAP_CAP_CC_MASK 0x1fU
#define SPWRMAP_CAP_CC_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_CAP_CC_MASK ) >> \
    SPWRMAP_CAP_CC_SHIFT )
#define SPWRMAP_CAP_CC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_CAP_CC_MASK ) | \
    ( ( ( _val ) << SPWRMAP_CAP_CC_SHIFT ) & \
      SPWRMAP_CAP_CC_MASK ) )
#define SPWRMAP_CAP_CC( _val ) \
  ( ( ( _val ) << SPWRMAP_CAP_CC_SHIFT ) & \
    SPWRMAP_CAP_CC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPNPVEND \
 *   SpaceWire Plug-and-Play - Device Vendor and Product ID (PNPVEND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PNPVEND_VI_SHIFT 16
#define SPWRMAP_PNPVEND_VI_MASK 0xffff0000U
#define SPWRMAP_PNPVEND_VI_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PNPVEND_VI_MASK ) >> \
    SPWRMAP_PNPVEND_VI_SHIFT )
#define SPWRMAP_PNPVEND_VI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PNPVEND_VI_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PNPVEND_VI_SHIFT ) & \
      SPWRMAP_PNPVEND_VI_MASK ) )
#define SPWRMAP_PNPVEND_VI( _val ) \
  ( ( ( _val ) << SPWRMAP_PNPVEND_VI_SHIFT ) & \
    SPWRMAP_PNPVEND_VI_MASK )

#define SPWRMAP_PNPVEND_PI_SHIFT 0
#define SPWRMAP_PNPVEND_PI_MASK 0xffffU
#define SPWRMAP_PNPVEND_PI_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PNPVEND_PI_MASK ) >> \
    SPWRMAP_PNPVEND_PI_SHIFT )
#define SPWRMAP_PNPVEND_PI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PNPVEND_PI_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PNPVEND_PI_SHIFT ) & \
      SPWRMAP_PNPVEND_PI_MASK ) )
#define SPWRMAP_PNPVEND_PI( _val ) \
  ( ( ( _val ) << SPWRMAP_PNPVEND_PI_SHIFT ) & \
    SPWRMAP_PNPVEND_PI_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPNPUVEND \
 *   SpaceWire Plug-and-Play - Unit Vendor and Product ID (PNPUVEND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PNPUVEND_VI_SHIFT 16
#define SPWRMAP_PNPUVEND_VI_MASK 0xffff0000U
#define SPWRMAP_PNPUVEND_VI_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PNPUVEND_VI_MASK ) >> \
    SPWRMAP_PNPUVEND_VI_SHIFT )
#define SPWRMAP_PNPUVEND_VI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PNPUVEND_VI_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PNPUVEND_VI_SHIFT ) & \
      SPWRMAP_PNPUVEND_VI_MASK ) )
#define SPWRMAP_PNPUVEND_VI( _val ) \
  ( ( ( _val ) << SPWRMAP_PNPUVEND_VI_SHIFT ) & \
    SPWRMAP_PNPUVEND_VI_MASK )

#define SPWRMAP_PNPUVEND_PI_SHIFT 0
#define SPWRMAP_PNPUVEND_PI_MASK 0xffffU
#define SPWRMAP_PNPUVEND_PI_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PNPUVEND_PI_MASK ) >> \
    SPWRMAP_PNPUVEND_PI_SHIFT )
#define SPWRMAP_PNPUVEND_PI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PNPUVEND_PI_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PNPUVEND_PI_SHIFT ) & \
      SPWRMAP_PNPUVEND_PI_MASK ) )
#define SPWRMAP_PNPUVEND_PI( _val ) \
  ( ( ( _val ) << SPWRMAP_PNPUVEND_PI_SHIFT ) & \
    SPWRMAP_PNPUVEND_PI_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPPNPUSN \
 *   SpaceWire Plug-and-Play - Unit Serial Number (PNPUSN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_PNPUSN_SN_SHIFT 0
#define SPWRMAP_PNPUSN_SN_MASK 0xffffffffU
#define SPWRMAP_PNPUSN_SN_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_PNPUSN_SN_MASK ) >> \
    SPWRMAP_PNPUSN_SN_SHIFT )
#define SPWRMAP_PNPUSN_SN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_PNPUSN_SN_MASK ) | \
    ( ( ( _val ) << SPWRMAP_PNPUSN_SN_SHIFT ) & \
      SPWRMAP_PNPUSN_SN_MASK ) )
#define SPWRMAP_PNPUSN_SN( _val ) \
  ( ( ( _val ) << SPWRMAP_PNPUSN_SN_SHIFT ) & \
    SPWRMAP_PNPUSN_SN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPMAXPLEN \
 *   Maximum packet length, ports 0-12 (MAXPLEN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_MAXPLEN_ML_SHIFT 0
#define SPWRMAP_MAXPLEN_ML_MASK 0x1ffffffU
#define SPWRMAP_MAXPLEN_ML_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_MAXPLEN_ML_MASK ) >> \
    SPWRMAP_MAXPLEN_ML_SHIFT )
#define SPWRMAP_MAXPLEN_ML_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_MAXPLEN_ML_MASK ) | \
    ( ( ( _val ) << SPWRMAP_MAXPLEN_ML_SHIFT ) & \
      SPWRMAP_MAXPLEN_ML_MASK ) )
#define SPWRMAP_MAXPLEN_ML( _val ) \
  ( ( ( _val ) << SPWRMAP_MAXPLEN_ML_SHIFT ) & \
    SPWRMAP_MAXPLEN_ML_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPCREDCNT Credit counter, ports 1-8 (CREDCNT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_CREDCNT_OC_SHIFT 6
#define SPWRMAP_CREDCNT_OC_MASK 0xfc0U
#define SPWRMAP_CREDCNT_OC_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_CREDCNT_OC_MASK ) >> \
    SPWRMAP_CREDCNT_OC_SHIFT )
#define SPWRMAP_CREDCNT_OC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_CREDCNT_OC_MASK ) | \
    ( ( ( _val ) << SPWRMAP_CREDCNT_OC_SHIFT ) & \
      SPWRMAP_CREDCNT_OC_MASK ) )
#define SPWRMAP_CREDCNT_OC( _val ) \
  ( ( ( _val ) << SPWRMAP_CREDCNT_OC_SHIFT ) & \
    SPWRMAP_CREDCNT_OC_MASK )

#define SPWRMAP_CREDCNT_IC_SHIFT 0
#define SPWRMAP_CREDCNT_IC_MASK 0x3fU
#define SPWRMAP_CREDCNT_IC_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_CREDCNT_IC_MASK ) >> \
    SPWRMAP_CREDCNT_IC_SHIFT )
#define SPWRMAP_CREDCNT_IC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_CREDCNT_IC_MASK ) | \
    ( ( ( _val ) << SPWRMAP_CREDCNT_IC_SHIFT ) & \
      SPWRMAP_CREDCNT_IC_MASK ) )
#define SPWRMAP_CREDCNT_IC( _val ) \
  ( ( ( _val ) << SPWRMAP_CREDCNT_IC_SHIFT ) & \
    SPWRMAP_CREDCNT_IC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWMAPRTCOMB \
 *   Routing table, combined port mapping and address control, addresses 1-255 (RTCOMB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWRMAP_RTCOMB_SR 0x80000000U

#define SPWRMAP_RTCOMB_EN 0x40000000U

#define SPWRMAP_RTCOMB_PR 0x20000000U

#define SPWRMAP_RTCOMB_HD 0x10000000U

#define SPWRMAP_RTCOMB_PE_SHIFT 1
#define SPWRMAP_RTCOMB_PE_MASK 0xffffeU
#define SPWRMAP_RTCOMB_PE_GET( _reg ) \
  ( ( ( _reg ) & SPWRMAP_RTCOMB_PE_MASK ) >> \
    SPWRMAP_RTCOMB_PE_SHIFT )
#define SPWRMAP_RTCOMB_PE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWRMAP_RTCOMB_PE_MASK ) | \
    ( ( ( _val ) << SPWRMAP_RTCOMB_PE_SHIFT ) & \
      SPWRMAP_RTCOMB_PE_MASK ) )
#define SPWRMAP_RTCOMB_PE( _val ) \
  ( ( ( _val ) << SPWRMAP_RTCOMB_PE_SHIFT ) & \
    SPWRMAP_RTCOMB_PE_MASK )

#define SPWRMAP_RTCOMB_PD 0x1U

/** @} */

/**
 * @brief This set of defines the SpaceWire Remote Memory Access Protocol
 *   (RMAP) address map.
 */
typedef struct spwrmap {
  uint32_t reserved_0_4;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPRTPMAP.
   */
  uint32_t rtpmap;

  uint32_t reserved_8_404[ 255 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPRTACTRL.
   */
  uint32_t rtactrl;

  uint32_t reserved_408_800[ 254 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPCTRLCFG.
   */
  uint32_t pctrlcfg;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPCTRL.
   */
  uint32_t pctrl;

  uint32_t reserved_808_880[ 30 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPSTSCFG.
   */
  uint32_t pstscfg;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPSTS.
   */
  uint32_t psts;

  uint32_t reserved_888_900[ 30 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPTIMER.
   */
  uint32_t ptimer;

  uint32_t reserved_904_980[ 31 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPCTRL2CFG.
   */
  uint32_t pctrl2cfg;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPCTRL2.
   */
  uint32_t pctrl2;

  uint32_t reserved_988_a00[ 30 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPRTRCFG.
   */
  uint32_t rtrcfg;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPTC.
   */
  uint32_t tc;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPVER.
   */
  uint32_t ver;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPIDIV.
   */
  uint32_t idiv;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPCFGWE.
   */
  uint32_t cfgwe;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPRESCALER.
   */
  uint32_t prescaler;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPIMASK.
   */
  uint32_t imask;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPIPMASK.
   */
  uint32_t ipmask;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPIP.
   */
  uint32_t pip;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPICODEGEN.
   */
  uint32_t icodegen;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPISR0.
   */
  uint32_t isr0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPISR1.
   */
  uint32_t isr1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPISRTIMER.
   */
  uint32_t isrtimer;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPAITIMER.
   */
  uint32_t aitimer;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPISRCTIMER.
   */
  uint32_t isrctimer;

  uint32_t reserved_a3c_a40;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPLRUNSTAT.
   */
  uint32_t lrunstat;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPCAP.
   */
  uint32_t cap;

  uint32_t reserved_a48_a50[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPNPVEND.
   */
  uint32_t pnpvend;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPNPUVEND.
   */
  uint32_t pnpuvend;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPPNPUSN.
   */
  uint32_t pnpusn;

  uint32_t reserved_a5c_e00[ 233 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPMAXPLEN.
   */
  uint32_t maxplen;

  uint32_t reserved_e04_e84[ 32 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPCREDCNT.
   */
  uint32_t credcnt;

  uint32_t reserved_e88_1004[ 95 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWMAPRTCOMB.
   */
  uint32_t rtcomb;
} spwrmap;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_SPWRMAP_REGS_H */
