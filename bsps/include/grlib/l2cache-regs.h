/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBL2CACHE
 *
 * @brief This header file defines the L2CACHE register block interface.
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

/* Generated from spec:/dev/grlib/if/l2cache-header */

#ifndef _GRLIB_L2CACHE_REGS_H
#define _GRLIB_L2CACHE_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/l2cache */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHE L2CACHE
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the L2CACHE interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CC L2C Control register (L2CC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CC_EN 0x80000000U

#define L2CACHE_L2CC_EDAC 0x40000000U

#define L2CACHE_L2CC_REPL_SHIFT 28
#define L2CACHE_L2CC_REPL_MASK 0x30000000U
#define L2CACHE_L2CC_REPL_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CC_REPL_MASK ) >> \
    L2CACHE_L2CC_REPL_SHIFT )
#define L2CACHE_L2CC_REPL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CC_REPL_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CC_REPL_SHIFT ) & \
      L2CACHE_L2CC_REPL_MASK ) )
#define L2CACHE_L2CC_REPL( _val ) \
  ( ( ( _val ) << L2CACHE_L2CC_REPL_SHIFT ) & \
    L2CACHE_L2CC_REPL_MASK )

#define L2CACHE_L2CC_BBS_SHIFT 16
#define L2CACHE_L2CC_BBS_MASK 0x70000U
#define L2CACHE_L2CC_BBS_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CC_BBS_MASK ) >> \
    L2CACHE_L2CC_BBS_SHIFT )
#define L2CACHE_L2CC_BBS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CC_BBS_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CC_BBS_SHIFT ) & \
      L2CACHE_L2CC_BBS_MASK ) )
#define L2CACHE_L2CC_BBS( _val ) \
  ( ( ( _val ) << L2CACHE_L2CC_BBS_SHIFT ) & \
    L2CACHE_L2CC_BBS_MASK )

#define L2CACHE_L2CC_INDEX_WAY_SHIFT 12
#define L2CACHE_L2CC_INDEX_WAY_MASK 0xf000U
#define L2CACHE_L2CC_INDEX_WAY_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CC_INDEX_WAY_MASK ) >> \
    L2CACHE_L2CC_INDEX_WAY_SHIFT )
#define L2CACHE_L2CC_INDEX_WAY_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CC_INDEX_WAY_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CC_INDEX_WAY_SHIFT ) & \
      L2CACHE_L2CC_INDEX_WAY_MASK ) )
#define L2CACHE_L2CC_INDEX_WAY( _val ) \
  ( ( ( _val ) << L2CACHE_L2CC_INDEX_WAY_SHIFT ) & \
    L2CACHE_L2CC_INDEX_WAY_MASK )

#define L2CACHE_L2CC_LOCK_SHIFT 8
#define L2CACHE_L2CC_LOCK_MASK 0xf00U
#define L2CACHE_L2CC_LOCK_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CC_LOCK_MASK ) >> \
    L2CACHE_L2CC_LOCK_SHIFT )
#define L2CACHE_L2CC_LOCK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CC_LOCK_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CC_LOCK_SHIFT ) & \
      L2CACHE_L2CC_LOCK_MASK ) )
#define L2CACHE_L2CC_LOCK( _val ) \
  ( ( ( _val ) << L2CACHE_L2CC_LOCK_SHIFT ) & \
    L2CACHE_L2CC_LOCK_MASK )

#define L2CACHE_L2CC_HPRHB 0x20U

#define L2CACHE_L2CC_HPB 0x10U

#define L2CACHE_L2CC_UC 0x8U

#define L2CACHE_L2CC_HC 0x4U

#define L2CACHE_L2CC_WP 0x2U

#define L2CACHE_L2CC_HP 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CS L2C Status register (L2CS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CS_LS 0x1000000U

#define L2CACHE_L2CS_AT 0x800000U

#define L2CACHE_L2CS_MP 0x400000U

#define L2CACHE_L2CS_MTRR_SHIFT 16
#define L2CACHE_L2CS_MTRR_MASK 0x3f0000U
#define L2CACHE_L2CS_MTRR_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CS_MTRR_MASK ) >> \
    L2CACHE_L2CS_MTRR_SHIFT )
