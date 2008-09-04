/** 
 *  @file  rtems/score/thread.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the Thread handler.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_THREAD_H
# error "Never use <rtems/score/thread.inl> directly; include <rtems/score/thread.h> instead."
#endif

#ifndef _RTEMS_SCORE_THREAD_INL
#define _RTEMS_SCORE_THREAD_INL

#include <rtems/score/sysstate.h>

/**
 *  @addtogroup ScoreThread 
 *  @{
 */

/**
 *  This routine halts multitasking and returns control to
 *  the "thread" (i.e. the BSP) which initially invoked the
 *  routine which initialized the system.
 */

RTEMS_INLINE_ROUTINE void _Thread_Stop_multitasking( void )
{
  Context_Control context_area;
  Context_Control *context_p = &context_area;

  if ( _System_state_Is_up(_System_state_Get ()) )
    context_p = &_Thread_Executing->Registers;

  _Context_Switch( context_p, &_Thread_BSP_context );
}

/**
 *  This function returns TRUE if the_thread is the currently executing
 *  thread, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_executing (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Executing );
}

/**
 *  This function returns TRUE if the_thread is the heir
 *  thread, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_heir (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Heir );
}

/**
 *  This function returns TRUE if the currently executing thread
 *  is also the heir thread, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_executing_also_the_heir( void )
{
  return ( _Thread_Executing == _Thread_Heir );
}

/**
 *  This routine clears any blocking state for the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 */

RTEMS_INLINE_ROUTINE void _Thread_Unblock (
  Thread_Control *the_thread
)
{
  _Thread_Clear_state( the_thread, STATES_BLOCKED );
}

/**
 *  This routine resets the current context of the calling thread
 *  to that of its initial state.
 */

RTEMS_INLINE_ROUTINE void _Thread_Restart_self( void )
{
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  if ( _Thread_Executing->fp_context != NULL )
    _Context_Restore_fp( &_Thread_Executing->fp_context );
#endif

  _CPU_Context_Restart_self( &_Thread_Executing->Registers );
}

/**
 *  This function returns a pointer to the highest priority
 *  ready thread.
 */

RTEMS_INLINE_ROUTINE void _Thread_Calculate_heir( void )
{
  _Thread_Heir = (Thread_Control *)
    _Thread_Ready_chain[ _Priority_Get_highest() ].first;
}

/**
 *  This function returns TRUE if the floating point context of
 *  the_thread is currently loaded in the floating point unit, and
 *  FALSE otherwise.
 */

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
RTEMS_INLINE_ROUTINE bool _Thread_Is_allocated_fp (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Allocated_fp );
}
#endif

/**
 *  This routine is invoked when the currently loaded floating
 *  point context is now longer associated with an active thread.
 */

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
RTEMS_INLINE_ROUTINE void _Thread_Deallocate_fp( void )
{
  _Thread_Allocated_fp = NULL;
}
#endif

/**
 *  This routine prevents dispatching.
 */

#if defined(RTEMS_HEAVY_STACK_DEBUG) || defined(RTEMS_HEAVY_MALLOC_DEBUG)
  #include <rtems/bspIo.h>
  #include <rtems/fatal.h>
  #include <rtems/stackchk.h>
  #include <rtems/score/sysstate.h>
  #include <rtems/score/heap.h>

  /*
   * This is currently not defined in any .h file, so we have to
   * extern it here.
   */
  extern Heap_Control  RTEMS_Malloc_Heap;
#endif

RTEMS_INLINE_ROUTINE void _Thread_Disable_dispatch( void )
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

  _Thread_Dispatch_disable_level += 1;
  RTEMS_COMPILER_MEMORY_BARRIER();

  /*
   * This check is even more brutal than the other one.  This enables
   * malloc heap integrity checking upon entry to every system call.
   */
  #if defined(RTEMS_HEAVY_MALLOC_DEBUG)
    if ( _Thread_Dispatch_disable_level == 1 ) {
      _Heap_Walk( &RTEMS_Malloc_Heap,99, FALSE );
    }
  #endif
}

/**
 *  This routine allows dispatching to occur again.  If this is
 *  the outer most dispatching critical section, then a dispatching
 *  operation will be performed and, if necessary, control of the
 *  processor will be transferred to the heir thread.
 */

#if ( (CPU_INLINE_ENABLE_DISPATCH == FALSE) || \
      (__RTEMS_DO_NOT_INLINE_THREAD_ENABLE_DISPATCH__ == 1) )
void _Thread_Enable_dispatch( void );
#else
/* inlining of enable dispatching must be true */
RTEMS_INLINE_ROUTINE void _Thread_Enable_dispatch( void )
{
  RTEMS_COMPILER_MEMORY_BARRIER();
  if ( (--_Thread_Dispatch_disable_level) == 0 )
    _Thread_Dispatch();
}
#endif


/**
 *  This routine allows dispatching to occur again.  However,
 *  no dispatching operation is performed even if this is the outer
 *  most dispatching critical section.
 */

RTEMS_INLINE_ROUTINE void _Thread_Unnest_dispatch( void )
{
  RTEMS_COMPILER_MEMORY_BARRIER();
  _Thread_Dispatch_disable_level -= 1;
}

/**
 *  This function returns TRUE if dispatching is disabled, and FALSE
 *  otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_dispatching_enabled( void )
{
  return ( _Thread_Dispatch_disable_level == 0 );
}

/**
 *  This function returns TRUE if dispatching is disabled, and FALSE
 *  otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_context_switch_necessary( void )
{
  return ( _Context_Switch_necessary );
}

/**
 *  This routine initializes the thread dispatching subsystem.
 */

RTEMS_INLINE_ROUTINE void _Thread_Dispatch_initialization( void )
{
  _Thread_Dispatch_disable_level = 1;
}

/**
 *  This function returns TRUE if the_thread is NULL and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_null (
  const Thread_Control *the_thread
)
{
  return ( the_thread == NULL );
}

/** @brief _Thread_Is_proxy_blocking
 *
 *  status which indicates that a proxy is blocking, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_proxy_blocking (
  uint32_t   code
)
{
  return (code == THREAD_STATUS_PROXY_BLOCKING);
}

/**
 *  This routine allocates an internal thread.
 */
 
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Internal_allocate( void )
{
  return (Thread_Control *) _Objects_Allocate( &_Thread_Internal_information );
}
 
/**
 *  This routine frees an internal thread.
 */
 
RTEMS_INLINE_ROUTINE void _Thread_Internal_free (
  Thread_Control *the_task
)
{
  _Objects_Free( &_Thread_Internal_information, &the_task->Object );
}

/**
 *  This routine returns the C library re-enterant pointer.
 */
 
RTEMS_INLINE_ROUTINE struct _reent **_Thread_Get_libc_reent( void )
{
  return _Thread_libc_reent;
}

/**
 *  This routine set the C library re-enterant pointer.
 */
 
RTEMS_INLINE_ROUTINE void _Thread_Set_libc_reent (
  struct _reent **libc_reent
)
{
  _Thread_libc_reent = libc_reent;
}

/**@}*/

#endif
/* end of include file */
