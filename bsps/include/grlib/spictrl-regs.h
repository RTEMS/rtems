/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBSPICTRL
 *
 * @brief This header file defines the SPICTRL register block interface.
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

/* Generated from spec:/dev/grlib/if/spictrl-header */

#ifndef _GRLIB_SPICTRL_REGS_H
#define _GRLIB_SPICTRL_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/spictrl */

/**
 * @defgroup RTEMSDeviceGRLIBSPICTRL SPICTRL
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the SPICTRL interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBSPICTRLCAP Capability register (CAP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPICTRL_CAP_SSSZ_SHIFT 24
#define SPICTRL_CAP_SSSZ_MASK 0xff000000U
#define SPICTRL_CAP_SSSZ_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_CAP_SSSZ_MASK ) >> \
    SPICTRL_CAP_SSSZ_SHIFT )
#define SPICTRL_CAP_SSSZ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_CAP_SSSZ_MASK ) | \
    ( ( ( _val ) << SPICTRL_CAP_SSSZ_SHIFT ) & \
      SPICTRL_CAP_SSSZ_MASK ) )
#define SPICTRL_CAP_SSSZ( _val ) \
  ( ( ( _val ) << SPICTRL_CAP_SSSZ_SHIFT ) & \
    SPICTRL_CAP_SSSZ_MASK )

#define SPICTRL_CAP_MAXWLEN_SHIFT 20
#define SPICTRL_CAP_MAXWLEN_MASK 0xf00000U
#define SPICTRL_CAP_MAXWLEN_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_CAP_MAXWLEN_MASK ) >> \
    SPICTRL_CAP_MAXWLEN_SHIFT )
#define SPICTRL_CAP_MAXWLEN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_CAP_MAXWLEN_MASK ) | \
    ( ( ( _val ) << SPICTRL_CAP_MAXWLEN_SHIFT ) & \
      SPICTRL_CAP_MAXWLEN_MASK ) )
#define SPICTRL_CAP_MAXWLEN( _val ) \
  ( ( ( _val ) << SPICTRL_CAP_MAXWLEN_SHIFT ) & \
    SPICTRL_CAP_MAXWLEN_MASK )

#define SPICTRL_CAP_TWEN 0x80000U

#define SPICTRL_CAP_AMODE 0x40000U

#define SPICTRL_CAP_ASELA 0x20000U

#define SPICTRL_CAP_SSEN 0x10000U

#define SPICTRL_CAP_FDEPTH_SHIFT 8
#define SPICTRL_CAP_FDEPTH_MASK 0xff00U
#define SPICTRL_CAP_FDEPTH_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_CAP_FDEPTH_MASK ) >> \
    SPICTRL_CAP_FDEPTH_SHIFT )
#define SPICTRL_CAP_FDEPTH_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_CAP_FDEPTH_MASK ) | \
    ( ( ( _val ) << SPICTRL_CAP_FDEPTH_SHIFT ) & \
      SPICTRL_CAP_FDEPTH_MASK ) )
#define SPICTRL_CAP_FDEPTH( _val ) \
  ( ( ( _val ) << SPICTRL_CAP_FDEPTH_SHIFT ) & \
    SPICTRL_CAP_FDEPTH_MASK )

#define SPICTRL_CAP_SR 0x80U

#define SPICTRL_CAP_FT_SHIFT 5
#define SPICTRL_CAP_FT_MASK 0x60U
#define SPICTRL_CAP_FT_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_CAP_FT_MASK ) >> \
    SPICTRL_CAP_FT_SHIFT )
#define SPICTRL_CAP_FT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_CAP_FT_MASK ) | \
    ( ( ( _val ) << SPICTRL_CAP_FT_SHIFT ) & \
      SPICTRL_CAP_FT_MASK ) )
#define SPICTRL_CAP_FT( _val ) \
  ( ( ( _val ) << SPICTRL_CAP_FT_SHIFT ) & \
    SPICTRL_CAP_FT_MASK )

#define SPICTRL_CAP_REV_SHIFT 0
#define SPICTRL_CAP_REV_MASK 0x1fU
#define SPICTRL_CAP_REV_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_CAP_REV_MASK ) >> \
    SPICTRL_CAP_REV_SHIFT )
#define SPICTRL_CAP_REV_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_CAP_REV_MASK ) | \
    ( ( ( _val ) << SPICTRL_CAP_REV_SHIFT ) & \
      SPICTRL_CAP_REV_MASK ) )
#define SPICTRL_CAP_REV( _val ) \
  ( ( ( _val ) << SPICTRL_CAP_REV_SHIFT ) & \
    SPICTRL_CAP_REV_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPICTRLMODE Mode register (MODE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPICTRL_MODE_LOOP 0x40000000U

#define SPICTRL_MODE_CPOL 0x20000000U

#define SPICTRL_MODE_CPHA 0x10000000U

#define SPICTRL_MODE_DIV_16 0x8000000U

#define SPICTRL_MODE_REV 0x4000000U

#define SPICTRL_MODE_MX 0x2000000U

#define SPICTRL_MODE_EN 0x1000000U

#define SPICTRL_MODE_LEN_SHIFT 20
#define SPICTRL_MODE_LEN_MASK 0xf00000U
#define SPICTRL_MODE_LEN_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_MODE_LEN_MASK ) >> \
    SPICTRL_MODE_LEN_SHIFT )
#define SPICTRL_MODE_LEN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_MODE_LEN_MASK ) | \
    ( ( ( _val ) << SPICTRL_MODE_LEN_SHIFT ) & \
      SPICTRL_MODE_LEN_MASK ) )
#define SPICTRL_MODE_LEN( _val ) \
  ( ( ( _val ) << SPICTRL_MODE_LEN_SHIFT ) & \
    SPICTRL_MODE_LEN_MASK )

#define SPICTRL_MODE_PM_SHIFT 16
#define SPICTRL_MODE_PM_MASK 0xf0000U
#define SPICTRL_MODE_PM_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_MODE_PM_MASK ) >> \
    SPICTRL_MODE_PM_SHIFT )
#define SPICTRL_MODE_PM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_MODE_PM_MASK ) | \
    ( ( ( _val ) << SPICTRL_MODE_PM_SHIFT ) & \
      SPICTRL_MODE_PM_MASK ) )
#define SPICTRL_MODE_PM( _val ) \
  ( ( ( _val ) << SPICTRL_MODE_PM_SHIFT ) & \
    SPICTRL_MODE_PM_MASK )

#define SPICTRL_MODE_TWEN 0x8000U

#define SPICTRL_MODE_ASEL 0x4000U

#define SPICTRL_MODE_FACT 0x2000U

#define SPICTRL_MODE_OD 0x1000U

#define SPICTRL_MODE_CG_SHIFT 7
#define SPICTRL_MODE_CG_MASK 0xf80U
#define SPICTRL_MODE_CG_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_MODE_CG_MASK ) >> \
    SPICTRL_MODE_CG_SHIFT )
#define SPICTRL_MODE_CG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_MODE_CG_MASK ) | \
    ( ( ( _val ) << SPICTRL_MODE_CG_SHIFT ) & \
      SPICTRL_MODE_CG_MASK ) )
#define SPICTRL_MODE_CG( _val ) \
  ( ( ( _val ) << SPICTRL_MODE_CG_SHIFT ) & \
    SPICTRL_MODE_CG_MASK )

#define SPICTRL_MODE_ASELDEL_SHIFT 5
#define SPICTRL_MODE_ASELDEL_MASK 0x60U
#define SPICTRL_MODE_ASELDEL_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_MODE_ASELDEL_MASK ) >> \
    SPICTRL_MODE_ASELDEL_SHIFT )
#define SPICTRL_MODE_ASELDEL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_MODE_ASELDEL_MASK ) | \
    ( ( ( _val ) << SPICTRL_MODE_ASELDEL_SHIFT ) & \
      SPICTRL_MODE_ASELDEL_MASK ) )
#define SPICTRL_MODE_ASELDEL( _val ) \
  ( ( ( _val ) << SPICTRL_MODE_ASELDEL_SHIFT ) & \
    SPICTRL_MODE_ASELDEL_MASK )

#define SPICTRL_MODE_TAC 0x10U

#define SPICTRL_MODE_TTO 0x8U

#define SPICTRL_MODE_IGSEL 0x4U

#define SPICTRL_MODE_CITE 0x2U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPICTRLEVENT Event register (EVENT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPICTRL_EVENT_TIP 0x80000000U

#define SPICTRL_EVENT_LT 0x4000U

#define SPICTRL_EVENT_OV 0x1000U

#define SPICTRL_EVENT_UN 0x800U

#define SPICTRL_EVENT_MME 0x400U

#define SPICTRL_EVENT_NE 0x200U

#define SPICTRL_EVENT_NF 0x100U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPICTRLMASK Mask register (MASK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPICTRL_MASK_TIPE 0x80000000U

#define SPICTRL_MASK_LTE 0x4000U

#define SPICTRL_MASK_OVE 0x1000U

#define SPICTRL_MASK_UNE 0x800U

#define SPICTRL_MASK_MMEE 0x400U

#define SPICTRL_MASK_NEEE 0x200U

#define SPICTRL_MASK_NFE 0x100U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPICTRLCMD Command register (CMD)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPICTRL_CMD_LST 0x400000U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPICTRLTX Transmit register (TX)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPICTRL_TX_TDATA_SHIFT 0
#define SPICTRL_TX_TDATA_MASK 0xffffffffU
#define SPICTRL_TX_TDATA_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_TX_TDATA_MASK ) >> \
    SPICTRL_TX_TDATA_SHIFT )
#define SPICTRL_TX_TDATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_TX_TDATA_MASK ) | \
    ( ( ( _val ) << SPICTRL_TX_TDATA_SHIFT ) & \
      SPICTRL_TX_TDATA_MASK ) )
#define SPICTRL_TX_TDATA( _val ) \
  ( ( ( _val ) << SPICTRL_TX_TDATA_SHIFT ) & \
    SPICTRL_TX_TDATA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPICTRLRX Receive register (RX)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPICTRL_RX_RDATA_SHIFT 0
#define SPICTRL_RX_RDATA_MASK 0xffffffffU
#define SPICTRL_RX_RDATA_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_RX_RDATA_MASK ) >> \
    SPICTRL_RX_RDATA_SHIFT )
#define SPICTRL_RX_RDATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_RX_RDATA_MASK ) | \
    ( ( ( _val ) << SPICTRL_RX_RDATA_SHIFT ) & \
      SPICTRL_RX_RDATA_MASK ) )
#define SPICTRL_RX_RDATA( _val ) \
  ( ( ( _val ) << SPICTRL_RX_RDATA_SHIFT ) & \
    SPICTRL_RX_RDATA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPICTRLSLVSEL Slave select register (SLVSEL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPICTRL_SLVSEL_SLVSEL_SHIFT 0
#define SPICTRL_SLVSEL_SLVSEL_MASK 0x3U
#define SPICTRL_SLVSEL_SLVSEL_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_SLVSEL_SLVSEL_MASK ) >> \
    SPICTRL_SLVSEL_SLVSEL_SHIFT )
#define SPICTRL_SLVSEL_SLVSEL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_SLVSEL_SLVSEL_MASK ) | \
    ( ( ( _val ) << SPICTRL_SLVSEL_SLVSEL_SHIFT ) & \
      SPICTRL_SLVSEL_SLVSEL_MASK ) )
#define SPICTRL_SLVSEL_SLVSEL( _val ) \
  ( ( ( _val ) << SPICTRL_SLVSEL_SLVSEL_SHIFT ) & \
    SPICTRL_SLVSEL_SLVSEL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPICTRLASLVSEL \
 *   Automatic slave select register (ASLVSEL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPICTRL_ASLVSEL_ASLVSEL_SHIFT 0
#define SPICTRL_ASLVSEL_ASLVSEL_MASK 0x3U
#define SPICTRL_ASLVSEL_ASLVSEL_GET( _reg ) \
  ( ( ( _reg ) & SPICTRL_ASLVSEL_ASLVSEL_MASK ) >> \
    SPICTRL_ASLVSEL_ASLVSEL_SHIFT )
#define SPICTRL_ASLVSEL_ASLVSEL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPICTRL_ASLVSEL_ASLVSEL_MASK ) | \
    ( ( ( _val ) << SPICTRL_ASLVSEL_ASLVSEL_SHIFT ) & \
      SPICTRL_ASLVSEL_ASLVSEL_MASK ) )
#define SPICTRL_ASLVSEL_ASLVSEL( _val ) \
  ( ( ( _val ) << SPICTRL_ASLVSEL_ASLVSEL_SHIFT ) & \
    SPICTRL_ASLVSEL_ASLVSEL_MASK )

/** @} */

/**
 * @brief This structure defines the SPICTRL register block memory map.
 */
typedef struct spictrl {
  /**
   * @brief See @ref RTEMSDeviceGRLIBSPICTRLCAP.
   */
  uint32_t cap;

  uint32_t reserved_4_20[ 7 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPICTRLMODE.
   */
  uint32_t mode;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPICTRLEVENT.
   */
  uint32_t event;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPICTRLMASK.
   */
  uint32_t mask;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPICTRLCMD.
   */
  uint32_t cmd;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPICTRLTX.
   */
  uint32_t tx;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPICTRLRX.
   */
  uint32_t rx;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPICTRLSLVSEL.
   */
  uint32_t slvsel;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPICTRLASLVSEL.
   */
  uint32_t aslvsel;
} spictrl;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_SPICTRL_REGS_H */