#define L2CACHE_L2CS_MTRR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CS_MTRR_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CS_MTRR_SHIFT ) & \
      L2CACHE_L2CS_MTRR_MASK ) )
#define L2CACHE_L2CS_MTRR( _val ) \
  ( ( ( _val ) << L2CACHE_L2CS_MTRR_SHIFT ) & \
    L2CACHE_L2CS_MTRR_MASK )

#define L2CACHE_L2CS_BBUS_W_SHIFT 13
#define L2CACHE_L2CS_BBUS_W_MASK 0xe000U
#define L2CACHE_L2CS_BBUS_W_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CS_BBUS_W_MASK ) >> \
    L2CACHE_L2CS_BBUS_W_SHIFT )
#define L2CACHE_L2CS_BBUS_W_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CS_BBUS_W_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CS_BBUS_W_SHIFT ) & \
      L2CACHE_L2CS_BBUS_W_MASK ) )
#define L2CACHE_L2CS_BBUS_W( _val ) \
  ( ( ( _val ) << L2CACHE_L2CS_BBUS_W_SHIFT ) & \
    L2CACHE_L2CS_BBUS_W_MASK )

#define L2CACHE_L2CS_WAY_SIZE_SHIFT 2
#define L2CACHE_L2CS_WAY_SIZE_MASK 0x1ffcU
#define L2CACHE_L2CS_WAY_SIZE_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CS_WAY_SIZE_MASK ) >> \
    L2CACHE_L2CS_WAY_SIZE_SHIFT )
#define L2CACHE_L2CS_WAY_SIZE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CS_WAY_SIZE_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CS_WAY_SIZE_SHIFT ) & \
      L2CACHE_L2CS_WAY_SIZE_MASK ) )
#define L2CACHE_L2CS_WAY_SIZE( _val ) \
  ( ( ( _val ) << L2CACHE_L2CS_WAY_SIZE_SHIFT ) & \
    L2CACHE_L2CS_WAY_SIZE_MASK )

#define L2CACHE_L2CS_WAY_SHIFT 0
#define L2CACHE_L2CS_WAY_MASK 0x3U
#define L2CACHE_L2CS_WAY_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CS_WAY_MASK ) >> \
    L2CACHE_L2CS_WAY_SHIFT )
#define L2CACHE_L2CS_WAY_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CS_WAY_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CS_WAY_SHIFT ) & \
      L2CACHE_L2CS_WAY_MASK ) )
#define L2CACHE_L2CS_WAY( _val ) \
  ( ( ( _val ) << L2CACHE_L2CS_WAY_SHIFT ) & \
    L2CACHE_L2CS_WAY_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CFMA \
 *   L2C Flush (Memory address) register (L2CFMA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CFMA_ADDR_SHIFT 5
#define L2CACHE_L2CFMA_ADDR_MASK 0xffffffe0U
#define L2CACHE_L2CFMA_ADDR_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CFMA_ADDR_MASK ) >> \
    L2CACHE_L2CFMA_ADDR_SHIFT )
#define L2CACHE_L2CFMA_ADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CFMA_ADDR_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CFMA_ADDR_SHIFT ) & \
      L2CACHE_L2CFMA_ADDR_MASK ) )
#define L2CACHE_L2CFMA_ADDR( _val ) \
  ( ( ( _val ) << L2CACHE_L2CFMA_ADDR_SHIFT ) & \
    L2CACHE_L2CFMA_ADDR_MASK )

#define L2CACHE_L2CFMA_DI 0x8U

#define L2CACHE_L2CFMA_FMODE_SHIFT 0
#define L2CACHE_L2CFMA_FMODE_MASK 0x7U
#define L2CACHE_L2CFMA_FMODE_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CFMA_FMODE_MASK ) >> \
    L2CACHE_L2CFMA_FMODE_SHIFT )
#define L2CACHE_L2CFMA_FMODE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CFMA_FMODE_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CFMA_FMODE_SHIFT ) & \
      L2CACHE_L2CFMA_FMODE_MASK ) )
