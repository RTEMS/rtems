/**
 * @file
 *
 * @ingroup bsp_start
 *
 * @brief System low level start.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_SHARED_START_H
#define LIBBSP_POWERPC_SHARED_START_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup bsp_start System Start
 *
 * @ingroup bsp_kit
 *
 * @brief System low level start.
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
