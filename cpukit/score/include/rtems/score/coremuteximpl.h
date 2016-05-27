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

#define CORE_MUTEX_TQ_OPERATIONS &_Thread_queue_Operations_priority

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

RTEMS_INLINE_ROUTINE Thread_Control *_CORE_mutex_Get_owner(
  const CORE_mutex_Control *the_mutex
)
{
  return the_mutex->holder;
}

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
  return _CORE_mutex_Get_owner( the_mutex ) != NULL;
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
    ++executing->resource_count;

    _CORE_mutex_Release( the_mutex, queue_context );
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

Status_Control _CORE_mutex_Seize_no_protocol_slow(
  CORE_mutex_Control            *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  bool                           wait,
  Watchdog_Interval              timeout,
  Thread_queue_Context          *queue_context
);

Status_Control _CORE_mutex_Surrender(
  CORE_mutex_Control   *the_mutex,
  Thread_queue_Context *queue_context
);

RTEMS_INLINE_ROUTINE void _CORE_mutex_Set_owner(
  CORE_mutex_Control *the_mutex,
  Thread_Control     *owner
)
{
  the_mutex->holder = owner;
}

RTEMS_INLINE_ROUTINE bool _CORE_mutex_Is_owner(
  const CORE_mutex_Control *the_mutex,
  const Thread_Control     *the_thread
)
{
  return _CORE_mutex_Get_owner( the_mutex ) == the_thread;
}

RTEMS_INLINE_ROUTINE void _CORE_mutex_Restore_priority(
  Thread_Control *executing
)
{
  /*
   *  Whether or not someone is waiting for the mutex, an
   *  inherited priority must be lowered if this is the last
   *  mutex (i.e. resource) this task has.
   */
  if ( !_Thread_Owns_resources( executing ) ) {
    /*
     * Ensure that the executing resource count is visible to all other
     * processors and that we read the latest priority restore hint.
     */
    _Atomic_Fence( ATOMIC_ORDER_ACQ_REL );

    if ( executing->priority_restore_hint ) {
      Per_CPU_Control *cpu_self;

      cpu_self = _Thread_Dispatch_disable();
      _Thread_Restore_priority( executing );
      _Thread_Dispatch_enable( cpu_self );
    }
  }
}

RTEMS_INLINE_ROUTINE void _CORE_recursive_mutex_Initialize(
  CORE_recursive_mutex_Control *the_mutex
)
{
  _Thread_queue_Initialize( &the_mutex->Mutex.Wait_queue );
  the_mutex->Mutex.holder = NULL;
  the_mutex->nest_level = 0;
}

RTEMS_INLINE_ROUTINE Status_Control _CORE_recursive_mutex_Seize_nested(
  CORE_recursive_mutex_Control *the_mutex
)
{
  ++the_mutex->nest_level;
  return STATUS_SUCCESSFUL;
}

