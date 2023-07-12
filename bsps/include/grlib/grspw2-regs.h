/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRSPW2
 *
 * @brief This header file defines the GRSPW2 register block interface.
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

/* Generated from spec:/dev/grlib/if/grspw2-header */

#ifndef _GRLIB_GRSPW2_REGS_H
#define _GRLIB_GRSPW2_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grspw2-dma */

/**
 * @defgroup RTEMSDeviceGRSPW2DMA GRSPW2 DMA
 *
 * @ingroup RTEMSDeviceGRSPW2
 *
 * @brief This group contains the GRSPW2 DMA interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRSPW2DMADMACTRL DMA control/status (DMACTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_DMACTRL_INTNUM_SHIFT 26
#define GRSPW2_DMACTRL_INTNUM_MASK 0xfc000000U
#define GRSPW2_DMACTRL_INTNUM_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DMACTRL_INTNUM_MASK ) >> \
    GRSPW2_DMACTRL_INTNUM_SHIFT )
#define GRSPW2_DMACTRL_INTNUM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DMACTRL_INTNUM_MASK ) | \
    ( ( ( _val ) << GRSPW2_DMACTRL_INTNUM_SHIFT ) & \
      GRSPW2_DMACTRL_INTNUM_MASK ) )
#define GRSPW2_DMACTRL_INTNUM( _val ) \
  ( ( ( _val ) << GRSPW2_DMACTRL_INTNUM_SHIFT ) & \
    GRSPW2_DMACTRL_INTNUM_MASK )

#define GRSPW2_DMACTRL_RES_SHIFT 24
#define GRSPW2_DMACTRL_RES_MASK 0x3000000U
#define GRSPW2_DMACTRL_RES_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DMACTRL_RES_MASK ) >> \
    GRSPW2_DMACTRL_RES_SHIFT )
#define GRSPW2_DMACTRL_RES_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DMACTRL_RES_MASK ) | \
    ( ( ( _val ) << GRSPW2_DMACTRL_RES_SHIFT ) & \
      GRSPW2_DMACTRL_RES_MASK ) )
#define GRSPW2_DMACTRL_RES( _val ) \
  ( ( ( _val ) << GRSPW2_DMACTRL_RES_SHIFT ) & \
    GRSPW2_DMACTRL_RES_MASK )

#define GRSPW2_DMACTRL_EP 0x800000U

#define GRSPW2_DMACTRL_TR 0x400000U

#define GRSPW2_DMACTRL_IE 0x200000U

#define GRSPW2_DMACTRL_IT 0x100000U

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
 * @defgroup RTEMSDeviceGRSPW2DMADMAMAXLEN DMA RX maximum length (DMAMAXLEN)
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

#define GRSPW2_DMAMAXLEN_RES_SHIFT 0
#define GRSPW2_DMAMAXLEN_RES_MASK 0x3U
#define GRSPW2_DMAMAXLEN_RES_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_DMAMAXLEN_RES_MASK ) >> \
    GRSPW2_DMAMAXLEN_RES_SHIFT )
#define GRSPW2_DMAMAXLEN_RES_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_DMAMAXLEN_RES_MASK ) | \
    ( ( ( _val ) << GRSPW2_DMAMAXLEN_RES_SHIFT ) & \
      GRSPW2_DMAMAXLEN_RES_MASK ) )
#define GRSPW2_DMAMAXLEN_RES( _val ) \
  ( ( ( _val ) << GRSPW2_DMAMAXLEN_RES_SHIFT ) & \
    GRSPW2_DMAMAXLEN_RES_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2DMADMATXDESC \
 *   DMA transmit descriptor table address (DMATXDESC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_DMATXDESC_DESCBASEADDR_SHIFT 0
#define GRSPW2_DMATXDESC_DESCBASEADDR_MASK 0xffffffffU
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
#define GRSPW2_DMATXDESC_DESCSEL_MASK 0xfffffff0U
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
 * @defgroup RTEMSDeviceGRSPW2DMADMARXDESC \
 *   DMA receive descriptor table address (DMARXDESC)
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
 * @defgroup RTEMSDeviceGRSPW2DMADMAADDR DMA address (DMAADDR)
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
 * @brief This structure defines the GRSPW2 DMA register block memory map.
 */
