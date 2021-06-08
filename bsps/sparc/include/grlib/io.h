/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBIO
 *
 * @brief This header file defines the register load/store interface.
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

/* Generated from spec:/bsp/sparc/if/grlib-io-header */

#ifndef _GRLIB_IO_H
#define _GRLIB_IO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/bsp/sparc/if/grlib-io-group */

/**
 * @defgroup RTEMSDeviceGRLIBIO Register Load/Store
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRLIB register load/store API.
 */

/* Generated from spec:/bsp/sparc/if/grlib-load-08 */

/**
 * @ingroup RTEMSDeviceGRLIBIO
 *
 * @brief Loads the memory-mapped unsigned 8-bit register.
 *
 * @param address is the address of the memory-mapped unsigned 8-bit register
 *   to load.
 *
 * @return Returns the loaded register value.
 */
static inline uint8_t grlib_load_8( const volatile uint8_t *address )
{
  return *address;
}

/* Generated from spec:/bsp/sparc/if/grlib-load-16 */

/**
 * @ingroup RTEMSDeviceGRLIBIO
 *
 * @brief Loads the memory-mapped unsigned 16-bit register.
 *
 * @param address is the address of the memory-mapped unsigned 16-bit register
 *   to load.
 *
 * @return Returns the loaded register value.
 */
static inline uint16_t grlib_load_16( const volatile uint16_t *address )
{
  return *address;
}

/* Generated from spec:/bsp/sparc/if/grlib-load-32 */

/**
 * @ingroup RTEMSDeviceGRLIBIO
 *
 * @brief Loads the memory-mapped unsigned 32-bit register.
 *
 * @param address is the address of the memory-mapped unsigned 32-bit register
 *   to load.
 *
 * @return Returns the loaded register value.
 */
static inline uint32_t grlib_load_32( const volatile uint32_t *address )
{
  return *address;
}

/* Generated from spec:/bsp/sparc/if/grlib-load-64 */

/**
 * @ingroup RTEMSDeviceGRLIBIO
 *
 * @brief Loads the memory-mapped unsigned 64-bit register.
 *
 * @param address is the address of the memory-mapped unsigned 64-bit register
 *   to load.
 *
 * @return Returns the loaded register value.
 */
static inline uint64_t grlib_load_64( const volatile uint64_t *address )
{
  return *address;
}

/* Generated from spec:/bsp/sparc/if/grlib-store-08 */

/**
 * @ingroup RTEMSDeviceGRLIBIO
 *
 * @brief Stores the value to the memory-mapped unsigned 8-bit register.
 *
 * @param address is the address of the memory-mapped unsigned 8-bit register.
 *
 * @param value is the value to store.
 */
static inline void grlib_store_8( volatile uint8_t *address, uint8_t value )
{
  *address = value;
}

/* Generated from spec:/bsp/sparc/if/grlib-store-16 */

/**
 * @ingroup RTEMSDeviceGRLIBIO
 *
 * @brief Stores the value to the memory-mapped unsigned 16-bit register.
 *
 * @param address is the address of the memory-mapped unsigned 16-bit register.
 *
 * @param value is the value to store.
 */
static inline void grlib_store_16( volatile uint16_t *address, uint16_t value )
{
  *address = value;
}

/* Generated from spec:/bsp/sparc/if/grlib-store-32 */

/**
 * @ingroup RTEMSDeviceGRLIBIO
 *
 * @brief Stores the value to the memory-mapped unsigned 32-bit register.
 *
 * @param address is the address of the memory-mapped unsigned 32-bit register.
 *
 * @param value is the value to store.
 */
static inline void grlib_store_32( volatile uint32_t *address, uint32_t value )
{
  *address = value;
}

/* Generated from spec:/bsp/sparc/if/grlib-store-64 */

/**
 * @ingroup RTEMSDeviceGRLIBIO
 *
 * @brief Stores the value to the memory-mapped unsigned 64-bit register.
 *
 * @param address is the address of the memory-mapped unsigned 64-bit register.
 *
 * @param value is the value to store.
 */
static inline void grlib_store_64( volatile uint64_t *address, uint64_t value )
{
  *address = value;
}

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_IO_H */