#define L2CACHE_L2CFMA_FMODE( _val ) \
  ( ( ( _val ) << L2CACHE_L2CFMA_FMODE_SHIFT ) & \
    L2CACHE_L2CFMA_FMODE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CFSI \
 *   L2C Flush (Set, Index) register (L2CFSI)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CFSI_INDEX_SHIFT 16
#define L2CACHE_L2CFSI_INDEX_MASK 0xffff0000U
#define L2CACHE_L2CFSI_INDEX_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CFSI_INDEX_MASK ) >> \
    L2CACHE_L2CFSI_INDEX_SHIFT )
#define L2CACHE_L2CFSI_INDEX_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CFSI_INDEX_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CFSI_INDEX_SHIFT ) & \
      L2CACHE_L2CFSI_INDEX_MASK ) )
#define L2CACHE_L2CFSI_INDEX( _val ) \
  ( ( ( _val ) << L2CACHE_L2CFSI_INDEX_SHIFT ) & \
    L2CACHE_L2CFSI_INDEX_MASK )

#define L2CACHE_L2CFSI_TAG_SHIFT 10
#define L2CACHE_L2CFSI_TAG_MASK 0xfffffc00U
#define L2CACHE_L2CFSI_TAG_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CFSI_TAG_MASK ) >> \
    L2CACHE_L2CFSI_TAG_SHIFT )
#define L2CACHE_L2CFSI_TAG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CFSI_TAG_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CFSI_TAG_SHIFT ) & \
      L2CACHE_L2CFSI_TAG_MASK ) )
#define L2CACHE_L2CFSI_TAG( _val ) \
  ( ( ( _val ) << L2CACHE_L2CFSI_TAG_SHIFT ) & \
    L2CACHE_L2CFSI_TAG_MASK )

#define L2CACHE_L2CFSI_FL 0x200U

#define L2CACHE_L2CFSI_VB 0x100U

#define L2CACHE_L2CFSI_DB 0x80U

#define L2CACHE_L2CFSI_WAY_SHIFT 4
#define L2CACHE_L2CFSI_WAY_MASK 0x30U
#define L2CACHE_L2CFSI_WAY_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CFSI_WAY_MASK ) >> \
    L2CACHE_L2CFSI_WAY_SHIFT )
#define L2CACHE_L2CFSI_WAY_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CFSI_WAY_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CFSI_WAY_SHIFT ) & \
      L2CACHE_L2CFSI_WAY_MASK ) )
#define L2CACHE_L2CFSI_WAY( _val ) \
  ( ( ( _val ) << L2CACHE_L2CFSI_WAY_SHIFT ) & \
    L2CACHE_L2CFSI_WAY_MASK )

#define L2CACHE_L2CFSI_DI 0x8U

#define L2CACHE_L2CFSI_WF 0x4U

#define L2CACHE_L2CFSI_FMODE_SHIFT 0
#define L2CACHE_L2CFSI_FMODE_MASK 0x3U
#define L2CACHE_L2CFSI_FMODE_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CFSI_FMODE_MASK ) >> \
    L2CACHE_L2CFSI_FMODE_SHIFT )
#define L2CACHE_L2CFSI_FMODE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CFSI_FMODE_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CFSI_FMODE_SHIFT ) & \
      L2CACHE_L2CFSI_FMODE_MASK ) )
#define L2CACHE_L2CFSI_FMODE( _val ) \
  ( ( ( _val ) << L2CACHE_L2CFSI_FMODE_SHIFT ) & \
    L2CACHE_L2CFSI_FMODE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CERR \
 *   L2CError status/control register (L2CERR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CERR_AHB_MASTER_INDEX_SHIFT 28
#define L2CACHE_L2CERR_AHB_MASTER_INDEX_MASK 0xf0000000U
#define L2CACHE_L2CERR_AHB_MASTER_INDEX_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CERR_AHB_MASTER_INDEX_MASK ) >> \
    L2CACHE_L2CERR_AHB_MASTER_INDEX_SHIFT )
#define L2CACHE_L2CERR_AHB_MASTER_INDEX_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CERR_AHB_MASTER_INDEX_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CERR_AHB_MASTER_INDEX_SHIFT ) & \
      L2CACHE_L2CERR_AHB_MASTER_INDEX_MASK ) )