typedef struct grspw2_dma {
  /**
   * @brief See @ref RTEMSDeviceGRSPW2DMADMACTRL.
   */
  uint32_t dmactrl;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2DMADMAMAXLEN.
   */
  uint32_t dmamaxlen;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2DMADMATXDESC.
   */
  uint32_t dmatxdesc;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2DMADMARXDESC.
   */
  uint32_t dmarxdesc;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2DMADMAADDR.
   */
  uint32_t dmaaddr;

  uint32_t reserved_14_20[ 3 ];
} grspw2_dma;

/** @} */

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

#define GRSPW2_CTRL_CC 0x2000000U

#define GRSPW2_CTRL_ID 0x1000000U

#define GRSPW2_CTRL_R 0x800000U

#define GRSPW2_CTRL_LE 0x400000U

#define GRSPW2_CTRL_PS 0x200000U

#define GRSPW2_CTRL_NP 0x100000U

#define GRSPW2_CTRL_PNPA_SHIFT 18
#define GRSPW2_CTRL_PNPA_MASK 0xc0000U
#define GRSPW2_CTRL_PNPA_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_CTRL_PNPA_MASK ) >> \
    GRSPW2_CTRL_PNPA_SHIFT )
#define GRSPW2_CTRL_PNPA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_CTRL_PNPA_MASK ) | \
    ( ( ( _val ) << GRSPW2_CTRL_PNPA_SHIFT ) & \
      GRSPW2_CTRL_PNPA_MASK ) )
#define GRSPW2_CTRL_PNPA( _val ) \
  ( ( ( _val ) << GRSPW2_CTRL_PNPA_SHIFT ) & \
    GRSPW2_CTRL_PNPA_MASK )

#define GRSPW2_CTRL_RD 0x20000U

#define GRSPW2_CTRL_RE 0x10000U

#define GRSPW2_CTRL_PE 0x8000U

#define GRSPW2_CTRL_R 0x4000U

#define GRSPW2_CTRL_TL 0x2000U

#define GRSPW2_CTRL_TF 0x1000U

#define GRSPW2_CTRL_TR 0x800U

#define GRSPW2_CTRL_TT 0x400U

#define GRSPW2_CTRL_LI 0x200U

#define GRSPW2_CTRL_TQ 0x100U

#define GRSPW2_CTRL_R 0x80U

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

#define GRSPW2_STS_NTXD 0x2000000U

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

#define GRSPW2_STS_AP 0x200U

#define GRSPW2_STS_EE 0x100U

#define GRSPW2_STS_IA 0x80U

#define GRSPW2_STS_RES_SHIFT 5
#define GRSPW2_STS_RES_MASK 0x60U
#define GRSPW2_STS_RES_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_STS_RES_MASK ) >> \
    GRSPW2_STS_RES_SHIFT )
#define GRSPW2_STS_RES_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_STS_RES_MASK ) | \
    ( ( ( _val ) << GRSPW2_STS_RES_SHIFT ) & \
      GRSPW2_STS_RES_MASK ) )
#define GRSPW2_STS_RES( _val ) \
  ( ( ( _val ) << GRSPW2_STS_RES_SHIFT ) & \
    GRSPW2_STS_RES_MASK )

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
 * @defgroup RTEMSDeviceGRSPW2INTCTRL Interrupt distribution control (INTCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_INTCTRL_INTNUM_SHIFT 26
#define GRSPW2_INTCTRL_INTNUM_MASK 0xfc000000U
#define GRSPW2_INTCTRL_INTNUM_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTCTRL_INTNUM_MASK ) >> \
    GRSPW2_INTCTRL_INTNUM_SHIFT )
#define GRSPW2_INTCTRL_INTNUM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTCTRL_INTNUM_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTCTRL_INTNUM_SHIFT ) & \
      GRSPW2_INTCTRL_INTNUM_MASK ) )
#define GRSPW2_INTCTRL_INTNUM( _val ) \
  ( ( ( _val ) << GRSPW2_INTCTRL_INTNUM_SHIFT ) & \
    GRSPW2_INTCTRL_INTNUM_MASK )

#define GRSPW2_INTCTRL_RS 0x2000000U

#define GRSPW2_INTCTRL_EE 0x1000000U

#define GRSPW2_INTCTRL_IA 0x800000U

#define GRSPW2_INTCTRL_RES 0x2U

