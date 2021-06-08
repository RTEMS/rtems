/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBAHBTRACE
 *
 * @brief This header file defines the AHBTRACE register block interface.
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

/* Generated from spec:/dev/grlib/if/ahbtrace-header */

#ifndef _GRLIB_AHBTRACE_REGS_H
#define _GRLIB_AHBTRACE_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/ahbtrace */

/**
 * @defgroup RTEMSDeviceGRLIBAHBTRACE AHBTRACE
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the AHBTRACE interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBAHBTRACECTRL Trace buffer control register (CTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define AHBTRACE_CTRL_DCNT_SHIFT 16
#define AHBTRACE_CTRL_DCNT_MASK 0x7f0000U
#define AHBTRACE_CTRL_DCNT_GET( _reg ) \
  ( ( ( _reg ) & AHBTRACE_CTRL_DCNT_MASK ) >> \
    AHBTRACE_CTRL_DCNT_SHIFT )
#define AHBTRACE_CTRL_DCNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~AHBTRACE_CTRL_DCNT_MASK ) | \
    ( ( ( _val ) << AHBTRACE_CTRL_DCNT_SHIFT ) & \
      AHBTRACE_CTRL_DCNT_MASK ) )
#define AHBTRACE_CTRL_DCNT( _val ) \
  ( ( ( _val ) << AHBTRACE_CTRL_DCNT_SHIFT ) & \
    AHBTRACE_CTRL_DCNT_MASK )

#define AHBTRACE_CTRL_PF 0x100U

#define AHBTRACE_CTRL_BW_SHIFT 6
#define AHBTRACE_CTRL_BW_MASK 0xc0U
#define AHBTRACE_CTRL_BW_GET( _reg ) \
  ( ( ( _reg ) & AHBTRACE_CTRL_BW_MASK ) >> \
    AHBTRACE_CTRL_BW_SHIFT )
#define AHBTRACE_CTRL_BW_SET( _reg, _val ) \
  ( ( ( _reg ) & ~AHBTRACE_CTRL_BW_MASK ) | \
    ( ( ( _val ) << AHBTRACE_CTRL_BW_SHIFT ) & \
      AHBTRACE_CTRL_BW_MASK ) )
#define AHBTRACE_CTRL_BW( _val ) \
  ( ( ( _val ) << AHBTRACE_CTRL_BW_SHIFT ) & \
    AHBTRACE_CTRL_BW_MASK )

#define AHBTRACE_CTRL_RF 0x20U

#define AHBTRACE_CTRL_AF 0x10U

#define AHBTRACE_CTRL_FR 0x8U

#define AHBTRACE_CTRL_FW 0x4U

#define AHBTRACE_CTRL_DM 0x2U

#define AHBTRACE_CTRL_EN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBAHBTRACEINDEX Trace buffer index register (INDEX)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define AHBTRACE_INDEX_INDEX_SHIFT 4
#define AHBTRACE_INDEX_INDEX_MASK 0x7f0U
#define AHBTRACE_INDEX_INDEX_GET( _reg ) \
  ( ( ( _reg ) & AHBTRACE_INDEX_INDEX_MASK ) >> \
    AHBTRACE_INDEX_INDEX_SHIFT )
#define AHBTRACE_INDEX_INDEX_SET( _reg, _val ) \
  ( ( ( _reg ) & ~AHBTRACE_INDEX_INDEX_MASK ) | \
    ( ( ( _val ) << AHBTRACE_INDEX_INDEX_SHIFT ) & \
      AHBTRACE_INDEX_INDEX_MASK ) )
#define AHBTRACE_INDEX_INDEX( _val ) \
  ( ( ( _val ) << AHBTRACE_INDEX_INDEX_SHIFT ) & \
    AHBTRACE_INDEX_INDEX_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBAHBTRACETIMETAG \
 *   Trace buffer time tag register (TIMETAG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define AHBTRACE_TIMETAG_TIMETAG_SHIFT 0
#define AHBTRACE_TIMETAG_TIMETAG_MASK 0xffffffffU
#define AHBTRACE_TIMETAG_TIMETAG_GET( _reg ) \
  ( ( ( _reg ) & AHBTRACE_TIMETAG_TIMETAG_MASK ) >> \
    AHBTRACE_TIMETAG_TIMETAG_SHIFT )
#define AHBTRACE_TIMETAG_TIMETAG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~AHBTRACE_TIMETAG_TIMETAG_MASK ) | \
    ( ( ( _val ) << AHBTRACE_TIMETAG_TIMETAG_SHIFT ) & \
      AHBTRACE_TIMETAG_TIMETAG_MASK ) )
#define AHBTRACE_TIMETAG_TIMETAG( _val ) \
  ( ( ( _val ) << AHBTRACE_TIMETAG_TIMETAG_SHIFT ) & \
    AHBTRACE_TIMETAG_TIMETAG_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBAHBTRACEMSFILT \
 *   Trace buffer master/slave filter register (MSFILT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define AHBTRACE_MSFILT_SMASK_15_0_SHIFT 16
#define AHBTRACE_MSFILT_SMASK_15_0_MASK 0xffff0000U
#define AHBTRACE_MSFILT_SMASK_15_0_GET( _reg ) \
  ( ( ( _reg ) & AHBTRACE_MSFILT_SMASK_15_0_MASK ) >> \
    AHBTRACE_MSFILT_SMASK_15_0_SHIFT )
#define AHBTRACE_MSFILT_SMASK_15_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~AHBTRACE_MSFILT_SMASK_15_0_MASK ) | \
    ( ( ( _val ) << AHBTRACE_MSFILT_SMASK_15_0_SHIFT ) & \
      AHBTRACE_MSFILT_SMASK_15_0_MASK ) )
#define AHBTRACE_MSFILT_SMASK_15_0( _val ) \
  ( ( ( _val ) << AHBTRACE_MSFILT_SMASK_15_0_SHIFT ) & \
    AHBTRACE_MSFILT_SMASK_15_0_MASK )

#define AHBTRACE_MSFILT_MMASK_15_0_SHIFT 0
#define AHBTRACE_MSFILT_MMASK_15_0_MASK 0xffffU
#define AHBTRACE_MSFILT_MMASK_15_0_GET( _reg ) \
  ( ( ( _reg ) & AHBTRACE_MSFILT_MMASK_15_0_MASK ) >> \
    AHBTRACE_MSFILT_MMASK_15_0_SHIFT )
#define AHBTRACE_MSFILT_MMASK_15_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~AHBTRACE_MSFILT_MMASK_15_0_MASK ) | \
    ( ( ( _val ) << AHBTRACE_MSFILT_MMASK_15_0_SHIFT ) & \
      AHBTRACE_MSFILT_MMASK_15_0_MASK ) )
#define AHBTRACE_MSFILT_MMASK_15_0( _val ) \
  ( ( ( _val ) << AHBTRACE_MSFILT_MMASK_15_0_SHIFT ) & \
    AHBTRACE_MSFILT_MMASK_15_0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBAHBTRACETBBA \
 *   Trace buffer break address registers (TBBA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define AHBTRACE_TBBA_BADDR_31_2_SHIFT 2
#define AHBTRACE_TBBA_BADDR_31_2_MASK 0xfffffffcU
#define AHBTRACE_TBBA_BADDR_31_2_GET( _reg ) \
  ( ( ( _reg ) & AHBTRACE_TBBA_BADDR_31_2_MASK ) >> \
    AHBTRACE_TBBA_BADDR_31_2_SHIFT )
#define AHBTRACE_TBBA_BADDR_31_2_SET( _reg, _val ) \
  ( ( ( _reg ) & ~AHBTRACE_TBBA_BADDR_31_2_MASK ) | \
    ( ( ( _val ) << AHBTRACE_TBBA_BADDR_31_2_SHIFT ) & \
      AHBTRACE_TBBA_BADDR_31_2_MASK ) )
#define AHBTRACE_TBBA_BADDR_31_2( _val ) \
  ( ( ( _val ) << AHBTRACE_TBBA_BADDR_31_2_SHIFT ) & \
    AHBTRACE_TBBA_BADDR_31_2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBAHBTRACETBBM \
 *   Trace buffer break mask registers (TBBM)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define AHBTRACE_TBBM_BMASK_31_2_SHIFT 2
#define AHBTRACE_TBBM_BMASK_31_2_MASK 0xfffffffcU
#define AHBTRACE_TBBM_BMASK_31_2_GET( _reg ) \
  ( ( ( _reg ) & AHBTRACE_TBBM_BMASK_31_2_MASK ) >> \
    AHBTRACE_TBBM_BMASK_31_2_SHIFT )
#define AHBTRACE_TBBM_BMASK_31_2_SET( _reg, _val ) \
  ( ( ( _reg ) & ~AHBTRACE_TBBM_BMASK_31_2_MASK ) | \
    ( ( ( _val ) << AHBTRACE_TBBM_BMASK_31_2_SHIFT ) & \
      AHBTRACE_TBBM_BMASK_31_2_MASK ) )
#define AHBTRACE_TBBM_BMASK_31_2( _val ) \
  ( ( ( _val ) << AHBTRACE_TBBM_BMASK_31_2_SHIFT ) & \
    AHBTRACE_TBBM_BMASK_31_2_MASK )

#define AHBTRACE_TBBM_LD 0x2U

#define AHBTRACE_TBBM_ST 0x1U

/** @} */

/**
 * @brief This structure defines the AHBTRACE register block memory map.
 */
typedef struct ahbtrace {
  /**
   * @brief See @ref RTEMSDeviceGRLIBAHBTRACECTRL.
   */
  uint32_t ctrl;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAHBTRACEINDEX.
   */
  uint32_t index;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAHBTRACETIMETAG.
   */
  uint32_t timetag;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAHBTRACEMSFILT.
   */
  uint32_t msfilt;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAHBTRACETBBA.
   */
  uint32_t tbba_0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAHBTRACETBBM.
   */
  uint32_t tbbm_0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAHBTRACETBBA.
   */
  uint32_t tbba_1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAHBTRACETBBM.
   */
  uint32_t tbbm_1;
} ahbtrace;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_AHBTRACE_REGS_H */
