/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsGR740ThSens
 *
 * @brief This header file defines the GR740 Temperatur Sensor Controller
 *   register block interface.
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

/* Generated from spec:/bsp/sparc/leon3/if/gr740-thsens-header */

#ifndef _BSP_GR740_THSENS_REGS_H
#define _BSP_GR740_THSENS_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/bsp/sparc/leon3/if/gr740-thsens */

/**
 * @defgroup RTEMSBSPsGR740ThSens GR740 Temperatur Sensor Controller
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This group contains the GR740 Temperatur Sensor Controller
 *   interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSBSPsGR740ThSensCTRL Control register (CTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_THSENS_CTRL_DIV_SHIFT 16
#define GR740_THSENS_CTRL_DIV_MASK 0x3ff0000U
#define GR740_THSENS_CTRL_DIV_GET( _reg ) \
  ( ( ( _reg ) & GR740_THSENS_CTRL_DIV_MASK ) >> \
    GR740_THSENS_CTRL_DIV_SHIFT )
#define GR740_THSENS_CTRL_DIV_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_THSENS_CTRL_DIV_MASK ) | \
    ( ( ( _val ) << GR740_THSENS_CTRL_DIV_SHIFT ) & \
      GR740_THSENS_CTRL_DIV_MASK ) )
#define GR740_THSENS_CTRL_DIV( _val ) \
  ( ( ( _val ) << GR740_THSENS_CTRL_DIV_SHIFT ) & \
    GR740_THSENS_CTRL_DIV_MASK )

#define GR740_THSENS_CTRL_ALEN 0x100U

#define GR740_THSENS_CTRL_PDN 0x80U

#define GR740_THSENS_CTRL_DCORRECT_SHIFT 2
#define GR740_THSENS_CTRL_DCORRECT_MASK 0x7cU
#define GR740_THSENS_CTRL_DCORRECT_GET( _reg ) \
  ( ( ( _reg ) & GR740_THSENS_CTRL_DCORRECT_MASK ) >> \
    GR740_THSENS_CTRL_DCORRECT_SHIFT )
#define GR740_THSENS_CTRL_DCORRECT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_THSENS_CTRL_DCORRECT_MASK ) | \
    ( ( ( _val ) << GR740_THSENS_CTRL_DCORRECT_SHIFT ) & \
      GR740_THSENS_CTRL_DCORRECT_MASK ) )
#define GR740_THSENS_CTRL_DCORRECT( _val ) \
  ( ( ( _val ) << GR740_THSENS_CTRL_DCORRECT_SHIFT ) & \
    GR740_THSENS_CTRL_DCORRECT_MASK )

#define GR740_THSENS_CTRL_SRSTN 0x2U

#define GR740_THSENS_CTRL_CLKEN 0x1U

/** @} */

/**
 * @defgroup RTEMSBSPsGR740ThSensSTATUS Status register (STATUS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_THSENS_STATUS_MAX_SHIFT 24
#define GR740_THSENS_STATUS_MAX_MASK 0x7f000000U
#define GR740_THSENS_STATUS_MAX_GET( _reg ) \
  ( ( ( _reg ) & GR740_THSENS_STATUS_MAX_MASK ) >> \
    GR740_THSENS_STATUS_MAX_SHIFT )
#define GR740_THSENS_STATUS_MAX_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_THSENS_STATUS_MAX_MASK ) | \
    ( ( ( _val ) << GR740_THSENS_STATUS_MAX_SHIFT ) & \
      GR740_THSENS_STATUS_MAX_MASK ) )
#define GR740_THSENS_STATUS_MAX( _val ) \
  ( ( ( _val ) << GR740_THSENS_STATUS_MAX_SHIFT ) & \
    GR740_THSENS_STATUS_MAX_MASK )

#define GR740_THSENS_STATUS_MIN_SHIFT 16
#define GR740_THSENS_STATUS_MIN_MASK 0x7f0000U
#define GR740_THSENS_STATUS_MIN_GET( _reg ) \
  ( ( ( _reg ) & GR740_THSENS_STATUS_MIN_MASK ) >> \
    GR740_THSENS_STATUS_MIN_SHIFT )
#define GR740_THSENS_STATUS_MIN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_THSENS_STATUS_MIN_MASK ) | \
    ( ( ( _val ) << GR740_THSENS_STATUS_MIN_SHIFT ) & \
      GR740_THSENS_STATUS_MIN_MASK ) )
#define GR740_THSENS_STATUS_MIN( _val ) \
  ( ( ( _val ) << GR740_THSENS_STATUS_MIN_SHIFT ) & \
    GR740_THSENS_STATUS_MIN_MASK )

#define GR740_THSENS_STATUS_SCLK 0x8000U

#define GR740_THSENS_STATUS_WE 0x400U

#define GR740_THSENS_STATUS_UPD 0x200U

#define GR740_THSENS_STATUS_ALACT 0x100U

#define GR740_THSENS_STATUS_DATA_SHIFT 0
#define GR740_THSENS_STATUS_DATA_MASK 0x7fU
#define GR740_THSENS_STATUS_DATA_GET( _reg ) \
  ( ( ( _reg ) & GR740_THSENS_STATUS_DATA_MASK ) >> \
    GR740_THSENS_STATUS_DATA_SHIFT )
#define GR740_THSENS_STATUS_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_THSENS_STATUS_DATA_MASK ) | \
    ( ( ( _val ) << GR740_THSENS_STATUS_DATA_SHIFT ) & \
      GR740_THSENS_STATUS_DATA_MASK ) )
#define GR740_THSENS_STATUS_DATA( _val ) \
  ( ( ( _val ) << GR740_THSENS_STATUS_DATA_SHIFT ) & \
    GR740_THSENS_STATUS_DATA_MASK )

/** @} */

/**
 * @defgroup RTEMSBSPsGR740ThSensTHRES Threshold register (THRES)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_THSENS_THRES_THRES_SHIFT 0
#define GR740_THSENS_THRES_THRES_MASK 0x7fU
#define GR740_THSENS_THRES_THRES_GET( _reg ) \
  ( ( ( _reg ) & GR740_THSENS_THRES_THRES_MASK ) >> \
    GR740_THSENS_THRES_THRES_SHIFT )
#define GR740_THSENS_THRES_THRES_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_THSENS_THRES_THRES_MASK ) | \
    ( ( ( _val ) << GR740_THSENS_THRES_THRES_SHIFT ) & \
      GR740_THSENS_THRES_THRES_MASK ) )
#define GR740_THSENS_THRES_THRES( _val ) \
  ( ( ( _val ) << GR740_THSENS_THRES_THRES_SHIFT ) & \
    GR740_THSENS_THRES_THRES_MASK )

/** @} */

/**
 * @brief This structure defines the GR740 Temperatur Sensor Controller
 *   register block memory map.
 */
typedef struct gr740_thsens {
  /**
   * @brief See @ref RTEMSBSPsGR740ThSensCTRL.
   */
  uint32_t ctrl;

  /**
   * @brief See @ref RTEMSBSPsGR740ThSensSTATUS.
   */
  uint32_t status;

  /**
   * @brief See @ref RTEMSBSPsGR740ThSensTHRES.
   */
  uint32_t thres;
} gr740_thsens;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _BSP_GR740_THSENS_REGS_H */
