/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief calloc()
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_NEWLIB
#include "malloc_p.h"
#include <stdlib.h>

#include <rtems/chain.h>

static RTEMS_CHAIN_DEFINE_EMPTY( _Malloc_GC_list );

RTEMS_INTERRUPT_LOCK_DEFINE( static, _Malloc_GC_lock, "Malloc GC" )

static void *_Malloc_Get_deferred_free( void )
{
  rtems_interrupt_lock_context lock_context;
  void *p;

  rtems_interrupt_lock_acquire( &_Malloc_GC_lock, &lock_context );
  p = rtems_chain_get_unprotected( &_Malloc_GC_list );
  rtems_interrupt_lock_release( &_Malloc_GC_lock, &lock_context );

  return p;
}

void _Malloc_Process_deferred_frees( void )
{
  rtems_chain_node *to_be_freed;

  /*
   *  If some free's have been deferred, then do them now.
   */
  while ( ( to_be_freed = _Malloc_Get_deferred_free() ) != NULL ) {
    free( to_be_freed );
  }
}

static void _Malloc_Deferred_free( void *p )
{
  rtems_interrupt_lock_context lock_context;
  rtems_chain_node *node;

  node = (rtems_chain_node *) p;

  rtems_interrupt_lock_acquire( &_Malloc_GC_lock, &lock_context );
  rtems_chain_initialize_node( node );
  rtems_chain_append_unprotected( &_Malloc_GC_list, node );
  rtems_interrupt_lock_release( &_Malloc_GC_lock, &lock_context );
}

void free(
  void *ptr
)
{
  if ( !ptr )
    return;

  /*
   *  Do not attempt to free memory if in a critical section or ISR.
   */
  if ( _Malloc_System_state() != MALLOC_SYSTEM_STATE_NORMAL ) {
      _Malloc_Deferred_free(ptr);
      return;
  }

  if ( !_Protected_heap_Free( RTEMS_Malloc_Heap, ptr ) ) {
    rtems_fatal( RTEMS_FATAL_SOURCE_INVALID_HEAP_FREE, (rtems_fatal_code) ptr );
  }
}
#endif
