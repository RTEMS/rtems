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

#ifndef _RTEMS_SCORE_THREAD_INL
#define _RTEMS_SCORE_THREAD_INL

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
  _Context_Switch( &_Thread_Executing->Registers, &_Thread_BSP_context );
}

/**
 *  This function returns TRUE if the_thread is the currently executing
 *  thread, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Thread_Is_executing (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Executing );
}

/**
 *  This function returns TRUE if the_thread is the heir
 *  thread, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Thread_Is_heir (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Heir );
}

/**
 *  This function returns TRUE if the currently executing thread
 *  is also the heir thread, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Thread_Is_executing_also_the_heir( void )
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
RTEMS_INLINE_ROUTINE boolean _Thread_Is_allocated_fp (
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
  #include <rtems/score/sysstate.h>
  #include <rtems/score/heap.h>
#endif

RTEMS_INLINE_ROUTINE void _Thread_Disable_dispatch( void )
{
  extern boolean rtems_stack_checker_is_blown( void );
  extern void malloc_walk(size_t, size_t);

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
      extern Heap_Control  RTEMS_Malloc_Heap;
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

#if ( CPU_INLINE_ENABLE_DISPATCH == TRUE )
RTEMS_INLINE_ROUTINE void _Thread_Enable_dispatch()
{
  RTEMS_COMPILER_MEMORY_BARRIER();
  if ( (--_Thread_Dispatch_disable_level) == 0 )
    _Thread_Dispatch();
}
#endif

#if ( CPU_INLINE_ENABLE_DISPATCH == FALSE )
void _Thread_Enable_dispatch( void );
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

RTEMS_INLINE_ROUTINE boolean _Thread_Is_dispatching_enabled( void )
{
  return ( _Thread_Dispatch_disable_level == 0 );
}

/**
 *  This function returns TRUE if dispatching is disabled, and FALSE
 *  otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Thread_Is_context_switch_necessary( void )
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

RTEMS_INLINE_ROUTINE boolean _Thread_Is_null (
  const Thread_Control *the_thread
)
{
  return ( the_thread == NULL );
}

/**
 *  This function maps thread IDs to thread control
 *  blocks.  If ID corresponds to a local thread, then it
 *  returns the_thread control pointer which maps to ID
 *  and location is set to OBJECTS_LOCAL.  If the thread ID is
 *  global and resides on a remote node, then location is set
 *  to OBJECTS_REMOTE, and the_thread is undefined.
 *  Otherwise, location is set to OBJECTS_ERROR and
 *  the_thread is undefined.
 *
 *  @note  The performance of many RTEMS services depends upon
 *         the quick execution of the "good object" path in this
 *         routine.  If there is a possibility of saving a few
 *         cycles off the execution time, this routine is worth
 *         further optimization attention.
 */

RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  uint32_t             the_api;
  uint32_t             the_class;
  Objects_Information *information;
  Thread_Control      *tp = (Thread_Control *) 0;
 
  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ) {
    _Thread_Disable_dispatch();
    *location = OBJECTS_LOCAL;
    tp = _Thread_Executing;
    goto done;
  }
 
  the_api = _Objects_Get_API( id );
  if ( the_api && the_api > OBJECTS_APIS_LAST ) {
    *location = OBJECTS_ERROR;
    goto done;
  }
  
  the_class = _Objects_Get_class( id );
  if ( the_class != 1 ) {       /* threads are always first class :) */
    *location = OBJECTS_ERROR;
    goto done;
  }
 
  information = _Objects_Information_table[ the_api ][ the_class ];
 
  if ( !information ) {
    *location = OBJECTS_ERROR;
    goto done;
  }
 
  tp = (Thread_Control *) _Objects_Get( information, id, location );
 
done:
  return tp;
}


/** @brief _Thread_Is_proxy_blocking
 *
 *  status which indicates that a proxy is blocking, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Thread_Is_proxy_blocking (
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
