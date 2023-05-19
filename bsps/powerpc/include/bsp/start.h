/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup powerpc_start
 *
 * @brief System low level start.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_POWERPC_SHARED_START_H
#define LIBBSP_POWERPC_SHARED_START_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsPowerPCSharedStart PowerPC System Start
 *
 * @ingroup RTEMSBSPsPowerPCShared
 *
 * @brief PowerPC low level start.
 *
 * @{
 */

#define BSP_START_TEXT_SECTION __attribute__((section(".bsp_start_text")))

#define BSP_START_DATA_SECTION __attribute__((section(".bsp_start_data")))

/**
* @brief System start entry.
*/
void _start(void);

/**
 * Zeros @a byte_count bytes starting at @a begin.
 *
 * It wraps around in case of an address overflow.  The stack will not be used.
 * The code is position independent.  It uses the data cache block zero
 * instruction in case the data cache is enabled.  There are no alignment
 * constains for @a begin and @a byte_count.
 *
 * @see bsp_start_zero_begin, bsp_start_zero_end, and bsp_start_zero_size.
 */
void BSP_START_TEXT_SECTION bsp_start_zero(void *begin, size_t byte_count);

/**
 * @brief Symbol which equals the bsp_start_zero() code begin.
 */
extern char bsp_start_zero_begin [];

/**
 * @brief Symbol which equals the bsp_start_zero() code end.
 */
extern char bsp_start_zero_end [];

/**
 * @brief Symbol which equals the bsp_start_zero() code size.
 */
extern char bsp_start_zero_size [];

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_SHARED_START_H */