#define GRSPW2_INTCTRL_TQ_SHIFT 21
#define GRSPW2_INTCTRL_TQ_MASK 0x600000U
#define GRSPW2_INTCTRL_TQ_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTCTRL_TQ_MASK ) >> \
    GRSPW2_INTCTRL_TQ_SHIFT )
#define GRSPW2_INTCTRL_TQ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTCTRL_TQ_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTCTRL_TQ_SHIFT ) & \
      GRSPW2_INTCTRL_TQ_MASK ) )
#define GRSPW2_INTCTRL_TQ( _val ) \
  ( ( ( _val ) << GRSPW2_INTCTRL_TQ_SHIFT ) & \
    GRSPW2_INTCTRL_TQ_MASK )

#define GRSPW2_INTCTRL_AQ 0x100000U

#define GRSPW2_INTCTRL_IQ 0x80000U

#define GRSPW2_INTCTRL_RES 0x40000U

#define GRSPW2_INTCTRL_AA_SHIFT 16
#define GRSPW2_INTCTRL_AA_MASK 0x30000U
#define GRSPW2_INTCTRL_AA_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTCTRL_AA_MASK ) >> \
    GRSPW2_INTCTRL_AA_SHIFT )
#define GRSPW2_INTCTRL_AA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTCTRL_AA_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTCTRL_AA_SHIFT ) & \
      GRSPW2_INTCTRL_AA_MASK ) )
#define GRSPW2_INTCTRL_AA( _val ) \
  ( ( ( _val ) << GRSPW2_INTCTRL_AA_SHIFT ) & \
    GRSPW2_INTCTRL_AA_MASK )

#define GRSPW2_INTCTRL_AT 0x8000U

#define GRSPW2_INTCTRL_IT 0x4000U

#define GRSPW2_INTCTRL_RES 0x2000U

#define GRSPW2_INTCTRL_ID_SHIFT 8
#define GRSPW2_INTCTRL_ID_MASK 0x1f00U
#define GRSPW2_INTCTRL_ID_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTCTRL_ID_MASK ) >> \
    GRSPW2_INTCTRL_ID_SHIFT )
#define GRSPW2_INTCTRL_ID_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTCTRL_ID_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTCTRL_ID_SHIFT ) & \
      GRSPW2_INTCTRL_ID_MASK ) )
#define GRSPW2_INTCTRL_ID( _val ) \
  ( ( ( _val ) << GRSPW2_INTCTRL_ID_SHIFT ) & \
    GRSPW2_INTCTRL_ID_MASK )

#define GRSPW2_INTCTRL_II 0x80U

#define GRSPW2_INTCTRL_TXINT 0x40U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2INTRX Interrupt-code receive (INTRX)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_INTRX_RXIRQ_SHIFT 0
#define GRSPW2_INTRX_RXIRQ_MASK 0xffffffffU
#define GRSPW2_INTRX_RXIRQ_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTRX_RXIRQ_MASK ) >> \
    GRSPW2_INTRX_RXIRQ_SHIFT )
#define GRSPW2_INTRX_RXIRQ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTRX_RXIRQ_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTRX_RXIRQ_SHIFT ) & \
      GRSPW2_INTRX_RXIRQ_MASK ) )
#define GRSPW2_INTRX_RXIRQ( _val ) \
  ( ( ( _val ) << GRSPW2_INTRX_RXIRQ_SHIFT ) & \
    GRSPW2_INTRX_RXIRQ_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2INTTO Interrupt timeout (INTTO)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_INTTO_INTTO_SHIFT 0
#define GRSPW2_INTTO_INTTO_MASK 0xffffffffU
#define GRSPW2_INTTO_INTTO_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTTO_INTTO_MASK ) >> \
    GRSPW2_INTTO_INTTO_SHIFT )
#define GRSPW2_INTTO_INTTO_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTTO_INTTO_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTTO_INTTO_SHIFT ) & \
      GRSPW2_INTTO_INTTO_MASK ) )
#define GRSPW2_INTTO_INTTO( _val ) \
  ( ( ( _val ) << GRSPW2_INTTO_INTTO_SHIFT ) & \
    GRSPW2_INTTO_INTTO_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2INTTOEXT Interrupt timeout extended (INTTOEXT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_INTTOEXT_INTTOEXT_SHIFT 0
#define GRSPW2_INTTOEXT_INTTOEXT_MASK 0xffffffffU
#define GRSPW2_INTTOEXT_INTTOEXT_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTTOEXT_INTTOEXT_MASK ) >> \
    GRSPW2_INTTOEXT_INTTOEXT_SHIFT )
#define GRSPW2_INTTOEXT_INTTOEXT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTTOEXT_INTTOEXT_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTTOEXT_INTTOEXT_SHIFT ) & \
      GRSPW2_INTTOEXT_INTTOEXT_MASK ) )
