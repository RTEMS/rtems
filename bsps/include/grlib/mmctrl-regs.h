/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBMMCTRL
 *
 * @brief This header file defines the MMCTRL register block interface.
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

/* Generated from spec:/dev/grlib/if/mmctrl-header */

#ifndef _GRLIB_MMCTRL_REGS_H
#define _GRLIB_MMCTRL_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/mmctrl */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRL MMCTRL
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the MMCTRL interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLSDCFG1 \
 *   SDRAM configuration register 1 (SDCFG1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_SDCFG1_RF 0x80000000U

#define MMCTRL_SDCFG1_TRP 0x40000000U

#define MMCTRL_SDCFG1_TRFC_SHIFT 27
#define MMCTRL_SDCFG1_TRFC_MASK 0x38000000U
#define MMCTRL_SDCFG1_TRFC_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_SDCFG1_TRFC_MASK ) >> \
    MMCTRL_SDCFG1_TRFC_SHIFT )
#define MMCTRL_SDCFG1_TRFC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_SDCFG1_TRFC_MASK ) | \
    ( ( ( _val ) << MMCTRL_SDCFG1_TRFC_SHIFT ) & \
      MMCTRL_SDCFG1_TRFC_MASK ) )
#define MMCTRL_SDCFG1_TRFC( _val ) \
  ( ( ( _val ) << MMCTRL_SDCFG1_TRFC_SHIFT ) & \
    MMCTRL_SDCFG1_TRFC_MASK )

#define MMCTRL_SDCFG1_TC 0x4000000U

#define MMCTRL_SDCFG1_BANKSZ_SHIFT 23
#define MMCTRL_SDCFG1_BANKSZ_MASK 0x3800000U
#define MMCTRL_SDCFG1_BANKSZ_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_SDCFG1_BANKSZ_MASK ) >> \
    MMCTRL_SDCFG1_BANKSZ_SHIFT )
#define MMCTRL_SDCFG1_BANKSZ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_SDCFG1_BANKSZ_MASK ) | \
    ( ( ( _val ) << MMCTRL_SDCFG1_BANKSZ_SHIFT ) & \
      MMCTRL_SDCFG1_BANKSZ_MASK ) )
#define MMCTRL_SDCFG1_BANKSZ( _val ) \
  ( ( ( _val ) << MMCTRL_SDCFG1_BANKSZ_SHIFT ) & \
    MMCTRL_SDCFG1_BANKSZ_MASK )

#define MMCTRL_SDCFG1_COLSZ_SHIFT 21
#define MMCTRL_SDCFG1_COLSZ_MASK 0x600000U
#define MMCTRL_SDCFG1_COLSZ_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_SDCFG1_COLSZ_MASK ) >> \
    MMCTRL_SDCFG1_COLSZ_SHIFT )
#define MMCTRL_SDCFG1_COLSZ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_SDCFG1_COLSZ_MASK ) | \
    ( ( ( _val ) << MMCTRL_SDCFG1_COLSZ_SHIFT ) & \
      MMCTRL_SDCFG1_COLSZ_MASK ) )
#define MMCTRL_SDCFG1_COLSZ( _val ) \
  ( ( ( _val ) << MMCTRL_SDCFG1_COLSZ_SHIFT ) & \
    MMCTRL_SDCFG1_COLSZ_MASK )

#define MMCTRL_SDCFG1_COMMAND_SHIFT 18
#define MMCTRL_SDCFG1_COMMAND_MASK 0x1c0000U
#define MMCTRL_SDCFG1_COMMAND_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_SDCFG1_COMMAND_MASK ) >> \
    MMCTRL_SDCFG1_COMMAND_SHIFT )
#define MMCTRL_SDCFG1_COMMAND_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_SDCFG1_COMMAND_MASK ) | \
    ( ( ( _val ) << MMCTRL_SDCFG1_COMMAND_SHIFT ) & \
      MMCTRL_SDCFG1_COMMAND_MASK ) )
