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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