#define GRSPW2_INTTOEXT_INTTOEXT( _val ) \
  ( ( ( _val ) << GRSPW2_INTTOEXT_INTTOEXT_SHIFT ) & \
    GRSPW2_INTTOEXT_INTTOEXT_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2TICKMASK Interrupt tick-out mask (TICKMASK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_TICKMASK_MASK_SHIFT 0
#define GRSPW2_TICKMASK_MASK_MASK 0xffffffffU
#define GRSPW2_TICKMASK_MASK_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_TICKMASK_MASK_MASK ) >> \
    GRSPW2_TICKMASK_MASK_SHIFT )
#define GRSPW2_TICKMASK_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_TICKMASK_MASK_MASK ) | \
    ( ( ( _val ) << GRSPW2_TICKMASK_MASK_SHIFT ) & \
      GRSPW2_TICKMASK_MASK_MASK ) )
#define GRSPW2_TICKMASK_MASK( _val ) \
  ( ( ( _val ) << GRSPW2_TICKMASK_MASK_SHIFT ) & \
    GRSPW2_TICKMASK_MASK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2AUTOACKTICKMASKEXT \
 *   Interrupt-code auto acknowledge mask / interrupt tick-out mask extended (AUTOACK_TICKMASKEXT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_AUTOACK_TICKMASKEXT_AAMASK_SHIFT 0
#define GRSPW2_AUTOACK_TICKMASKEXT_AAMASK_MASK 0xffffffffU
#define GRSPW2_AUTOACK_TICKMASKEXT_AAMASK_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_AUTOACK_TICKMASKEXT_AAMASK_MASK ) >> \
    GRSPW2_AUTOACK_TICKMASKEXT_AAMASK_SHIFT )
#define GRSPW2_AUTOACK_TICKMASKEXT_AAMASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_AUTOACK_TICKMASKEXT_AAMASK_MASK ) | \
    ( ( ( _val ) << GRSPW2_AUTOACK_TICKMASKEXT_AAMASK_SHIFT ) & \
      GRSPW2_AUTOACK_TICKMASKEXT_AAMASK_MASK ) )
#define GRSPW2_AUTOACK_TICKMASKEXT_AAMASK( _val ) \
  ( ( ( _val ) << GRSPW2_AUTOACK_TICKMASKEXT_AAMASK_SHIFT ) & \
    GRSPW2_AUTOACK_TICKMASKEXT_AAMASK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2INTCFG \
 *   Interrupt distribution configuration (INTCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_INTCFG_INTNUM3_SHIFT 26
#define GRSPW2_INTCFG_INTNUM3_MASK 0xfc000000U
#define GRSPW2_INTCFG_INTNUM3_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTCFG_INTNUM3_MASK ) >> \
    GRSPW2_INTCFG_INTNUM3_SHIFT )
#define GRSPW2_INTCFG_INTNUM3_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTCFG_INTNUM3_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTCFG_INTNUM3_SHIFT ) & \
      GRSPW2_INTCFG_INTNUM3_MASK ) )
#define GRSPW2_INTCFG_INTNUM3( _val ) \
  ( ( ( _val ) << GRSPW2_INTCFG_INTNUM3_SHIFT ) & \
    GRSPW2_INTCFG_INTNUM3_MASK )

#define GRSPW2_INTCFG_INTNUM2_SHIFT 20
#define GRSPW2_INTCFG_INTNUM2_MASK 0x3f00000U
#define GRSPW2_INTCFG_INTNUM2_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTCFG_INTNUM2_MASK ) >> \
    GRSPW2_INTCFG_INTNUM2_SHIFT )
#define GRSPW2_INTCFG_INTNUM2_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTCFG_INTNUM2_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTCFG_INTNUM2_SHIFT ) & \
      GRSPW2_INTCFG_INTNUM2_MASK ) )
#define GRSPW2_INTCFG_INTNUM2( _val ) \
  ( ( ( _val ) << GRSPW2_INTCFG_INTNUM2_SHIFT ) & \
    GRSPW2_INTCFG_INTNUM2_MASK )