#define MMCTRL_SDCFG1_COMMAND( _val ) \
  ( ( ( _val ) << MMCTRL_SDCFG1_COMMAND_SHIFT ) & \
    MMCTRL_SDCFG1_COMMAND_MASK )

#define MMCTRL_SDCFG1_MS 0x10000U

#define MMCTRL_SDCFG1_64 0x8000U

#define MMCTRL_SDCFG1_RFLOAD_SHIFT 0
#define MMCTRL_SDCFG1_RFLOAD_MASK 0x7fffU
#define MMCTRL_SDCFG1_RFLOAD_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_SDCFG1_RFLOAD_MASK ) >> \
    MMCTRL_SDCFG1_RFLOAD_SHIFT )
#define MMCTRL_SDCFG1_RFLOAD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_SDCFG1_RFLOAD_MASK ) | \
    ( ( ( _val ) << MMCTRL_SDCFG1_RFLOAD_SHIFT ) & \
      MMCTRL_SDCFG1_RFLOAD_MASK ) )
#define MMCTRL_SDCFG1_RFLOAD( _val ) \
  ( ( ( _val ) << MMCTRL_SDCFG1_RFLOAD_SHIFT ) & \
    MMCTRL_SDCFG1_RFLOAD_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLSDCFG2 \
 *   SDRAM configuration register 2 (SDCFG2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_SDCFG2_CE 0x40000000U

#define MMCTRL_SDCFG2_EN2T 0x8000U

#define MMCTRL_SDCFG2_DCS 0x4000U

#define MMCTRL_SDCFG2_BPARK 0x2000U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLMUXCFG Mux configuration register (MUXCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_MUXCFG_ERRLOC_SHIFT 20
#define MMCTRL_MUXCFG_ERRLOC_MASK 0xfff00000U
#define MMCTRL_MUXCFG_ERRLOC_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_MUXCFG_ERRLOC_MASK ) >> \
    MMCTRL_MUXCFG_ERRLOC_SHIFT )
#define MMCTRL_MUXCFG_ERRLOC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_MUXCFG_ERRLOC_MASK ) | \
    ( ( ( _val ) << MMCTRL_MUXCFG_ERRLOC_SHIFT ) & \
      MMCTRL_MUXCFG_ERRLOC_MASK ) )
#define MMCTRL_MUXCFG_ERRLOC( _val ) \
  ( ( ( _val ) << MMCTRL_MUXCFG_ERRLOC_SHIFT ) & \
    MMCTRL_MUXCFG_ERRLOC_MASK )

#define MMCTRL_MUXCFG_DDERR 0x80000U

#define MMCTRL_MUXCFG_DWIDTH_SHIFT 16
#define MMCTRL_MUXCFG_DWIDTH_MASK 0x70000U
#define MMCTRL_MUXCFG_DWIDTH_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_MUXCFG_DWIDTH_MASK ) >> \
    MMCTRL_MUXCFG_DWIDTH_SHIFT )
#define MMCTRL_MUXCFG_DWIDTH_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_MUXCFG_DWIDTH_MASK ) | \
    ( ( ( _val ) << MMCTRL_MUXCFG_DWIDTH_SHIFT ) & \
      MMCTRL_MUXCFG_DWIDTH_MASK ) )
#define MMCTRL_MUXCFG_DWIDTH( _val ) \
  ( ( ( _val ) << MMCTRL_MUXCFG_DWIDTH_SHIFT ) & \
    MMCTRL_MUXCFG_DWIDTH_MASK )

#define MMCTRL_MUXCFG_BEID_SHIFT 12
#define MMCTRL_MUXCFG_BEID_MASK 0xf000U
#define MMCTRL_MUXCFG_BEID_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_MUXCFG_BEID_MASK ) >> \
    MMCTRL_MUXCFG_BEID_SHIFT )
