/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRGPIO
 *
 * @brief This header file defines the GRGPIO register block interface.
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

/* Generated from spec:/dev/grlib/if/grgpio-header */

#ifndef _GRLIB_GRGPIO_REGS_H
#define _GRLIB_GRGPIO_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grgpio */

/**
 * @defgroup RTEMSDeviceGRGPIO GRGPIO
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRGPIO interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRGPIODATA I/O port data register (DATA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_DATA_DATA_SHIFT 0
#define GRGPIO_DATA_DATA_MASK 0xffffffffU
#define GRGPIO_DATA_DATA_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_DATA_DATA_MASK ) >> \
    GRGPIO_DATA_DATA_SHIFT )
#define GRGPIO_DATA_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_DATA_DATA_MASK ) | \
    ( ( ( _val ) << GRGPIO_DATA_DATA_SHIFT ) & \
      GRGPIO_DATA_DATA_MASK ) )
#define GRGPIO_DATA_DATA( _val ) \
  ( ( ( _val ) << GRGPIO_DATA_DATA_SHIFT ) & \
    GRGPIO_DATA_DATA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOOUTPUT I/O port output register (OUTPUT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_OUTPUT_DATA_SHIFT 0
#define GRGPIO_OUTPUT_DATA_MASK 0xffffffffU
#define GRGPIO_OUTPUT_DATA_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_OUTPUT_DATA_MASK ) >> \
    GRGPIO_OUTPUT_DATA_SHIFT )
#define GRGPIO_OUTPUT_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_OUTPUT_DATA_MASK ) | \
    ( ( ( _val ) << GRGPIO_OUTPUT_DATA_SHIFT ) & \
      GRGPIO_OUTPUT_DATA_MASK ) )
#define GRGPIO_OUTPUT_DATA( _val ) \
  ( ( ( _val ) << GRGPIO_OUTPUT_DATA_SHIFT ) & \
    GRGPIO_OUTPUT_DATA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIODIRECTION I/O port direction register (DIRECTION)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_DIRECTION_DIR_SHIFT 0
#define GRGPIO_DIRECTION_DIR_MASK 0xffffffffU
#define GRGPIO_DIRECTION_DIR_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_DIRECTION_DIR_MASK ) >> \
    GRGPIO_DIRECTION_DIR_SHIFT )
#define GRGPIO_DIRECTION_DIR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_DIRECTION_DIR_MASK ) | \
    ( ( ( _val ) << GRGPIO_DIRECTION_DIR_SHIFT ) & \
      GRGPIO_DIRECTION_DIR_MASK ) )
#define GRGPIO_DIRECTION_DIR( _val ) \
  ( ( ( _val ) << GRGPIO_DIRECTION_DIR_SHIFT ) & \
    GRGPIO_DIRECTION_DIR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOIMASK Interrupt mask register (IMASK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_IMASK_MASK_SHIFT 0
#define GRGPIO_IMASK_MASK_MASK 0xffffffffU
#define GRGPIO_IMASK_MASK_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_IMASK_MASK_MASK ) >> \
    GRGPIO_IMASK_MASK_SHIFT )
#define GRGPIO_IMASK_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_IMASK_MASK_MASK ) | \
    ( ( ( _val ) << GRGPIO_IMASK_MASK_SHIFT ) & \
      GRGPIO_IMASK_MASK_MASK ) )
#define GRGPIO_IMASK_MASK( _val ) \
  ( ( ( _val ) << GRGPIO_IMASK_MASK_SHIFT ) & \
    GRGPIO_IMASK_MASK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOIPOL Interrupt polarity register (IPOL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_IPOL_POL_SHIFT 0
#define GRGPIO_IPOL_POL_MASK 0xffffffffU
#define GRGPIO_IPOL_POL_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_IPOL_POL_MASK ) >> \
    GRGPIO_IPOL_POL_SHIFT )
#define GRGPIO_IPOL_POL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_IPOL_POL_MASK ) | \
    ( ( ( _val ) << GRGPIO_IPOL_POL_SHIFT ) & \
      GRGPIO_IPOL_POL_MASK ) )
#define GRGPIO_IPOL_POL( _val ) \
  ( ( ( _val ) << GRGPIO_IPOL_POL_SHIFT ) & \
    GRGPIO_IPOL_POL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOIEDGE Interrupt edge register (IEDGE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_IEDGE_EDGE_SHIFT 0
#define GRGPIO_IEDGE_EDGE_MASK 0xffffffffU
#define GRGPIO_IEDGE_EDGE_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_IEDGE_EDGE_MASK ) >> \
    GRGPIO_IEDGE_EDGE_SHIFT )
#define GRGPIO_IEDGE_EDGE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_IEDGE_EDGE_MASK ) | \
    ( ( ( _val ) << GRGPIO_IEDGE_EDGE_SHIFT ) & \
      GRGPIO_IEDGE_EDGE_MASK ) )
#define GRGPIO_IEDGE_EDGE( _val ) \
  ( ( ( _val ) << GRGPIO_IEDGE_EDGE_SHIFT ) & \
    GRGPIO_IEDGE_EDGE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOBYPASS Bypass register (BYPASS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_BYPASS_BYPASS_SHIFT 0
#define GRGPIO_BYPASS_BYPASS_MASK 0xffffffffU
#define GRGPIO_BYPASS_BYPASS_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_BYPASS_BYPASS_MASK ) >> \
    GRGPIO_BYPASS_BYPASS_SHIFT )
#define GRGPIO_BYPASS_BYPASS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_BYPASS_BYPASS_MASK ) | \
    ( ( ( _val ) << GRGPIO_BYPASS_BYPASS_SHIFT ) & \
      GRGPIO_BYPASS_BYPASS_MASK ) )
#define GRGPIO_BYPASS_BYPASS( _val ) \
  ( ( ( _val ) << GRGPIO_BYPASS_BYPASS_SHIFT ) & \
    GRGPIO_BYPASS_BYPASS_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOCAP Capability register (CAP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_CAP_PU 0x40000U

#define GRGPIO_CAP_IER 0x20000U

#define GRGPIO_CAP_IFL 0x10000U

#define GRGPIO_CAP_IRQGEN_SHIFT 8
#define GRGPIO_CAP_IRQGEN_MASK 0x1f00U
#define GRGPIO_CAP_IRQGEN_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_CAP_IRQGEN_MASK ) >> \
    GRGPIO_CAP_IRQGEN_SHIFT )
#define GRGPIO_CAP_IRQGEN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_CAP_IRQGEN_MASK ) | \
    ( ( ( _val ) << GRGPIO_CAP_IRQGEN_SHIFT ) & \
      GRGPIO_CAP_IRQGEN_MASK ) )
#define GRGPIO_CAP_IRQGEN( _val ) \
  ( ( ( _val ) << GRGPIO_CAP_IRQGEN_SHIFT ) & \
    GRGPIO_CAP_IRQGEN_MASK )

#define GRGPIO_CAP_NLINES_SHIFT 0
#define GRGPIO_CAP_NLINES_MASK 0x1fU
#define GRGPIO_CAP_NLINES_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_CAP_NLINES_MASK ) >> \
    GRGPIO_CAP_NLINES_SHIFT )
#define GRGPIO_CAP_NLINES_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_CAP_NLINES_MASK ) | \
    ( ( ( _val ) << GRGPIO_CAP_NLINES_SHIFT ) & \
      GRGPIO_CAP_NLINES_MASK ) )
#define GRGPIO_CAP_NLINES( _val ) \
  ( ( ( _val ) << GRGPIO_CAP_NLINES_SHIFT ) & \
    GRGPIO_CAP_NLINES_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOIRQMAPR \
 *   Interrupt map register n, where n = 0 .. 3 (IRQMAPR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_IRQMAPR_IRQMAP_I_0_SHIFT 24
#define GRGPIO_IRQMAPR_IRQMAP_I_0_MASK 0x1f000000U
#define GRGPIO_IRQMAPR_IRQMAP_I_0_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_IRQMAPR_IRQMAP_I_0_MASK ) >> \
    GRGPIO_IRQMAPR_IRQMAP_I_0_SHIFT )
#define GRGPIO_IRQMAPR_IRQMAP_I_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_IRQMAPR_IRQMAP_I_0_MASK ) | \
    ( ( ( _val ) << GRGPIO_IRQMAPR_IRQMAP_I_0_SHIFT ) & \
      GRGPIO_IRQMAPR_IRQMAP_I_0_MASK ) )
#define GRGPIO_IRQMAPR_IRQMAP_I_0( _val ) \
  ( ( ( _val ) << GRGPIO_IRQMAPR_IRQMAP_I_0_SHIFT ) & \
    GRGPIO_IRQMAPR_IRQMAP_I_0_MASK )

#define GRGPIO_IRQMAPR_IRQMAP_I_1_SHIFT 16
#define GRGPIO_IRQMAPR_IRQMAP_I_1_MASK 0x1f0000U
#define GRGPIO_IRQMAPR_IRQMAP_I_1_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_IRQMAPR_IRQMAP_I_1_MASK ) >> \
    GRGPIO_IRQMAPR_IRQMAP_I_1_SHIFT )
#define GRGPIO_IRQMAPR_IRQMAP_I_1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_IRQMAPR_IRQMAP_I_1_MASK ) | \
    ( ( ( _val ) << GRGPIO_IRQMAPR_IRQMAP_I_1_SHIFT ) & \
      GRGPIO_IRQMAPR_IRQMAP_I_1_MASK ) )
#define GRGPIO_IRQMAPR_IRQMAP_I_1( _val ) \
  ( ( ( _val ) << GRGPIO_IRQMAPR_IRQMAP_I_1_SHIFT ) & \
    GRGPIO_IRQMAPR_IRQMAP_I_1_MASK )

#define GRGPIO_IRQMAPR_IRQMAP_I_2_SHIFT 8
#define GRGPIO_IRQMAPR_IRQMAP_I_2_MASK 0x1f00U
#define GRGPIO_IRQMAPR_IRQMAP_I_2_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_IRQMAPR_IRQMAP_I_2_MASK ) >> \
    GRGPIO_IRQMAPR_IRQMAP_I_2_SHIFT )
#define GRGPIO_IRQMAPR_IRQMAP_I_2_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_IRQMAPR_IRQMAP_I_2_MASK ) | \
    ( ( ( _val ) << GRGPIO_IRQMAPR_IRQMAP_I_2_SHIFT ) & \
      GRGPIO_IRQMAPR_IRQMAP_I_2_MASK ) )
#define GRGPIO_IRQMAPR_IRQMAP_I_2( _val ) \
  ( ( ( _val ) << GRGPIO_IRQMAPR_IRQMAP_I_2_SHIFT ) & \
    GRGPIO_IRQMAPR_IRQMAP_I_2_MASK )

#define GRGPIO_IRQMAPR_IRQMAP_I_3_SHIFT 0
#define GRGPIO_IRQMAPR_IRQMAP_I_3_MASK 0x1fU
#define GRGPIO_IRQMAPR_IRQMAP_I_3_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_IRQMAPR_IRQMAP_I_3_MASK ) >> \
    GRGPIO_IRQMAPR_IRQMAP_I_3_SHIFT )
#define GRGPIO_IRQMAPR_IRQMAP_I_3_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_IRQMAPR_IRQMAP_I_3_MASK ) | \
    ( ( ( _val ) << GRGPIO_IRQMAPR_IRQMAP_I_3_SHIFT ) & \
      GRGPIO_IRQMAPR_IRQMAP_I_3_MASK ) )
#define GRGPIO_IRQMAPR_IRQMAP_I_3( _val ) \
  ( ( ( _val ) << GRGPIO_IRQMAPR_IRQMAP_I_3_SHIFT ) & \
    GRGPIO_IRQMAPR_IRQMAP_I_3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOIAVAIL Interrupt available register (IAVAIL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_IAVAIL_IMASK_SHIFT 0
#define GRGPIO_IAVAIL_IMASK_MASK 0xffffffffU
#define GRGPIO_IAVAIL_IMASK_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_IAVAIL_IMASK_MASK ) >> \
    GRGPIO_IAVAIL_IMASK_SHIFT )
#define GRGPIO_IAVAIL_IMASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_IAVAIL_IMASK_MASK ) | \
    ( ( ( _val ) << GRGPIO_IAVAIL_IMASK_SHIFT ) & \
      GRGPIO_IAVAIL_IMASK_MASK ) )
#define GRGPIO_IAVAIL_IMASK( _val ) \
  ( ( ( _val ) << GRGPIO_IAVAIL_IMASK_SHIFT ) & \
    GRGPIO_IAVAIL_IMASK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOIFLAG Interrupt flag register (IFLAG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_IFLAG_IFLAG_SHIFT 0
#define GRGPIO_IFLAG_IFLAG_MASK 0xffffffffU
#define GRGPIO_IFLAG_IFLAG_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_IFLAG_IFLAG_MASK ) >> \
    GRGPIO_IFLAG_IFLAG_SHIFT )
#define GRGPIO_IFLAG_IFLAG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_IFLAG_IFLAG_MASK ) | \
    ( ( ( _val ) << GRGPIO_IFLAG_IFLAG_SHIFT ) & \
      GRGPIO_IFLAG_IFLAG_MASK ) )
#define GRGPIO_IFLAG_IFLAG( _val ) \
  ( ( ( _val ) << GRGPIO_IFLAG_IFLAG_SHIFT ) & \
    GRGPIO_IFLAG_IFLAG_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOIPEN Interrupt enable register (IPEN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_IPEN_IPEN_SHIFT 0
#define GRGPIO_IPEN_IPEN_MASK 0xffffffffU
#define GRGPIO_IPEN_IPEN_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_IPEN_IPEN_MASK ) >> \
    GRGPIO_IPEN_IPEN_SHIFT )
#define GRGPIO_IPEN_IPEN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_IPEN_IPEN_MASK ) | \
    ( ( ( _val ) << GRGPIO_IPEN_IPEN_SHIFT ) & \
      GRGPIO_IPEN_IPEN_MASK ) )
#define GRGPIO_IPEN_IPEN( _val ) \
  ( ( ( _val ) << GRGPIO_IPEN_IPEN_SHIFT ) & \
    GRGPIO_IPEN_IPEN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOPULSE Pulse register (PULSE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_PULSE_PULSE_SHIFT 0
#define GRGPIO_PULSE_PULSE_MASK 0xffffffffU
#define GRGPIO_PULSE_PULSE_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_PULSE_PULSE_MASK ) >> \
    GRGPIO_PULSE_PULSE_SHIFT )
#define GRGPIO_PULSE_PULSE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_PULSE_PULSE_MASK ) | \
    ( ( ( _val ) << GRGPIO_PULSE_PULSE_SHIFT ) & \
      GRGPIO_PULSE_PULSE_MASK ) )
#define GRGPIO_PULSE_PULSE( _val ) \
  ( ( ( _val ) << GRGPIO_PULSE_PULSE_SHIFT ) & \
    GRGPIO_PULSE_PULSE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOLOR Logical-OR registers (LOR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_LOR_DATA_SHIFT 0
#define GRGPIO_LOR_DATA_MASK 0xffffffffU
#define GRGPIO_LOR_DATA_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_LOR_DATA_MASK ) >> \
    GRGPIO_LOR_DATA_SHIFT )
#define GRGPIO_LOR_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_LOR_DATA_MASK ) | \
    ( ( ( _val ) << GRGPIO_LOR_DATA_SHIFT ) & \
      GRGPIO_LOR_DATA_MASK ) )
#define GRGPIO_LOR_DATA( _val ) \
  ( ( ( _val ) << GRGPIO_LOR_DATA_SHIFT ) & \
    GRGPIO_LOR_DATA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOLAND Logical-AND registers (LAND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_LAND_DATA_SHIFT 0
#define GRGPIO_LAND_DATA_MASK 0xffffffffU
#define GRGPIO_LAND_DATA_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_LAND_DATA_MASK ) >> \
    GRGPIO_LAND_DATA_SHIFT )
#define GRGPIO_LAND_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_LAND_DATA_MASK ) | \
    ( ( ( _val ) << GRGPIO_LAND_DATA_SHIFT ) & \
      GRGPIO_LAND_DATA_MASK ) )
#define GRGPIO_LAND_DATA( _val ) \
  ( ( ( _val ) << GRGPIO_LAND_DATA_SHIFT ) & \
    GRGPIO_LAND_DATA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPIOLXOR Logical-XOR registers (LXOR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRGPIO_LXOR_DATA_SHIFT 0
#define GRGPIO_LXOR_DATA_MASK 0xffffffffU
#define GRGPIO_LXOR_DATA_GET( _reg ) \
  ( ( ( _reg ) & GRGPIO_LXOR_DATA_MASK ) >> \
    GRGPIO_LXOR_DATA_SHIFT )
#define GRGPIO_LXOR_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRGPIO_LXOR_DATA_MASK ) | \
    ( ( ( _val ) << GRGPIO_LXOR_DATA_SHIFT ) & \
      GRGPIO_LXOR_DATA_MASK ) )
#define GRGPIO_LXOR_DATA( _val ) \
  ( ( ( _val ) << GRGPIO_LXOR_DATA_SHIFT ) & \
    GRGPIO_LXOR_DATA_MASK )

/** @} */

