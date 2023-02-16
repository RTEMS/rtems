/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBFTMCTRL
 *
 * @brief This header file defines the FTMCTRL register block interface.
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

/* Generated from spec:/dev/grlib/if/ftmctrl-header */

#ifndef _GRLIB_FTMCTRL_REGS_H
#define _GRLIB_FTMCTRL_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/ftmctrl */

/**
 * @defgroup RTEMSDeviceGRLIBFTMCTRL FTMCTRL
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the FTMCTRL interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBFTMCTRLMCFG1 \
 *   Memory configuration register 1 (MCFG1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define FTMCTRL_MCFG1_PBRDY 0x40000000U

#define FTMCTRL_MCFG1_ABRDY 0x20000000U

#define FTMCTRL_MCFG1_IOBUSW_SHIFT 27
#define FTMCTRL_MCFG1_IOBUSW_MASK 0x18000000U
#define FTMCTRL_MCFG1_IOBUSW_GET( _reg ) \
  ( ( ( _reg ) & FTMCTRL_MCFG1_IOBUSW_MASK ) >> \
    FTMCTRL_MCFG1_IOBUSW_SHIFT )
#define FTMCTRL_MCFG1_IOBUSW_SET( _reg, _val ) \
  ( ( ( _reg ) & ~FTMCTRL_MCFG1_IOBUSW_MASK ) | \
    ( ( ( _val ) << FTMCTRL_MCFG1_IOBUSW_SHIFT ) & \
      FTMCTRL_MCFG1_IOBUSW_MASK ) )
#define FTMCTRL_MCFG1_IOBUSW( _val ) \
  ( ( ( _val ) << FTMCTRL_MCFG1_IOBUSW_SHIFT ) & \
    FTMCTRL_MCFG1_IOBUSW_MASK )

#define FTMCTRL_MCFG1_IBRDY 0x4000000U

#define FTMCTRL_MCFG1_BEXCN 0x2000000U

#define FTMCTRL_MCFG1_IO_WAITSTATES_SHIFT 20
#define FTMCTRL_MCFG1_IO_WAITSTATES_MASK 0xf00000U
#define FTMCTRL_MCFG1_IO_WAITSTATES_GET( _reg ) \
  ( ( ( _reg ) & FTMCTRL_MCFG1_IO_WAITSTATES_MASK ) >> \
    FTMCTRL_MCFG1_IO_WAITSTATES_SHIFT )
#define FTMCTRL_MCFG1_IO_WAITSTATES_SET( _reg, _val ) \
  ( ( ( _reg ) & ~FTMCTRL_MCFG1_IO_WAITSTATES_MASK ) | \
    ( ( ( _val ) << FTMCTRL_MCFG1_IO_WAITSTATES_SHIFT ) & \
      FTMCTRL_MCFG1_IO_WAITSTATES_MASK ) )
#define FTMCTRL_MCFG1_IO_WAITSTATES( _val ) \
  ( ( ( _val ) << FTMCTRL_MCFG1_IO_WAITSTATES_SHIFT ) & \
    FTMCTRL_MCFG1_IO_WAITSTATES_MASK )

#define FTMCTRL_MCFG1_IOEN 0x80000U

#define FTMCTRL_MCFG1_R 0x40000U

#define FTMCTRL_MCFG1_ROMBANKSZ_SHIFT 14
#define FTMCTRL_MCFG1_ROMBANKSZ_MASK 0x3c000U
#define FTMCTRL_MCFG1_ROMBANKSZ_GET( _reg ) \
  ( ( ( _reg ) & FTMCTRL_MCFG1_ROMBANKSZ_MASK ) >> \
    FTMCTRL_MCFG1_ROMBANKSZ_SHIFT )
#define FTMCTRL_MCFG1_ROMBANKSZ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~FTMCTRL_MCFG1_ROMBANKSZ_MASK ) | \
    ( ( ( _val ) << FTMCTRL_MCFG1_ROMBANKSZ_SHIFT ) & \
      FTMCTRL_MCFG1_ROMBANKSZ_MASK ) )
#define FTMCTRL_MCFG1_ROMBANKSZ( _val ) \
  ( ( ( _val ) << FTMCTRL_MCFG1_ROMBANKSZ_SHIFT ) & \
    FTMCTRL_MCFG1_ROMBANKSZ_MASK )

#define FTMCTRL_MCFG1_PWEN 0x800U

#define FTMCTRL_MCFG1_PROM_WIDTH_SHIFT 8
#define FTMCTRL_MCFG1_PROM_WIDTH_MASK 0x300U
#define FTMCTRL_MCFG1_PROM_WIDTH_GET( _reg ) \
  ( ( ( _reg ) & FTMCTRL_MCFG1_PROM_WIDTH_MASK ) >> \
    FTMCTRL_MCFG1_PROM_WIDTH_SHIFT )
#define FTMCTRL_MCFG1_PROM_WIDTH_SET( _reg, _val ) \
  ( ( ( _reg ) & ~FTMCTRL_MCFG1_PROM_WIDTH_MASK ) | \
    ( ( ( _val ) << FTMCTRL_MCFG1_PROM_WIDTH_SHIFT ) & \
      FTMCTRL_MCFG1_PROM_WIDTH_MASK ) )
#define FTMCTRL_MCFG1_PROM_WIDTH( _val ) \
  ( ( ( _val ) << FTMCTRL_MCFG1_PROM_WIDTH_SHIFT ) & \
    FTMCTRL_MCFG1_PROM_WIDTH_MASK )

#define FTMCTRL_MCFG1_PROM_WRITE_WS_SHIFT 4
#define FTMCTRL_MCFG1_PROM_WRITE_WS_MASK 0xf0U
#define FTMCTRL_MCFG1_PROM_WRITE_WS_GET( _reg ) \
  ( ( ( _reg ) & FTMCTRL_MCFG1_PROM_WRITE_WS_MASK ) >> \
    FTMCTRL_MCFG1_PROM_WRITE_WS_SHIFT )
#define FTMCTRL_MCFG1_PROM_WRITE_WS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~FTMCTRL_MCFG1_PROM_WRITE_WS_MASK ) | \
    ( ( ( _val ) << FTMCTRL_MCFG1_PROM_WRITE_WS_SHIFT ) & \
      FTMCTRL_MCFG1_PROM_WRITE_WS_MASK ) )
#define FTMCTRL_MCFG1_PROM_WRITE_WS( _val ) \
  ( ( ( _val ) << FTMCTRL_MCFG1_PROM_WRITE_WS_SHIFT ) & \
    FTMCTRL_MCFG1_PROM_WRITE_WS_MASK )

#define FTMCTRL_MCFG1_PROM_READ_WS_SHIFT 0
#define FTMCTRL_MCFG1_PROM_READ_WS_MASK 0xfU
#define FTMCTRL_MCFG1_PROM_READ_WS_GET( _reg ) \
  ( ( ( _reg ) & FTMCTRL_MCFG1_PROM_READ_WS_MASK ) >> \
    FTMCTRL_MCFG1_PROM_READ_WS_SHIFT )
#define FTMCTRL_MCFG1_PROM_READ_WS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~FTMCTRL_MCFG1_PROM_READ_WS_MASK ) | \
    ( ( ( _val ) << FTMCTRL_MCFG1_PROM_READ_WS_SHIFT ) & \
      FTMCTRL_MCFG1_PROM_READ_WS_MASK ) )
#define FTMCTRL_MCFG1_PROM_READ_WS( _val ) \
  ( ( ( _val ) << FTMCTRL_MCFG1_PROM_READ_WS_SHIFT ) & \
    FTMCTRL_MCFG1_PROM_READ_WS_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBFTMCTRLMCFG3 \
 *   Memory configuration register 3 (MCFG3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define FTMCTRL_MCFG3_ME 0x8000000U

#define FTMCTRL_MCFG3_WB 0x800U

#define FTMCTRL_MCFG3_RB 0x400U

#define FTMCTRL_MCFG3_PE 0x100U

#define FTMCTRL_MCFG3_TCB_SHIFT 0
#define FTMCTRL_MCFG3_TCB_MASK 0xffU
#define FTMCTRL_MCFG3_TCB_GET( _reg ) \
  ( ( ( _reg ) & FTMCTRL_MCFG3_TCB_MASK ) >> \
    FTMCTRL_MCFG3_TCB_SHIFT )
#define FTMCTRL_MCFG3_TCB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~FTMCTRL_MCFG3_TCB_MASK ) | \
    ( ( ( _val ) << FTMCTRL_MCFG3_TCB_SHIFT ) & \
      FTMCTRL_MCFG3_TCB_MASK ) )
#define FTMCTRL_MCFG3_TCB( _val ) \
  ( ( ( _val ) << FTMCTRL_MCFG3_TCB_SHIFT ) & \
    FTMCTRL_MCFG3_TCB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBFTMCTRLMCFG5 \
 *   Memory configuration register 5 (MCFG5)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define FTMCTRL_MCFG5_IOHWS_SHIFT 23
#define FTMCTRL_MCFG5_IOHWS_MASK 0x3f800000U
#define FTMCTRL_MCFG5_IOHWS_GET( _reg ) \
  ( ( ( _reg ) & FTMCTRL_MCFG5_IOHWS_MASK ) >> \
    FTMCTRL_MCFG5_IOHWS_SHIFT )
#define FTMCTRL_MCFG5_IOHWS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~FTMCTRL_MCFG5_IOHWS_MASK ) | \
    ( ( ( _val ) << FTMCTRL_MCFG5_IOHWS_SHIFT ) & \
      FTMCTRL_MCFG5_IOHWS_MASK ) )
#define FTMCTRL_MCFG5_IOHWS( _val ) \
  ( ( ( _val ) << FTMCTRL_MCFG5_IOHWS_SHIFT ) & \
    FTMCTRL_MCFG5_IOHWS_MASK )

#define FTMCTRL_MCFG5_ROMHWS_SHIFT 7
#define FTMCTRL_MCFG5_ROMHWS_MASK 0x3f80U
#define FTMCTRL_MCFG5_ROMHWS_GET( _reg ) \
  ( ( ( _reg ) & FTMCTRL_MCFG5_ROMHWS_MASK ) >> \
    FTMCTRL_MCFG5_ROMHWS_SHIFT )
#define FTMCTRL_MCFG5_ROMHWS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~FTMCTRL_MCFG5_ROMHWS_MASK ) | \
    ( ( ( _val ) << FTMCTRL_MCFG5_ROMHWS_SHIFT ) & \
      FTMCTRL_MCFG5_ROMHWS_MASK ) )
#define FTMCTRL_MCFG5_ROMHWS( _val ) \
  ( ( ( _val ) << FTMCTRL_MCFG5_ROMHWS_SHIFT ) & \
    FTMCTRL_MCFG5_ROMHWS_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBFTMCTRLMCFG7 \
 *   Memory configuration register 7 (MCFG7)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define FTMCTRL_MCFG7_BRDYNCNT_SHIFT 16
#define FTMCTRL_MCFG7_BRDYNCNT_MASK 0xffff0000U
#define FTMCTRL_MCFG7_BRDYNCNT_GET( _reg ) \
  ( ( ( _reg ) & FTMCTRL_MCFG7_BRDYNCNT_MASK ) >> \
    FTMCTRL_MCFG7_BRDYNCNT_SHIFT )
#define FTMCTRL_MCFG7_BRDYNCNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~FTMCTRL_MCFG7_BRDYNCNT_MASK ) | \
    ( ( ( _val ) << FTMCTRL_MCFG7_BRDYNCNT_SHIFT ) & \
      FTMCTRL_MCFG7_BRDYNCNT_MASK ) )
#define FTMCTRL_MCFG7_BRDYNCNT( _val ) \
  ( ( ( _val ) << FTMCTRL_MCFG7_BRDYNCNT_SHIFT ) & \
    FTMCTRL_MCFG7_BRDYNCNT_MASK )

#define FTMCTRL_MCFG7_BRDYNRLD_SHIFT 0
#define FTMCTRL_MCFG7_BRDYNRLD_MASK 0xffffU
#define FTMCTRL_MCFG7_BRDYNRLD_GET( _reg ) \
  ( ( ( _reg ) & FTMCTRL_MCFG7_BRDYNRLD_MASK ) >> \
    FTMCTRL_MCFG7_BRDYNRLD_SHIFT )
#define FTMCTRL_MCFG7_BRDYNRLD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~FTMCTRL_MCFG7_BRDYNRLD_MASK ) | \
    ( ( ( _val ) << FTMCTRL_MCFG7_BRDYNRLD_SHIFT ) & \
      FTMCTRL_MCFG7_BRDYNRLD_MASK ) )
#define FTMCTRL_MCFG7_BRDYNRLD( _val ) \
  ( ( ( _val ) << FTMCTRL_MCFG7_BRDYNRLD_SHIFT ) & \
    FTMCTRL_MCFG7_BRDYNRLD_MASK )

/** @} */

/**
 * @brief This structure defines the FTMCTRL register block memory map.
 */
typedef struct ftmctrl {
  /**
   * @brief See @ref RTEMSDeviceGRLIBFTMCTRLMCFG1.
   */
  uint32_t mcfg1;

  uint32_t reserved_4_8;

  /**
   * @brief See @ref RTEMSDeviceGRLIBFTMCTRLMCFG3.
   */
  uint32_t mcfg3;

  uint32_t reserved_c_10;

  /**
   * @brief See @ref RTEMSDeviceGRLIBFTMCTRLMCFG5.
   */
  uint32_t mcfg5;

  uint32_t reserved_14_18;

  /**
   * @brief See @ref RTEMSDeviceGRLIBFTMCTRLMCFG7.
   */
  uint32_t mcfg7;
} ftmctrl;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_FTMCTRL_REGS_H */
