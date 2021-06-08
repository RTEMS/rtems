/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGR1553B
 *
 * @brief This header file defines the GR1553B register block interface.
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

/* Generated from spec:/dev/grlib/if/gr1553b-header */

#ifndef _GRLIB_GR1553B_REGS_H
#define _GRLIB_GR1553B_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/gr1553b */

/**
 * @defgroup RTEMSDeviceGR1553B GR1553B
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GR1553B interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGR1553BIRQ GR1553B IRQ Register (IRQ)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_IRQ_BMTOF 0x20000U

#define GR1553B_IRQ_BMD 0x10000U

#define GR1553B_IRQ_RTTE 0x400U

#define GR1553B_IRQ_RTD 0x200U

#define GR1553B_IRQ_RTEV 0x100U

#define GR1553B_IRQ_BCWK 0x4U

#define GR1553B_IRQ_BCD 0x2U

#define GR1553B_IRQ_BCEV 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BIRQE GR1553B IRQ Enable Register (IRQE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_IRQE_BMTOE 0x20000U

#define GR1553B_IRQE_BMDE 0x10000U

#define GR1553B_IRQE_RTTEE 0x400U

#define GR1553B_IRQE_RTDE 0x200U

#define GR1553B_IRQE_RTEVE 0x100U

#define GR1553B_IRQE_BCWKE 0x4U

#define GR1553B_IRQE_BCDE 0x2U

#define GR1553B_IRQE_BCEVE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BHC GR1553B Hardware Configuration Register (HC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_HC_MOD 0x80000000U

#define GR1553B_HC_CVER 0x1000U

#define GR1553B_HC_XKEYS 0x800U

#define GR1553B_HC_ENDIAN_SHIFT 9
#define GR1553B_HC_ENDIAN_MASK 0x600U
#define GR1553B_HC_ENDIAN_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_HC_ENDIAN_MASK ) >> \
    GR1553B_HC_ENDIAN_SHIFT )
#define GR1553B_HC_ENDIAN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_HC_ENDIAN_MASK ) | \
    ( ( ( _val ) << GR1553B_HC_ENDIAN_SHIFT ) & \
      GR1553B_HC_ENDIAN_MASK ) )
#define GR1553B_HC_ENDIAN( _val ) \
  ( ( ( _val ) << GR1553B_HC_ENDIAN_SHIFT ) & \
    GR1553B_HC_ENDIAN_MASK )

#define GR1553B_HC_SCLK 0x100U

#define GR1553B_HC_CCFREQ_SHIFT 0
#define GR1553B_HC_CCFREQ_MASK 0xffU
#define GR1553B_HC_CCFREQ_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_HC_CCFREQ_MASK ) >> \
    GR1553B_HC_CCFREQ_SHIFT )
#define GR1553B_HC_CCFREQ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_HC_CCFREQ_MASK ) | \
    ( ( ( _val ) << GR1553B_HC_CCFREQ_SHIFT ) & \
      GR1553B_HC_CCFREQ_MASK ) )
#define GR1553B_HC_CCFREQ( _val ) \
  ( ( ( _val ) << GR1553B_HC_CCFREQ_SHIFT ) & \
    GR1553B_HC_CCFREQ_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBCSC \
 *   GR1553B BC Status and Config Register (BCSC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BCSC_BCSUP 0x80000000U

#define GR1553B_BCSC_BCFEAT_SHIFT 28
#define GR1553B_BCSC_BCFEAT_MASK 0x70000000U
#define GR1553B_BCSC_BCFEAT_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCSC_BCFEAT_MASK ) >> \
    GR1553B_BCSC_BCFEAT_SHIFT )
#define GR1553B_BCSC_BCFEAT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCSC_BCFEAT_MASK ) | \
    ( ( ( _val ) << GR1553B_BCSC_BCFEAT_SHIFT ) & \
      GR1553B_BCSC_BCFEAT_MASK ) )
#define GR1553B_BCSC_BCFEAT( _val ) \
  ( ( ( _val ) << GR1553B_BCSC_BCFEAT_SHIFT ) & \
    GR1553B_BCSC_BCFEAT_MASK )

#define GR1553B_BCSC_BCCHK 0x10000U

#define GR1553B_BCSC_ASADL_SHIFT 11
#define GR1553B_BCSC_ASADL_MASK 0xf800U
#define GR1553B_BCSC_ASADL_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCSC_ASADL_MASK ) >> \
    GR1553B_BCSC_ASADL_SHIFT )
#define GR1553B_BCSC_ASADL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCSC_ASADL_MASK ) | \
    ( ( ( _val ) << GR1553B_BCSC_ASADL_SHIFT ) & \
      GR1553B_BCSC_ASADL_MASK ) )
#define GR1553B_BCSC_ASADL( _val ) \
  ( ( ( _val ) << GR1553B_BCSC_ASADL_SHIFT ) & \
    GR1553B_BCSC_ASADL_MASK )

#define GR1553B_BCSC_ASST_SHIFT 8
#define GR1553B_BCSC_ASST_MASK 0x300U
#define GR1553B_BCSC_ASST_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCSC_ASST_MASK ) >> \
    GR1553B_BCSC_ASST_SHIFT )
#define GR1553B_BCSC_ASST_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCSC_ASST_MASK ) | \
    ( ( ( _val ) << GR1553B_BCSC_ASST_SHIFT ) & \
      GR1553B_BCSC_ASST_MASK ) )
#define GR1553B_BCSC_ASST( _val ) \
  ( ( ( _val ) << GR1553B_BCSC_ASST_SHIFT ) & \
    GR1553B_BCSC_ASST_MASK )

#define GR1553B_BCSC_SCADL_SHIFT 3
#define GR1553B_BCSC_SCADL_MASK 0xf8U
#define GR1553B_BCSC_SCADL_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCSC_SCADL_MASK ) >> \
    GR1553B_BCSC_SCADL_SHIFT )
#define GR1553B_BCSC_SCADL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCSC_SCADL_MASK ) | \
    ( ( ( _val ) << GR1553B_BCSC_SCADL_SHIFT ) & \
      GR1553B_BCSC_SCADL_MASK ) )
#define GR1553B_BCSC_SCADL( _val ) \
  ( ( ( _val ) << GR1553B_BCSC_SCADL_SHIFT ) & \
    GR1553B_BCSC_SCADL_MASK )

#define GR1553B_BCSC_SCST_SHIFT 0
#define GR1553B_BCSC_SCST_MASK 0x7U
#define GR1553B_BCSC_SCST_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCSC_SCST_MASK ) >> \
    GR1553B_BCSC_SCST_SHIFT )
#define GR1553B_BCSC_SCST_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCSC_SCST_MASK ) | \
    ( ( ( _val ) << GR1553B_BCSC_SCST_SHIFT ) & \
      GR1553B_BCSC_SCST_MASK ) )
#define GR1553B_BCSC_SCST( _val ) \
  ( ( ( _val ) << GR1553B_BCSC_SCST_SHIFT ) & \
    GR1553B_BCSC_SCST_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBCA GR1553B BC Action Register (BCA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BCA_BCKEY_SHIFT 16
#define GR1553B_BCA_BCKEY_MASK 0xffff0000U
#define GR1553B_BCA_BCKEY_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCA_BCKEY_MASK ) >> \
    GR1553B_BCA_BCKEY_SHIFT )
#define GR1553B_BCA_BCKEY_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCA_BCKEY_MASK ) | \
    ( ( ( _val ) << GR1553B_BCA_BCKEY_SHIFT ) & \
      GR1553B_BCA_BCKEY_MASK ) )
#define GR1553B_BCA_BCKEY( _val ) \
  ( ( ( _val ) << GR1553B_BCA_BCKEY_SHIFT ) & \
    GR1553B_BCA_BCKEY_MASK )

#define GR1553B_BCA_ASSTP 0x200U

#define GR1553B_BCA_ASSRT 0x100U

#define GR1553B_BCA_CLRT 0x10U

#define GR1553B_BCA_SETT 0x8U

#define GR1553B_BCA_SCSTP 0x4U

#define GR1553B_BCA_SCSUS 0x2U

#define GR1553B_BCA_SCSRT 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBCTNP \
 *   GR1553B BC Transfer list next pointer register (BCTNP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BCTNP_POINTER_SHIFT 0
#define GR1553B_BCTNP_POINTER_MASK 0xffffffffU
#define GR1553B_BCTNP_POINTER_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCTNP_POINTER_MASK ) >> \
    GR1553B_BCTNP_POINTER_SHIFT )
#define GR1553B_BCTNP_POINTER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCTNP_POINTER_MASK ) | \
    ( ( ( _val ) << GR1553B_BCTNP_POINTER_SHIFT ) & \
      GR1553B_BCTNP_POINTER_MASK ) )
#define GR1553B_BCTNP_POINTER( _val ) \
  ( ( ( _val ) << GR1553B_BCTNP_POINTER_SHIFT ) & \
    GR1553B_BCTNP_POINTER_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBCANP \
 *   GR1553B BC Asynchronous list next pointer register (BCANP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BCANP_POINTER_SHIFT 0
#define GR1553B_BCANP_POINTER_MASK 0xffffffffU
#define GR1553B_BCANP_POINTER_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCANP_POINTER_MASK ) >> \
    GR1553B_BCANP_POINTER_SHIFT )
#define GR1553B_BCANP_POINTER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCANP_POINTER_MASK ) | \
    ( ( ( _val ) << GR1553B_BCANP_POINTER_SHIFT ) & \
      GR1553B_BCANP_POINTER_MASK ) )
#define GR1553B_BCANP_POINTER( _val ) \
  ( ( ( _val ) << GR1553B_BCANP_POINTER_SHIFT ) & \
    GR1553B_BCANP_POINTER_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBCT GR1553B BC Timer register (BCT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BCT_SCTM_SHIFT 0
#define GR1553B_BCT_SCTM_MASK 0xffffffU
#define GR1553B_BCT_SCTM_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCT_SCTM_MASK ) >> \
    GR1553B_BCT_SCTM_SHIFT )
#define GR1553B_BCT_SCTM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCT_SCTM_MASK ) | \
    ( ( ( _val ) << GR1553B_BCT_SCTM_SHIFT ) & \
      GR1553B_BCT_SCTM_MASK ) )
#define GR1553B_BCT_SCTM( _val ) \
  ( ( ( _val ) << GR1553B_BCT_SCTM_SHIFT ) & \
    GR1553B_BCT_SCTM_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBCRP \
 *   GR1553B BC Transfer-triggered IRQ ring position register (BCRP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BCRP_POSITION_SHIFT 0
#define GR1553B_BCRP_POSITION_MASK 0xffffffffU
#define GR1553B_BCRP_POSITION_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCRP_POSITION_MASK ) >> \
    GR1553B_BCRP_POSITION_SHIFT )
#define GR1553B_BCRP_POSITION_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCRP_POSITION_MASK ) | \
    ( ( ( _val ) << GR1553B_BCRP_POSITION_SHIFT ) & \
      GR1553B_BCRP_POSITION_MASK ) )
#define GR1553B_BCRP_POSITION( _val ) \
  ( ( ( _val ) << GR1553B_BCRP_POSITION_SHIFT ) & \
    GR1553B_BCRP_POSITION_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBCBS GR1553B BC per-RT Bus swap register (BCBS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BCBS_SWAP_SHIFT 0
#define GR1553B_BCBS_SWAP_MASK 0xffffffffU
#define GR1553B_BCBS_SWAP_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCBS_SWAP_MASK ) >> \
    GR1553B_BCBS_SWAP_SHIFT )
#define GR1553B_BCBS_SWAP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCBS_SWAP_MASK ) | \
    ( ( ( _val ) << GR1553B_BCBS_SWAP_SHIFT ) & \
      GR1553B_BCBS_SWAP_MASK ) )
#define GR1553B_BCBS_SWAP( _val ) \
  ( ( ( _val ) << GR1553B_BCBS_SWAP_SHIFT ) & \
    GR1553B_BCBS_SWAP_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBCTCP \
 *   GR1553B BC Transfer list current slot pointer (BCTCP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BCTCP_POINTER_SHIFT 0
#define GR1553B_BCTCP_POINTER_MASK 0xffffffffU
#define GR1553B_BCTCP_POINTER_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCTCP_POINTER_MASK ) >> \
    GR1553B_BCTCP_POINTER_SHIFT )
#define GR1553B_BCTCP_POINTER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCTCP_POINTER_MASK ) | \
    ( ( ( _val ) << GR1553B_BCTCP_POINTER_SHIFT ) & \
      GR1553B_BCTCP_POINTER_MASK ) )
#define GR1553B_BCTCP_POINTER( _val ) \
  ( ( ( _val ) << GR1553B_BCTCP_POINTER_SHIFT ) & \
    GR1553B_BCTCP_POINTER_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBCACP \
 *   GR1553B BC Asynchronous list current slot pointer (BCACP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BCACP_POINTER_SHIFT 0
#define GR1553B_BCACP_POINTER_MASK 0xffffffffU
#define GR1553B_BCACP_POINTER_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BCACP_POINTER_MASK ) >> \
    GR1553B_BCACP_POINTER_SHIFT )
#define GR1553B_BCACP_POINTER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BCACP_POINTER_MASK ) | \
    ( ( ( _val ) << GR1553B_BCACP_POINTER_SHIFT ) & \
      GR1553B_BCACP_POINTER_MASK ) )
#define GR1553B_BCACP_POINTER( _val ) \
  ( ( ( _val ) << GR1553B_BCACP_POINTER_SHIFT ) & \
    GR1553B_BCACP_POINTER_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BRTS GR1553B RT Status register (RTS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_RTS_RTSUP 0x80000000U

#define GR1553B_RTS_ACT 0x8U

#define GR1553B_RTS_SHDA 0x4U

#define GR1553B_RTS_SHDB 0x2U

#define GR1553B_RTS_RUN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BRTC GR1553B RT Config register (RTC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_RTC_RTKEY_SHIFT 16
#define GR1553B_RTC_RTKEY_MASK 0xffff0000U
#define GR1553B_RTC_RTKEY_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTC_RTKEY_MASK ) >> \
    GR1553B_RTC_RTKEY_SHIFT )
#define GR1553B_RTC_RTKEY_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTC_RTKEY_MASK ) | \
    ( ( ( _val ) << GR1553B_RTC_RTKEY_SHIFT ) & \
      GR1553B_RTC_RTKEY_MASK ) )
#define GR1553B_RTC_RTKEY( _val ) \
  ( ( ( _val ) << GR1553B_RTC_RTKEY_SHIFT ) & \
    GR1553B_RTC_RTKEY_MASK )

#define GR1553B_RTC_SYS 0x8000U

#define GR1553B_RTC_SYDS 0x4000U

#define GR1553B_RTC_BRS 0x2000U

#define GR1553B_RTC_RTEIS 0x40U

#define GR1553B_RTC_RTADDR_SHIFT 1
#define GR1553B_RTC_RTADDR_MASK 0x3eU
#define GR1553B_RTC_RTADDR_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTC_RTADDR_MASK ) >> \
    GR1553B_RTC_RTADDR_SHIFT )
#define GR1553B_RTC_RTADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTC_RTADDR_MASK ) | \
    ( ( ( _val ) << GR1553B_RTC_RTADDR_SHIFT ) & \
      GR1553B_RTC_RTADDR_MASK ) )
#define GR1553B_RTC_RTADDR( _val ) \
  ( ( ( _val ) << GR1553B_RTC_RTADDR_SHIFT ) & \
    GR1553B_RTC_RTADDR_MASK )

#define GR1553B_RTC_RTEN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BRTBS GR1553B RT Bus status register (RTBS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_RTBS_TFDE 0x100U

#define GR1553B_RTBS_SREQ 0x10U

#define GR1553B_RTBS_BUSY 0x8U

#define GR1553B_RTBS_SSF 0x4U

#define GR1553B_RTBS_DBCA 0x2U

#define GR1553B_RTBS_TFLG 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BRTSW GR1553B RT Status words register (RTSW)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_RTSW_BITW_SHIFT 16
#define GR1553B_RTSW_BITW_MASK 0xffff0000U
#define GR1553B_RTSW_BITW_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTSW_BITW_MASK ) >> \
    GR1553B_RTSW_BITW_SHIFT )
#define GR1553B_RTSW_BITW_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTSW_BITW_MASK ) | \
    ( ( ( _val ) << GR1553B_RTSW_BITW_SHIFT ) & \
      GR1553B_RTSW_BITW_MASK ) )
#define GR1553B_RTSW_BITW( _val ) \
  ( ( ( _val ) << GR1553B_RTSW_BITW_SHIFT ) & \
    GR1553B_RTSW_BITW_MASK )

#define GR1553B_RTSW_VECW_SHIFT 0
#define GR1553B_RTSW_VECW_MASK 0xffffU
#define GR1553B_RTSW_VECW_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTSW_VECW_MASK ) >> \
    GR1553B_RTSW_VECW_SHIFT )
#define GR1553B_RTSW_VECW_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTSW_VECW_MASK ) | \
    ( ( ( _val ) << GR1553B_RTSW_VECW_SHIFT ) & \
      GR1553B_RTSW_VECW_MASK ) )
#define GR1553B_RTSW_VECW( _val ) \
  ( ( ( _val ) << GR1553B_RTSW_VECW_SHIFT ) & \
    GR1553B_RTSW_VECW_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BRTSY GR1553B RT Sync register (RTSY)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_RTSY_SYTM_SHIFT 16
#define GR1553B_RTSY_SYTM_MASK 0xffff0000U
#define GR1553B_RTSY_SYTM_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTSY_SYTM_MASK ) >> \
    GR1553B_RTSY_SYTM_SHIFT )
#define GR1553B_RTSY_SYTM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTSY_SYTM_MASK ) | \
    ( ( ( _val ) << GR1553B_RTSY_SYTM_SHIFT ) & \
      GR1553B_RTSY_SYTM_MASK ) )
#define GR1553B_RTSY_SYTM( _val ) \
  ( ( ( _val ) << GR1553B_RTSY_SYTM_SHIFT ) & \
    GR1553B_RTSY_SYTM_MASK )

#define GR1553B_RTSY_SYD_SHIFT 0
#define GR1553B_RTSY_SYD_MASK 0xffffU
#define GR1553B_RTSY_SYD_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTSY_SYD_MASK ) >> \
    GR1553B_RTSY_SYD_SHIFT )
#define GR1553B_RTSY_SYD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTSY_SYD_MASK ) | \
    ( ( ( _val ) << GR1553B_RTSY_SYD_SHIFT ) & \
      GR1553B_RTSY_SYD_MASK ) )
#define GR1553B_RTSY_SYD( _val ) \
  ( ( ( _val ) << GR1553B_RTSY_SYD_SHIFT ) & \
    GR1553B_RTSY_SYD_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BRTSTBA \
 *   GR1553B RT Subaddress table base address register (RTSTBA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_RTSTBA_SATB_SHIFT 9
#define GR1553B_RTSTBA_SATB_MASK 0xfffffe00U
#define GR1553B_RTSTBA_SATB_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTSTBA_SATB_MASK ) >> \
    GR1553B_RTSTBA_SATB_SHIFT )
#define GR1553B_RTSTBA_SATB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTSTBA_SATB_MASK ) | \
    ( ( ( _val ) << GR1553B_RTSTBA_SATB_SHIFT ) & \
      GR1553B_RTSTBA_SATB_MASK ) )
#define GR1553B_RTSTBA_SATB( _val ) \
  ( ( ( _val ) << GR1553B_RTSTBA_SATB_SHIFT ) & \
    GR1553B_RTSTBA_SATB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BRTMCC \
 *   GR1553B RT Mode code control register (RTMCC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_RTMCC_RRTB_SHIFT 28
#define GR1553B_RTMCC_RRTB_MASK 0x30000000U
#define GR1553B_RTMCC_RRTB_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_RRTB_MASK ) >> \
    GR1553B_RTMCC_RRTB_SHIFT )
#define GR1553B_RTMCC_RRTB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_RRTB_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_RRTB_SHIFT ) & \
      GR1553B_RTMCC_RRTB_MASK ) )
#define GR1553B_RTMCC_RRTB( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_RRTB_SHIFT ) & \
    GR1553B_RTMCC_RRTB_MASK )

#define GR1553B_RTMCC_RRT_SHIFT 26
#define GR1553B_RTMCC_RRT_MASK 0xc000000U
#define GR1553B_RTMCC_RRT_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_RRT_MASK ) >> \
    GR1553B_RTMCC_RRT_SHIFT )
#define GR1553B_RTMCC_RRT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_RRT_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_RRT_SHIFT ) & \
      GR1553B_RTMCC_RRT_MASK ) )
#define GR1553B_RTMCC_RRT( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_RRT_SHIFT ) & \
    GR1553B_RTMCC_RRT_MASK )

#define GR1553B_RTMCC_ITFB_SHIFT 24
#define GR1553B_RTMCC_ITFB_MASK 0x3000000U
#define GR1553B_RTMCC_ITFB_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_ITFB_MASK ) >> \
    GR1553B_RTMCC_ITFB_SHIFT )
#define GR1553B_RTMCC_ITFB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_ITFB_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_ITFB_SHIFT ) & \
      GR1553B_RTMCC_ITFB_MASK ) )
#define GR1553B_RTMCC_ITFB( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_ITFB_SHIFT ) & \
    GR1553B_RTMCC_ITFB_MASK )

#define GR1553B_RTMCC_ITF_SHIFT 22
#define GR1553B_RTMCC_ITF_MASK 0xc00000U
#define GR1553B_RTMCC_ITF_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_ITF_MASK ) >> \
    GR1553B_RTMCC_ITF_SHIFT )
#define GR1553B_RTMCC_ITF_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_ITF_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_ITF_SHIFT ) & \
      GR1553B_RTMCC_ITF_MASK ) )
#define GR1553B_RTMCC_ITF( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_ITF_SHIFT ) & \
    GR1553B_RTMCC_ITF_MASK )

#define GR1553B_RTMCC_ISTB_SHIFT 20
#define GR1553B_RTMCC_ISTB_MASK 0x300000U
#define GR1553B_RTMCC_ISTB_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_ISTB_MASK ) >> \
    GR1553B_RTMCC_ISTB_SHIFT )
#define GR1553B_RTMCC_ISTB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_ISTB_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_ISTB_SHIFT ) & \
      GR1553B_RTMCC_ISTB_MASK ) )
#define GR1553B_RTMCC_ISTB( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_ISTB_SHIFT ) & \
    GR1553B_RTMCC_ISTB_MASK )

#define GR1553B_RTMCC_IST_SHIFT 18
#define GR1553B_RTMCC_IST_MASK 0xc0000U
#define GR1553B_RTMCC_IST_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_IST_MASK ) >> \
    GR1553B_RTMCC_IST_SHIFT )
#define GR1553B_RTMCC_IST_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_IST_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_IST_SHIFT ) & \
      GR1553B_RTMCC_IST_MASK ) )
#define GR1553B_RTMCC_IST( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_IST_SHIFT ) & \
    GR1553B_RTMCC_IST_MASK )

#define GR1553B_RTMCC_DBC_SHIFT 16
#define GR1553B_RTMCC_DBC_MASK 0x30000U
#define GR1553B_RTMCC_DBC_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_DBC_MASK ) >> \
    GR1553B_RTMCC_DBC_SHIFT )
#define GR1553B_RTMCC_DBC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_DBC_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_DBC_SHIFT ) & \
      GR1553B_RTMCC_DBC_MASK ) )
#define GR1553B_RTMCC_DBC( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_DBC_SHIFT ) & \
    GR1553B_RTMCC_DBC_MASK )

#define GR1553B_RTMCC_TBW_SHIFT 14
#define GR1553B_RTMCC_TBW_MASK 0xc000U
#define GR1553B_RTMCC_TBW_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_TBW_MASK ) >> \
    GR1553B_RTMCC_TBW_SHIFT )
#define GR1553B_RTMCC_TBW_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_TBW_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_TBW_SHIFT ) & \
      GR1553B_RTMCC_TBW_MASK ) )
#define GR1553B_RTMCC_TBW( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_TBW_SHIFT ) & \
    GR1553B_RTMCC_TBW_MASK )

#define GR1553B_RTMCC_TVW_SHIFT 12
#define GR1553B_RTMCC_TVW_MASK 0x3000U
#define GR1553B_RTMCC_TVW_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_TVW_MASK ) >> \
    GR1553B_RTMCC_TVW_SHIFT )
#define GR1553B_RTMCC_TVW_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_TVW_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_TVW_SHIFT ) & \
      GR1553B_RTMCC_TVW_MASK ) )
#define GR1553B_RTMCC_TVW( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_TVW_SHIFT ) & \
    GR1553B_RTMCC_TVW_MASK )

#define GR1553B_RTMCC_TSB_SHIFT 10
#define GR1553B_RTMCC_TSB_MASK 0xc00U
#define GR1553B_RTMCC_TSB_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_TSB_MASK ) >> \
    GR1553B_RTMCC_TSB_SHIFT )
#define GR1553B_RTMCC_TSB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_TSB_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_TSB_SHIFT ) & \
      GR1553B_RTMCC_TSB_MASK ) )
#define GR1553B_RTMCC_TSB( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_TSB_SHIFT ) & \
    GR1553B_RTMCC_TSB_MASK )

#define GR1553B_RTMCC_TS_SHIFT 8
#define GR1553B_RTMCC_TS_MASK 0x300U
#define GR1553B_RTMCC_TS_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_TS_MASK ) >> \
    GR1553B_RTMCC_TS_SHIFT )
#define GR1553B_RTMCC_TS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_TS_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_TS_SHIFT ) & \
      GR1553B_RTMCC_TS_MASK ) )
#define GR1553B_RTMCC_TS( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_TS_SHIFT ) & \
    GR1553B_RTMCC_TS_MASK )

#define GR1553B_RTMCC_SDB_SHIFT 6
#define GR1553B_RTMCC_SDB_MASK 0xc0U
#define GR1553B_RTMCC_SDB_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_SDB_MASK ) >> \
    GR1553B_RTMCC_SDB_SHIFT )
#define GR1553B_RTMCC_SDB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_SDB_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_SDB_SHIFT ) & \
      GR1553B_RTMCC_SDB_MASK ) )
#define GR1553B_RTMCC_SDB( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_SDB_SHIFT ) & \
    GR1553B_RTMCC_SDB_MASK )

#define GR1553B_RTMCC_SD_SHIFT 4
#define GR1553B_RTMCC_SD_MASK 0x30U
#define GR1553B_RTMCC_SD_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_SD_MASK ) >> \
    GR1553B_RTMCC_SD_SHIFT )
#define GR1553B_RTMCC_SD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_SD_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_SD_SHIFT ) & \
      GR1553B_RTMCC_SD_MASK ) )
#define GR1553B_RTMCC_SD( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_SD_SHIFT ) & \
    GR1553B_RTMCC_SD_MASK )

#define GR1553B_RTMCC_SB_SHIFT 2
#define GR1553B_RTMCC_SB_MASK 0xcU
#define GR1553B_RTMCC_SB_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_SB_MASK ) >> \
    GR1553B_RTMCC_SB_SHIFT )
#define GR1553B_RTMCC_SB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_SB_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_SB_SHIFT ) & \
      GR1553B_RTMCC_SB_MASK ) )
#define GR1553B_RTMCC_SB( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_SB_SHIFT ) & \
    GR1553B_RTMCC_SB_MASK )

#define GR1553B_RTMCC_S_SHIFT 0
#define GR1553B_RTMCC_S_MASK 0x3U
#define GR1553B_RTMCC_S_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTMCC_S_MASK ) >> \
    GR1553B_RTMCC_S_SHIFT )
#define GR1553B_RTMCC_S_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTMCC_S_MASK ) | \
    ( ( ( _val ) << GR1553B_RTMCC_S_SHIFT ) & \
      GR1553B_RTMCC_S_MASK ) )
#define GR1553B_RTMCC_S( _val ) \
  ( ( ( _val ) << GR1553B_RTMCC_S_SHIFT ) & \
    GR1553B_RTMCC_S_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BRTTTC \
 *   GR1553B RT Time tag control register (RTTTC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_RTTTC_TRES_SHIFT 16
#define GR1553B_RTTTC_TRES_MASK 0xffff0000U
#define GR1553B_RTTTC_TRES_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTTTC_TRES_MASK ) >> \
    GR1553B_RTTTC_TRES_SHIFT )
#define GR1553B_RTTTC_TRES_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTTTC_TRES_MASK ) | \
    ( ( ( _val ) << GR1553B_RTTTC_TRES_SHIFT ) & \
      GR1553B_RTTTC_TRES_MASK ) )
#define GR1553B_RTTTC_TRES( _val ) \
  ( ( ( _val ) << GR1553B_RTTTC_TRES_SHIFT ) & \
    GR1553B_RTTTC_TRES_MASK )

#define GR1553B_RTTTC_TVAL_SHIFT 0
#define GR1553B_RTTTC_TVAL_MASK 0xffffU
#define GR1553B_RTTTC_TVAL_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTTTC_TVAL_MASK ) >> \
    GR1553B_RTTTC_TVAL_SHIFT )
#define GR1553B_RTTTC_TVAL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTTTC_TVAL_MASK ) | \
    ( ( ( _val ) << GR1553B_RTTTC_TVAL_SHIFT ) & \
      GR1553B_RTTTC_TVAL_MASK ) )
#define GR1553B_RTTTC_TVAL( _val ) \
  ( ( ( _val ) << GR1553B_RTTTC_TVAL_SHIFT ) & \
    GR1553B_RTTTC_TVAL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BRTELM \
 *   GR1553B RT Event log size mask register (RTELM)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_RTELM_MASK_SHIFT 0
#define GR1553B_RTELM_MASK_MASK 0xffffffffU
#define GR1553B_RTELM_MASK_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTELM_MASK_MASK ) >> \
    GR1553B_RTELM_MASK_SHIFT )
#define GR1553B_RTELM_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTELM_MASK_MASK ) | \
    ( ( ( _val ) << GR1553B_RTELM_MASK_SHIFT ) & \
      GR1553B_RTELM_MASK_MASK ) )
#define GR1553B_RTELM_MASK( _val ) \
  ( ( ( _val ) << GR1553B_RTELM_MASK_SHIFT ) & \
    GR1553B_RTELM_MASK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BRTELP \
 *   GR1553B RT Event log position register (RTELP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_RTELP_POINTER_SHIFT 0
#define GR1553B_RTELP_POINTER_MASK 0xffffffffU
#define GR1553B_RTELP_POINTER_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTELP_POINTER_MASK ) >> \
    GR1553B_RTELP_POINTER_SHIFT )
#define GR1553B_RTELP_POINTER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTELP_POINTER_MASK ) | \
    ( ( ( _val ) << GR1553B_RTELP_POINTER_SHIFT ) & \
      GR1553B_RTELP_POINTER_MASK ) )
#define GR1553B_RTELP_POINTER( _val ) \
  ( ( ( _val ) << GR1553B_RTELP_POINTER_SHIFT ) & \
    GR1553B_RTELP_POINTER_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BRTELIP \
 *   GR1553B RT Event Log interrupt position register (RTELIP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_RTELIP_POINTER_SHIFT 0
#define GR1553B_RTELIP_POINTER_MASK 0xffffffffU
#define GR1553B_RTELIP_POINTER_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_RTELIP_POINTER_MASK ) >> \
    GR1553B_RTELIP_POINTER_SHIFT )
#define GR1553B_RTELIP_POINTER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_RTELIP_POINTER_MASK ) | \
    ( ( ( _val ) << GR1553B_RTELIP_POINTER_SHIFT ) & \
      GR1553B_RTELIP_POINTER_MASK ) )
#define GR1553B_RTELIP_POINTER( _val ) \
  ( ( ( _val ) << GR1553B_RTELIP_POINTER_SHIFT ) & \
    GR1553B_RTELIP_POINTER_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBMS GR1553B BM Status register (BMS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BMS_BMSUP 0x80000000U

#define GR1553B_BMS_KEYEN 0x40000000U

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBMC GR1553B BM Control register (BMC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BMC_BMKEY_SHIFT 16
#define GR1553B_BMC_BMKEY_MASK 0xffff0000U
#define GR1553B_BMC_BMKEY_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BMC_BMKEY_MASK ) >> \
    GR1553B_BMC_BMKEY_SHIFT )
#define GR1553B_BMC_BMKEY_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BMC_BMKEY_MASK ) | \
    ( ( ( _val ) << GR1553B_BMC_BMKEY_SHIFT ) & \
      GR1553B_BMC_BMKEY_MASK ) )
#define GR1553B_BMC_BMKEY( _val ) \
  ( ( ( _val ) << GR1553B_BMC_BMKEY_SHIFT ) & \
    GR1553B_BMC_BMKEY_MASK )

#define GR1553B_BMC_WRSTP 0x20U

#define GR1553B_BMC_EXST 0x10U

#define GR1553B_BMC_IMCL 0x8U

#define GR1553B_BMC_UDWL 0x4U

#define GR1553B_BMC_MANL 0x2U

#define GR1553B_BMC_BMEN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBMRTAF \
 *   GR1553B BM RT Address filter register (BMRTAF)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BMRTAF_MASK_SHIFT 0
#define GR1553B_BMRTAF_MASK_MASK 0xffffffffU
#define GR1553B_BMRTAF_MASK_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BMRTAF_MASK_MASK ) >> \
    GR1553B_BMRTAF_MASK_SHIFT )
#define GR1553B_BMRTAF_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BMRTAF_MASK_MASK ) | \
    ( ( ( _val ) << GR1553B_BMRTAF_MASK_SHIFT ) & \
      GR1553B_BMRTAF_MASK_MASK ) )
#define GR1553B_BMRTAF_MASK( _val ) \
  ( ( ( _val ) << GR1553B_BMRTAF_MASK_SHIFT ) & \
    GR1553B_BMRTAF_MASK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBMRTSF \
 *   GR1553B BM RT Subaddress filter register (BMRTSF)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BMRTSF_MASK_SHIFT 0
#define GR1553B_BMRTSF_MASK_MASK 0xffffffffU
#define GR1553B_BMRTSF_MASK_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BMRTSF_MASK_MASK ) >> \
    GR1553B_BMRTSF_MASK_SHIFT )
#define GR1553B_BMRTSF_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BMRTSF_MASK_MASK ) | \
    ( ( ( _val ) << GR1553B_BMRTSF_MASK_SHIFT ) & \
      GR1553B_BMRTSF_MASK_MASK ) )
#define GR1553B_BMRTSF_MASK( _val ) \
  ( ( ( _val ) << GR1553B_BMRTSF_MASK_SHIFT ) & \
    GR1553B_BMRTSF_MASK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBMRTMC \
 *   GR1553B BM RT Mode code filter register (BMRTMC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BMRTMC_STSB 0x40000U

#define GR1553B_BMRTMC_STS 0x20000U

#define GR1553B_BMRTMC_TLC 0x10000U

#define GR1553B_BMRTMC_TSW 0x8000U

#define GR1553B_BMRTMC_RRTB 0x4000U

#define GR1553B_BMRTMC_RRT 0x2000U

#define GR1553B_BMRTMC_ITFB 0x1000U

#define GR1553B_BMRTMC_ITF 0x800U

#define GR1553B_BMRTMC_ISTB 0x400U

#define GR1553B_BMRTMC_IST 0x200U

#define GR1553B_BMRTMC_DBC 0x100U

#define GR1553B_BMRTMC_TBW 0x80U

#define GR1553B_BMRTMC_TVW 0x40U

#define GR1553B_BMRTMC_TSB 0x20U

#define GR1553B_BMRTMC_TS 0x10U

#define GR1553B_BMRTMC_SDB 0x8U

#define GR1553B_BMRTMC_SD 0x4U

#define GR1553B_BMRTMC_SB 0x2U

#define GR1553B_BMRTMC_S 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBMLBS GR1553B BM Log buffer start (BMLBS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BMLBS_START_SHIFT 0
#define GR1553B_BMLBS_START_MASK 0xffffffffU
#define GR1553B_BMLBS_START_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BMLBS_START_MASK ) >> \
    GR1553B_BMLBS_START_SHIFT )
#define GR1553B_BMLBS_START_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BMLBS_START_MASK ) | \
    ( ( ( _val ) << GR1553B_BMLBS_START_SHIFT ) & \
      GR1553B_BMLBS_START_MASK ) )
#define GR1553B_BMLBS_START( _val ) \
  ( ( ( _val ) << GR1553B_BMLBS_START_SHIFT ) & \
    GR1553B_BMLBS_START_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBMLBE GR1553B BM Log buffer end (BMLBE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BMLBE_END_SHIFT 0
#define GR1553B_BMLBE_END_MASK 0xffffffffU
#define GR1553B_BMLBE_END_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BMLBE_END_MASK ) >> \
    GR1553B_BMLBE_END_SHIFT )
#define GR1553B_BMLBE_END_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BMLBE_END_MASK ) | \
    ( ( ( _val ) << GR1553B_BMLBE_END_SHIFT ) & \
      GR1553B_BMLBE_END_MASK ) )
#define GR1553B_BMLBE_END( _val ) \
  ( ( ( _val ) << GR1553B_BMLBE_END_SHIFT ) & \
    GR1553B_BMLBE_END_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBMLBP GR1553B BM Log buffer position (BMLBP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BMLBP_POSITION_SHIFT 0
#define GR1553B_BMLBP_POSITION_MASK 0xffffffffU
#define GR1553B_BMLBP_POSITION_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BMLBP_POSITION_MASK ) >> \
    GR1553B_BMLBP_POSITION_SHIFT )
#define GR1553B_BMLBP_POSITION_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BMLBP_POSITION_MASK ) | \
    ( ( ( _val ) << GR1553B_BMLBP_POSITION_SHIFT ) & \
      GR1553B_BMLBP_POSITION_MASK ) )
#define GR1553B_BMLBP_POSITION( _val ) \
  ( ( ( _val ) << GR1553B_BMLBP_POSITION_SHIFT ) & \
    GR1553B_BMLBP_POSITION_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGR1553BBMTTC \
 *   GR1553B BM Time tag control register (BMTTC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR1553B_BMTTC_TRES_SHIFT 24
#define GR1553B_BMTTC_TRES_MASK 0xff000000U
#define GR1553B_BMTTC_TRES_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BMTTC_TRES_MASK ) >> \
    GR1553B_BMTTC_TRES_SHIFT )
#define GR1553B_BMTTC_TRES_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BMTTC_TRES_MASK ) | \
    ( ( ( _val ) << GR1553B_BMTTC_TRES_SHIFT ) & \
      GR1553B_BMTTC_TRES_MASK ) )
#define GR1553B_BMTTC_TRES( _val ) \
  ( ( ( _val ) << GR1553B_BMTTC_TRES_SHIFT ) & \
    GR1553B_BMTTC_TRES_MASK )

#define GR1553B_BMTTC_TVAL_SHIFT 0
#define GR1553B_BMTTC_TVAL_MASK 0xffffffU
#define GR1553B_BMTTC_TVAL_GET( _reg ) \
  ( ( ( _reg ) & GR1553B_BMTTC_TVAL_MASK ) >> \
    GR1553B_BMTTC_TVAL_SHIFT )
#define GR1553B_BMTTC_TVAL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR1553B_BMTTC_TVAL_MASK ) | \
    ( ( ( _val ) << GR1553B_BMTTC_TVAL_SHIFT ) & \
      GR1553B_BMTTC_TVAL_MASK ) )
#define GR1553B_BMTTC_TVAL( _val ) \
  ( ( ( _val ) << GR1553B_BMTTC_TVAL_SHIFT ) & \
    GR1553B_BMTTC_TVAL_MASK )

/** @} */

