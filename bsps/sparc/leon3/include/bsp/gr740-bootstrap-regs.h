/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsGR740Bootstrap
 *
 * @brief This header file defines the GR740 Boostrap Signals register block
 *   interface.
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

/* Generated from spec:/bsp/sparc/leon3/if/gr740-bootstrap-header */

#ifndef _BSP_GR740_BOOTSTRAP_REGS_H
#define _BSP_GR740_BOOTSTRAP_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/bsp/sparc/leon3/if/gr740-bootstrap */

/**
 * @defgroup RTEMSBSPsGR740Bootstrap GR740 Bootstrap Signals
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This group contains the GR740 Bootstrap Signals interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSBSPsGR740BootstrapBOOTSTRAP Bootstrap register (BOOTSTRAP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GR740_BOOTSTRAP_BOOTSTRAP_B10 0x2000000U

#define GR740_BOOTSTRAP_BOOTSTRAP_B9 0x1000000U

#define GR740_BOOTSTRAP_BOOTSTRAP_B8 0x800000U

#define GR740_BOOTSTRAP_BOOTSTRAP_B7 0x400000U

#define GR740_BOOTSTRAP_BOOTSTRAP_B6 0x200000U

#define GR740_BOOTSTRAP_BOOTSTRAP_B5 0x100000U

#define GR740_BOOTSTRAP_BOOTSTRAP_B4 0x80000U

#define GR740_BOOTSTRAP_BOOTSTRAP_B3 0x40000U

#define GR740_BOOTSTRAP_BOOTSTRAP_B2 0x20000U

#define GR740_BOOTSTRAP_BOOTSTRAP_B1 0x10000U

#define GR740_BOOTSTRAP_BOOTSTRAP_GPIO_SHIFT 0
#define GR740_BOOTSTRAP_BOOTSTRAP_GPIO_MASK 0xffffU
#define GR740_BOOTSTRAP_BOOTSTRAP_GPIO_GET( _reg ) \
  ( ( ( _reg ) & GR740_BOOTSTRAP_BOOTSTRAP_GPIO_MASK ) >> \
    GR740_BOOTSTRAP_BOOTSTRAP_GPIO_SHIFT )
#define GR740_BOOTSTRAP_BOOTSTRAP_GPIO_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GR740_BOOTSTRAP_BOOTSTRAP_GPIO_MASK ) | \
    ( ( ( _val ) << GR740_BOOTSTRAP_BOOTSTRAP_GPIO_SHIFT ) & \
      GR740_BOOTSTRAP_BOOTSTRAP_GPIO_MASK ) )
#define GR740_BOOTSTRAP_BOOTSTRAP_GPIO( _val ) \
  ( ( ( _val ) << GR740_BOOTSTRAP_BOOTSTRAP_GPIO_SHIFT ) & \
    GR740_BOOTSTRAP_BOOTSTRAP_GPIO_MASK )

/** @} */

/**
 * @brief This structure defines the GR740 Bootstrap Signals register block
 *   memory map.
 */
typedef struct gr740_bootstrap {
  /**
   * @brief See @ref RTEMSBSPsGR740BootstrapBOOTSTRAP.
   */
  uint32_t bootstrap;
} gr740_bootstrap;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _BSP_GR740_BOOTSTRAP_REGS_H */
