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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREMUTEXIMPL_H
#define _RTEMS_SCORE_COREMUTEXIMPL_H

#include <rtems/score/coremutex.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/sysstate.h>
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
 *  @brief Callout which provides to support global/multiprocessor operations.
 *
 *  The following type defines the callout which the API provides
 *  to support global/multiprocessor operations on mutexes.
 */
typedef void ( *CORE_mutex_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

/**
 *  @brief The possible Mutex handler return statuses.
 *
 *  This enumerated type defines the possible Mutex handler return statuses.
 */
typedef enum {
  /** This status indicates that the operation completed successfully. */
  CORE_MUTEX_STATUS_SUCCESSFUL,
  /** This status indicates that the calling task did not want to block
   *  and the operation was unable to complete immediately because the
   *  resource was unavailable.
   */
  CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT,
#if defined(RTEMS_POSIX_API)
  /** This status indicates that an attempt was made to relock a mutex
   *  for which nesting is not configured.
   */
  CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED,
#endif
  /** This status indicates that an attempt was made to release a mutex
   *  by a thread other than the thread which locked it.
   */
  CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE,
  /** This status indicates that the thread was blocked waiting for an
   *  operation to complete and the mutex was deleted.
   */
  CORE_MUTEX_WAS_DELETED,
  /** This status indicates that the calling task was willing to block
   *  but the operation was unable to complete within the time allotted
   *  because the resource never became available.
   */
  CORE_MUTEX_TIMEOUT,

#if defined(__RTEMS_STRICT_ORDER_MUTEX__)
  /** This status indicates that a thread not release the mutex which has
   *  the priority inheritance property in a right order.
   */
  CORE_MUTEX_RELEASE_NOT_ORDER,
#endif

  /** This status indicates that a thread of logically greater importance
   *  than the ceiling priority attempted to lock this mutex.
   */
  CORE_MUTEX_STATUS_CEILING_VIOLATED

}   CORE_mutex_Status;

/**
 *  @brief The last status value.
 *
 *  This is the last status value.
 */
#define CORE_MUTEX_STATUS_LAST CORE_MUTEX_STATUS_CEILING_VIOLATED

/**
 *  This is the value of a mutex when it is unlocked.
 */
#define CORE_MUTEX_UNLOCKED 1

/**
 *  This is the value of a mutex when it is locked.
 */
#define CORE_MUTEX_LOCKED   0

/**
 *  @brief Initializes the mutex based on the parameters passed.
 *
 *  This routine initializes the mutex based on the parameters passed.
 *
 *  @param[in,out] the_mutex is the mutex to initalize
 *  @param[in,out] executing The currently executing thread.
 *  @param[in] the_mutex_attributes is the attributes associated with this
 *         mutex instance
 *  @param[in] initial_lock is the initial value of the mutex
 *
 *  @retval This method returns CORE_MUTEX_STATUS_SUCCESSFUL if successful.
 */
CORE_mutex_Status _CORE_mutex_Initialize(
  CORE_mutex_Control           *the_mutex,
  Thread_Control               *executing,
  const CORE_mutex_Attributes  *the_mutex_attributes,
  uint32_t                      initial_lock
);

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
 *  @param[in] level is the interrupt level
 *
 *  @retval This routine returns 0 if "trylock" can resolve whether or not
 *  the mutex is immediately obtained or there was an error attempting to
 *  get it.  It returns 1 to indicate that the caller cannot obtain
 *  the mutex and will have to block to do so.
 *
 *  @note  For performance reasons, this routine is implemented as
 *         a macro that uses two support routines.
 */

RTEMS_INLINE_ROUTINE int _CORE_mutex_Seize_interrupt_trylock_body(
  CORE_mutex_Control  *the_mutex,
  Thread_Control      *executing,
  ISR_Level            level
);

#if defined(__RTEMS_DO_NOT_INLINE_CORE_MUTEX_SEIZE__)
  /**
   *  @brief Interrupt trylock CORE mutex seize.
   *
   *  When doing test coverage analysis or trying to minimize the code
   *  space for RTEMS, it is often helpful to not inline this method
   *  multiple times.  It is fairly large and has a high branch complexity
   *  which makes it harder to get full binary test coverage.
   *
   *  @param[in] the_mutex will attempt to lock
   *  @param[in] level_p is the interrupt level
   */
  int _CORE_mutex_Seize_interrupt_trylock(
    CORE_mutex_Control  *the_mutex,
    Thread_Control      *executing,
    ISR_Level            level
  );
#else
  /**
   *  The default is to favor speed and inlining this definitely saves
   *  a few instructions.  This is very important for mutex performance.
   *
   *  @param[in] _mutex will attempt to lock
   *  @param[in] _level is the interrupt level
   */
  #define _CORE_mutex_Seize_interrupt_trylock( _mutex, _executing, _level ) \
     _CORE_mutex_Seize_interrupt_trylock_body( _mutex, _executing, _level )
#endif

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
 */
void _CORE_mutex_Seize_interrupt_blocking(
  CORE_mutex_Control  *the_mutex,
  Thread_Control      *executing,
  Watchdog_Interval    timeout
);
/**
 *  @brief Verifies that a mutex blocking seize is performed safely.
 *
 *  This macro is to verify that a mutex blocking seize is
 *  performed from a safe system state.  For example, one
 *  cannot block inside an isr.
 *
 *  @retval this method returns true if dispatch is in an unsafe state.
 */
#ifdef RTEMS_SMP
  #define _CORE_mutex_Check_dispatch_for_seize(_wait) \
      (_Thread_Dispatch_get_disable_level() != 1 \
        && (_wait) \
        && (_System_state_Get() >= SYSTEM_STATE_UP))
#else
  #define _CORE_mutex_Check_dispatch_for_seize(_wait) \
      (!_Thread_Dispatch_is_enabled() \
        && (_wait) \
        && (_System_state_Get() >= SYSTEM_STATE_UP))
#endif

/**
 *  @brief Attempt to obtain the mutex.
 *
 *  This routine attempts to obtain the mutex.  If the mutex is available,
 *  then it will return immediately.  Otherwise, it will invoke the
 *  support routine @a _Core_mutex_Seize_interrupt_blocking.
 *
 *  @param[in] _the_mutex is the mutex to attempt to lock
 *  @param[in] _id is the Id of the owning API level Semaphore object
 *  @param[in] _wait is true if the thread is willing to wait
 *  @param[in] _timeout is the maximum number of ticks to block
 *  @param[in] _level is a temporary variable used to contain the ISR
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
RTEMS_INLINE_ROUTINE void _CORE_mutex_Seize_body(
  CORE_mutex_Control  *the_mutex,
  Thread_Control      *executing,
  Objects_Id           id,
  bool                 wait,
  Watchdog_Interval    timeout,
  ISR_Level            level
)
{
  if ( _CORE_mutex_Check_dispatch_for_seize( wait ) ) {
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      false,
      INTERNAL_ERROR_MUTEX_OBTAIN_FROM_BAD_STATE
    );
  }
  if ( _CORE_mutex_Seize_interrupt_trylock( the_mutex, executing, level ) ) {
    if ( !wait ) {
      _ISR_Enable( level );
      executing->Wait.return_code =
        CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT;
    } else {
      _Thread_queue_Enter_critical_section( &the_mutex->Wait_queue );
      executing->Wait.queue = &the_mutex->Wait_queue;
      executing->Wait.id = id;
      _Thread_Disable_dispatch();
      _ISR_Enable( level );
      _CORE_mutex_Seize_interrupt_blocking( the_mutex, executing, timeout );
    }
  }
}

/**
 *  This method is used to obtain a core mutex.
 *
 *  @param[in] _the_mutex is the mutex to attempt to lock
 *  @param[in] _id is the Id of the owning API level Semaphore object
 *  @param[in] _wait is true if the thread is willing to wait
 *  @param[in] _timeout is the maximum number of ticks to block
 *  @param[in] _level is a temporary variable used to contain the ISR
 *         disable level cookie
 */
#if defined(__RTEMS_DO_NOT_INLINE_CORE_MUTEX_SEIZE__)
  void _CORE_mutex_Seize(
    CORE_mutex_Control  *_the_mutex,
    Thread_Control      *_executing,
    Objects_Id           _id,
    bool                 _wait,
    Watchdog_Interval    _timeout,
    ISR_Level            _level
  );
#else
  #define _CORE_mutex_Seize( _executing, _mtx, _id, _wait, _timeout, _level ) \
     _CORE_mutex_Seize_body( _executing, _mtx, _id, _wait, _timeout, _level )
#endif

/**
 *  @brief Frees a unit to the mutex.
 *
 *  This routine frees a unit to the mutex.  If a task was blocked waiting for
 *  a unit from this mutex, then that task will be readied and the unit
 *  given to that task.  Otherwise, the unit will be returned to the mutex.
 *
 *  @param[in] the_mutex is the mutex to surrender
 *  @param[in] id is the id of the RTEMS Object associated with this mutex
 *  @param[in] api_mutex_mp_support is the routine that will be called when
 *         unblocking a remote mutex
 *
 *  @retval an indication of whether the routine succeeded or failed
 */
CORE_mutex_Status _CORE_mutex_Surrender(
  CORE_mutex_Control                *the_mutex,
  Objects_Id                         id,
  CORE_mutex_API_mp_support_callout  api_mutex_mp_support
);

/**
 *  @brief Flush all waiting threads.
 *
 *  This routine assists in the deletion of a mutex by flushing the associated
 *  wait queue.
 *
 *  @param[in] the_mutex is the mutex to flush
 *  @param[in] remote_extract_callout is the routine to invoke when a remote
 *         thread is extracted
 *  @param[in] status is the status value which each unblocked thread will
 *         return to its caller.
 */
void _CORE_mutex_Flush(
  CORE_mutex_Control         *the_mutex,
  Thread_queue_Flush_callout  remote_extract_callout,
  uint32_t                    status
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
  CORE_mutex_Control  *the_mutex
)
{
  return the_mutex->lock == CORE_MUTEX_LOCKED;
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

/**
 * @brief Doex core mutex use priority blocking.
 *
 * This routine returns true if the mutex's wait discipline is PRIORITY and
 * false otherwise.
 *
 * @param[in] the_attribute is the attribute set of the mutex.
 *
 * @retval true The mutex is using priority blocking order.
 * @retval false The mutex is not using priority blocking order.
 *
 */
RTEMS_INLINE_ROUTINE bool _CORE_mutex_Is_priority(
  CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY;
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
  CORE_mutex_Attributes *the_attribute
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
  CORE_mutex_Attributes *the_attribute
)
{
  return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING;
}

/*
 *  Seize Mutex with Quick Success Path
 *
 *  NOTE: There is no MACRO version of this routine.  A body is in
 *  coremutexseize.c that is duplicated from the .inl by hand.
 *
 *  NOTE: The Doxygen for this routine is in the .h file.
 */

RTEMS_INLINE_ROUTINE int _CORE_mutex_Seize_interrupt_trylock_body(
  CORE_mutex_Control  *the_mutex,
  Thread_Control      *executing,
  ISR_Level            level
)
{
  /* disabled when you get here */

  executing->Wait.return_code = CORE_MUTEX_STATUS_SUCCESSFUL;
  if ( !_CORE_mutex_Is_locked( the_mutex ) ) {
    the_mutex->lock       = CORE_MUTEX_LOCKED;
    the_mutex->holder     = executing;
    the_mutex->holder_id  = executing->Object.id;
    the_mutex->nest_count = 1;
    if ( _CORE_mutex_Is_inherit_priority( &the_mutex->Attributes ) ||
         _CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ){

#ifdef __RTEMS_STRICT_ORDER_MUTEX__
       _Chain_Prepend_unprotected( &executing->lock_mutex,
                                   &the_mutex->queue.lock_queue );
       the_mutex->queue.priority_before = executing->current_priority;
#endif

      executing->resource_count++;
    }

    if ( !_CORE_mutex_Is_priority_ceiling( &the_mutex->Attributes ) ) {
      _ISR_Enable( level );
      return 0;
    } /* else must be CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING
       *
       * we possibly bump the priority of the current holder -- which
       * happens to be _Thread_Executing.
       */
    {
      Priority_Control  ceiling;
      Priority_Control  current;

      ceiling = the_mutex->Attributes.priority_ceiling;
      current = executing->current_priority;
      if ( current == ceiling ) {
        _ISR_Enable( level );
        return 0;
      }

      if ( current > ceiling ) {
        _Thread_Disable_dispatch();
        _ISR_Enable( level );
        _Thread_Change_priority(
          the_mutex->holder,
          the_mutex->Attributes.priority_ceiling,
         false
        );
        _Thread_Enable_dispatch();
        return 0;
      }
      /* if ( current < ceiling ) */ {
        executing->Wait.return_code = CORE_MUTEX_STATUS_CEILING_VIOLATED;
        the_mutex->lock       = CORE_MUTEX_UNLOCKED;
        the_mutex->nest_count = 0;     /* undo locking above */
        executing->resource_count--;   /* undo locking above */
        _ISR_Enable( level );
        return 0;
      }
    }
    return 0;
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
        _ISR_Enable( level );
        return 0;
      #if defined(RTEMS_POSIX_API)
        case CORE_MUTEX_NESTING_IS_ERROR:
          executing->Wait.return_code = CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED;
          _ISR_Enable( level );
          return 0;
      #endif
      case CORE_MUTEX_NESTING_BLOCKS:
        break;
    }
  }

  /*
   *  The mutex is not available and the caller must deal with the possibility
   *  of blocking.
   */
  return 1;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