/**
 * @brief This structure defines the GR1553B register block memory map.
 */
typedef struct gr1553b {
  /**
   * @brief See @ref RTEMSDeviceGR1553BIRQ.
   */
  uint32_t irq;

  /**
   * @brief See @ref RTEMSDeviceGR1553BIRQE.
   */
  uint32_t irqe;

  uint32_t reserved_8_10[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGR1553BHC.
   */
  uint32_t hc;

  uint32_t reserved_14_40[ 11 ];

  /**
   * @brief See @ref RTEMSDeviceGR1553BBCSC.
   */
  uint32_t bcsc;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBCA.
   */
  uint32_t bca;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBCTNP.
   */
  uint32_t bctnp;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBCANP.
   */
  uint32_t bcanp;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBCT.
   */
  uint32_t bct;

  uint32_t reserved_54_58;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBCRP.
   */
  uint32_t bcrp;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBCBS.
   */
  uint32_t bcbs;

  uint32_t reserved_60_68[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGR1553BBCTCP.
   */
  uint32_t bctcp;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBCACP.
   */
  uint32_t bcacp;

  uint32_t reserved_70_80[ 4 ];

  /**
   * @brief See @ref RTEMSDeviceGR1553BRTS.
   */
  uint32_t rts;

  /**
   * @brief See @ref RTEMSDeviceGR1553BRTC.
   */
  uint32_t rtc;

  /**
   * @brief See @ref RTEMSDeviceGR1553BRTBS.
   */
  uint32_t rtbs;

  /**
   * @brief See @ref RTEMSDeviceGR1553BRTSW.
   */
  uint32_t rtsw;

  /**
   * @brief See @ref RTEMSDeviceGR1553BRTSY.
   */
  uint32_t rtsy;

  /**
   * @brief See @ref RTEMSDeviceGR1553BRTSTBA.
   */
  uint32_t rtstba;

  /**
   * @brief See @ref RTEMSDeviceGR1553BRTMCC.
   */
  uint32_t rtmcc;

  uint32_t reserved_9c_a4[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGR1553BRTTTC.
   */
  uint32_t rtttc;

  uint32_t reserved_a8_ac;

  /**
   * @brief See @ref RTEMSDeviceGR1553BRTELM.
   */
  uint32_t rtelm;

  /**
   * @brief See @ref RTEMSDeviceGR1553BRTELP.
   */
  uint32_t rtelp;

  /**
   * @brief See @ref RTEMSDeviceGR1553BRTELIP.
   */
  uint32_t rtelip;

  uint32_t reserved_b8_c0[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGR1553BBMS.
   */
  uint32_t bms;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBMC.
   */
  uint32_t bmc;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBMRTAF.
   */
  uint32_t bmrtaf;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBMRTSF.
   */
  uint32_t bmrtsf;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBMRTMC.
   */
  uint32_t bmrtmc;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBMLBS.
   */
  uint32_t bmlbs;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBMLBE.
   */
  uint32_t bmlbe;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBMLBP.
   */
  uint32_t bmlbp;

  /**
   * @brief See @ref RTEMSDeviceGR1553BBMTTC.
   */
  uint32_t bmttc;
} gr1553b;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GR1553B_REGS_H */