#define L2CACHE_L2CERR_AHB_MASTER_INDEX( _val ) \
  ( ( ( _val ) << L2CACHE_L2CERR_AHB_MASTER_INDEX_SHIFT ) & \
    L2CACHE_L2CERR_AHB_MASTER_INDEX_MASK )

#define L2CACHE_L2CERR_SCRUB 0x8000000U

#define L2CACHE_L2CERR_TYPE_SHIFT 24
#define L2CACHE_L2CERR_TYPE_MASK 0x7000000U
#define L2CACHE_L2CERR_TYPE_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CERR_TYPE_MASK ) >> \
    L2CACHE_L2CERR_TYPE_SHIFT )
#define L2CACHE_L2CERR_TYPE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CERR_TYPE_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CERR_TYPE_SHIFT ) & \
      L2CACHE_L2CERR_TYPE_MASK ) )
#define L2CACHE_L2CERR_TYPE( _val ) \
  ( ( ( _val ) << L2CACHE_L2CERR_TYPE_SHIFT ) & \
    L2CACHE_L2CERR_TYPE_MASK )

#define L2CACHE_L2CERR_TAG_DATA 0x800000U

#define L2CACHE_L2CERR_COR_UCOR 0x400000U

#define L2CACHE_L2CERR_MULTI 0x200000U

#define L2CACHE_L2CERR_VALID 0x100000U

#define L2CACHE_L2CERR_DISERESP 0x80000U

#define L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER_SHIFT 16
#define L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER_MASK 0x70000U
#define L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER_MASK ) >> \
    L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER_SHIFT )
#define L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER_SHIFT ) & \
      L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER_MASK ) )
#define L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER( _val ) \
  ( ( ( _val ) << L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER_SHIFT ) & \
    L2CACHE_L2CERR_CORRECTABLE_ERROR_COUNTER_MASK )

#define L2CACHE_L2CERR_IRQ_PENDING_SHIFT 12
#define L2CACHE_L2CERR_IRQ_PENDING_MASK 0xf000U
#define L2CACHE_L2CERR_IRQ_PENDING_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CERR_IRQ_PENDING_MASK ) >> \
    L2CACHE_L2CERR_IRQ_PENDING_SHIFT )
#define L2CACHE_L2CERR_IRQ_PENDING_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CERR_IRQ_PENDING_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CERR_IRQ_PENDING_SHIFT ) & \
      L2CACHE_L2CERR_IRQ_PENDING_MASK ) )
#define L2CACHE_L2CERR_IRQ_PENDING( _val ) \
  ( ( ( _val ) << L2CACHE_L2CERR_IRQ_PENDING_SHIFT ) & \
    L2CACHE_L2CERR_IRQ_PENDING_MASK )

#define L2CACHE_L2CERR_IRQ_MASK_SHIFT 8
#define L2CACHE_L2CERR_IRQ_MASK_MASK 0xf00U
#define L2CACHE_L2CERR_IRQ_MASK_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CERR_IRQ_MASK_MASK ) >> \
    L2CACHE_L2CERR_IRQ_MASK_SHIFT )
#define L2CACHE_L2CERR_IRQ_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CERR_IRQ_MASK_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CERR_IRQ_MASK_SHIFT ) & \
      L2CACHE_L2CERR_IRQ_MASK_MASK ) )
#define L2CACHE_L2CERR_IRQ_MASK( _val ) \
  ( ( ( _val ) << L2CACHE_L2CERR_IRQ_MASK_SHIFT ) & \
    L2CACHE_L2CERR_IRQ_MASK_MASK )

#define L2CACHE_L2CERR_SELECT_CB_SHIFT 6
#define L2CACHE_L2CERR_SELECT_CB_MASK 0xc0U
#define L2CACHE_L2CERR_SELECT_CB_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CERR_SELECT_CB_MASK ) >> \
    L2CACHE_L2CERR_SELECT_CB_SHIFT )
