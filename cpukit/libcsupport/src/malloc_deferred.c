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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_NEWLIB
#include <stdlib.h>

#include "malloc_p.h"

#include <rtems/chain.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threaddispatch.h>

static RTEMS_CHAIN_DEFINE_EMPTY( _Malloc_GC_list );

RTEMS_INTERRUPT_LOCK_DEFINE( static, _Malloc_GC_lock, "Malloc GC" )

bool malloc_is_system_state_OK(void)
{
  return !_System_state_Is_up( _System_state_Get() )
    || _Thread_Dispatch_is_enabled();
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

void malloc_deferred_frees_process( void )
{
  rtems_chain_node *to_be_freed;

  /*
   *  If some free's have been deferred, then do them now.
   */
  while ( ( to_be_freed = _Malloc_Get_deferred_free() ) != NULL ) {
    free( to_be_freed );
  }
}

void malloc_deferred_free( void *p )
{
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire( &_Malloc_GC_lock, &lock_context );
  rtems_chain_append_unprotected( &_Malloc_GC_list, (rtems_chain_node *) p );
  rtems_interrupt_lock_release( &_Malloc_GC_lock, &lock_context );
}
#endif
