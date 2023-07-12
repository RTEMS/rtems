/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRCAN
 *
 * @brief This header file defines the GRCAN register block interface.
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

/* Generated from spec:/dev/grlib/if/grcan-header */

#ifndef _GRLIB_GRCAN_REGS_H
#define _GRLIB_GRCAN_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grcan */

/**
 * @defgroup RTEMSDeviceGRCAN GRCAN
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRCAN interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRCANCanCONF Configuration Register (CanCONF)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANCONF_SCALER_SHIFT 24
#define GRCAN_CANCONF_SCALER_MASK 0xff000000U
#define GRCAN_CANCONF_SCALER_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANCONF_SCALER_MASK ) >> \
    GRCAN_CANCONF_SCALER_SHIFT )
#define GRCAN_CANCONF_SCALER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANCONF_SCALER_MASK ) | \
    ( ( ( _val ) << GRCAN_CANCONF_SCALER_SHIFT ) & \
      GRCAN_CANCONF_SCALER_MASK ) )
#define GRCAN_CANCONF_SCALER( _val ) \
  ( ( ( _val ) << GRCAN_CANCONF_SCALER_SHIFT ) & \
    GRCAN_CANCONF_SCALER_MASK )

#define GRCAN_CANCONF_PS1_SHIFT 20
#define GRCAN_CANCONF_PS1_MASK 0xf00000U
#define GRCAN_CANCONF_PS1_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANCONF_PS1_MASK ) >> \
    GRCAN_CANCONF_PS1_SHIFT )
#define GRCAN_CANCONF_PS1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANCONF_PS1_MASK ) | \
    ( ( ( _val ) << GRCAN_CANCONF_PS1_SHIFT ) & \
      GRCAN_CANCONF_PS1_MASK ) )
#define GRCAN_CANCONF_PS1( _val ) \
  ( ( ( _val ) << GRCAN_CANCONF_PS1_SHIFT ) & \
    GRCAN_CANCONF_PS1_MASK )

#define GRCAN_CANCONF_PS2_SHIFT 16
#define GRCAN_CANCONF_PS2_MASK 0xf0000U
#define GRCAN_CANCONF_PS2_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANCONF_PS2_MASK ) >> \
    GRCAN_CANCONF_PS2_SHIFT )
#define GRCAN_CANCONF_PS2_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANCONF_PS2_MASK ) | \
    ( ( ( _val ) << GRCAN_CANCONF_PS2_SHIFT ) & \
      GRCAN_CANCONF_PS2_MASK ) )
#define GRCAN_CANCONF_PS2( _val ) \
  ( ( ( _val ) << GRCAN_CANCONF_PS2_SHIFT ) & \
    GRCAN_CANCONF_PS2_MASK )

#define GRCAN_CANCONF_RSJ_SHIFT 12
#define GRCAN_CANCONF_RSJ_MASK 0x7000U
#define GRCAN_CANCONF_RSJ_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANCONF_RSJ_MASK ) >> \
    GRCAN_CANCONF_RSJ_SHIFT )
#define GRCAN_CANCONF_RSJ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANCONF_RSJ_MASK ) | \
    ( ( ( _val ) << GRCAN_CANCONF_RSJ_SHIFT ) & \
      GRCAN_CANCONF_RSJ_MASK ) )
#define GRCAN_CANCONF_RSJ( _val ) \
  ( ( ( _val ) << GRCAN_CANCONF_RSJ_SHIFT ) & \
    GRCAN_CANCONF_RSJ_MASK )

#define GRCAN_CANCONF_BPR_SHIFT 8
#define GRCAN_CANCONF_BPR_MASK 0x300U
#define GRCAN_CANCONF_BPR_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANCONF_BPR_MASK ) >> \
    GRCAN_CANCONF_BPR_SHIFT )
#define GRCAN_CANCONF_BPR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANCONF_BPR_MASK ) | \
    ( ( ( _val ) << GRCAN_CANCONF_BPR_SHIFT ) & \
      GRCAN_CANCONF_BPR_MASK ) )
#define GRCAN_CANCONF_BPR( _val ) \
  ( ( ( _val ) << GRCAN_CANCONF_BPR_SHIFT ) & \
    GRCAN_CANCONF_BPR_MASK )

#define GRCAN_CANCONF_SAM 0x20U

#define GRCAN_CANCONF_SILNT 0x10U

#define GRCAN_CANCONF_SELECT 0x8U

#define GRCAN_CANCONF_ENABLE1 0x4U

#define GRCAN_CANCONF_ENABLE0 0x2U

#define GRCAN_CANCONF_ABORT 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanSTAT Status Register (CanSTAT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANSTAT_TXCHANNELS_SHIFT 28
#define GRCAN_CANSTAT_TXCHANNELS_MASK 0xf0000000U
#define GRCAN_CANSTAT_TXCHANNELS_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANSTAT_TXCHANNELS_MASK ) >> \
    GRCAN_CANSTAT_TXCHANNELS_SHIFT )
#define GRCAN_CANSTAT_TXCHANNELS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANSTAT_TXCHANNELS_MASK ) | \
    ( ( ( _val ) << GRCAN_CANSTAT_TXCHANNELS_SHIFT ) & \
      GRCAN_CANSTAT_TXCHANNELS_MASK ) )
#define GRCAN_CANSTAT_TXCHANNELS( _val ) \
  ( ( ( _val ) << GRCAN_CANSTAT_TXCHANNELS_SHIFT ) & \
    GRCAN_CANSTAT_TXCHANNELS_MASK )

#define GRCAN_CANSTAT_RXCHANNELS_SHIFT 24
#define GRCAN_CANSTAT_RXCHANNELS_MASK 0xf000000U
#define GRCAN_CANSTAT_RXCHANNELS_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANSTAT_RXCHANNELS_MASK ) >> \
    GRCAN_CANSTAT_RXCHANNELS_SHIFT )
#define GRCAN_CANSTAT_RXCHANNELS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANSTAT_RXCHANNELS_MASK ) | \
    ( ( ( _val ) << GRCAN_CANSTAT_RXCHANNELS_SHIFT ) & \
      GRCAN_CANSTAT_RXCHANNELS_MASK ) )
#define GRCAN_CANSTAT_RXCHANNELS( _val ) \
  ( ( ( _val ) << GRCAN_CANSTAT_RXCHANNELS_SHIFT ) & \
    GRCAN_CANSTAT_RXCHANNELS_MASK )

#define GRCAN_CANSTAT_TXERRCNT_SHIFT 16
#define GRCAN_CANSTAT_TXERRCNT_MASK 0xff0000U
#define GRCAN_CANSTAT_TXERRCNT_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANSTAT_TXERRCNT_MASK ) >> \
    GRCAN_CANSTAT_TXERRCNT_SHIFT )
#define GRCAN_CANSTAT_TXERRCNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANSTAT_TXERRCNT_MASK ) | \
    ( ( ( _val ) << GRCAN_CANSTAT_TXERRCNT_SHIFT ) & \
      GRCAN_CANSTAT_TXERRCNT_MASK ) )
#define GRCAN_CANSTAT_TXERRCNT( _val ) \
  ( ( ( _val ) << GRCAN_CANSTAT_TXERRCNT_SHIFT ) & \
    GRCAN_CANSTAT_TXERRCNT_MASK )

#define GRCAN_CANSTAT_RXERRCNT_SHIFT 8
#define GRCAN_CANSTAT_RXERRCNT_MASK 0xff00U
#define GRCAN_CANSTAT_RXERRCNT_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANSTAT_RXERRCNT_MASK ) >> \
    GRCAN_CANSTAT_RXERRCNT_SHIFT )
#define GRCAN_CANSTAT_RXERRCNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANSTAT_RXERRCNT_MASK ) | \
    ( ( ( _val ) << GRCAN_CANSTAT_RXERRCNT_SHIFT ) & \
      GRCAN_CANSTAT_RXERRCNT_MASK ) )
#define GRCAN_CANSTAT_RXERRCNT( _val ) \
  ( ( ( _val ) << GRCAN_CANSTAT_RXERRCNT_SHIFT ) & \
    GRCAN_CANSTAT_RXERRCNT_MASK )

#define GRCAN_CANSTAT_ACTIVE 0x10U

#define GRCAN_CANSTAT_AHBERR 0x8U

#define GRCAN_CANSTAT_OR 0x4U

#define GRCAN_CANSTAT_OFF 0x2U

#define GRCAN_CANSTAT_PASS 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanCTRL Control Register (CanCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANCTRL_RESET 0x2U

#define GRCAN_CANCTRL_ENABLE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanMASK SYNC Mask Filter Register (CanMASK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANMASK_MASK_SHIFT 0
#define GRCAN_CANMASK_MASK_MASK 0x1fffffffU
#define GRCAN_CANMASK_MASK_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANMASK_MASK_MASK ) >> \
    GRCAN_CANMASK_MASK_SHIFT )
#define GRCAN_CANMASK_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANMASK_MASK_MASK ) | \
    ( ( ( _val ) << GRCAN_CANMASK_MASK_SHIFT ) & \
      GRCAN_CANMASK_MASK_MASK ) )
#define GRCAN_CANMASK_MASK( _val ) \
  ( ( ( _val ) << GRCAN_CANMASK_MASK_SHIFT ) & \
    GRCAN_CANMASK_MASK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanCODE SYNC Code Filter Register (CanCODE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANCODE_SYNC_SHIFT 0
#define GRCAN_CANCODE_SYNC_MASK 0x1fffffffU
#define GRCAN_CANCODE_SYNC_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANCODE_SYNC_MASK ) >> \
    GRCAN_CANCODE_SYNC_SHIFT )
#define GRCAN_CANCODE_SYNC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANCODE_SYNC_MASK ) | \
    ( ( ( _val ) << GRCAN_CANCODE_SYNC_SHIFT ) & \
      GRCAN_CANCODE_SYNC_MASK ) )
#define GRCAN_CANCODE_SYNC( _val ) \
  ( ( ( _val ) << GRCAN_CANCODE_SYNC_SHIFT ) & \
    GRCAN_CANCODE_SYNC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxCTRL \
 *   Transmit Channel Control Register (CanTxCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANTXCTRL_SINGLE 0x4U

#define GRCAN_CANTXCTRL_ONGOING 0x2U

#define GRCAN_CANTXCTRL_ENABLE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxADDR \
 *   Transmit Channel Address Register (CanTxADDR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANTXADDR_ADDR_SHIFT 10
#define GRCAN_CANTXADDR_ADDR_MASK 0xfffffc00U
#define GRCAN_CANTXADDR_ADDR_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANTXADDR_ADDR_MASK ) >> \
    GRCAN_CANTXADDR_ADDR_SHIFT )
#define GRCAN_CANTXADDR_ADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANTXADDR_ADDR_MASK ) | \
    ( ( ( _val ) << GRCAN_CANTXADDR_ADDR_SHIFT ) & \
      GRCAN_CANTXADDR_ADDR_MASK ) )
#define GRCAN_CANTXADDR_ADDR( _val ) \
  ( ( ( _val ) << GRCAN_CANTXADDR_ADDR_SHIFT ) & \
    GRCAN_CANTXADDR_ADDR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxSIZE \
 *   Transmit Channel Size Register (CanTxSIZE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANTXSIZE_SIZE_SHIFT 6
#define GRCAN_CANTXSIZE_SIZE_MASK 0x1fffc0U
#define GRCAN_CANTXSIZE_SIZE_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANTXSIZE_SIZE_MASK ) >> \
    GRCAN_CANTXSIZE_SIZE_SHIFT )
#define GRCAN_CANTXSIZE_SIZE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANTXSIZE_SIZE_MASK ) | \
    ( ( ( _val ) << GRCAN_CANTXSIZE_SIZE_SHIFT ) & \
      GRCAN_CANTXSIZE_SIZE_MASK ) )
#define GRCAN_CANTXSIZE_SIZE( _val ) \
  ( ( ( _val ) << GRCAN_CANTXSIZE_SIZE_SHIFT ) & \
    GRCAN_CANTXSIZE_SIZE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxWR Transmit Channel Write Register (CanTxWR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANTXWR_WRITE_SHIFT 4
#define GRCAN_CANTXWR_WRITE_MASK 0xffff0U
#define GRCAN_CANTXWR_WRITE_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANTXWR_WRITE_MASK ) >> \
    GRCAN_CANTXWR_WRITE_SHIFT )
#define GRCAN_CANTXWR_WRITE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANTXWR_WRITE_MASK ) | \
    ( ( ( _val ) << GRCAN_CANTXWR_WRITE_SHIFT ) & \
      GRCAN_CANTXWR_WRITE_MASK ) )
#define GRCAN_CANTXWR_WRITE( _val ) \
  ( ( ( _val ) << GRCAN_CANTXWR_WRITE_SHIFT ) & \
    GRCAN_CANTXWR_WRITE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxRD Transmit Channel Read Register (CanTxRD)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANTXRD_READ_SHIFT 4
#define GRCAN_CANTXRD_READ_MASK 0xffff0U
#define GRCAN_CANTXRD_READ_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANTXRD_READ_MASK ) >> \
    GRCAN_CANTXRD_READ_SHIFT )
#define GRCAN_CANTXRD_READ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANTXRD_READ_MASK ) | \
    ( ( ( _val ) << GRCAN_CANTXRD_READ_SHIFT ) & \
      GRCAN_CANTXRD_READ_MASK ) )
#define GRCAN_CANTXRD_READ( _val ) \
  ( ( ( _val ) << GRCAN_CANTXRD_READ_SHIFT ) & \
    GRCAN_CANTXRD_READ_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxIRQ \
 *   Transmit Channel Interrupt Register (CanTxIRQ)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANTXIRQ_IRQ_SHIFT 4
#define GRCAN_CANTXIRQ_IRQ_MASK 0xffff0U
#define GRCAN_CANTXIRQ_IRQ_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANTXIRQ_IRQ_MASK ) >> \
    GRCAN_CANTXIRQ_IRQ_SHIFT )
#define GRCAN_CANTXIRQ_IRQ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANTXIRQ_IRQ_MASK ) | \
    ( ( ( _val ) << GRCAN_CANTXIRQ_IRQ_SHIFT ) & \
      GRCAN_CANTXIRQ_IRQ_MASK ) )
#define GRCAN_CANTXIRQ_IRQ( _val ) \
  ( ( ( _val ) << GRCAN_CANTXIRQ_IRQ_SHIFT ) & \
    GRCAN_CANTXIRQ_IRQ_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxCTRL \
 *   Receive Channel Control Register (CanRxCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANRXCTRL_ONGOING 0x2U

#define GRCAN_CANRXCTRL_ENABLE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxADDR \
 *   Receive Channel Address Register (CanRxADDR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANRXADDR_ADDR_SHIFT 10
#define GRCAN_CANRXADDR_ADDR_MASK 0xfffffc00U
#define GRCAN_CANRXADDR_ADDR_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANRXADDR_ADDR_MASK ) >> \
    GRCAN_CANRXADDR_ADDR_SHIFT )
#define GRCAN_CANRXADDR_ADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANRXADDR_ADDR_MASK ) | \
    ( ( ( _val ) << GRCAN_CANRXADDR_ADDR_SHIFT ) & \
      GRCAN_CANRXADDR_ADDR_MASK ) )
#define GRCAN_CANRXADDR_ADDR( _val ) \
  ( ( ( _val ) << GRCAN_CANRXADDR_ADDR_SHIFT ) & \
    GRCAN_CANRXADDR_ADDR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxSIZE \
 *   Receive Channel Size Register (CanRxSIZE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANRXSIZE_SIZE_SHIFT 6
#define GRCAN_CANRXSIZE_SIZE_MASK 0x1fffc0U
#define GRCAN_CANRXSIZE_SIZE_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANRXSIZE_SIZE_MASK ) >> \
    GRCAN_CANRXSIZE_SIZE_SHIFT )
#define GRCAN_CANRXSIZE_SIZE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANRXSIZE_SIZE_MASK ) | \
    ( ( ( _val ) << GRCAN_CANRXSIZE_SIZE_SHIFT ) & \
      GRCAN_CANRXSIZE_SIZE_MASK ) )
#define GRCAN_CANRXSIZE_SIZE( _val ) \
  ( ( ( _val ) << GRCAN_CANRXSIZE_SIZE_SHIFT ) & \
    GRCAN_CANRXSIZE_SIZE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxWR Receive Channel Write Register (CanRxWR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANRXWR_WRITE_SHIFT 4
#define GRCAN_CANRXWR_WRITE_MASK 0xffff0U
#define GRCAN_CANRXWR_WRITE_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANRXWR_WRITE_MASK ) >> \
    GRCAN_CANRXWR_WRITE_SHIFT )
#define GRCAN_CANRXWR_WRITE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANRXWR_WRITE_MASK ) | \
    ( ( ( _val ) << GRCAN_CANRXWR_WRITE_SHIFT ) & \
      GRCAN_CANRXWR_WRITE_MASK ) )
#define GRCAN_CANRXWR_WRITE( _val ) \
  ( ( ( _val ) << GRCAN_CANRXWR_WRITE_SHIFT ) & \
    GRCAN_CANRXWR_WRITE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxRD Receive Channel Read Register (CanRxRD)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANRXRD_READ_SHIFT 4
#define GRCAN_CANRXRD_READ_MASK 0xffff0U
#define GRCAN_CANRXRD_READ_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANRXRD_READ_MASK ) >> \
    GRCAN_CANRXRD_READ_SHIFT )
#define GRCAN_CANRXRD_READ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANRXRD_READ_MASK ) | \
    ( ( ( _val ) << GRCAN_CANRXRD_READ_SHIFT ) & \
      GRCAN_CANRXRD_READ_MASK ) )
#define GRCAN_CANRXRD_READ( _val ) \
  ( ( ( _val ) << GRCAN_CANRXRD_READ_SHIFT ) & \
    GRCAN_CANRXRD_READ_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxIRQ \
 *   Receive Channel Interrupt Register (CanRxIRQ)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANRXIRQ_IRQ_SHIFT 4
#define GRCAN_CANRXIRQ_IRQ_MASK 0xffff0U
#define GRCAN_CANRXIRQ_IRQ_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANRXIRQ_IRQ_MASK ) >> \
    GRCAN_CANRXIRQ_IRQ_SHIFT )
#define GRCAN_CANRXIRQ_IRQ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANRXIRQ_IRQ_MASK ) | \
    ( ( ( _val ) << GRCAN_CANRXIRQ_IRQ_SHIFT ) & \
      GRCAN_CANRXIRQ_IRQ_MASK ) )
#define GRCAN_CANRXIRQ_IRQ( _val ) \
  ( ( ( _val ) << GRCAN_CANRXIRQ_IRQ_SHIFT ) & \
    GRCAN_CANRXIRQ_IRQ_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxMASK \
 *   Receive Channel Mask Register (CanRxMASK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANRXMASK_AM_SHIFT 0
#define GRCAN_CANRXMASK_AM_MASK 0x1fffffffU
#define GRCAN_CANRXMASK_AM_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANRXMASK_AM_MASK ) >> \
    GRCAN_CANRXMASK_AM_SHIFT )
#define GRCAN_CANRXMASK_AM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANRXMASK_AM_MASK ) | \
    ( ( ( _val ) << GRCAN_CANRXMASK_AM_SHIFT ) & \
      GRCAN_CANRXMASK_AM_MASK ) )
#define GRCAN_CANRXMASK_AM( _val ) \
  ( ( ( _val ) << GRCAN_CANRXMASK_AM_SHIFT ) & \
    GRCAN_CANRXMASK_AM_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxCODE \
 *   Receive Channel Code Register (CanRxCODE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCAN_CANRXCODE_AC_SHIFT 0
#define GRCAN_CANRXCODE_AC_MASK 0x1fffffffU
#define GRCAN_CANRXCODE_AC_GET( _reg ) \
  ( ( ( _reg ) & GRCAN_CANRXCODE_AC_MASK ) >> \
    GRCAN_CANRXCODE_AC_SHIFT )
#define GRCAN_CANRXCODE_AC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCAN_CANRXCODE_AC_MASK ) | \
    ( ( ( _val ) << GRCAN_CANRXCODE_AC_SHIFT ) & \
      GRCAN_CANRXCODE_AC_MASK ) )
#define GRCAN_CANRXCODE_AC( _val ) \
  ( ( ( _val ) << GRCAN_CANRXCODE_AC_SHIFT ) & \
    GRCAN_CANRXCODE_AC_MASK )

/** @} */

