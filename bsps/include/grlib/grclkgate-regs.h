/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRCLKGATE
 *
 * @brief This header file defines the GRCLKGATE register block interface.
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

/* Generated from spec:/dev/grlib/if/grclkgate-header */

#ifndef _GRLIB_GRCLKGATE_REGS_H
#define _GRLIB_GRCLKGATE_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grclkgate */

/**
 * @defgroup RTEMSDeviceGRCLKGATE GRCLKGATE
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRCLKGATE interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRCLKGATEUNLOCK Unlock register (UNLOCK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCLKGATE_UNLOCK_UNLOCK_SHIFT 0
#define GRCLKGATE_UNLOCK_UNLOCK_MASK 0xffffffffU
#define GRCLKGATE_UNLOCK_UNLOCK_GET( _reg ) \
  ( ( ( _reg ) & GRCLKGATE_UNLOCK_UNLOCK_MASK ) >> \
    GRCLKGATE_UNLOCK_UNLOCK_SHIFT )
#define GRCLKGATE_UNLOCK_UNLOCK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCLKGATE_UNLOCK_UNLOCK_MASK ) | \
    ( ( ( _val ) << GRCLKGATE_UNLOCK_UNLOCK_SHIFT ) & \
      GRCLKGATE_UNLOCK_UNLOCK_MASK ) )
#define GRCLKGATE_UNLOCK_UNLOCK( _val ) \
  ( ( ( _val ) << GRCLKGATE_UNLOCK_UNLOCK_SHIFT ) & \
    GRCLKGATE_UNLOCK_UNLOCK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCLKGATECLKEN Clock enable register (CLKEN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCLKGATE_CLKEN_ENABLE_SHIFT 0
#define GRCLKGATE_CLKEN_ENABLE_MASK 0xffffffffU
#define GRCLKGATE_CLKEN_ENABLE_GET( _reg ) \
  ( ( ( _reg ) & GRCLKGATE_CLKEN_ENABLE_MASK ) >> \
    GRCLKGATE_CLKEN_ENABLE_SHIFT )
#define GRCLKGATE_CLKEN_ENABLE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCLKGATE_CLKEN_ENABLE_MASK ) | \
    ( ( ( _val ) << GRCLKGATE_CLKEN_ENABLE_SHIFT ) & \
      GRCLKGATE_CLKEN_ENABLE_MASK ) )
#define GRCLKGATE_CLKEN_ENABLE( _val ) \
  ( ( ( _val ) << GRCLKGATE_CLKEN_ENABLE_SHIFT ) & \
    GRCLKGATE_CLKEN_ENABLE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCLKGATERESET Reset register (RESET)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCLKGATE_RESET_RESET_SHIFT 0
#define GRCLKGATE_RESET_RESET_MASK 0xffffffffU
#define GRCLKGATE_RESET_RESET_GET( _reg ) \
  ( ( ( _reg ) & GRCLKGATE_RESET_RESET_MASK ) >> \
    GRCLKGATE_RESET_RESET_SHIFT )
#define GRCLKGATE_RESET_RESET_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCLKGATE_RESET_RESET_MASK ) | \
    ( ( ( _val ) << GRCLKGATE_RESET_RESET_SHIFT ) & \
      GRCLKGATE_RESET_RESET_MASK ) )
#define GRCLKGATE_RESET_RESET( _val ) \
  ( ( ( _val ) << GRCLKGATE_RESET_RESET_SHIFT ) & \
    GRCLKGATE_RESET_RESET_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCLKGATEOVERRIDE CPU/FPU override register (OVERRIDE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRCLKGATE_OVERRIDE_FOVERRIDE_SHIFT 16
#define GRCLKGATE_OVERRIDE_FOVERRIDE_MASK 0xf0000U
#define GRCLKGATE_OVERRIDE_FOVERRIDE_GET( _reg ) \
  ( ( ( _reg ) & GRCLKGATE_OVERRIDE_FOVERRIDE_MASK ) >> \
    GRCLKGATE_OVERRIDE_FOVERRIDE_SHIFT )
#define GRCLKGATE_OVERRIDE_FOVERRIDE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCLKGATE_OVERRIDE_FOVERRIDE_MASK ) | \
    ( ( ( _val ) << GRCLKGATE_OVERRIDE_FOVERRIDE_SHIFT ) & \
      GRCLKGATE_OVERRIDE_FOVERRIDE_MASK ) )
#define GRCLKGATE_OVERRIDE_FOVERRIDE( _val ) \
  ( ( ( _val ) << GRCLKGATE_OVERRIDE_FOVERRIDE_SHIFT ) & \
    GRCLKGATE_OVERRIDE_FOVERRIDE_MASK )

#define GRCLKGATE_OVERRIDE_OVERRIDE_SHIFT 0
#define GRCLKGATE_OVERRIDE_OVERRIDE_MASK 0xfU
#define GRCLKGATE_OVERRIDE_OVERRIDE_GET( _reg ) \
  ( ( ( _reg ) & GRCLKGATE_OVERRIDE_OVERRIDE_MASK ) >> \
    GRCLKGATE_OVERRIDE_OVERRIDE_SHIFT )
#define GRCLKGATE_OVERRIDE_OVERRIDE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRCLKGATE_OVERRIDE_OVERRIDE_MASK ) | \
    ( ( ( _val ) << GRCLKGATE_OVERRIDE_OVERRIDE_SHIFT ) & \
      GRCLKGATE_OVERRIDE_OVERRIDE_MASK ) )
#define GRCLKGATE_OVERRIDE_OVERRIDE( _val ) \
  ( ( ( _val ) << GRCLKGATE_OVERRIDE_OVERRIDE_SHIFT ) & \
    GRCLKGATE_OVERRIDE_OVERRIDE_MASK )

/** @} */

/**
 * @brief This structure defines the GRCLKGATE register block memory map.
 */
typedef struct grclkgate {
  /**
   * @brief See @ref RTEMSDeviceGRCLKGATEUNLOCK.
   */
  uint32_t unlock;

  /**
   * @brief See @ref RTEMSDeviceGRCLKGATECLKEN.
   */
  uint32_t clken;

  /**
   * @brief See @ref RTEMSDeviceGRCLKGATERESET.
   */
  uint32_t reset;

  /**
   * @brief See @ref RTEMSDeviceGRCLKGATEOVERRIDE.
   */
  uint32_t override;
} grclkgate;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRCLKGATE_REGS_H */