/**
 * @brief This structure defines the GRGPIO register block memory map.
 */
typedef struct grgpio {
  /**
   * @brief See @ref RTEMSDeviceGRGPIODATA.
   */
  uint32_t data;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOOUTPUT.
   */
  uint32_t output;

  /**
   * @brief See @ref RTEMSDeviceGRGPIODIRECTION.
   */
  uint32_t direction;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOIMASK.
   */
  uint32_t imask;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOIPOL.
   */
  uint32_t ipol;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOIEDGE.
   */
  uint32_t iedge;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOBYPASS.
   */
  uint32_t bypass;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOCAP.
   */
  uint32_t cap;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOIRQMAPR.
   */
  uint32_t irqmapr[ 8 ];

  /**
   * @brief See @ref RTEMSDeviceGRGPIOIAVAIL.
   */
  uint32_t iavail;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOIFLAG.
   */
  uint32_t iflag;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOIPEN.
   */
  uint32_t ipen;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOPULSE.
   */
  uint32_t pulse;

  uint32_t reserved_50_54;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOLOR.
   */
  uint32_t lor_output;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOLOR.
   */
  uint32_t lor_direction;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOLOR.
   */
  uint32_t lor_imask;

  uint32_t reserved_60_64;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOLAND.
   */
  uint32_t land_output;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOLAND.
   */
  uint32_t land_direction;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOLAND.
   */
  uint32_t land_imask;

  uint32_t reserved_70_74;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOLXOR.
   */
  uint32_t lxor_output;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOLXOR.
   */
  uint32_t lxor_direction;

  /**
   * @brief See @ref RTEMSDeviceGRGPIOLXOR.
   */
  uint32_t lxor_imask;
} grgpio;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRGPIO_REGS_H */
