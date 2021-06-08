/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRSPW2
 *
 * @brief This header file defines the GRSPW2 register block interface.
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

/* Generated from spec:/dev/grlib/if/grspw2-header */

#ifndef _GRLIB_GRSPW2_REGS_H
#define _GRLIB_GRSPW2_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grspw2 */

/**
 * @defgroup RTEMSDeviceGRSPW2 GRSPW2
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRSPW2 interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRSPW2CTRL Control (CTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_CTRL_RA 0x80000000U

#define GRSPW2_CTRL_RX 0x40000000U

#define GRSPW2_CTRL_RC 0x20000000U

#define GRSPW2_CTRL_NCH_SHIFT 27
#define GRSPW2_CTRL_NCH_MASK 0x18000000U
#define GRSPW2_CTRL_NCH_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_CTRL_NCH_MASK ) >> \
    GRSPW2_CTRL_NCH_SHIFT )
#define GRSPW2_CTRL_NCH_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_CTRL_NCH_MASK ) | \
    ( ( ( _val ) << GRSPW2_CTRL_NCH_SHIFT ) & \
      GRSPW2_CTRL_NCH_MASK ) )
#define GRSPW2_CTRL_NCH( _val ) \
  ( ( ( _val ) << GRSPW2_CTRL_NCH_SHIFT ) & \
    GRSPW2_CTRL_NCH_MASK )

#define GRSPW2_CTRL_PO 0x4000000U

#define GRSPW2_CTRL_RD 0x20000U

#define GRSPW2_CTRL_RE 0x10000U

#define GRSPW2_CTRL_TL 0x2000U

#define GRSPW2_CTRL_TF 0x1000U

#define GRSPW2_CTRL_TR 0x800U

#define GRSPW2_CTRL_TT 0x400U

#define GRSPW2_CTRL_LI 0x200U

#define GRSPW2_CTRL_TQ 0x100U

#define GRSPW2_CTRL_RS 0x40U

#define GRSPW2_CTRL_PM 0x20U

#define GRSPW2_CTRL_TI 0x10U

#define GRSPW2_CTRL_IE 0x8U

#define GRSPW2_CTRL_AS 0x4U

#define GRSPW2_CTRL_LS 0x2U

#define GRSPW2_CTRL_LD 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2STS Status (STS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_STS_NRXD_SHIFT 26
#define GRSPW2_STS_NRXD_MASK 0xc000000U
#define GRSPW2_STS_NRXD_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_STS_NRXD_MASK ) >> \
    GRSPW2_STS_NRXD_SHIFT )
#define GRSPW2_STS_NRXD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_STS_NRXD_MASK ) | \
    ( ( ( _val ) << GRSPW2_STS_NRXD_SHIFT ) & \
      GRSPW2_STS_NRXD_MASK ) )
#define GRSPW2_STS_NRXD( _val ) \
  ( ( ( _val ) << GRSPW2_STS_NRXD_SHIFT ) & \
    GRSPW2_STS_NRXD_MASK )

#define GRSPW2_STS_NTXD_SHIFT 24
#define GRSPW2_STS_NTXD_MASK 0x3000000U
#define GRSPW2_STS_NTXD_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_STS_NTXD_MASK ) >> \
    GRSPW2_STS_NTXD_SHIFT )
#define GRSPW2_STS_NTXD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_STS_NTXD_MASK ) | \
    ( ( ( _val ) << GRSPW2_STS_NTXD_SHIFT ) & \
      GRSPW2_STS_NTXD_MASK ) )
#define GRSPW2_STS_NTXD( _val ) \
  ( ( ( _val ) << GRSPW2_STS_NTXD_SHIFT ) & \
    GRSPW2_STS_NTXD_MASK )

#define GRSPW2_STS_LS_SHIFT 21
#define GRSPW2_STS_LS_MASK 0xe00000U
#define GRSPW2_STS_LS_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_STS_LS_MASK ) >> \
    GRSPW2_STS_LS_SHIFT )
#define GRSPW2_STS_LS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_STS_LS_MASK ) | \
    ( ( ( _val ) << GRSPW2_STS_LS_SHIFT ) & \
      GRSPW2_STS_LS_MASK ) )
#define GRSPW2_STS_LS( _val ) \
  ( ( ( _val ) << GRSPW2_STS_LS_SHIFT ) & \
    GRSPW2_STS_LS_MASK )

#define GRSPW2_STS_EE 0x100U

#define GRSPW2_STS_IA 0x80U

#define GRSPW2_STS_PE 0x10U

#define GRSPW2_STS_DE 0x8U

#define GRSPW2_STS_ER 0x4U

#define GRSPW2_STS_CE 0x2U

#define GRSPW2_STS_TO 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2DEFADDR Default address (DEFADDR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_DEFADDR_DEFMASK_SHIFT 8
#define GRSPW2_DEFADDR_DEFMASK_MASK 0xff00U
#define GRSPW2_DEFADDR_DEFMASK_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DEFADDR_DEFMASK_MASK ) >> \
    GRSPW2_DEFADDR_DEFMASK_SHIFT )
#define GRSPW2_DEFADDR_DEFMASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DEFADDR_DEFMASK_MASK ) | \
    ( ( ( _val ) << GRSPW2_DEFADDR_DEFMASK_SHIFT ) & \
      GRSPW2_DEFADDR_DEFMASK_MASK ) )
#define GRSPW2_DEFADDR_DEFMASK( _val ) \
  ( ( ( _val ) << GRSPW2_DEFADDR_DEFMASK_SHIFT ) & \
    GRSPW2_DEFADDR_DEFMASK_MASK )

#define GRSPW2_DEFADDR_DEFADDR_SHIFT 0
#define GRSPW2_DEFADDR_DEFADDR_MASK 0xffU
#define GRSPW2_DEFADDR_DEFADDR_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DEFADDR_DEFADDR_MASK ) >> \
    GRSPW2_DEFADDR_DEFADDR_SHIFT )
#define GRSPW2_DEFADDR_DEFADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DEFADDR_DEFADDR_MASK ) | \
    ( ( ( _val ) << GRSPW2_DEFADDR_DEFADDR_SHIFT ) & \
      GRSPW2_DEFADDR_DEFADDR_MASK ) )
#define GRSPW2_DEFADDR_DEFADDR( _val ) \
  ( ( ( _val ) << GRSPW2_DEFADDR_DEFADDR_SHIFT ) & \
    GRSPW2_DEFADDR_DEFADDR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2CLKDIV Clock divisor (CLKDIV)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_CLKDIV_CLKDIVSTART_SHIFT 8
#define GRSPW2_CLKDIV_CLKDIVSTART_MASK 0xff00U
#define GRSPW2_CLKDIV_CLKDIVSTART_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_CLKDIV_CLKDIVSTART_MASK ) >> \
    GRSPW2_CLKDIV_CLKDIVSTART_SHIFT )
#define GRSPW2_CLKDIV_CLKDIVSTART_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_CLKDIV_CLKDIVSTART_MASK ) | \
    ( ( ( _val ) << GRSPW2_CLKDIV_CLKDIVSTART_SHIFT ) & \
      GRSPW2_CLKDIV_CLKDIVSTART_MASK ) )
#define GRSPW2_CLKDIV_CLKDIVSTART( _val ) \
  ( ( ( _val ) << GRSPW2_CLKDIV_CLKDIVSTART_SHIFT ) & \
    GRSPW2_CLKDIV_CLKDIVSTART_MASK )

#define GRSPW2_CLKDIV_CLKDIVRUN_SHIFT 0
#define GRSPW2_CLKDIV_CLKDIVRUN_MASK 0xffU
#define GRSPW2_CLKDIV_CLKDIVRUN_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_CLKDIV_CLKDIVRUN_MASK ) >> \
    GRSPW2_CLKDIV_CLKDIVRUN_SHIFT )
#define GRSPW2_CLKDIV_CLKDIVRUN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_CLKDIV_CLKDIVRUN_MASK ) | \
    ( ( ( _val ) << GRSPW2_CLKDIV_CLKDIVRUN_SHIFT ) & \
      GRSPW2_CLKDIV_CLKDIVRUN_MASK ) )
#define GRSPW2_CLKDIV_CLKDIVRUN( _val ) \
  ( ( ( _val ) << GRSPW2_CLKDIV_CLKDIVRUN_SHIFT ) & \
    GRSPW2_CLKDIV_CLKDIVRUN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2DKEY Destination key (DKEY)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_DKEY_DESTKEY_SHIFT 0
#define GRSPW2_DKEY_DESTKEY_MASK 0xffU
#define GRSPW2_DKEY_DESTKEY_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DKEY_DESTKEY_MASK ) >> \
    GRSPW2_DKEY_DESTKEY_SHIFT )
#define GRSPW2_DKEY_DESTKEY_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DKEY_DESTKEY_MASK ) | \
    ( ( ( _val ) << GRSPW2_DKEY_DESTKEY_SHIFT ) & \
      GRSPW2_DKEY_DESTKEY_MASK ) )
#define GRSPW2_DKEY_DESTKEY( _val ) \
  ( ( ( _val ) << GRSPW2_DKEY_DESTKEY_SHIFT ) & \
    GRSPW2_DKEY_DESTKEY_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2TC Time-code (TC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_TC_TCTRL_SHIFT 6
#define GRSPW2_TC_TCTRL_MASK 0xc0U
#define GRSPW2_TC_TCTRL_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_TC_TCTRL_MASK ) >> \
    GRSPW2_TC_TCTRL_SHIFT )
#define GRSPW2_TC_TCTRL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_TC_TCTRL_MASK ) | \
    ( ( ( _val ) << GRSPW2_TC_TCTRL_SHIFT ) & \
      GRSPW2_TC_TCTRL_MASK ) )
#define GRSPW2_TC_TCTRL( _val ) \
  ( ( ( _val ) << GRSPW2_TC_TCTRL_SHIFT ) & \
    GRSPW2_TC_TCTRL_MASK )

#define GRSPW2_TC_TIMECNT_SHIFT 0
#define GRSPW2_TC_TIMECNT_MASK 0x3fU
#define GRSPW2_TC_TIMECNT_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_TC_TIMECNT_MASK ) >> \
    GRSPW2_TC_TIMECNT_SHIFT )
#define GRSPW2_TC_TIMECNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_TC_TIMECNT_MASK ) | \
    ( ( ( _val ) << GRSPW2_TC_TIMECNT_SHIFT ) & \
      GRSPW2_TC_TIMECNT_MASK ) )
#define GRSPW2_TC_TIMECNT( _val ) \
  ( ( ( _val ) << GRSPW2_TC_TIMECNT_SHIFT ) & \
    GRSPW2_TC_TIMECNT_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2DMACTRL DMA control/status, channel 1 (DMACTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_DMACTRL_EP 0x800000U

#define GRSPW2_DMACTRL_TR 0x400000U

#define GRSPW2_DMACTRL_RP 0x80000U

#define GRSPW2_DMACTRL_TP 0x40000U

#define GRSPW2_DMACTRL_TL 0x20000U

#define GRSPW2_DMACTRL_LE 0x10000U

#define GRSPW2_DMACTRL_SP 0x8000U

#define GRSPW2_DMACTRL_SA 0x4000U

#define GRSPW2_DMACTRL_EN 0x2000U

#define GRSPW2_DMACTRL_NS 0x1000U

#define GRSPW2_DMACTRL_RD 0x800U

#define GRSPW2_DMACTRL_RX 0x400U

#define GRSPW2_DMACTRL_AT 0x200U

#define GRSPW2_DMACTRL_RA 0x100U

#define GRSPW2_DMACTRL_TA 0x80U

#define GRSPW2_DMACTRL_PR 0x40U

#define GRSPW2_DMACTRL_PS 0x20U

#define GRSPW2_DMACTRL_AI 0x10U

#define GRSPW2_DMACTRL_RI 0x8U

#define GRSPW2_DMACTRL_TI 0x4U

#define GRSPW2_DMACTRL_RE 0x2U

#define GRSPW2_DMACTRL_TE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2DMAMAXLEN \
 *   DMA RX maximum length, channel 1 (DMAMAXLEN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_DMAMAXLEN_RXMAXLEN_SHIFT 2
#define GRSPW2_DMAMAXLEN_RXMAXLEN_MASK 0x1fffffcU
#define GRSPW2_DMAMAXLEN_RXMAXLEN_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DMAMAXLEN_RXMAXLEN_MASK ) >> \
    GRSPW2_DMAMAXLEN_RXMAXLEN_SHIFT )
#define GRSPW2_DMAMAXLEN_RXMAXLEN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DMAMAXLEN_RXMAXLEN_MASK ) | \
    ( ( ( _val ) << GRSPW2_DMAMAXLEN_RXMAXLEN_SHIFT ) & \
      GRSPW2_DMAMAXLEN_RXMAXLEN_MASK ) )
#define GRSPW2_DMAMAXLEN_RXMAXLEN( _val ) \
  ( ( ( _val ) << GRSPW2_DMAMAXLEN_RXMAXLEN_SHIFT ) & \
    GRSPW2_DMAMAXLEN_RXMAXLEN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2DMATXDESC \
 *   DMA transmitter descriptor table address, channel 1 (DMATXDESC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_DMATXDESC_DESCBASEADDR_SHIFT 10
#define GRSPW2_DMATXDESC_DESCBASEADDR_MASK 0xfffffc00U
#define GRSPW2_DMATXDESC_DESCBASEADDR_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DMATXDESC_DESCBASEADDR_MASK ) >> \
    GRSPW2_DMATXDESC_DESCBASEADDR_SHIFT )
#define GRSPW2_DMATXDESC_DESCBASEADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DMATXDESC_DESCBASEADDR_MASK ) | \
    ( ( ( _val ) << GRSPW2_DMATXDESC_DESCBASEADDR_SHIFT ) & \
      GRSPW2_DMATXDESC_DESCBASEADDR_MASK ) )
#define GRSPW2_DMATXDESC_DESCBASEADDR( _val ) \
  ( ( ( _val ) << GRSPW2_DMATXDESC_DESCBASEADDR_SHIFT ) & \
    GRSPW2_DMATXDESC_DESCBASEADDR_MASK )

#define GRSPW2_DMATXDESC_DESCSEL_SHIFT 4
#define GRSPW2_DMATXDESC_DESCSEL_MASK 0x3f0U
#define GRSPW2_DMATXDESC_DESCSEL_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DMATXDESC_DESCSEL_MASK ) >> \
    GRSPW2_DMATXDESC_DESCSEL_SHIFT )
#define GRSPW2_DMATXDESC_DESCSEL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DMATXDESC_DESCSEL_MASK ) | \
    ( ( ( _val ) << GRSPW2_DMATXDESC_DESCSEL_SHIFT ) & \
      GRSPW2_DMATXDESC_DESCSEL_MASK ) )
#define GRSPW2_DMATXDESC_DESCSEL( _val ) \
  ( ( ( _val ) << GRSPW2_DMATXDESC_DESCSEL_SHIFT ) & \
    GRSPW2_DMATXDESC_DESCSEL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2DMARXDESC \
 *   DMA receiver descriptor table address, channel 1 (DMARXDESC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_DMARXDESC_DESCBASEADDR_SHIFT 10
#define GRSPW2_DMARXDESC_DESCBASEADDR_MASK 0xfffffc00U
#define GRSPW2_DMARXDESC_DESCBASEADDR_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DMARXDESC_DESCBASEADDR_MASK ) >> \
    GRSPW2_DMARXDESC_DESCBASEADDR_SHIFT )
#define GRSPW2_DMARXDESC_DESCBASEADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DMARXDESC_DESCBASEADDR_MASK ) | \
    ( ( ( _val ) << GRSPW2_DMARXDESC_DESCBASEADDR_SHIFT ) & \
      GRSPW2_DMARXDESC_DESCBASEADDR_MASK ) )
#define GRSPW2_DMARXDESC_DESCBASEADDR( _val ) \
  ( ( ( _val ) << GRSPW2_DMARXDESC_DESCBASEADDR_SHIFT ) & \
    GRSPW2_DMARXDESC_DESCBASEADDR_MASK )

#define GRSPW2_DMARXDESC_DESCSEL_SHIFT 3
#define GRSPW2_DMARXDESC_DESCSEL_MASK 0x3f8U
#define GRSPW2_DMARXDESC_DESCSEL_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DMARXDESC_DESCSEL_MASK ) >> \
    GRSPW2_DMARXDESC_DESCSEL_SHIFT )
#define GRSPW2_DMARXDESC_DESCSEL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DMARXDESC_DESCSEL_MASK ) | \
    ( ( ( _val ) << GRSPW2_DMARXDESC_DESCSEL_SHIFT ) & \
      GRSPW2_DMARXDESC_DESCSEL_MASK ) )
#define GRSPW2_DMARXDESC_DESCSEL( _val ) \
  ( ( ( _val ) << GRSPW2_DMARXDESC_DESCSEL_SHIFT ) & \
    GRSPW2_DMARXDESC_DESCSEL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2DMAADDR DMA address, channel 1 (DMAADDR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_DMAADDR_MASK_SHIFT 8
#define GRSPW2_DMAADDR_MASK_MASK 0xff00U
#define GRSPW2_DMAADDR_MASK_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DMAADDR_MASK_MASK ) >> \
    GRSPW2_DMAADDR_MASK_SHIFT )
#define GRSPW2_DMAADDR_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DMAADDR_MASK_MASK ) | \
    ( ( ( _val ) << GRSPW2_DMAADDR_MASK_SHIFT ) & \
      GRSPW2_DMAADDR_MASK_MASK ) )
#define GRSPW2_DMAADDR_MASK( _val ) \
  ( ( ( _val ) << GRSPW2_DMAADDR_MASK_SHIFT ) & \
    GRSPW2_DMAADDR_MASK_MASK )

#define GRSPW2_DMAADDR_ADDR_SHIFT 0
#define GRSPW2_DMAADDR_ADDR_MASK 0xffU
#define GRSPW2_DMAADDR_ADDR_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DMAADDR_ADDR_MASK ) >> \
    GRSPW2_DMAADDR_ADDR_SHIFT )
#define GRSPW2_DMAADDR_ADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DMAADDR_ADDR_MASK ) | \
    ( ( ( _val ) << GRSPW2_DMAADDR_ADDR_SHIFT ) & \
      GRSPW2_DMAADDR_ADDR_MASK ) )
#define GRSPW2_DMAADDR_ADDR( _val ) \
  ( ( ( _val ) << GRSPW2_DMAADDR_ADDR_SHIFT ) & \
    GRSPW2_DMAADDR_ADDR_MASK )

/** @} */

/**
 * @brief This structure defines the GRSPW2 register block memory map.
 */
typedef struct grspw2 {
  /**
   * @brief See @ref RTEMSDeviceGRSPW2CTRL.
   */
  uint32_t ctrl;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2STS.
   */
  uint32_t sts;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2DEFADDR.
   */
  uint32_t defaddr;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2CLKDIV.
   */
  uint32_t clkdiv;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2DKEY.
   */
  uint32_t dkey;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2TC.
   */
  uint32_t tc;

  uint32_t reserved_18_20[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPW2DMACTRL.
   */
  uint32_t dmactrl;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2DMAMAXLEN.
   */
  uint32_t dmamaxlen;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2DMATXDESC.
   */
  uint32_t dmatxdesc;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2DMARXDESC.
   */
  uint32_t dmarxdesc;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2DMAADDR.
   */
  uint32_t dmaaddr;
} grspw2;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRSPW2_REGS_H */
