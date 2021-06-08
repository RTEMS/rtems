/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBL4STAT
 *
 * @brief This header file defines the L4STAT register block interface.
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

/* Generated from spec:/dev/grlib/if/l4stat-header */

#ifndef _GRLIB_L4STAT_REGS_H
#define _GRLIB_L4STAT_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/l4stat */

/**
 * @defgroup RTEMSDeviceGRLIBL4STAT L4STAT
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the L4STAT interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBL4STATCVAL Counter 0-15 value register (CVAL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L4STAT_CVAL_CVAL_SHIFT 0
#define L4STAT_CVAL_CVAL_MASK 0xffffffffU
#define L4STAT_CVAL_CVAL_GET( _reg ) \
  ( ( ( _reg ) & L4STAT_CVAL_CVAL_MASK ) >> \
    L4STAT_CVAL_CVAL_SHIFT )
#define L4STAT_CVAL_CVAL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L4STAT_CVAL_CVAL_MASK ) | \
    ( ( ( _val ) << L4STAT_CVAL_CVAL_SHIFT ) & \
      L4STAT_CVAL_CVAL_MASK ) )
#define L4STAT_CVAL_CVAL( _val ) \
  ( ( ( _val ) << L4STAT_CVAL_CVAL_SHIFT ) & \
    L4STAT_CVAL_CVAL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL4STATCCTRL Counter 0-15 control register (CCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L4STAT_CCTRL_NCPU_SHIFT 28
#define L4STAT_CCTRL_NCPU_MASK 0xf0000000U
#define L4STAT_CCTRL_NCPU_GET( _reg ) \
  ( ( ( _reg ) & L4STAT_CCTRL_NCPU_MASK ) >> \
    L4STAT_CCTRL_NCPU_SHIFT )
#define L4STAT_CCTRL_NCPU_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L4STAT_CCTRL_NCPU_MASK ) | \
    ( ( ( _val ) << L4STAT_CCTRL_NCPU_SHIFT ) & \
      L4STAT_CCTRL_NCPU_MASK ) )
#define L4STAT_CCTRL_NCPU( _val ) \
  ( ( ( _val ) << L4STAT_CCTRL_NCPU_SHIFT ) & \
    L4STAT_CCTRL_NCPU_MASK )

#define L4STAT_CCTRL_NCNT_SHIFT 23
#define L4STAT_CCTRL_NCNT_MASK 0xf800000U
#define L4STAT_CCTRL_NCNT_GET( _reg ) \
  ( ( ( _reg ) & L4STAT_CCTRL_NCNT_MASK ) >> \
    L4STAT_CCTRL_NCNT_SHIFT )
#define L4STAT_CCTRL_NCNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L4STAT_CCTRL_NCNT_MASK ) | \
    ( ( ( _val ) << L4STAT_CCTRL_NCNT_SHIFT ) & \
      L4STAT_CCTRL_NCNT_MASK ) )
#define L4STAT_CCTRL_NCNT( _val ) \
  ( ( ( _val ) << L4STAT_CCTRL_NCNT_SHIFT ) & \
    L4STAT_CCTRL_NCNT_MASK )

#define L4STAT_CCTRL_MC 0x400000U

#define L4STAT_CCTRL_IA 0x200000U

#define L4STAT_CCTRL_DS 0x100000U

#define L4STAT_CCTRL_EE 0x80000U

#define L4STAT_CCTRL_AE 0x40000U

#define L4STAT_CCTRL_EL 0x20000U

#define L4STAT_CCTRL_CD 0x10000U

#define L4STAT_CCTRL_SU_SHIFT 14
#define L4STAT_CCTRL_SU_MASK 0xc000U
#define L4STAT_CCTRL_SU_GET( _reg ) \
  ( ( ( _reg ) & L4STAT_CCTRL_SU_MASK ) >> \
    L4STAT_CCTRL_SU_SHIFT )
#define L4STAT_CCTRL_SU_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L4STAT_CCTRL_SU_MASK ) | \
    ( ( ( _val ) << L4STAT_CCTRL_SU_SHIFT ) & \
      L4STAT_CCTRL_SU_MASK ) )
#define L4STAT_CCTRL_SU( _val ) \
  ( ( ( _val ) << L4STAT_CCTRL_SU_SHIFT ) & \
    L4STAT_CCTRL_SU_MASK )

#define L4STAT_CCTRL_CL 0x2000U

#define L4STAT_CCTRL_EN 0x1000U

#define L4STAT_CCTRL_EVENT_ID_SHIFT 4
#define L4STAT_CCTRL_EVENT_ID_MASK 0xff0U
#define L4STAT_CCTRL_EVENT_ID_GET( _reg ) \
  ( ( ( _reg ) & L4STAT_CCTRL_EVENT_ID_MASK ) >> \
    L4STAT_CCTRL_EVENT_ID_SHIFT )
#define L4STAT_CCTRL_EVENT_ID_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L4STAT_CCTRL_EVENT_ID_MASK ) | \
    ( ( ( _val ) << L4STAT_CCTRL_EVENT_ID_SHIFT ) & \
      L4STAT_CCTRL_EVENT_ID_MASK ) )
#define L4STAT_CCTRL_EVENT_ID( _val ) \
  ( ( ( _val ) << L4STAT_CCTRL_EVENT_ID_SHIFT ) & \
    L4STAT_CCTRL_EVENT_ID_MASK )

#define L4STAT_CCTRL_CPU_AHBM_SHIFT 0
#define L4STAT_CCTRL_CPU_AHBM_MASK 0xfU
#define L4STAT_CCTRL_CPU_AHBM_GET( _reg ) \
  ( ( ( _reg ) & L4STAT_CCTRL_CPU_AHBM_MASK ) >> \
    L4STAT_CCTRL_CPU_AHBM_SHIFT )
#define L4STAT_CCTRL_CPU_AHBM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L4STAT_CCTRL_CPU_AHBM_MASK ) | \
    ( ( ( _val ) << L4STAT_CCTRL_CPU_AHBM_SHIFT ) & \
      L4STAT_CCTRL_CPU_AHBM_MASK ) )
#define L4STAT_CCTRL_CPU_AHBM( _val ) \
  ( ( ( _val ) << L4STAT_CCTRL_CPU_AHBM_SHIFT ) & \
    L4STAT_CCTRL_CPU_AHBM_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL4STATCSVAL \
 *   Counter 0-15 max/latch register (CSVAL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L4STAT_CSVAL_CSVAL_SHIFT 0
#define L4STAT_CSVAL_CSVAL_MASK 0xffffffffU
#define L4STAT_CSVAL_CSVAL_GET( _reg ) \
  ( ( ( _reg ) & L4STAT_CSVAL_CSVAL_MASK ) >> \
    L4STAT_CSVAL_CSVAL_SHIFT )
#define L4STAT_CSVAL_CSVAL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L4STAT_CSVAL_CSVAL_MASK ) | \
    ( ( ( _val ) << L4STAT_CSVAL_CSVAL_SHIFT ) & \
      L4STAT_CSVAL_CSVAL_MASK ) )
#define L4STAT_CSVAL_CSVAL( _val ) \
  ( ( ( _val ) << L4STAT_CSVAL_CSVAL_SHIFT ) & \
    L4STAT_CSVAL_CSVAL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBL4STATTSTAMP Timestamp register (TSTAMP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define L4STAT_TSTAMP_TSTAMP_SHIFT 0
#define L4STAT_TSTAMP_TSTAMP_MASK 0xffffffffU
#define L4STAT_TSTAMP_TSTAMP_GET( _reg ) \
  ( ( ( _reg ) & L4STAT_TSTAMP_TSTAMP_MASK ) >> \
    L4STAT_TSTAMP_TSTAMP_SHIFT )
#define L4STAT_TSTAMP_TSTAMP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~L4STAT_TSTAMP_TSTAMP_MASK ) | \
    ( ( ( _val ) << L4STAT_TSTAMP_TSTAMP_SHIFT ) & \
      L4STAT_TSTAMP_TSTAMP_MASK ) )
#define L4STAT_TSTAMP_TSTAMP( _val ) \
  ( ( ( _val ) << L4STAT_TSTAMP_TSTAMP_SHIFT ) & \
    L4STAT_TSTAMP_TSTAMP_MASK )

/** @} */

/**
 * @brief This structure defines the L4STAT register block memory map.
 */
typedef struct l4stat {
  /**
   * @brief See @ref RTEMSDeviceGRLIBL4STATCVAL.
   */
  uint32_t cval_0;

  uint32_t reserved_4_3c[ 14 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBL4STATCVAL.
   */
  uint32_t cval_1;

  uint32_t reserved_40_80[ 16 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBL4STATCCTRL.
   */
  uint32_t cctrl_0;

  uint32_t reserved_84_cc[ 18 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBL4STATCCTRL.
   */
  uint32_t cctrl_1;

  uint32_t reserved_d0_100[ 12 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBL4STATCSVAL.
   */
  uint32_t csval_0;

  uint32_t reserved_104_13c[ 14 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBL4STATCSVAL.
   */
  uint32_t csval_1;

  uint32_t reserved_140_180[ 16 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBL4STATTSTAMP.
   */
  uint32_t tstamp;
} l4stat;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_L4STAT_REGS_H */