#define L2CACHE_L2CERR_SELECT_CB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CERR_SELECT_CB_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CERR_SELECT_CB_SHIFT ) & \
      L2CACHE_L2CERR_SELECT_CB_MASK ) )
#define L2CACHE_L2CERR_SELECT_CB( _val ) \
  ( ( ( _val ) << L2CACHE_L2CERR_SELECT_CB_SHIFT ) & \
    L2CACHE_L2CERR_SELECT_CB_MASK )

#define L2CACHE_L2CERR_SELECT_TCB_SHIFT 4
#define L2CACHE_L2CERR_SELECT_TCB_MASK 0x30U
#define L2CACHE_L2CERR_SELECT_TCB_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CERR_SELECT_TCB_MASK ) >> \
    L2CACHE_L2CERR_SELECT_TCB_SHIFT )
#define L2CACHE_L2CERR_SELECT_TCB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CERR_SELECT_TCB_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CERR_SELECT_TCB_SHIFT ) & \
      L2CACHE_L2CERR_SELECT_TCB_MASK ) )
#define L2CACHE_L2CERR_SELECT_TCB( _val ) \
  ( ( ( _val ) << L2CACHE_L2CERR_SELECT_TCB_SHIFT ) & \
    L2CACHE_L2CERR_SELECT_TCB_MASK )

#define L2CACHE_L2CERR_XCB 0x8U

#define L2CACHE_L2CERR_RCB 0x4U

#define L2CACHE_L2CERR_COMP 0x2U

#define L2CACHE_L2CERR_RST 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CERRA \
 *   L2C Error address register (L2CERRA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CERRA_EADDR_SHIFT 0
#define L2CACHE_L2CERRA_EADDR_MASK 0xffffffffU
#define L2CACHE_L2CERRA_EADDR_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CERRA_EADDR_MASK ) >> \
    L2CACHE_L2CERRA_EADDR_SHIFT )
#define L2CACHE_L2CERRA_EADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CERRA_EADDR_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CERRA_EADDR_SHIFT ) & \
      L2CACHE_L2CERRA_EADDR_MASK ) )
#define L2CACHE_L2CERRA_EADDR( _val ) \
  ( ( ( _val ) << L2CACHE_L2CERRA_EADDR_SHIFT ) & \
    L2CACHE_L2CERRA_EADDR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CTCB L2C TAG-Check-Bits register (L2CTCB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CTCB_TCB_SHIFT 0
#define L2CACHE_L2CTCB_TCB_MASK 0x7fU
#define L2CACHE_L2CTCB_TCB_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CTCB_TCB_MASK ) >> \
    L2CACHE_L2CTCB_TCB_SHIFT )
#define L2CACHE_L2CTCB_TCB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CTCB_TCB_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CTCB_TCB_SHIFT ) & \
      L2CACHE_L2CTCB_TCB_MASK ) )
#define L2CACHE_L2CTCB_TCB( _val ) \
  ( ( ( _val ) << L2CACHE_L2CTCB_TCB_SHIFT ) & \
    L2CACHE_L2CTCB_TCB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CCB L2C Data-Check-Bits register (L2CCB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CCB_CB_SHIFT 0
#define L2CACHE_L2CCB_CB_MASK 0xfffffffU
#define L2CACHE_L2CCB_CB_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CCB_CB_MASK ) >> \
    L2CACHE_L2CCB_CB_SHIFT )
#define L2CACHE_L2CCB_CB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CCB_CB_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CCB_CB_SHIFT ) & \
      L2CACHE_L2CCB_CB_MASK ) )