#define GRSPW2_INTCFG_INTNUM1_SHIFT 14
#define GRSPW2_INTCFG_INTNUM1_MASK 0xfc000U
#define GRSPW2_INTCFG_INTNUM1_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTCFG_INTNUM1_MASK ) >> \
    GRSPW2_INTCFG_INTNUM1_SHIFT )
#define GRSPW2_INTCFG_INTNUM1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTCFG_INTNUM1_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTCFG_INTNUM1_SHIFT ) & \
      GRSPW2_INTCFG_INTNUM1_MASK ) )
#define GRSPW2_INTCFG_INTNUM1( _val ) \
  ( ( ( _val ) << GRSPW2_INTCFG_INTNUM1_SHIFT ) & \
    GRSPW2_INTCFG_INTNUM1_MASK )

#define GRSPW2_INTCFG_INTNUM0_SHIFT 8
#define GRSPW2_INTCFG_INTNUM0_MASK 0x3f00U
#define GRSPW2_INTCFG_INTNUM0_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTCFG_INTNUM0_MASK ) >> \
    GRSPW2_INTCFG_INTNUM0_SHIFT )
#define GRSPW2_INTCFG_INTNUM0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTCFG_INTNUM0_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTCFG_INTNUM0_SHIFT ) & \
      GRSPW2_INTCFG_INTNUM0_MASK ) )
#define GRSPW2_INTCFG_INTNUM0( _val ) \
  ( ( ( _val ) << GRSPW2_INTCFG_INTNUM0_SHIFT ) & \
    GRSPW2_INTCFG_INTNUM0_MASK )

#define GRSPW2_INTCFG_NUMINT_SHIFT 4
#define GRSPW2_INTCFG_NUMINT_MASK 0xf0U
#define GRSPW2_INTCFG_NUMINT_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_INTCFG_NUMINT_MASK ) >> \
    GRSPW2_INTCFG_NUMINT_SHIFT )
#define GRSPW2_INTCFG_NUMINT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_INTCFG_NUMINT_MASK ) | \
    ( ( ( _val ) << GRSPW2_INTCFG_NUMINT_SHIFT ) & \
      GRSPW2_INTCFG_NUMINT_MASK ) )
#define GRSPW2_INTCFG_NUMINT( _val ) \
  ( ( ( _val ) << GRSPW2_INTCFG_NUMINT_SHIFT ) & \
    GRSPW2_INTCFG_NUMINT_MASK )

#define GRSPW2_INTCFG_PR 0x8U

#define GRSPW2_INTCFG_IR 0x4U

#define GRSPW2_INTCFG_IT 0x2U

#define GRSPW2_INTCFG_EE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2ISR Interrupt distribution ISR (ISR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_ISR_ISR_SHIFT 0
#define GRSPW2_ISR_ISR_MASK 0xffffffffU
#define GRSPW2_ISR_ISR_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_ISR_ISR_MASK ) >> \
    GRSPW2_ISR_ISR_SHIFT )
#define GRSPW2_ISR_ISR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_ISR_ISR_MASK ) | \
    ( ( ( _val ) << GRSPW2_ISR_ISR_SHIFT ) & \
      GRSPW2_ISR_ISR_MASK ) )
#define GRSPW2_ISR_ISR( _val ) \
  ( ( ( _val ) << GRSPW2_ISR_ISR_SHIFT ) & \
    GRSPW2_ISR_ISR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2ISREXT \
 *   Interrupt distribution ISR extended (ISREXT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_ISREXT_ISR_SHIFT 0
#define GRSPW2_ISREXT_ISR_MASK 0xffffffffU
#define GRSPW2_ISREXT_ISR_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_ISREXT_ISR_MASK ) >> \
    GRSPW2_ISREXT_ISR_SHIFT )
#define GRSPW2_ISREXT_ISR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_ISREXT_ISR_MASK ) | \
    ( ( ( _val ) << GRSPW2_ISREXT_ISR_SHIFT ) & \
      GRSPW2_ISREXT_ISR_MASK ) )