/**
 * @brief This structure defines the GRCAN register block memory map.
 */
typedef struct grcan {
  /**
   * @brief See @ref RTEMSDeviceGRCANCanCONF.
   */
  uint32_t canconf;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanSTAT.
   */
  uint32_t canstat;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanCTRL.
   */
  uint32_t canctrl;

  uint32_t reserved_c_18[ 3 ];

  /**
   * @brief See @ref RTEMSDeviceGRCANCanMASK.
   */
  uint32_t canmask;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanCODE.
   */
  uint32_t cancode;

  uint32_t reserved_20_200[ 120 ];

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxCTRL.
   */
  uint32_t cantxctrl;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxADDR.
   */
  uint32_t cantxaddr;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxSIZE.
   */
  uint32_t cantxsize;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxWR.
   */
  uint32_t cantxwr;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxRD.
   */
  uint32_t cantxrd;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxIRQ.
   */
  uint32_t cantxirq;

  uint32_t reserved_218_300[ 58 ];

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxCTRL.
   */
  uint32_t canrxctrl;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxADDR.
   */
  uint32_t canrxaddr;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxSIZE.
   */
  uint32_t canrxsize;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxWR.
   */
  uint32_t canrxwr;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxRD.
   */
  uint32_t canrxrd;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxIRQ.
   */
  uint32_t canrxirq;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxMASK.
   */
  uint32_t canrxmask;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxCODE.
   */
  uint32_t canrxcode;
} grcan;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRCAN_REGS_H */
