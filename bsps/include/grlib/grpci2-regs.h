/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRPCI2
 *
 * @brief This header file defines the GRPCI2 register block interface.
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

/* Generated from spec:/dev/grlib/if/grpci2-header */

#ifndef _GRLIB_GRPCI2_REGS_H
#define _GRLIB_GRPCI2_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grpci2 */

/**
 * @defgroup RTEMSDeviceGRPCI2 GRPCI2
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRPCI2 interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRPCI2CTRL Control register (CTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_CTRL_RE 0x80000000U

#define GRPCI2_CTRL_MR 0x40000000U

#define GRPCI2_CTRL_TR 0x20000000U

#define GRPCI2_CTRL_SI 0x8000000U

#define GRPCI2_CTRL_PE 0x4000000U

#define GRPCI2_CTRL_ER 0x2000000U

#define GRPCI2_CTRL_EI 0x1000000U

#define GRPCI2_CTRL_BUS_NUMBER_SHIFT 16
#define GRPCI2_CTRL_BUS_NUMBER_MASK 0xff0000U
#define GRPCI2_CTRL_BUS_NUMBER_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_CTRL_BUS_NUMBER_MASK ) >> \
    GRPCI2_CTRL_BUS_NUMBER_SHIFT )
#define GRPCI2_CTRL_BUS_NUMBER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_CTRL_BUS_NUMBER_MASK ) | \
    ( ( ( _val ) << GRPCI2_CTRL_BUS_NUMBER_SHIFT ) & \
      GRPCI2_CTRL_BUS_NUMBER_MASK ) )
#define GRPCI2_CTRL_BUS_NUMBER( _val ) \
  ( ( ( _val ) << GRPCI2_CTRL_BUS_NUMBER_SHIFT ) & \
    GRPCI2_CTRL_BUS_NUMBER_MASK )

#define GRPCI2_CTRL_DFA 0x800U

#define GRPCI2_CTRL_IB 0x400U

#define GRPCI2_CTRL_CB 0x200U

#define GRPCI2_CTRL_DIF 0x100U

#define GRPCI2_CTRL_DEVICE_INT_MASK_SHIFT 4
#define GRPCI2_CTRL_DEVICE_INT_MASK_MASK 0xf0U
#define GRPCI2_CTRL_DEVICE_INT_MASK_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_CTRL_DEVICE_INT_MASK_MASK ) >> \
    GRPCI2_CTRL_DEVICE_INT_MASK_SHIFT )
#define GRPCI2_CTRL_DEVICE_INT_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_CTRL_DEVICE_INT_MASK_MASK ) | \
    ( ( ( _val ) << GRPCI2_CTRL_DEVICE_INT_MASK_SHIFT ) & \
      GRPCI2_CTRL_DEVICE_INT_MASK_MASK ) )
#define GRPCI2_CTRL_DEVICE_INT_MASK( _val ) \
  ( ( ( _val ) << GRPCI2_CTRL_DEVICE_INT_MASK_SHIFT ) & \
    GRPCI2_CTRL_DEVICE_INT_MASK_MASK )

#define GRPCI2_CTRL_HOST_INT_MASK_SHIFT 0
#define GRPCI2_CTRL_HOST_INT_MASK_MASK 0xfU
#define GRPCI2_CTRL_HOST_INT_MASK_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_CTRL_HOST_INT_MASK_MASK ) >> \
    GRPCI2_CTRL_HOST_INT_MASK_SHIFT )
#define GRPCI2_CTRL_HOST_INT_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_CTRL_HOST_INT_MASK_MASK ) | \
    ( ( ( _val ) << GRPCI2_CTRL_HOST_INT_MASK_SHIFT ) & \
      GRPCI2_CTRL_HOST_INT_MASK_MASK ) )
#define GRPCI2_CTRL_HOST_INT_MASK( _val ) \
  ( ( ( _val ) << GRPCI2_CTRL_HOST_INT_MASK_SHIFT ) & \
    GRPCI2_CTRL_HOST_INT_MASK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2STATCAP Status and Capability register (STATCAP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_STATCAP_HOST 0x80000000U

#define GRPCI2_STATCAP_MST 0x40000000U

#define GRPCI2_STATCAP_TAR 0x20000000U

#define GRPCI2_STATCAP_DMA 0x10000000U

#define GRPCI2_STATCAP_DI 0x8000000U

#define GRPCI2_STATCAP_HI 0x4000000U

#define GRPCI2_STATCAP_IRQ_MODE_SHIFT 24
#define GRPCI2_STATCAP_IRQ_MODE_MASK 0x3000000U
#define GRPCI2_STATCAP_IRQ_MODE_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_STATCAP_IRQ_MODE_MASK ) >> \
    GRPCI2_STATCAP_IRQ_MODE_SHIFT )
#define GRPCI2_STATCAP_IRQ_MODE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_STATCAP_IRQ_MODE_MASK ) | \
    ( ( ( _val ) << GRPCI2_STATCAP_IRQ_MODE_SHIFT ) & \
      GRPCI2_STATCAP_IRQ_MODE_MASK ) )
#define GRPCI2_STATCAP_IRQ_MODE( _val ) \
  ( ( ( _val ) << GRPCI2_STATCAP_IRQ_MODE_SHIFT ) & \
    GRPCI2_STATCAP_IRQ_MODE_MASK )

#define GRPCI2_STATCAP_TRACE 0x800000U

#define GRPCI2_STATCAP_CFGDO 0x100000U

#define GRPCI2_STATCAP_CFGER 0x80000U

#define GRPCI2_STATCAP_CORE_INT_STATUS_SHIFT 12
#define GRPCI2_STATCAP_CORE_INT_STATUS_MASK 0x7f000U
#define GRPCI2_STATCAP_CORE_INT_STATUS_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_STATCAP_CORE_INT_STATUS_MASK ) >> \
    GRPCI2_STATCAP_CORE_INT_STATUS_SHIFT )
#define GRPCI2_STATCAP_CORE_INT_STATUS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_STATCAP_CORE_INT_STATUS_MASK ) | \
    ( ( ( _val ) << GRPCI2_STATCAP_CORE_INT_STATUS_SHIFT ) & \
      GRPCI2_STATCAP_CORE_INT_STATUS_MASK ) )
#define GRPCI2_STATCAP_CORE_INT_STATUS( _val ) \
  ( ( ( _val ) << GRPCI2_STATCAP_CORE_INT_STATUS_SHIFT ) & \
    GRPCI2_STATCAP_CORE_INT_STATUS_MASK )

#define GRPCI2_STATCAP_HOST_INT_STATUS_SHIFT 8
#define GRPCI2_STATCAP_HOST_INT_STATUS_MASK 0xf00U
#define GRPCI2_STATCAP_HOST_INT_STATUS_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_STATCAP_HOST_INT_STATUS_MASK ) >> \
    GRPCI2_STATCAP_HOST_INT_STATUS_SHIFT )
#define GRPCI2_STATCAP_HOST_INT_STATUS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_STATCAP_HOST_INT_STATUS_MASK ) | \
    ( ( ( _val ) << GRPCI2_STATCAP_HOST_INT_STATUS_SHIFT ) & \
      GRPCI2_STATCAP_HOST_INT_STATUS_MASK ) )
#define GRPCI2_STATCAP_HOST_INT_STATUS( _val ) \
  ( ( ( _val ) << GRPCI2_STATCAP_HOST_INT_STATUS_SHIFT ) & \
    GRPCI2_STATCAP_HOST_INT_STATUS_MASK )

#define GRPCI2_STATCAP_FDEPTH_SHIFT 2
#define GRPCI2_STATCAP_FDEPTH_MASK 0x1cU
#define GRPCI2_STATCAP_FDEPTH_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_STATCAP_FDEPTH_MASK ) >> \
    GRPCI2_STATCAP_FDEPTH_SHIFT )
#define GRPCI2_STATCAP_FDEPTH_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_STATCAP_FDEPTH_MASK ) | \
    ( ( ( _val ) << GRPCI2_STATCAP_FDEPTH_SHIFT ) & \
      GRPCI2_STATCAP_FDEPTH_MASK ) )
#define GRPCI2_STATCAP_FDEPTH( _val ) \
  ( ( ( _val ) << GRPCI2_STATCAP_FDEPTH_SHIFT ) & \
    GRPCI2_STATCAP_FDEPTH_MASK )

#define GRPCI2_STATCAP_FNUM_SHIFT 0
#define GRPCI2_STATCAP_FNUM_MASK 0x3U
#define GRPCI2_STATCAP_FNUM_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_STATCAP_FNUM_MASK ) >> \
    GRPCI2_STATCAP_FNUM_SHIFT )
#define GRPCI2_STATCAP_FNUM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_STATCAP_FNUM_MASK ) | \
    ( ( ( _val ) << GRPCI2_STATCAP_FNUM_SHIFT ) & \
      GRPCI2_STATCAP_FNUM_MASK ) )
#define GRPCI2_STATCAP_FNUM( _val ) \
  ( ( ( _val ) << GRPCI2_STATCAP_FNUM_SHIFT ) & \
    GRPCI2_STATCAP_FNUM_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2BCIM PCI master prefetch burst limit (BCIM)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_BCIM_AHB_MASTER_UNMASK_SHIFT 16
#define GRPCI2_BCIM_AHB_MASTER_UNMASK_MASK 0xffff0000U
#define GRPCI2_BCIM_AHB_MASTER_UNMASK_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_BCIM_AHB_MASTER_UNMASK_MASK ) >> \
    GRPCI2_BCIM_AHB_MASTER_UNMASK_SHIFT )
#define GRPCI2_BCIM_AHB_MASTER_UNMASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_BCIM_AHB_MASTER_UNMASK_MASK ) | \
    ( ( ( _val ) << GRPCI2_BCIM_AHB_MASTER_UNMASK_SHIFT ) & \
      GRPCI2_BCIM_AHB_MASTER_UNMASK_MASK ) )
#define GRPCI2_BCIM_AHB_MASTER_UNMASK( _val ) \
  ( ( ( _val ) << GRPCI2_BCIM_AHB_MASTER_UNMASK_SHIFT ) & \
    GRPCI2_BCIM_AHB_MASTER_UNMASK_MASK )

#define GRPCI2_BCIM_BURST_LENGTH_SHIFT 0
#define GRPCI2_BCIM_BURST_LENGTH_MASK 0xffU
#define GRPCI2_BCIM_BURST_LENGTH_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_BCIM_BURST_LENGTH_MASK ) >> \
    GRPCI2_BCIM_BURST_LENGTH_SHIFT )
#define GRPCI2_BCIM_BURST_LENGTH_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_BCIM_BURST_LENGTH_MASK ) | \
    ( ( ( _val ) << GRPCI2_BCIM_BURST_LENGTH_SHIFT ) & \
      GRPCI2_BCIM_BURST_LENGTH_MASK ) )
#define GRPCI2_BCIM_BURST_LENGTH( _val ) \
  ( ( ( _val ) << GRPCI2_BCIM_BURST_LENGTH_SHIFT ) & \
    GRPCI2_BCIM_BURST_LENGTH_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2AHB2PCI AHB to PCI mapping for PCI IO (AHB2PCI)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_AHB2PCI_AHB_TO_PCI_IO_SHIFT 16
#define GRPCI2_AHB2PCI_AHB_TO_PCI_IO_MASK 0xffff0000U
#define GRPCI2_AHB2PCI_AHB_TO_PCI_IO_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_AHB2PCI_AHB_TO_PCI_IO_MASK ) >> \
    GRPCI2_AHB2PCI_AHB_TO_PCI_IO_SHIFT )
#define GRPCI2_AHB2PCI_AHB_TO_PCI_IO_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_AHB2PCI_AHB_TO_PCI_IO_MASK ) | \
    ( ( ( _val ) << GRPCI2_AHB2PCI_AHB_TO_PCI_IO_SHIFT ) & \
      GRPCI2_AHB2PCI_AHB_TO_PCI_IO_MASK ) )
#define GRPCI2_AHB2PCI_AHB_TO_PCI_IO( _val ) \
  ( ( ( _val ) << GRPCI2_AHB2PCI_AHB_TO_PCI_IO_SHIFT ) & \
    GRPCI2_AHB2PCI_AHB_TO_PCI_IO_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2DMACTRL DMA control and status register (DMACTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_DMACTRL_SAFE 0x80000000U

#define GRPCI2_DMACTRL_CHIRQ_SHIFT 12
#define GRPCI2_DMACTRL_CHIRQ_MASK 0xff000U
#define GRPCI2_DMACTRL_CHIRQ_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_DMACTRL_CHIRQ_MASK ) >> \
    GRPCI2_DMACTRL_CHIRQ_SHIFT )
#define GRPCI2_DMACTRL_CHIRQ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_DMACTRL_CHIRQ_MASK ) | \
    ( ( ( _val ) << GRPCI2_DMACTRL_CHIRQ_SHIFT ) & \
      GRPCI2_DMACTRL_CHIRQ_MASK ) )
#define GRPCI2_DMACTRL_CHIRQ( _val ) \
  ( ( ( _val ) << GRPCI2_DMACTRL_CHIRQ_SHIFT ) & \
    GRPCI2_DMACTRL_CHIRQ_MASK )

#define GRPCI2_DMACTRL_MA 0x800U

#define GRPCI2_DMACTRL_TA 0x400U

#define GRPCI2_DMACTRL_PE 0x200U

#define GRPCI2_DMACTRL_AE 0x100U

#define GRPCI2_DMACTRL_DE 0x80U

#define GRPCI2_DMACTRL_NUMCH_SHIFT 4
#define GRPCI2_DMACTRL_NUMCH_MASK 0x70U
#define GRPCI2_DMACTRL_NUMCH_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_DMACTRL_NUMCH_MASK ) >> \
    GRPCI2_DMACTRL_NUMCH_SHIFT )
#define GRPCI2_DMACTRL_NUMCH_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_DMACTRL_NUMCH_MASK ) | \
    ( ( ( _val ) << GRPCI2_DMACTRL_NUMCH_SHIFT ) & \
      GRPCI2_DMACTRL_NUMCH_MASK ) )
#define GRPCI2_DMACTRL_NUMCH( _val ) \
  ( ( ( _val ) << GRPCI2_DMACTRL_NUMCH_SHIFT ) & \
    GRPCI2_DMACTRL_NUMCH_MASK )

#define GRPCI2_DMACTRL_ACTIVE 0x8U

#define GRPCI2_DMACTRL_DIS 0x4U

#define GRPCI2_DMACTRL_IE 0x2U

#define GRPCI2_DMACTRL_EN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2DMABASE \
 *   DMA descriptor base address register (DMABASE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_DMABASE_BASE_SHIFT 0
#define GRPCI2_DMABASE_BASE_MASK 0xffffffffU
#define GRPCI2_DMABASE_BASE_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_DMABASE_BASE_MASK ) >> \
    GRPCI2_DMABASE_BASE_SHIFT )
#define GRPCI2_DMABASE_BASE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_DMABASE_BASE_MASK ) | \
    ( ( ( _val ) << GRPCI2_DMABASE_BASE_SHIFT ) & \
      GRPCI2_DMABASE_BASE_MASK ) )
#define GRPCI2_DMABASE_BASE( _val ) \
  ( ( ( _val ) << GRPCI2_DMABASE_BASE_SHIFT ) & \
    GRPCI2_DMABASE_BASE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2DMACHAN DMA channel active register (DMACHAN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_DMACHAN_CHAN_SHIFT 0
#define GRPCI2_DMACHAN_CHAN_MASK 0xffffffffU
#define GRPCI2_DMACHAN_CHAN_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_DMACHAN_CHAN_MASK ) >> \
    GRPCI2_DMACHAN_CHAN_SHIFT )
#define GRPCI2_DMACHAN_CHAN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_DMACHAN_CHAN_MASK ) | \
    ( ( ( _val ) << GRPCI2_DMACHAN_CHAN_SHIFT ) & \
      GRPCI2_DMACHAN_CHAN_MASK ) )
#define GRPCI2_DMACHAN_CHAN( _val ) \
  ( ( ( _val ) << GRPCI2_DMACHAN_CHAN_SHIFT ) & \
    GRPCI2_DMACHAN_CHAN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2PCI2AHB \
 *   PCI BAR to AHB address mapping register (PCI2AHB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_PCI2AHB_ADDR_SHIFT 0
#define GRPCI2_PCI2AHB_ADDR_MASK 0xffffffffU
#define GRPCI2_PCI2AHB_ADDR_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_PCI2AHB_ADDR_MASK ) >> \
    GRPCI2_PCI2AHB_ADDR_SHIFT )
#define GRPCI2_PCI2AHB_ADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_PCI2AHB_ADDR_MASK ) | \
    ( ( ( _val ) << GRPCI2_PCI2AHB_ADDR_SHIFT ) & \
      GRPCI2_PCI2AHB_ADDR_MASK ) )
#define GRPCI2_PCI2AHB_ADDR( _val ) \
  ( ( ( _val ) << GRPCI2_PCI2AHB_ADDR_SHIFT ) & \
    GRPCI2_PCI2AHB_ADDR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2AHBM2PCI \
 *   AHB master to PCI memory address mapping register (AHBM2PCI)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_AHBM2PCI_ADDR_SHIFT 0
#define GRPCI2_AHBM2PCI_ADDR_MASK 0xffffffffU
#define GRPCI2_AHBM2PCI_ADDR_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_AHBM2PCI_ADDR_MASK ) >> \
    GRPCI2_AHBM2PCI_ADDR_SHIFT )
#define GRPCI2_AHBM2PCI_ADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_AHBM2PCI_ADDR_MASK ) | \
    ( ( ( _val ) << GRPCI2_AHBM2PCI_ADDR_SHIFT ) & \
      GRPCI2_AHBM2PCI_ADDR_MASK ) )
#define GRPCI2_AHBM2PCI_ADDR( _val ) \
  ( ( ( _val ) << GRPCI2_AHBM2PCI_ADDR_SHIFT ) & \
    GRPCI2_AHBM2PCI_ADDR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2TCTRC \
 *   PCI trace Control and Status register (TCTRC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_TCTRC_TRIG_INDEX_SHIFT 16
#define GRPCI2_TCTRC_TRIG_INDEX_MASK 0xffff0000U
#define GRPCI2_TCTRC_TRIG_INDEX_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_TCTRC_TRIG_INDEX_MASK ) >> \
    GRPCI2_TCTRC_TRIG_INDEX_SHIFT )
#define GRPCI2_TCTRC_TRIG_INDEX_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_TCTRC_TRIG_INDEX_MASK ) | \
    ( ( ( _val ) << GRPCI2_TCTRC_TRIG_INDEX_SHIFT ) & \
      GRPCI2_TCTRC_TRIG_INDEX_MASK ) )
#define GRPCI2_TCTRC_TRIG_INDEX( _val ) \
  ( ( ( _val ) << GRPCI2_TCTRC_TRIG_INDEX_SHIFT ) & \
    GRPCI2_TCTRC_TRIG_INDEX_MASK )

#define GRPCI2_TCTRC_AR 0x8000U

#define GRPCI2_TCTRC_EN 0x4000U

#define GRPCI2_TCTRC_DEPTH_SHIFT 4
#define GRPCI2_TCTRC_DEPTH_MASK 0xff0U
#define GRPCI2_TCTRC_DEPTH_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_TCTRC_DEPTH_MASK ) >> \
    GRPCI2_TCTRC_DEPTH_SHIFT )
#define GRPCI2_TCTRC_DEPTH_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_TCTRC_DEPTH_MASK ) | \
    ( ( ( _val ) << GRPCI2_TCTRC_DEPTH_SHIFT ) & \
      GRPCI2_TCTRC_DEPTH_MASK ) )
#define GRPCI2_TCTRC_DEPTH( _val ) \
  ( ( ( _val ) << GRPCI2_TCTRC_DEPTH_SHIFT ) & \
    GRPCI2_TCTRC_DEPTH_MASK )

#define GRPCI2_TCTRC_SO 0x2U

#define GRPCI2_TCTRC_SA 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2TMODE PCI trace counter and mode register (TMODE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_TMODE_TRACING_MODE_SHIFT 24
#define GRPCI2_TMODE_TRACING_MODE_MASK 0xf000000U
#define GRPCI2_TMODE_TRACING_MODE_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_TMODE_TRACING_MODE_MASK ) >> \
    GRPCI2_TMODE_TRACING_MODE_SHIFT )
#define GRPCI2_TMODE_TRACING_MODE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_TMODE_TRACING_MODE_MASK ) | \
    ( ( ( _val ) << GRPCI2_TMODE_TRACING_MODE_SHIFT ) & \
      GRPCI2_TMODE_TRACING_MODE_MASK ) )
#define GRPCI2_TMODE_TRACING_MODE( _val ) \
  ( ( ( _val ) << GRPCI2_TMODE_TRACING_MODE_SHIFT ) & \
    GRPCI2_TMODE_TRACING_MODE_MASK )

#define GRPCI2_TMODE_TRIG_COUNT_SHIFT 16
#define GRPCI2_TMODE_TRIG_COUNT_MASK 0xff0000U
#define GRPCI2_TMODE_TRIG_COUNT_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_TMODE_TRIG_COUNT_MASK ) >> \
    GRPCI2_TMODE_TRIG_COUNT_SHIFT )
#define GRPCI2_TMODE_TRIG_COUNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_TMODE_TRIG_COUNT_MASK ) | \
    ( ( ( _val ) << GRPCI2_TMODE_TRIG_COUNT_SHIFT ) & \
      GRPCI2_TMODE_TRIG_COUNT_MASK ) )
#define GRPCI2_TMODE_TRIG_COUNT( _val ) \
  ( ( ( _val ) << GRPCI2_TMODE_TRIG_COUNT_SHIFT ) & \
    GRPCI2_TMODE_TRIG_COUNT_MASK )

#define GRPCI2_TMODE_DELAYED_STOP_SHIFT 0
#define GRPCI2_TMODE_DELAYED_STOP_MASK 0xffffU
#define GRPCI2_TMODE_DELAYED_STOP_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_TMODE_DELAYED_STOP_MASK ) >> \
    GRPCI2_TMODE_DELAYED_STOP_SHIFT )
#define GRPCI2_TMODE_DELAYED_STOP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_TMODE_DELAYED_STOP_MASK ) | \
    ( ( ( _val ) << GRPCI2_TMODE_DELAYED_STOP_SHIFT ) & \
      GRPCI2_TMODE_DELAYED_STOP_MASK ) )
#define GRPCI2_TMODE_DELAYED_STOP( _val ) \
  ( ( ( _val ) << GRPCI2_TMODE_DELAYED_STOP_SHIFT ) & \
    GRPCI2_TMODE_DELAYED_STOP_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2TADP PCI trace AD pattern register (TADP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_TADP_PATTERN_SHIFT 0
#define GRPCI2_TADP_PATTERN_MASK 0xffffffffU
#define GRPCI2_TADP_PATTERN_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_TADP_PATTERN_MASK ) >> \
    GRPCI2_TADP_PATTERN_SHIFT )
#define GRPCI2_TADP_PATTERN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_TADP_PATTERN_MASK ) | \
    ( ( ( _val ) << GRPCI2_TADP_PATTERN_SHIFT ) & \
      GRPCI2_TADP_PATTERN_MASK ) )
#define GRPCI2_TADP_PATTERN( _val ) \
  ( ( ( _val ) << GRPCI2_TADP_PATTERN_SHIFT ) & \
    GRPCI2_TADP_PATTERN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2TADM PCI trace AD mask register (TADM)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_TADM_MASK_SHIFT 0
#define GRPCI2_TADM_MASK_MASK 0xffffffffU
#define GRPCI2_TADM_MASK_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_TADM_MASK_MASK ) >> \
    GRPCI2_TADM_MASK_SHIFT )
#define GRPCI2_TADM_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_TADM_MASK_MASK ) | \
    ( ( ( _val ) << GRPCI2_TADM_MASK_SHIFT ) & \
      GRPCI2_TADM_MASK_MASK ) )
#define GRPCI2_TADM_MASK( _val ) \
  ( ( ( _val ) << GRPCI2_TADM_MASK_SHIFT ) & \
    GRPCI2_TADM_MASK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2TCP PCI trace Ctrl signal pattern register (TCP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_TCP_CBE_3_0_SHIFT 16
#define GRPCI2_TCP_CBE_3_0_MASK 0xf0000U
#define GRPCI2_TCP_CBE_3_0_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_TCP_CBE_3_0_MASK ) >> \
    GRPCI2_TCP_CBE_3_0_SHIFT )
#define GRPCI2_TCP_CBE_3_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_TCP_CBE_3_0_MASK ) | \
    ( ( ( _val ) << GRPCI2_TCP_CBE_3_0_SHIFT ) & \
      GRPCI2_TCP_CBE_3_0_MASK ) )
#define GRPCI2_TCP_CBE_3_0( _val ) \
  ( ( ( _val ) << GRPCI2_TCP_CBE_3_0_SHIFT ) & \
    GRPCI2_TCP_CBE_3_0_MASK )

#define GRPCI2_TCP_FRAME 0x8000U

#define GRPCI2_TCP_IRDY 0x4000U

#define GRPCI2_TCP_TRDY 0x2000U

#define GRPCI2_TCP_STOP 0x1000U

#define GRPCI2_TCP_DEVSEL 0x800U

#define GRPCI2_TCP_PAR 0x400U

#define GRPCI2_TCP_PERR 0x200U

#define GRPCI2_TCP_SERR 0x100U

#define GRPCI2_TCP_IDSEL 0x80U

#define GRPCI2_TCP_REQ 0x40U

#define GRPCI2_TCP_GNT 0x20U

#define GRPCI2_TCP_LOCK 0x10U

#define GRPCI2_TCP_RST 0x8U

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2TCM PCI trace Ctrl signal mask register (TCM)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_TCM_CBE_3_0_SHIFT 16
#define GRPCI2_TCM_CBE_3_0_MASK 0xf0000U
#define GRPCI2_TCM_CBE_3_0_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_TCM_CBE_3_0_MASK ) >> \
    GRPCI2_TCM_CBE_3_0_SHIFT )
#define GRPCI2_TCM_CBE_3_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_TCM_CBE_3_0_MASK ) | \
    ( ( ( _val ) << GRPCI2_TCM_CBE_3_0_SHIFT ) & \
      GRPCI2_TCM_CBE_3_0_MASK ) )
#define GRPCI2_TCM_CBE_3_0( _val ) \
  ( ( ( _val ) << GRPCI2_TCM_CBE_3_0_SHIFT ) & \
    GRPCI2_TCM_CBE_3_0_MASK )

#define GRPCI2_TCM_FRAME 0x8000U

#define GRPCI2_TCM_IRDY 0x4000U

#define GRPCI2_TCM_TRDY 0x2000U

#define GRPCI2_TCM_STOP 0x1000U

#define GRPCI2_TCM_DEVSEL 0x800U

#define GRPCI2_TCM_PAR 0x400U

#define GRPCI2_TCM_PERR 0x200U

#define GRPCI2_TCM_SERR 0x100U

#define GRPCI2_TCM_IDSEL 0x80U

#define GRPCI2_TCM_REQ 0x40U

#define GRPCI2_TCM_GNT 0x20U

#define GRPCI2_TCM_LOCK 0x10U

#define GRPCI2_TCM_RST 0x8U

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2TADS PCI trace PCI AD state register (TADS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_TADS_SIGNAL_SHIFT 0
#define GRPCI2_TADS_SIGNAL_MASK 0xffffffffU
#define GRPCI2_TADS_SIGNAL_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_TADS_SIGNAL_MASK ) >> \
    GRPCI2_TADS_SIGNAL_SHIFT )
#define GRPCI2_TADS_SIGNAL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_TADS_SIGNAL_MASK ) | \
    ( ( ( _val ) << GRPCI2_TADS_SIGNAL_SHIFT ) & \
      GRPCI2_TADS_SIGNAL_MASK ) )
#define GRPCI2_TADS_SIGNAL( _val ) \
  ( ( ( _val ) << GRPCI2_TADS_SIGNAL_SHIFT ) & \
    GRPCI2_TADS_SIGNAL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRPCI2TCS \
 *   PCI trace PCI Ctrl signal state register (TCS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRPCI2_TCS_CBE_3_0_SHIFT 16
#define GRPCI2_TCS_CBE_3_0_MASK 0xf0000U
#define GRPCI2_TCS_CBE_3_0_GET( _reg ) \
  ( ( ( _reg ) & GRPCI2_TCS_CBE_3_0_MASK ) >> \
    GRPCI2_TCS_CBE_3_0_SHIFT )
#define GRPCI2_TCS_CBE_3_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRPCI2_TCS_CBE_3_0_MASK ) | \
    ( ( ( _val ) << GRPCI2_TCS_CBE_3_0_SHIFT ) & \
      GRPCI2_TCS_CBE_3_0_MASK ) )
#define GRPCI2_TCS_CBE_3_0( _val ) \
  ( ( ( _val ) << GRPCI2_TCS_CBE_3_0_SHIFT ) & \
    GRPCI2_TCS_CBE_3_0_MASK )

#define GRPCI2_TCS_FRAME 0x8000U

#define GRPCI2_TCS_IRDY 0x4000U

#define GRPCI2_TCS_TRDY 0x2000U

#define GRPCI2_TCS_STOP 0x1000U

#define GRPCI2_TCS_DEVSEL 0x800U

#define GRPCI2_TCS_PAR 0x400U

#define GRPCI2_TCS_PERR 0x200U

#define GRPCI2_TCS_SERR 0x100U

#define GRPCI2_TCS_IDSEL 0x80U

#define GRPCI2_TCS_REQ 0x40U

#define GRPCI2_TCS_GNT 0x20U

#define GRPCI2_TCS_LOCK 0x10U

#define GRPCI2_TCS_RST 0x8U

/** @} */

