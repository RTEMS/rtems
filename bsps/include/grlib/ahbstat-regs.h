/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBAHBSTAT
 *
 * @brief This header file defines the AHBSTAT register block interface.
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

/* Generated from spec:/dev/grlib/if/ahbstat-header */

#ifndef _GRLIB_AHBSTAT_REGS_H
#define _GRLIB_AHBSTAT_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/ahbstat */

/**
 * @defgroup RTEMSDeviceGRLIBAHBSTAT AHBSTAT
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the AHBSTAT interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBAHBSTATAHBS AHB Status register (AHBS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define AHBSTAT_AHBS_ME 0x2000U

#define AHBSTAT_AHBS_FW 0x1000U

#define AHBSTAT_AHBS_CF 0x800U

#define AHBSTAT_AHBS_AF 0x400U

#define AHBSTAT_AHBS_CE 0x200U

#define AHBSTAT_AHBS_NE 0x100U

#define AHBSTAT_AHBS_HWRITE 0x80U

#define AHBSTAT_AHBS_HMASTER_SHIFT 3
#define AHBSTAT_AHBS_HMASTER_MASK 0x78U
#define AHBSTAT_AHBS_HMASTER_GET( _reg ) \
  ( ( ( _reg ) & AHBSTAT_AHBS_HMASTER_MASK ) >> \
    AHBSTAT_AHBS_HMASTER_SHIFT )
#define AHBSTAT_AHBS_HMASTER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~AHBSTAT_AHBS_HMASTER_MASK ) | \
    ( ( ( _val ) << AHBSTAT_AHBS_HMASTER_SHIFT ) & \
      AHBSTAT_AHBS_HMASTER_MASK ) )
#define AHBSTAT_AHBS_HMASTER( _val ) \
  ( ( ( _val ) << AHBSTAT_AHBS_HMASTER_SHIFT ) & \
    AHBSTAT_AHBS_HMASTER_MASK )

#define AHBSTAT_AHBS_HSIZE_SHIFT 0
#define AHBSTAT_AHBS_HSIZE_MASK 0x7U
#define AHBSTAT_AHBS_HSIZE_GET( _reg ) \
  ( ( ( _reg ) & AHBSTAT_AHBS_HSIZE_MASK ) >> \
    AHBSTAT_AHBS_HSIZE_SHIFT )
#define AHBSTAT_AHBS_HSIZE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~AHBSTAT_AHBS_HSIZE_MASK ) | \
    ( ( ( _val ) << AHBSTAT_AHBS_HSIZE_SHIFT ) & \
      AHBSTAT_AHBS_HSIZE_MASK ) )
#define AHBSTAT_AHBS_HSIZE( _val ) \
  ( ( ( _val ) << AHBSTAT_AHBS_HSIZE_SHIFT ) & \
    AHBSTAT_AHBS_HSIZE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBAHBSTATAHBFAR \
 *   AHB Failing address register (AHBFAR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define AHBSTAT_AHBFAR_HADDR_SHIFT 0
#define AHBSTAT_AHBFAR_HADDR_MASK 0xffffffffU
#define AHBSTAT_AHBFAR_HADDR_GET( _reg ) \
  ( ( ( _reg ) & AHBSTAT_AHBFAR_HADDR_MASK ) >> \
    AHBSTAT_AHBFAR_HADDR_SHIFT )
#define AHBSTAT_AHBFAR_HADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~AHBSTAT_AHBFAR_HADDR_MASK ) | \
    ( ( ( _val ) << AHBSTAT_AHBFAR_HADDR_SHIFT ) & \
      AHBSTAT_AHBFAR_HADDR_MASK ) )
#define AHBSTAT_AHBFAR_HADDR( _val ) \
  ( ( ( _val ) << AHBSTAT_AHBFAR_HADDR_SHIFT ) & \
    AHBSTAT_AHBFAR_HADDR_MASK )

/** @} */

/**
 * @brief This structure defines the AHBSTAT register block memory map.
 */
typedef struct ahbstat {
  /**
   * @brief See @ref RTEMSDeviceGRLIBAHBSTATAHBS.
   */
  uint32_t ahbs;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAHBSTATAHBFAR.
   */
  uint32_t ahbfar;
} ahbstat;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_AHBSTAT_REGS_H */
