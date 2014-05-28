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

#include <assert.h>
#include <bsp/linker-symbols.h>
#include <bsp/nocache-heap.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/apimutex.h>

/** @brief Uncached RAM pool
 *
 * Allocate the whole bsp_nocache for the nocache heap */
static char nocache_pool[1024 * 1024]
  __attribute__( ( section( ".bsp_nocache" ) ) );

/** @brief Nocache heap
 *
 * Heap control for the uncached RAM heap */
static Heap_Control nocache_heap;

/** @brief Init nocache heap
 *
 * Constructor for the uncached RAM heap
 * @returns 0 on succuss, error code from errno.h on failure
 */
void altera_cyclone_v_nocache_init_heap( void )
{
  uintptr_t heap_status = 0;

  heap_status = _Heap_Initialize(
    &nocache_heap,
    &nocache_pool[0],
    sizeof( nocache_pool ),
    0
    );
  assert( heap_status != 0 );
}

/** @brief Allocate uncached RAM
 *
 * Allocates RAM from the uncached heap
 * @param size  Number of bytes to be allocated
 * @returns  Pointer to the allocated RAM
 */
void *altera_cyclone_v_nocache_malloc( const size_t size )
{
  void* ret = NULL;

  _RTEMS_Lock_allocator();
  ret = _Heap_Allocate( &nocache_heap, size );
  _RTEMS_Unlock_allocator();

  return ret;
}

/** @brief Free uncached RAM
 *
 * Releases RAM from the uncached heap
 * @param ptr Address of the RAM to be released
 */
void altera_cyclone_v_nocache_free( void *ptr )
{
  if ( ptr != NULL ) {
    bool ok;

    _RTEMS_Lock_allocator();
    ok = _Heap_Free( &nocache_heap, ptr );
    _RTEMS_Unlock_allocator();

    assert( ok );
  }
}
