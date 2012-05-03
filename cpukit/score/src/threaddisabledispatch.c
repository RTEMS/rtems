/*
 *  _Thread_Disable_dispatch
 *
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/thread.h>

#if defined ( __THREAD_DO_NOT_INLINE_DISABLE_DISPATCH__ )
void _Thread_Disable_dispatch( void )
{
  /*
   *  This check is very brutal to system performance but is very helpful
   *  at finding blown stack problems.  If you have a stack problem and
   *  need help finding it, then uncomment this code.  Every system
   *  call will check the stack and since mutexes are used frequently
   *  in most systems, you might get lucky.
   */
  #if defined(RTEMS_HEAVY_STACK_DEBUG)
    if (_System_state_Is_up(_System_state_Get()) && (_ISR_Nest_level == 0)) {
      if ( rtems_stack_checker_is_blown() ) {
	printk( "Stack blown!!\n" );
	rtems_fatal_error_occurred( 99 );
      }
    }
  #endif

  _Thread_Dispatch_increment_disable_level();
  RTEMS_COMPILER_MEMORY_BARRIER();

  /*
   * This check is even more brutal than the other one.  This enables
   * malloc heap integrity checking upon entry to every system call.
   */
  #if defined(RTEMS_HEAVY_MALLOC_DEBUG)
    if ( _Thread_Dispatch_get_disable_level() == 1 ) {
      _Heap_Walk( RTEMS_Malloc_Heap,99, false );
    }
  #endif
}
#endif