#define L2CACHE_L2CCB_CB( _val ) \
  ( ( ( _val ) << L2CACHE_L2CCB_CB_SHIFT ) & \
    L2CACHE_L2CCB_CB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CSCRUB \
 *   L2C Scrub control/status register (L2CSCRUB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CSCRUB_INDEX_SHIFT 16
#define L2CACHE_L2CSCRUB_INDEX_MASK 0xffff0000U
#define L2CACHE_L2CSCRUB_INDEX_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CSCRUB_INDEX_MASK ) >> \
    L2CACHE_L2CSCRUB_INDEX_SHIFT )
#define L2CACHE_L2CSCRUB_INDEX_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CSCRUB_INDEX_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CSCRUB_INDEX_SHIFT ) & \
      L2CACHE_L2CSCRUB_INDEX_MASK ) )
#define L2CACHE_L2CSCRUB_INDEX( _val ) \
  ( ( ( _val ) << L2CACHE_L2CSCRUB_INDEX_SHIFT ) & \
    L2CACHE_L2CSCRUB_INDEX_MASK )

#define L2CACHE_L2CSCRUB_WAY_SHIFT 2
#define L2CACHE_L2CSCRUB_WAY_MASK 0xcU
#define L2CACHE_L2CSCRUB_WAY_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CSCRUB_WAY_MASK ) >> \
    L2CACHE_L2CSCRUB_WAY_SHIFT )
#define L2CACHE_L2CSCRUB_WAY_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CSCRUB_WAY_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CSCRUB_WAY_SHIFT ) & \
      L2CACHE_L2CSCRUB_WAY_MASK ) )
#define L2CACHE_L2CSCRUB_WAY( _val ) \
  ( ( ( _val ) << L2CACHE_L2CSCRUB_WAY_SHIFT ) & \
    L2CACHE_L2CSCRUB_WAY_MASK )

#define L2CACHE_L2CSCRUB_PEN 0x2U

#define L2CACHE_L2CSCRUB_EN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CSDEL L2C Scrub delay register (L2CSDEL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CSDEL_DEL_SHIFT 0
#define L2CACHE_L2CSDEL_DEL_MASK 0xffffU
#define L2CACHE_L2CSDEL_DEL_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CSDEL_DEL_MASK ) >> \
    L2CACHE_L2CSDEL_DEL_SHIFT )
#define L2CACHE_L2CSDEL_DEL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CSDEL_DEL_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CSDEL_DEL_SHIFT ) & \
      L2CACHE_L2CSDEL_DEL_MASK ) )
#define L2CACHE_L2CSDEL_DEL( _val ) \
  ( ( ( _val ) << L2CACHE_L2CSDEL_DEL_SHIFT ) & \
    L2CACHE_L2CSDEL_DEL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CEINJ \
 *   L2C Error injection register (L2CEINJ)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CEINJ_ADDR_SHIFT 2
#define L2CACHE_L2CEINJ_ADDR_MASK 0xfffffffcU
#define L2CACHE_L2CEINJ_ADDR_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CEINJ_ADDR_MASK ) >> \
    L2CACHE_L2CEINJ_ADDR_SHIFT )
#define L2CACHE_L2CEINJ_ADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CEINJ_ADDR_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CEINJ_ADDR_SHIFT ) & \
      L2CACHE_L2CEINJ_ADDR_MASK ) )
#define L2CACHE_L2CEINJ_ADDR( _val ) \
  ( ( ( _val ) << L2CACHE_L2CEINJ_ADDR_SHIFT ) & \
    L2CACHE_L2CEINJ_ADDR_MASK )

