/** 
 *  @file  rtems/score/thread.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the Thread handler.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREAD_H
# error "Never use <rtems/score/thread.inl> directly; include <rtems/score/thread.h> instead."
#endif

#ifndef _RTEMS_SCORE_THREAD_INL
#define _RTEMS_SCORE_THREAD_INL

#include <rtems/score/sysstate.h>
#include <rtems/score/context.h>

/**
 *  @addtogroup ScoreThread 
 *  @{
 */

#if defined(RTEMS_SMP)

  /*
   * The _Thread_Dispatch_... functions are prototyped in thread.h.
   */

#else

  /** @brief _Thread_Dispatch_in_critical_section
   * 
   * This routine returns true if thread dispatch indicates
   * that we are in a critical section.
   */
  RTEMS_INLINE_ROUTINE bool _Thread_Dispatch_in_critical_section(void)
  {
     if (  _Thread_Dispatch_disable_level == 0 )
      return false;

     return true;
  }

  /** @brief _Thread_Dispatch_get_disable_level
   * 
   * This routine returns value of the the thread dispatch level.
   */
  RTEMS_INLINE_ROUTINE uint32_t _Thread_Dispatch_get_disable_level(void)
  {
    return _Thread_Dispatch_disable_level;
  }

  /** @brief _Thread_Dispatch_set_disable_level
   * 
   * This routine sets thread dispatch level to the 
   * value passed in.
   */
  RTEMS_INLINE_ROUTINE uint32_t _Thread_Dispatch_set_disable_level(uint32_t value)
  {
    _Thread_Dispatch_disable_level = value;
    return value;
  }

  /** @brief _Thread_Dispatch_increment_disable_level
   * 
   * This rountine increments the thread dispatch level
   */
  RTEMS_INLINE_ROUTINE uint32_t _Thread_Dispatch_increment_disable_level(void)
  {
    _Thread_Dispatch_disable_level++;
    return _Thread_Dispatch_disable_level;
  }

  /** @brief _Thread_Dispatch_decrement_disable_level
   * 
   * This routine decrements the thread dispatch level.
   */
  RTEMS_INLINE_ROUTINE uint32_t _Thread_Dispatch_decrement_disable_level(void)
  {
    _Thread_Dispatch_disable_level--;
    return _Thread_Dispatch_disable_level;
  }

  /** @brief _Thread_Dispatch_initialization
   * 
   *  This routine initializes the thread dispatching subsystem.
   */
  RTEMS_INLINE_ROUTINE void _Thread_Dispatch_initialization( void )
  {
    _Thread_Dispatch_set_disable_level( 1 );
  }

#endif

/**
 *  This routine halts multitasking and returns control to
 *  the "thread" (i.e. the BSP) which initially invoked the
 *  routine which initialized the system.
 */

RTEMS_INLINE_ROUTINE void _Thread_Stop_multitasking( void )
{
#if defined(_CPU_Stop_multitasking)
  _CPU_Stop_multitasking( &_Thread_BSP_context );
#else
  /*
   *  This may look a bit of an odd but _Context_Restart_self is just
   *  a very careful restore of a specific context which ensures that
   *  if we were running within the same context, it would work.
   *
   *  And we will not return to this thread, so there is no point of
   *  saving the context.
   */
  _Context_Restart_self( &_Thread_BSP_context );
#endif

  /***************************************************************
   ***************************************************************
   *   SYSTEM SHUTS DOWN!!!  WE DO NOT RETURN TO THIS POINT!!!   *
   ***************************************************************
   ***************************************************************
   */
}

/**
 *  This function returns true if the_thread is the currently executing
 *  thread, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_executing (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Executing );
}

/**
 *  This function returns true if the_thread is the heir
 *  thread, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_heir (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Heir );
}

/**
 *  This function returns true if the currently executing thread
 *  is also the heir thread, and false otherwise.
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
 *  This function returns true if the floating point context of
 *  the_thread is currently loaded in the floating point unit, and
 *  false otherwise.
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

#if defined ( __THREAD_DO_NOT_INLINE_DISABLE_DISPATCH__ )
void _Thread_Disable_dispatch( void );
#else
RTEMS_INLINE_ROUTINE void _Thread_Disable_dispatch( void )
{
  _Thread_Dispatch_increment_disable_level();
  RTEMS_COMPILER_MEMORY_BARRIER();
}
#endif

/**
 *  This routine allows dispatching to occur again.  If this is
 *  the outer most dispatching critical section, then a dispatching
 *  operation will be performed and, if necessary, control of the
 *  processor will be transferred to the heir thread.
 */

#if defined ( __THREAD_DO_NOT_INLINE_ENABLE_DISPATCH__ )
  void _Thread_Enable_dispatch( void );
#else
  /* inlining of enable dispatching must be true */
  RTEMS_INLINE_ROUTINE void _Thread_Enable_dispatch( void )
  {
    RTEMS_COMPILER_MEMORY_BARRIER();
    if ( _Thread_Dispatch_decrement_disable_level() == 0 )
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
  _Thread_Dispatch_decrement_disable_level();
}

/**
 *  This function returns true if dispatching is disabled, and false
 *  otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_dispatching_enabled( void )
{
  return  ( _Thread_Dispatch_in_critical_section() == false );
}

/**
 *  This function returns true if dispatching is disabled, and false
 *  otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_context_switch_necessary( void )
{
  return ( _Thread_Dispatch_necessary );
}

/**
 *  This function returns true if the_thread is NULL and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_null (
  const Thread_Control *the_thread
)
{
  return ( the_thread == NULL );
}

/** @brief _Thread_Is_proxy_blocking
 *
 *  status which indicates that a proxy is blocking, and false otherwise.
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

/**
 *  This routine evaluates the current scheduling information for the
 *  system and determines if a context switch is required.  This
 *  is usually called after changing an execution mode such as preemptability
 *  for a thread.
 *
 *  @param[in] are_signals_pending specifies whether or not the API
 *             level signals are pending and a dispatch is needed.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Evaluate_is_dispatch_needed(
  bool are_signals_pending
)
{
  Thread_Control     *executing;

  executing = _Thread_Executing;

  if ( are_signals_pending ||
       (!_Thread_Is_heir( executing ) && executing->is_preemptible) ) {
    _Thread_Dispatch_necessary = true;
    return true;
  }

  return false;
}

/**@}*/

#endif
/* end of include file */