#define GRSPW2_ISREXT_ISR( _val ) \
  ( ( ( _val ) << GRSPW2_ISREXT_ISR_SHIFT ) & \
    GRSPW2_ISREXT_ISR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2PRESCALER \
 *   Interrupt distribution prescaler reload (PRESCALER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_PRESCALER_R 0x80000000U

#define GRSPW2_PRESCALER_RL_SHIFT 0
#define GRSPW2_PRESCALER_RL_MASK 0x7fffffffU
#define GRSPW2_PRESCALER_RL_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_PRESCALER_RL_MASK ) >> \
    GRSPW2_PRESCALER_RL_SHIFT )
#define GRSPW2_PRESCALER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_PRESCALER_RL_MASK ) | \
    ( ( ( _val ) << GRSPW2_PRESCALER_RL_SHIFT ) & \
      GRSPW2_PRESCALER_RL_MASK ) )
#define GRSPW2_PRESCALER_RL( _val ) \
  ( ( ( _val ) << GRSPW2_PRESCALER_RL_SHIFT ) & \
    GRSPW2_PRESCALER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2ISRTIMER \
 *   Interrupt distribution ISR timer reload (ISRTIMER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_ISRTIMER_EN 0x80000000U

#define GRSPW2_ISRTIMER_RL_SHIFT 0
#define GRSPW2_ISRTIMER_RL_MASK 0x7fffffffU
#define GRSPW2_ISRTIMER_RL_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_ISRTIMER_RL_MASK ) >> \
    GRSPW2_ISRTIMER_RL_SHIFT )
#define GRSPW2_ISRTIMER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_ISRTIMER_RL_MASK ) | \
    ( ( ( _val ) << GRSPW2_ISRTIMER_RL_SHIFT ) & \
      GRSPW2_ISRTIMER_RL_MASK ) )
#define GRSPW2_ISRTIMER_RL( _val ) \
  ( ( ( _val ) << GRSPW2_ISRTIMER_RL_SHIFT ) & \
    GRSPW2_ISRTIMER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2IATIMER \
 *   Interrupt distribution INT / ACK timer reload (IATIMER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_IATIMER_EN 0x80000000U

#define GRSPW2_IATIMER_RL_SHIFT 0
#define GRSPW2_IATIMER_RL_MASK 0x7fffffffU
#define GRSPW2_IATIMER_RL_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_IATIMER_RL_MASK ) >> \
    GRSPW2_IATIMER_RL_SHIFT )
#define GRSPW2_IATIMER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_IATIMER_RL_MASK ) | \
    ( ( ( _val ) << GRSPW2_IATIMER_RL_SHIFT ) & \
      GRSPW2_IATIMER_RL_MASK ) )
#define GRSPW2_IATIMER_RL( _val ) \
  ( ( ( _val ) << GRSPW2_IATIMER_RL_SHIFT ) & \
    GRSPW2_IATIMER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2ICTIMER \
 *   Interrupt distribution change timer reload (ICTIMER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_ICTIMER_EN 0x80000000U

#define GRSPW2_ICTIMER_RL_SHIFT 0
#define GRSPW2_ICTIMER_RL_MASK 0x7fffffffU
#define GRSPW2_ICTIMER_RL_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_ICTIMER_RL_MASK ) >> \
    GRSPW2_ICTIMER_RL_SHIFT )
#define GRSPW2_ICTIMER_RL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_ICTIMER_RL_MASK ) | \
    ( ( ( _val ) << GRSPW2_ICTIMER_RL_SHIFT ) & \
      GRSPW2_ICTIMER_RL_MASK ) )
#define GRSPW2_ICTIMER_RL( _val ) \
  ( ( ( _val ) << GRSPW2_ICTIMER_RL_SHIFT ) & \
    GRSPW2_ICTIMER_RL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2PNPVEND \
 *   SpaceWire Plug-and-Play - Device Vendor and Product ID (PNPVEND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_PNPVEND_VEND_SHIFT 16
#define GRSPW2_PNPVEND_VEND_MASK 0xffff0000U
#define GRSPW2_PNPVEND_VEND_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_PNPVEND_VEND_MASK ) >> \
    GRSPW2_PNPVEND_VEND_SHIFT )
#define GRSPW2_PNPVEND_VEND_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_PNPVEND_VEND_MASK ) | \
    ( ( ( _val ) << GRSPW2_PNPVEND_VEND_SHIFT ) & \
      GRSPW2_PNPVEND_VEND_MASK ) )
#define GRSPW2_PNPVEND_VEND( _val ) \
  ( ( ( _val ) << GRSPW2_PNPVEND_VEND_SHIFT ) & \
    GRSPW2_PNPVEND_VEND_MASK )

