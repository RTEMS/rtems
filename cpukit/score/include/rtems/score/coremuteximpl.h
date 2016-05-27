/**
 * @file
 *
 * @ingroup ScoreMutex
 *
 * @brief CORE Mutex Implementation
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREMUTEXIMPL_H
#define _RTEMS_SCORE_COREMUTEXIMPL_H

#include <rtems/score/coremutex.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/status.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreMutex
 */
/**@{**/

/**
 *  @brief Initializes the mutex based on the parameters passed.
 *
 *  This routine initializes the mutex based on the parameters passed.
 *
 *  @param[in,out] the_mutex is the mutex to initalize
 *  @param[in,out] executing The currently executing thread.
 *  @param[in] the_mutex_attributes is the attributes associated with this
 *         mutex instance
 *  @param[in] initially_locked If true, then the mutex is initially locked by
 *  the executing thread.
 *
 *  @retval This method returns STATUS_SUCCESSFUL if successful.
 */
Status_Control _CORE_mutex_Initialize(
  CORE_mutex_Control           *the_mutex,
  Thread_Control               *executing,
  const CORE_mutex_Attributes  *the_mutex_attributes,
  bool                          initially_locked
);

RTEMS_INLINE_ROUTINE void _CORE_mutex_Destroy( CORE_mutex_Control *the_mutex )
{
  _Thread_queue_Destroy( &the_mutex->Wait_queue );
}

RTEMS_INLINE_ROUTINE void _CORE_mutex_Acquire_critical(
  CORE_mutex_Control   *the_mutex,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Acquire_critical(
    &the_mutex->Wait_queue,
    &queue_context->Lock_context
  );
}

RTEMS_INLINE_ROUTINE void _CORE_mutex_Release(
  CORE_mutex_Control   *the_mutex,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Release(
    &the_mutex->Wait_queue,
    &queue_context->Lock_context
  );
}

/**
 *  @brief Performs the blocking portion of a mutex obtain.
 *
 *  This routine performs the blocking portion of a mutex obtain.
 *  It is an actual subroutine and is not implemented as something
 *  that may be inlined.
 *
 *  @param[in,out] the_mutex is the mutex to attempt to lock
 *  @param[in,out] executing The currently executing thread.
 *  @param[in] timeout is the maximum number of ticks to block
 *  @param[in] lock_context is the interrupt level
 */
Status_Control _CORE_mutex_Seize_interrupt_blocking(
  CORE_mutex_Control   *the_mutex,
  Thread_Control       *executing,
  Watchdog_Interval     timeout,
  Thread_queue_Context *queue_context
);

/**
 * @brief Is mutex locked.
 *
 * This routine returns true if the mutex specified is locked and false
 * otherwise.
 *
 * @param[in] the_mutex is the mutex to check.
 *
 * @retval true The mutex is locked.
 * @retval false The mutex is not locked.
 */
RTEMS_INLINE_ROUTINE bool _CORE_mutex_Is_locked(
  const CORE_mutex_Control *the_mutex
)
{
  return the_mutex->holder != NULL;
}

/**
 * @brief Does mutex use priority inheritance.
 *
 * This routine returns true if the mutex's wait discipline is
 * INHERIT_PRIORITY and false otherwise.
 *
 * @param[in] the_attribute is the attribute set of the mutex.
 *
 * @retval true The mutex is using priority inheritance.
 * @retval false The mutex is not using priority inheritance.
 */
RTEMS_INLINE_ROUTINE bool _CORE_mutex_Is_inherit_priority(
  const CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT;
}

/**
 * @brief Does mutex use priority ceiling.
 *
 * This routine returns true if the mutex's wait discipline is
 * PRIORITY_CEILING and false otherwise.
 *
 * @param[in] the_attribute is the attribute set of the mutex.
 *
 * @retval true The mutex is using priority ceiling.
 * @retval false The mutex is not using priority ceiling.
 */
RTEMS_INLINE_ROUTINE bool _CORE_mutex_Is_priority_ceiling(
  const CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING;
}

/**
 *  @brief Attempt to receive a unit from the_mutex.
 *
 *  This routine attempts to receive a unit from the_mutex.
 *  If a unit is available or if the wait flag is false, then the routine
 *  returns.  Otherwise, the calling task is blocked until a unit becomes
 *  available.
 *
 *  @param[in,out] executing The currently executing thread.
 *  @param[in,out] the_mutex is the mutex to attempt to lock
 *  @param[in] queue_context is the interrupt level
 *
 *  @retval STATUS_UNAVAILABLE The mutex is already locked.
 *  @retval other Otherwise.
 */
