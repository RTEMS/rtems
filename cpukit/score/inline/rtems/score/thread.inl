/**
 * @file
 *
 * @brief Inlined Routines from the Thread Handler
 *
 * This file contains the macro implementation of the inlined
 * routines from the Thread handler.
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
 * @addtogroup ScoreThread
 */
/**@{**/


/**
 * This routine halts multitasking and returns control to
 * the "thread" (i.e. the BSP) which initially invoked the
 * routine which initialized the system.
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
 * This function returns true if the_thread is the currently executing
 * thread, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_executing (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Executing );
}

/**
 * This function returns true if the_thread is the heir
 * thread, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_heir (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Heir );
}

/**
 * This function returns true if the currently executing thread
 * is also the heir thread, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_executing_also_the_heir( void )
{
  return ( _Thread_Executing == _Thread_Heir );
}

/**
 * This routine clears any blocking state for the_thread.  It performs
 * any necessary scheduling operations including the selection of
 * a new heir thread.
 */

RTEMS_INLINE_ROUTINE void _Thread_Unblock (
  Thread_Control *the_thread
)
{
  _Thread_Clear_state( the_thread, STATES_BLOCKED );
}

/**
 * This routine resets the current context of the calling thread
 * to that of its initial state.
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
 * This function returns true if the floating point context of
 * the_thread is currently loaded in the floating point unit, and
 * false otherwise.
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
 * This routine is invoked when the currently loaded floating
 * point context is now longer associated with an active thread.
 */

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
RTEMS_INLINE_ROUTINE void _Thread_Deallocate_fp( void )
{
  _Thread_Allocated_fp = NULL;
}
#endif

/**
 * This function returns true if dispatching is disabled, and false
 * otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_context_switch_necessary( void )
{
  return ( _Thread_Dispatch_necessary );
}

/**
 * This function returns true if the_thread is NULL and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_null (
  const Thread_Control *the_thread
)
{
  return ( the_thread == NULL );
}

/**
 * @brief Is proxy blocking.
 *
 * status which indicates that a proxy is blocking, and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_proxy_blocking (
  uint32_t   code
)
{
  return (code == THREAD_STATUS_PROXY_BLOCKING);
}

/**
 * This routine allocates an internal thread.
 */

RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Internal_allocate( void )
{
  return (Thread_Control *) _Objects_Allocate( &_Thread_Internal_information );
}

/**
 * This routine frees an internal thread.
 */

RTEMS_INLINE_ROUTINE void _Thread_Internal_free (
  Thread_Control *the_task
)
{
  _Objects_Free( &_Thread_Internal_information, &the_task->Object );
}

RTEMS_INLINE_ROUTINE void _Thread_Set_global_exit_status(
  uint32_t exit_status
)
{
  Thread_Control *idle = (Thread_Control *)
    _Thread_Internal_information.local_table[ 1 ];

  idle->Wait.return_code = exit_status;
}

RTEMS_INLINE_ROUTINE uint32_t _Thread_Get_global_exit_status( void )
{
  const Thread_Control *idle = (const Thread_Control *)
    _Thread_Internal_information.local_table[ 1 ];

  return idle->Wait.return_code;
}

/**
 * @brief Issues a thread dispatch if necessary.
 *
 * @param[in] executing The executing thread.
 * @param[in] needs_asr_dispatching Indicates whether or not the API
 *            level signals are pending and a dispatch is necessary.
 */
RTEMS_INLINE_ROUTINE void _Thread_Dispatch_if_necessary(
  Thread_Control *executing,
  bool            needs_asr_dispatching
)
{
  if ( _Thread_Dispatch_is_enabled() ) {
    bool dispatch_necessary = needs_asr_dispatching;

    if ( !_Thread_Is_heir( executing ) && executing->is_preemptible ) {
      dispatch_necessary = true;
      _Thread_Dispatch_necessary = dispatch_necessary;
    }

    if ( dispatch_necessary ) {
      _Thread_Dispatch();
    }
  }
}

/**
 * This routine returns the C library re-enterant pointer.
 */

RTEMS_INLINE_ROUTINE struct _reent **_Thread_Get_libc_reent( void )
{
  return _Thread_libc_reent;
}

/**
 * This routine set the C library re-enterant pointer.
 */

RTEMS_INLINE_ROUTINE void _Thread_Set_libc_reent (
  struct _reent **libc_reent
)
{
  _Thread_libc_reent = libc_reent;
}

/** @}*/

#endif
/* end of include file */