#define MMCTRL_MUXCFG_BEID_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_MUXCFG_BEID_MASK ) | \
    ( ( ( _val ) << MMCTRL_MUXCFG_BEID_SHIFT ) & \
      MMCTRL_MUXCFG_BEID_MASK ) )
#define MMCTRL_MUXCFG_BEID( _val ) \
  ( ( ( _val ) << MMCTRL_MUXCFG_BEID_SHIFT ) & \
    MMCTRL_MUXCFG_BEID_MASK )

#define MMCTRL_MUXCFG_DATAMUX_SHIFT 5
#define MMCTRL_MUXCFG_DATAMUX_MASK 0xe0U
#define MMCTRL_MUXCFG_DATAMUX_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_MUXCFG_DATAMUX_MASK ) >> \
    MMCTRL_MUXCFG_DATAMUX_SHIFT )
#define MMCTRL_MUXCFG_DATAMUX_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_MUXCFG_DATAMUX_MASK ) | \
    ( ( ( _val ) << MMCTRL_MUXCFG_DATAMUX_SHIFT ) & \
      MMCTRL_MUXCFG_DATAMUX_MASK ) )
#define MMCTRL_MUXCFG_DATAMUX( _val ) \
  ( ( ( _val ) << MMCTRL_MUXCFG_DATAMUX_SHIFT ) & \
    MMCTRL_MUXCFG_DATAMUX_MASK )

#define MMCTRL_MUXCFG_CEN 0x10U

#define MMCTRL_MUXCFG_BAUPD 0x8U

#define MMCTRL_MUXCFG_BAEN 0x4U

#define MMCTRL_MUXCFG_CODE 0x2U

#define MMCTRL_MUXCFG_EDEN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLFTDA FT diagnostic address register (FTDA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_FTDA_FTDA_SHIFT 2
#define MMCTRL_FTDA_FTDA_MASK 0xfffffffcU
#define MMCTRL_FTDA_FTDA_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_FTDA_FTDA_MASK ) >> \
    MMCTRL_FTDA_FTDA_SHIFT )
#define MMCTRL_FTDA_FTDA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_FTDA_FTDA_MASK ) | \
    ( ( ( _val ) << MMCTRL_FTDA_FTDA_SHIFT ) & \
      MMCTRL_FTDA_FTDA_MASK ) )
#define MMCTRL_FTDA_FTDA( _val ) \
  ( ( ( _val ) << MMCTRL_FTDA_FTDA_SHIFT ) & \
    MMCTRL_FTDA_FTDA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLFTDC FT diagnostic checkbits register (FTDC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_FTDC_CBD_SHIFT 24
#define MMCTRL_FTDC_CBD_MASK 0xff000000U
#define MMCTRL_FTDC_CBD_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_FTDC_CBD_MASK ) >> \
    MMCTRL_FTDC_CBD_SHIFT )
#define MMCTRL_FTDC_CBD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_FTDC_CBD_MASK ) | \
    ( ( ( _val ) << MMCTRL_FTDC_CBD_SHIFT ) & \
      MMCTRL_FTDC_CBD_MASK ) )
#define MMCTRL_FTDC_CBD( _val ) \
  ( ( ( _val ) << MMCTRL_FTDC_CBD_SHIFT ) & \
    MMCTRL_FTDC_CBD_MASK )

#define MMCTRL_FTDC_CBC_SHIFT 16
#define MMCTRL_FTDC_CBC_MASK 0xff0000U
#define MMCTRL_FTDC_CBC_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_FTDC_CBC_MASK ) >> \
    MMCTRL_FTDC_CBC_SHIFT )
#define MMCTRL_FTDC_CBC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_FTDC_CBC_MASK ) | \
    ( ( ( _val ) << MMCTRL_FTDC_CBC_SHIFT ) & \
      MMCTRL_FTDC_CBC_MASK ) )
#define MMCTRL_FTDC_CBC( _val ) \
  ( ( ( _val ) << MMCTRL_FTDC_CBC_SHIFT ) & \
    MMCTRL_FTDC_CBC_MASK )