RTEMS_INLINE_ROUTINE Status_Control _CORE_mutex_Seize_interrupt_trylock(
  CORE_mutex_Control   *the_mutex,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
)
{
  /* disabled when you get here */

  if ( !_CORE_mutex_Is_locked( the_mutex ) ) {
    the_mutex->holder     = executing;
    the_mutex->nest_count = 1;
    if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ||
         _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ){
      executing->resource_count++;
    }

    if ( !_CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ) {
      _CORE_mutex_Release( the_mutex, queue_context );
    } else {
      /*
       * must be CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING
       *
       * we possibly bump the priority of the current holder -- which
       * happens to be _Thread_Executing.
       */
      Priority_Control  ceiling;
      Priority_Control  current;

      ceiling = the_mutex->Attributes.priority_ceiling;
      current = executing->current_priority;
      if ( current == ceiling ) {
        _CORE_mutex_Release( the_mutex, queue_context );
      } else if ( current > ceiling ) {
        Per_CPU_Control *cpu_self;

        cpu_self = _Thread_Dispatch_disable_critical(
          &queue_context->Lock_context
        );
        _CORE_mutex_Release( the_mutex, queue_context );
        _Thread_Raise_priority( executing, ceiling );
        _Thread_Dispatch_enable( cpu_self );
      } else /* if ( current < ceiling ) */ {
        the_mutex->holder = NULL;
        the_mutex->nest_count = 0;     /* undo locking above */
        executing->resource_count--;   /* undo locking above */
        _CORE_mutex_Release( the_mutex, queue_context );
        return STATUS_MUTEX_CEILING_VIOLATED;
      }
    }

    return STATUS_SUCCESSFUL;
  }

  /*
   *  At this point, we know the mutex was not available.  If this thread
   *  is the thread that has locked the mutex, let's see if we are allowed
   *  to nest access.
   */
  if ( _Thread_Is_executing( the_mutex->holder ) ) {
    switch ( the_mutex->Attributes.lock_nesting_behavior ) {
      case CORE_MUTEX_NESTING_ACQUIRES:
        the_mutex->nest_count++;
        _CORE_mutex_Release( the_mutex, queue_context );
        return STATUS_SUCCESSFUL;
      #if defined(RTEMS_POSIX_API)
        case CORE_MUTEX_NESTING_IS_ERROR:
          _CORE_mutex_Release( the_mutex, queue_context );
          return STATUS_NESTING_NOT_ALLOWED;
      #endif
      case CORE_MUTEX_NESTING_BLOCKS:
        break;
    }
  }

  /*
   *  The mutex is not available and the caller must deal with the possibility
   *  of blocking.
   */
  return STATUS_UNAVAILABLE;
}

/**
 *  @brief Attempt to obtain the mutex.
 *
 *  This routine attempts to obtain the mutex.  If the mutex is available,
 *  then it will return immediately.  Otherwise, it will invoke the
 *  support routine @a _Core_mutex_Seize_interrupt_blocking.
 *
 *  @param[in] the_mutex is the mutex to attempt to lock
 *  @param[in] wait is true if the thread is willing to wait
 *  @param[in] timeout is the maximum number of ticks to block
 *  @param[in] queue_context is a temporary variable used to contain the ISR
 *         disable level cookie
 *
 *  @note If the mutex is called from an interrupt service routine,
 *        with context switching disabled, or before multitasking,
 *        then a fatal error is generated.
 *
 *  The logic on this routine is as follows:
 *
 *  * If incorrect system state
 *      return an error
 *  * If mutex is available without any contention or blocking
 *      obtain it with interrupts disabled and returned
 *  * If the caller is willing to wait
 *      then they are blocked.
 */
RTEMS_INLINE_ROUTINE Status_Control _CORE_mutex_Seize(
  CORE_mutex_Control   *the_mutex,
  Thread_Control       *executing,
  bool                  wait,
  Watchdog_Interval     timeout,
  Thread_queue_Context *queue_context
)
{
  Status_Control status;

  _CORE_mutex_Acquire_critical( the_mutex, queue_context );

  status = _CORE_mutex_Seize_interrupt_trylock(
    the_mutex,
    executing,
    queue_context
  );

  if ( status != STATUS_UNAVAILABLE ) {
    return status;
  }

  if ( !wait ) {
    _CORE_mutex_Release( the_mutex, queue_context );
    return status;
  }

  return _CORE_mutex_Seize_interrupt_blocking(
    the_mutex,
    executing,
    timeout,
    queue_context
  );
}

Status_Control _CORE_mutex_Surrender(
  CORE_mutex_Control   *the_mutex,
  Thread_queue_Context *queue_context
);

RTEMS_INLINE_ROUTINE void _CORE_mutex_Flush(
  CORE_mutex_Control        *the_mutex,
  Thread_queue_Flush_filter  filter,
  Thread_queue_Context      *queue_context
)
{
  _Thread_queue_Flush_critical(
    &the_mutex->Wait_queue.Queue,
    the_mutex->operations,
    filter,
    queue_context
  );
}

RTEMS_INLINE_ROUTINE bool _CORE_mutex_Is_owner(
  const CORE_mutex_Control *the_mutex,
  const Thread_Control     *the_thread
)
{
  return the_mutex->holder == the_thread;
}

/**
 * @brief Does core mutex use FIFO blocking.
 *
 * This routine returns true if the mutex's wait discipline is FIFO and false
 * otherwise.
 *
 * @param[in] the_attribute is the attribute set of the mutex.
 *
 * @retval true The mutex is using FIFO blocking order.
 * @retval false The mutex is not using FIFO blocking order.
 */
RTEMS_INLINE_ROUTINE bool _CORE_mutex_Is_fifo(
  const CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_FIFO;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