/**
 * @brief This structure defines the GRPCI2 register block memory map.
 */
typedef struct grpci2 {
  /**
   * @brief See @ref RTEMSDeviceGRPCI2CTRL.
   */
  uint32_t ctrl;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2STATCAP.
   */
  uint32_t statcap;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2BCIM.
   */
  uint32_t bcim;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2AHB2PCI.
   */
  uint32_t ahb2pci;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2DMACTRL.
   */
  uint32_t dmactrl;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2DMABASE.
   */
  uint32_t dmabase;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2DMACHAN.
   */
  uint32_t dmachan;

  uint32_t reserved_1c_20;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2PCI2AHB.
   */
  uint32_t pci2ahb_0;

  uint32_t reserved_24_34[ 4 ];

  /**
   * @brief See @ref RTEMSDeviceGRPCI2PCI2AHB.
   */
  uint32_t pci2ahb_1;

  uint32_t reserved_38_40[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRPCI2AHBM2PCI.
   */
  uint32_t ahbm2pci_0;

  uint32_t reserved_44_7c[ 14 ];

  /**
   * @brief See @ref RTEMSDeviceGRPCI2AHBM2PCI.
   */
  uint32_t ahbm2pci_1;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2TCTRC.
   */
  uint32_t tctrc;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2TMODE.
   */
  uint32_t tmode;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2TADP.
   */
  uint32_t tadp;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2TADM.
   */
  uint32_t tadm;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2TCP.
   */
  uint32_t tcp;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2TCM.
   */
  uint32_t tcm;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2TADS.
   */
  uint32_t tads;

  /**
   * @brief See @ref RTEMSDeviceGRPCI2TCS.
   */
  uint32_t tcs;
} grpci2;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRPCI2_REGS_H */