#define L2CACHE_L2CEINJ_INJ 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CACCC \
 *   L2C Access control register (L2CACCC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CACCC_DSC 0x4000U

#define L2CACHE_L2CACCC_SH 0x2000U

#define L2CACHE_L2CACCC_SPLITQ 0x400U

#define L2CACHE_L2CACCC_NHM 0x200U

#define L2CACHE_L2CACCC_BERR 0x100U

#define L2CACHE_L2CACCC_OAPM 0x80U

#define L2CACHE_L2CACCC_FLINE 0x40U

#define L2CACHE_L2CACCC_DBPF 0x20U

#define L2CACHE_L2CACCC_128WF 0x10U

#define L2CACHE_L2CACCC_DBPWS 0x4U

#define L2CACHE_L2CACCC_SPLIT 0x2U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CEINJCFG \
 *   L2C injection configuration register (L2CEINJCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CEINJCFG_EDI 0x400U

#define L2CACHE_L2CEINJCFG_TER 0x200U

#define L2CACHE_L2CEINJCFG_IMD 0x100U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL2CACHEL2CMTRR \
 *   L2C Memory type range register (L2CMTRR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L2CACHE_L2CMTRR_ADDR_SHIFT 18
#define L2CACHE_L2CMTRR_ADDR_MASK 0xfffc0000U
#define L2CACHE_L2CMTRR_ADDR_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CMTRR_ADDR_MASK ) >> \
    L2CACHE_L2CMTRR_ADDR_SHIFT )
#define L2CACHE_L2CMTRR_ADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CMTRR_ADDR_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CMTRR_ADDR_SHIFT ) & \
      L2CACHE_L2CMTRR_ADDR_MASK ) )
#define L2CACHE_L2CMTRR_ADDR( _val ) \
  ( ( ( _val ) << L2CACHE_L2CMTRR_ADDR_SHIFT ) & \
    L2CACHE_L2CMTRR_ADDR_MASK )

#define L2CACHE_L2CMTRR_ACC_SHIFT 16
#define L2CACHE_L2CMTRR_ACC_MASK 0x30000U
#define L2CACHE_L2CMTRR_ACC_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CMTRR_ACC_MASK ) >> \
    L2CACHE_L2CMTRR_ACC_SHIFT )
#define L2CACHE_L2CMTRR_ACC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CMTRR_ACC_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CMTRR_ACC_SHIFT ) & \
      L2CACHE_L2CMTRR_ACC_MASK ) )
#define L2CACHE_L2CMTRR_ACC( _val ) \
  ( ( ( _val ) << L2CACHE_L2CMTRR_ACC_SHIFT ) & \
    L2CACHE_L2CMTRR_ACC_MASK )

#define L2CACHE_L2CMTRR_MASK_SHIFT 2
#define L2CACHE_L2CMTRR_MASK_MASK 0xfffcU
#define L2CACHE_L2CMTRR_MASK_GET( _reg ) \
  ( ( ( _reg ) & L2CACHE_L2CMTRR_MASK_MASK ) >> \
    L2CACHE_L2CMTRR_MASK_SHIFT )
#define L2CACHE_L2CMTRR_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L2CACHE_L2CMTRR_MASK_MASK ) | \
    ( ( ( _val ) << L2CACHE_L2CMTRR_MASK_SHIFT ) & \
      L2CACHE_L2CMTRR_MASK_MASK ) )
#define L2CACHE_L2CMTRR_MASK( _val ) \
  ( ( ( _val ) << L2CACHE_L2CMTRR_MASK_SHIFT ) & \
    L2CACHE_L2CMTRR_MASK_MASK )

#define L2CACHE_L2CMTRR_WP 0x2U

#define L2CACHE_L2CMTRR_AC 0x1U

/** @} */

/**
 * @brief This structure defines the L2CACHE register block memory map.
 */
typedef struct l2cache {
  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CC.
   */
  uint32_t l2cc;

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CS.
   */
  uint32_t l2cs;

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CFMA.
   */
  uint32_t l2cfma;

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CFSI.
   */
  uint32_t l2cfsi;

  uint32_t reserved_10_20[ 4 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CERR.
   */
  uint32_t l2cerr;

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CERRA.
   */
  uint32_t l2cerra;

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CTCB.
   */
  uint32_t l2ctcb;

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CCB.
   */
  uint32_t l2ccb;

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CSCRUB.
   */
  uint32_t l2cscrub;

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CSDEL.
   */
  uint32_t l2csdel;

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CEINJ.
   */
  uint32_t l2ceinj;

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CACCC.
   */
  uint32_t l2caccc;

  uint32_t reserved_40_4c[ 3 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CEINJCFG.
   */
  uint32_t l2ceinjcfg;

  uint32_t reserved_50_80[ 12 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBL2CACHEL2CMTRR.
   */
  uint32_t l2cmtrr;
} l2cache;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_L2CACHE_REGS_H */
