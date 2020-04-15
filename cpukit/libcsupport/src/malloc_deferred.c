/**
 *  @file
 *
 *  @brief Malloc Deferred Support
 *  @ingroup libcsupport
 */

/*
 *  Process free requests deferred because they were from ISR
 *  or other critical section.
 *
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
#include <stdlib.h>
#include <string.h>

#include "malloc_p.h"

#include <rtems/chain.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threaddispatch.h>

static RTEMS_CHAIN_DEFINE_EMPTY( _Malloc_GC_list );

RTEMS_INTERRUPT_LOCK_DEFINE( static, _Malloc_GC_lock, "Malloc GC" )

Malloc_System_state _Malloc_System_state( void )
{
  System_state_Codes state = _System_state_Get();

  if ( _System_state_Is_up( state ) ) {
    if ( _Thread_Dispatch_is_enabled() ) {
      return MALLOC_SYSTEM_STATE_NORMAL;
    } else {
      return MALLOC_SYSTEM_STATE_NO_ALLOCATION;
    }
  } else if ( _System_state_Is_before_multitasking( state ) ) {
    return MALLOC_SYSTEM_STATE_NORMAL;
  } else {
    return MALLOC_SYSTEM_STATE_NO_PROTECTION;
  }
}

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

void *rtems_heap_allocate_aligned_with_boundary(
  size_t    size,
  uintptr_t alignment,
  uintptr_t boundary
)
{
  Heap_Control *heap = RTEMS_Malloc_Heap;
  void *p;

  switch ( _Malloc_System_state() ) {
    case MALLOC_SYSTEM_STATE_NORMAL:
      _RTEMS_Lock_allocator();
      _Malloc_Process_deferred_frees();
      p = _Heap_Allocate_aligned_with_boundary(
        heap,
        size,
        alignment,
        boundary
      );
      _RTEMS_Unlock_allocator();
      break;
    case MALLOC_SYSTEM_STATE_NO_PROTECTION:
      p = _Heap_Allocate_aligned_with_boundary(
        heap,
        size,
        alignment,
        boundary
      );
      break;
    default:
      /*
       *  Do not attempt to allocate memory if not in correct system state.
       */
      return NULL;
  }

  if ( p == NULL && alignment == 0 && boundary == 0 ) {
    p = (*rtems_malloc_extend_handler)( heap, size );
  }

  /*
   *  If the user wants us to dirty the allocated memory, then do it.
   */
  if ( p != NULL && rtems_malloc_dirty_helper != NULL )
    (*rtems_malloc_dirty_helper)( p, size );

  return p;
}

void _Malloc_Deferred_free( void *p )
{
  rtems_interrupt_lock_context lock_context;
  rtems_chain_node *node;

  node = (rtems_chain_node *) p;

  rtems_interrupt_lock_acquire( &_Malloc_GC_lock, &lock_context );
  rtems_chain_initialize_node( node );
  rtems_chain_append_unprotected( &_Malloc_GC_list, node );
  rtems_interrupt_lock_release( &_Malloc_GC_lock, &lock_context );
}

void *rtems_malloc( size_t size )
{
  if ( size == 0 ) {
    return NULL;
  }

  return rtems_heap_allocate_aligned_with_boundary( size, 0, 0 );
}

void *rtems_calloc( size_t nelem, size_t elsize )
{
  size_t  length;
  void   *p;

  length = nelem * elsize;
  p = rtems_malloc( length );
  RTEMS_OBFUSCATE_VARIABLE( p );
  if ( RTEMS_PREDICT_FALSE( p == NULL ) ) {
    return p;
  }

  return memset( p, 0, length );
}
#endif