RTEMS_INLINE_ROUTINE Status_Control _CORE_recursive_mutex_Seize_no_protocol(
  CORE_recursive_mutex_Control  *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  bool                           wait,
  Watchdog_Interval              timeout,
  Status_Control              ( *nested )( CORE_recursive_mutex_Control * ),
  Thread_queue_Context          *queue_context
)
{
  Thread_Control *owner;

  _CORE_mutex_Acquire_critical( &the_mutex->Mutex, queue_context );

  owner = _CORE_mutex_Get_owner( &the_mutex->Mutex );

  if ( owner == NULL ) {
    _CORE_mutex_Set_owner( &the_mutex->Mutex, executing );
    _CORE_mutex_Release( &the_mutex->Mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  if ( owner == executing ) {
    Status_Control status;

    status = ( *nested )( the_mutex );
    _CORE_mutex_Release( &the_mutex->Mutex, queue_context );
    return status;
  }

  return _CORE_mutex_Seize_no_protocol_slow(
    &the_mutex->Mutex,
    operations,
    executing,
    wait,
    timeout,
    queue_context
  );
}

RTEMS_INLINE_ROUTINE void
_CORE_recursive_mutex_Surrender_no_protocol_finalize(
  CORE_recursive_mutex_Control  *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_queue_Context          *queue_context
)
{
  unsigned int    nest_level;
  Thread_Control *new_owner;

  nest_level = the_mutex->nest_level;

  if ( nest_level > 0 ) {
    the_mutex->nest_level = nest_level - 1;
    _CORE_mutex_Release( &the_mutex->Mutex, queue_context );
    return;
  }

  new_owner = _Thread_queue_First_locked(
    &the_mutex->Mutex.Wait_queue,
    operations
  );
  _CORE_mutex_Set_owner( &the_mutex->Mutex, new_owner );

  if ( new_owner == NULL ) {
    _CORE_mutex_Release( &the_mutex->Mutex, queue_context );
    return;
  }

  _Thread_queue_Extract_critical(
    &the_mutex->Mutex.Wait_queue.Queue,
    operations,
    new_owner,
    queue_context
  );
}

RTEMS_INLINE_ROUTINE Status_Control _CORE_recursive_mutex_Surrender_no_protocol(
  CORE_recursive_mutex_Control  *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  Thread_queue_Context          *queue_context
)
{
  _CORE_mutex_Acquire_critical( &the_mutex->Mutex, queue_context );

  if ( !_CORE_mutex_Is_owner( &the_mutex->Mutex, executing ) ) {
    _CORE_mutex_Release( &the_mutex->Mutex, queue_context );
    return STATUS_NOT_OWNER;
  }

  _CORE_recursive_mutex_Surrender_no_protocol_finalize(
    the_mutex,
    operations,
    queue_context
  );
  return STATUS_SUCCESSFUL;
}

/*
 * The Classic no protocol recursive mutex has the nice property that everyone
 * can release it.
 */
RTEMS_INLINE_ROUTINE void _CORE_recursive_mutex_Surrender_no_protocol_classic(
  CORE_recursive_mutex_Control  *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_queue_Context          *queue_context
)
{
  _CORE_mutex_Acquire_critical( &the_mutex->Mutex, queue_context );
  _CORE_recursive_mutex_Surrender_no_protocol_finalize(
    the_mutex,
    operations,
    queue_context
  );
}

RTEMS_INLINE_ROUTINE void _CORE_ceiling_mutex_Initialize(
  CORE_ceiling_mutex_Control *the_mutex,
  Priority_Control            priority_ceiling
)
{
  _CORE_recursive_mutex_Initialize( &the_mutex->Recursive );
  the_mutex->priority_ceiling = priority_ceiling;
}

RTEMS_INLINE_ROUTINE Status_Control _CORE_ceiling_mutex_Set_owner(
  CORE_ceiling_mutex_Control *the_mutex,
  Thread_Control             *owner,
  Thread_queue_Context       *queue_context
)
{
  Priority_Control  priority_ceiling;
  Priority_Control  current_priority;
  Per_CPU_Control  *cpu_self;

  priority_ceiling = the_mutex->priority_ceiling;
  current_priority = owner->current_priority;

  if ( current_priority < priority_ceiling ) {
    _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
    return STATUS_MUTEX_CEILING_VIOLATED;
  }

  _CORE_mutex_Set_owner( &the_mutex->Recursive.Mutex, owner );
  ++owner->resource_count;

  if ( current_priority == priority_ceiling ) {
    _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  cpu_self = _Thread_Dispatch_disable_critical( &queue_context->Lock_context );
  _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
  _Thread_Raise_priority( owner, priority_ceiling );
  _Thread_Dispatch_enable( cpu_self );
  return STATUS_SUCCESSFUL;
}

RTEMS_INLINE_ROUTINE Status_Control _CORE_ceiling_mutex_Seize(
  CORE_ceiling_mutex_Control    *the_mutex,
  Thread_Control                *executing,
  bool                           wait,
  Watchdog_Interval              timeout,
  Status_Control              ( *nested )( CORE_recursive_mutex_Control * ),
  Thread_queue_Context          *queue_context
)
{
  Thread_Control *owner;

  _CORE_mutex_Acquire_critical( &the_mutex->Recursive.Mutex, queue_context );

  owner = _CORE_mutex_Get_owner( &the_mutex->Recursive.Mutex );

  if ( owner == NULL ) {
    return _CORE_ceiling_mutex_Set_owner(
      the_mutex,
      executing,
      queue_context
    );
  }

  if ( owner == executing ) {
    Status_Control status;

    status = ( *nested )( &the_mutex->Recursive );
    _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
    return status;
  }

  return _CORE_mutex_Seize_no_protocol_slow(
    &the_mutex->Recursive.Mutex,
    CORE_MUTEX_TQ_OPERATIONS,
    executing,
    wait,
    timeout,
    queue_context
  );
}

RTEMS_INLINE_ROUTINE Status_Control _CORE_ceiling_mutex_Surrender(
  CORE_ceiling_mutex_Control *the_mutex,
  Thread_Control             *executing,
  Thread_queue_Context       *queue_context
)
{
  unsigned int    nest_level;
  Thread_Control *new_owner;

  _CORE_mutex_Acquire_critical( &the_mutex->Recursive.Mutex, queue_context );

  if ( !_CORE_mutex_Is_owner( &the_mutex->Recursive.Mutex, executing ) ) {
    _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
    return STATUS_NOT_OWNER;
  }

  nest_level = the_mutex->Recursive.nest_level;

  if ( nest_level > 0 ) {
    the_mutex->Recursive.nest_level = nest_level - 1;
    _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  --executing->resource_count;

  new_owner = _Thread_queue_First_locked(
    &the_mutex->Recursive.Mutex.Wait_queue,
    CORE_MUTEX_TQ_OPERATIONS
  );
  _CORE_mutex_Set_owner( &the_mutex->Recursive.Mutex, new_owner );

  if ( new_owner != NULL ) {
    bool unblock;

    /*
     * We must extract the thread now since this will restore its default
     * thread lock.  This is necessary to avoid a deadlock in the
     * _Thread_Change_priority() below due to a recursive thread queue lock
     * acquire.
     */
    unblock = _Thread_queue_Extract_locked(
      &the_mutex->Recursive.Mutex.Wait_queue.Queue,
      CORE_MUTEX_TQ_OPERATIONS,
      new_owner,
      queue_context
    );

#if defined(RTEMS_MULTIPROCESSING)
    if ( _Objects_Is_local_id( new_owner->Object.id ) )
#endif
    {
      ++new_owner->resource_count;
      _Thread_Raise_priority( new_owner, the_mutex->priority_ceiling );
    }

    _Thread_queue_Unblock_critical(
      unblock,
      &the_mutex->Recursive.Mutex.Wait_queue.Queue,
      new_owner,
      &queue_context->Lock_context
    );
  } else {
    _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
  }

  _CORE_mutex_Restore_priority( executing );
  return STATUS_SUCCESSFUL;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
