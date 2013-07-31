/**
 * @file
 *
 * @brief Heap handling for uncached RAM
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef NOCACHE_HEAP_H_
#define NOCACHE_HEAP_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief Initialize nocache heap.
 *
 * Initializes a heap for uncached RAM
 */
void altera_cyclone_v_nocache_init_heap( void );

/** @brief Nocache alloc.
 *
 * Allocate memory from uncached heap.
 * @param   size  Number of bytes to be allocated.
 * @returns  Pointer to the allocated memory.
 */
void *altera_cyclone_v_nocache_malloc( const size_t size );

/** @brief Nocache free.
 *
 * Release memory from uncached heap.
 * @param ptr Address of the memory to be released.
 */
void altera_cyclone_v_nocache_free( void *ptr );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NOCACHE_HEAP_H_ */