#define GRSPW2_PNPVEND_PROD_SHIFT 0
#define GRSPW2_PNPVEND_PROD_MASK 0xffffU
#define GRSPW2_PNPVEND_PROD_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_PNPVEND_PROD_MASK ) >> \
    GRSPW2_PNPVEND_PROD_SHIFT )
#define GRSPW2_PNPVEND_PROD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_PNPVEND_PROD_MASK ) | \
    ( ( ( _val ) << GRSPW2_PNPVEND_PROD_SHIFT ) & \
      GRSPW2_PNPVEND_PROD_MASK ) )
#define GRSPW2_PNPVEND_PROD( _val ) \
  ( ( ( _val ) << GRSPW2_PNPVEND_PROD_SHIFT ) & \
    GRSPW2_PNPVEND_PROD_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2PNPOA0 \
 *   SpaceWire Plug-and-Play - Owner Address 0 (PNPOA0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_PNPOA0_RA_SHIFT 0
#define GRSPW2_PNPOA0_RA_MASK 0xffffffffU
#define GRSPW2_PNPOA0_RA_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_PNPOA0_RA_MASK ) >> \
    GRSPW2_PNPOA0_RA_SHIFT )
#define GRSPW2_PNPOA0_RA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_PNPOA0_RA_MASK ) | \
    ( ( ( _val ) << GRSPW2_PNPOA0_RA_SHIFT ) & \
      GRSPW2_PNPOA0_RA_MASK ) )
#define GRSPW2_PNPOA0_RA( _val ) \
  ( ( ( _val ) << GRSPW2_PNPOA0_RA_SHIFT ) & \
    GRSPW2_PNPOA0_RA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2PNPOA1 \
 *   SpaceWire Plug-and-Play - Owner Address 1 (PNPOA1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_PNPOA1_RA_SHIFT 0
#define GRSPW2_PNPOA1_RA_MASK 0xffffffffU
#define GRSPW2_PNPOA1_RA_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_PNPOA1_RA_MASK ) >> \
    GRSPW2_PNPOA1_RA_SHIFT )
#define GRSPW2_PNPOA1_RA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_PNPOA1_RA_MASK ) | \
    ( ( ( _val ) << GRSPW2_PNPOA1_RA_SHIFT ) & \
      GRSPW2_PNPOA1_RA_MASK ) )
#define GRSPW2_PNPOA1_RA( _val ) \
  ( ( ( _val ) << GRSPW2_PNPOA1_RA_SHIFT ) & \
    GRSPW2_PNPOA1_RA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2PNPOA2 \
 *   SpaceWire Plug-and-Play - Owner Address 2 (PNPOA2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_PNPOA2_RA_SHIFT 0
#define GRSPW2_PNPOA2_RA_MASK 0xffffffffU
#define GRSPW2_PNPOA2_RA_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_PNPOA2_RA_MASK ) >> \
    GRSPW2_PNPOA2_RA_SHIFT )
#define GRSPW2_PNPOA2_RA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_PNPOA2_RA_MASK ) | \
    ( ( ( _val ) << GRSPW2_PNPOA2_RA_SHIFT ) & \
      GRSPW2_PNPOA2_RA_MASK ) )
#define GRSPW2_PNPOA2_RA( _val ) \
  ( ( ( _val ) << GRSPW2_PNPOA2_RA_SHIFT ) & \
    GRSPW2_PNPOA2_RA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2PNPDEVID \
 *   SpaceWire Plug-and-Play - Device ID (PNPDEVID)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_PNPDEVID_DID_SHIFT 0
#define GRSPW2_PNPDEVID_DID_MASK 0xffffffffU
#define GRSPW2_PNPDEVID_DID_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_PNPDEVID_DID_MASK ) >> \
    GRSPW2_PNPDEVID_DID_SHIFT )
#define GRSPW2_PNPDEVID_DID_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_PNPDEVID_DID_MASK ) | \
    ( ( ( _val ) << GRSPW2_PNPDEVID_DID_SHIFT ) & \
      GRSPW2_PNPDEVID_DID_MASK ) )