#define MMCTRL_FTDC_CBB_SHIFT 8
#define MMCTRL_FTDC_CBB_MASK 0xff00U
#define MMCTRL_FTDC_CBB_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_FTDC_CBB_MASK ) >> \
    MMCTRL_FTDC_CBB_SHIFT )
#define MMCTRL_FTDC_CBB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_FTDC_CBB_MASK ) | \
    ( ( ( _val ) << MMCTRL_FTDC_CBB_SHIFT ) & \
      MMCTRL_FTDC_CBB_MASK ) )
#define MMCTRL_FTDC_CBB( _val ) \
  ( ( ( _val ) << MMCTRL_FTDC_CBB_SHIFT ) & \
    MMCTRL_FTDC_CBB_MASK )

#define MMCTRL_FTDC_CBA_SHIFT 0
#define MMCTRL_FTDC_CBA_MASK 0xffU
#define MMCTRL_FTDC_CBA_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_FTDC_CBA_MASK ) >> \
    MMCTRL_FTDC_CBA_SHIFT )
#define MMCTRL_FTDC_CBA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_FTDC_CBA_MASK ) | \
    ( ( ( _val ) << MMCTRL_FTDC_CBA_SHIFT ) & \
      MMCTRL_FTDC_CBA_MASK ) )
#define MMCTRL_FTDC_CBA( _val ) \
  ( ( ( _val ) << MMCTRL_FTDC_CBA_SHIFT ) & \
    MMCTRL_FTDC_CBA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLFTDD FT diagnostic data register (FTDD)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_FTDD_DATA_SHIFT 0
#define MMCTRL_FTDD_DATA_MASK 0xffffffffU
#define MMCTRL_FTDD_DATA_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_FTDD_DATA_MASK ) >> \
    MMCTRL_FTDD_DATA_SHIFT )
#define MMCTRL_FTDD_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_FTDD_DATA_MASK ) | \
    ( ( ( _val ) << MMCTRL_FTDD_DATA_SHIFT ) & \
      MMCTRL_FTDD_DATA_MASK ) )
#define MMCTRL_FTDD_DATA( _val ) \
  ( ( ( _val ) << MMCTRL_FTDD_DATA_SHIFT ) & \
    MMCTRL_FTDD_DATA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLFTBND FT boundary address register (FTBND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_FTBND_FTBND_31_3_SHIFT 3
#define MMCTRL_FTBND_FTBND_31_3_MASK 0xfffffff8U
#define MMCTRL_FTBND_FTBND_31_3_GET( _reg ) \
  ( ( ( _reg ) & MMCTRL_FTBND_FTBND_31_3_MASK ) >> \
    MMCTRL_FTBND_FTBND_31_3_SHIFT )
#define MMCTRL_FTBND_FTBND_31_3_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MMCTRL_FTBND_FTBND_31_3_MASK ) | \
    ( ( ( _val ) << MMCTRL_FTBND_FTBND_31_3_SHIFT ) & \
      MMCTRL_FTBND_FTBND_31_3_MASK ) )
#define MMCTRL_FTBND_FTBND_31_3( _val ) \
  ( ( ( _val ) << MMCTRL_FTBND_FTBND_31_3_SHIFT ) & \
    MMCTRL_FTBND_FTBND_31_3_MASK )

/** @} */

/**
 * @brief This structure defines the MMCTRL register block memory map.
 */
typedef struct mmctrl {
  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLSDCFG1.
   */
  uint32_t sdcfg1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLSDCFG2.
   */
  uint32_t sdcfg2;

  uint32_t reserved_8_20[ 6 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLMUXCFG.
   */
  uint32_t muxcfg;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLFTDA.
   */
  uint32_t ftda;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLFTDC.
   */
  uint32_t ftdc;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLFTDD.
   */
  uint32_t ftdd;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLFTBND.
   */
  uint32_t ftbnd;
} mmctrl;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_MMCTRL_REGS_H */