#define GRSPW2_PNPDEVID_DID( _val ) \
  ( ( ( _val ) << GRSPW2_PNPDEVID_DID_SHIFT ) & \
    GRSPW2_PNPDEVID_DID_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2PNPUVEND \
 *   SpaceWire Plug-and-Play - Unit Vendor and Product ID (PNPUVEND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_PNPUVEND_VEND_SHIFT 16
#define GRSPW2_PNPUVEND_VEND_MASK 0xffff0000U
#define GRSPW2_PNPUVEND_VEND_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_PNPUVEND_VEND_MASK ) >> \
    GRSPW2_PNPUVEND_VEND_SHIFT )
#define GRSPW2_PNPUVEND_VEND_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_PNPUVEND_VEND_MASK ) | \
    ( ( ( _val ) << GRSPW2_PNPUVEND_VEND_SHIFT ) & \
      GRSPW2_PNPUVEND_VEND_MASK ) )
#define GRSPW2_PNPUVEND_VEND( _val ) \
  ( ( ( _val ) << GRSPW2_PNPUVEND_VEND_SHIFT ) & \
    GRSPW2_PNPUVEND_VEND_MASK )

#define GRSPW2_PNPUVEND_PROD_SHIFT 0
#define GRSPW2_PNPUVEND_PROD_MASK 0xffffU
#define GRSPW2_PNPUVEND_PROD_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_PNPUVEND_PROD_MASK ) >> \
    GRSPW2_PNPUVEND_PROD_SHIFT )
#define GRSPW2_PNPUVEND_PROD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_PNPUVEND_PROD_MASK ) | \
    ( ( ( _val ) << GRSPW2_PNPUVEND_PROD_SHIFT ) & \
      GRSPW2_PNPUVEND_PROD_MASK ) )
#define GRSPW2_PNPUVEND_PROD( _val ) \
  ( ( ( _val ) << GRSPW2_PNPUVEND_PROD_SHIFT ) & \
    GRSPW2_PNPUVEND_PROD_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPW2PNPUSN \
 *   SpaceWire Plug-and-Play - Unit Serial Number (PNPUSN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRSPW2_PNPUSN_USN_SHIFT 0
#define GRSPW2_PNPUSN_USN_MASK 0xffffffffU
#define GRSPW2_PNPUSN_USN_GET( _reg ) \
  ( ( ( _reg ) & GRSPW2_PNPUSN_USN_MASK ) >> \
    GRSPW2_PNPUSN_USN_SHIFT )
#define GRSPW2_PNPUSN_USN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRSPW2_PNPUSN_USN_MASK ) | \
    ( ( ( _val ) << GRSPW2_PNPUSN_USN_SHIFT ) & \
      GRSPW2_PNPUSN_USN_MASK ) )
#define GRSPW2_PNPUSN_USN( _val ) \
  ( ( ( _val ) << GRSPW2_PNPUSN_USN_SHIFT ) & \
    GRSPW2_PNPUSN_USN_MASK )

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
   * @brief See @ref RTEMSDeviceGRSPW2DMA.
   */
  grspw2_dma dma[ 4 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPW2INTCTRL.
   */
  uint32_t intctrl;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2INTRX.
   */
  uint32_t intrx;

  uint32_t reserved_a8_ac;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2INTTO.
   */
  uint32_t intto;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2INTTOEXT.
   */
  uint32_t inttoext;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2TICKMASK.
   */
  uint32_t tickmask;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2AUTOACKTICKMASKEXT.
   */
  uint32_t autoack_tickmaskext;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2INTCFG.
   */
  uint32_t intcfg;

  uint32_t reserved_c0_c4;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2ISR.
   */
  uint32_t isr;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2ISREXT.
   */
  uint32_t isrext;

  uint32_t reserved_cc_d0;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2PRESCALER.
   */
  uint32_t prescaler;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2ISRTIMER.
   */
  uint32_t isrtimer;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2IATIMER.
   */
  uint32_t iatimer;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2ICTIMER.
   */
  uint32_t ictimer;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2PNPVEND.
   */
  uint32_t pnpvend;

  uint32_t reserved_e4_e8;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2PNPOA0.
   */
  uint32_t pnpoa0;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2PNPOA1.
   */
  uint32_t pnpoa1;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2PNPOA2.
   */
  uint32_t pnpoa2;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2PNPDEVID.
   */
  uint32_t pnpdevid;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2PNPUVEND.
   */
  uint32_t pnpuvend;

  /**
   * @brief See @ref RTEMSDeviceGRSPW2PNPUSN.
   */
  uint32_t pnpusn;
} grspw2;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRSPW2_REGS_H */
