/** 
 *  @file  rtems/score/coremutex.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Mutex Handler.  A mutex is an enhanced version of the standard
 *  Dijkstra binary semaphore used to provide synchronization and mutual
 *  exclusion capabilities.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_COREMUTEX_H
#define _RTEMS_SCORE_COREMUTEX_H

/**
 *  @defgroup ScoreMutex Mutex Handler
 *
 *  This group contains functionality which provides the foundation
 *  Mutex services used in all of the APIs supported by RTEMS.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/interr.h>
#include <rtems/score/sysstate.h>

/**
 *  The following type defines the callout which the API provides
 *  to support global/multiprocessor operations on mutexes.
 */
typedef void ( *CORE_mutex_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

/**
 *  Blocking disciplines for a mutex.
 */
typedef enum {
  /** This specifies that threads will wait for the mutex in FIFO order. */
  CORE_MUTEX_DISCIPLINES_FIFO,
  /** This specifies that threads will wait for the mutex in priority order.  */
  CORE_MUTEX_DISCIPLINES_PRIORITY,
  /** This specifies that threads will wait for the mutex in priority order.
   *  Additionally, the Priority Inheritance Protocol will be in effect.
   */
  CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT,
  /** This specifies that threads will wait for the mutex in priority order.
   *  Additionally, the Priority Ceiling Protocol will be in effect.
   */
  CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING
}   CORE_mutex_Disciplines;

/**
 *  Mutex handler return statuses.
 */
typedef enum {
  /** This status indicates that the operation completed successfully. */
  CORE_MUTEX_STATUS_SUCCESSFUL,
  /** This status indicates that the calling task did not want to block
   *  and the operation was unable to complete immediately because the
   *  resource was unavailable.
   */
  CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT,
  /** This status indicates that an attempt was made to relock a mutex
   *  for which nesting is not configured.
   */
  CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED,
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
  /** This status indicates that a thread of logically greater importance
   *  than the ceiling priority attempted to lock this mutex.
   */
  CORE_MUTEX_STATUS_CEILING_VIOLATED
}   CORE_mutex_Status;

/**
 *  Mutex lock nesting behavior
 *
 *  + CORE_MUTEX_NESTING_ACQUIRES:
 *    This sequence has no blocking or errors:
 *
 *         + lock(m)
 *         + lock(m)
 *         + unlock(m)
 *         + unlock(m)
 *
 *  + CORE_MUTEX_NESTING_IS_ERROR
 *    This sequence returns an error at the indicated point:
 *
 *        + lock(m)
 *        + lock(m)   - already locked error
 *        + unlock(m)
 *
 *  + CORE_MUTEX_NESTING_BLOCKS
 *    This sequence performs as indicated:
 *        + lock(m)
 *        + lock(m)   - deadlocks or timeouts
 *        + unlock(m) - releases
 */
typedef enum {
  CORE_MUTEX_NESTING_ACQUIRES,
  CORE_MUTEX_NESTING_IS_ERROR,
  CORE_MUTEX_NESTING_BLOCKS
}  CORE_mutex_Nesting_behaviors;

/**
 *  This is the value of a mutex when it is unlocked.
 */
#define CORE_MUTEX_UNLOCKED 1

/**
 *  This is the value of a mutex when it is locked.
 */
#define CORE_MUTEX_LOCKED   0

/**
 *  The following defines the control block used to manage the
 *  attributes of each mutex.
 */
typedef struct {
  /** This field determines what the behavior of this mutex instance will
   *  be when attempting to acquire the mutex when it is already locked.
   */
  CORE_mutex_Nesting_behaviors lock_nesting_behavior;
  /** When this field is TRUE, then only the thread that locked the mutex
   *  is allowed to unlock it.
   */
  boolean                      only_owner_release;
  /** This field indicates whether threads waiting on the mutex block in
   *  FIFO or priority order.
   */
  CORE_mutex_Disciplines       discipline;
  /** This field contains the ceiling priority to be used if that protocol
   *  is selected.
   */
  Priority_Control             priority_ceiling;
}   CORE_mutex_Attributes;

/**
 *  The following defines the control block used to manage each mutex.
 */
typedef struct {
  /** This field is the Waiting Queue used to manage the set of tasks
   *  which are blocked waiting to lock the mutex.
   */
  Thread_queue_Control    Wait_queue;
  /** This element is the set of attributes which define this instance's
   *  behavior.
   */
  CORE_mutex_Attributes   Attributes;
  /** This element contains the current state of the mutex.
   */
  uint32_t                lock;
  /** This element contains the number of times the mutex has been acquired
   *  nested.  This must be zero (0) before the mutex is actually unlocked.
   */
  uint32_t                nest_count;
  /** This is the number of waiting threads. */
  uint32_t                blocked_count;
  /** This element points to the thread which is currently holding this mutex.
   *  The holder is the last thread to successfully lock the mutex and which
   *  has not unlocked it.  If the thread is not locked, there is no holder.
   */
  Thread_Control         *holder;
  /** This element contains the object Id of the holding thread.  */
  Objects_Id              holder_id;
}   CORE_mutex_Control;

/**
 *
 *  This routine initializes the mutex based on the parameters passed.
 *
 *  @param the_mutex (in) is the mutex to initalize
 *  @param the_mutex_attributes (in) is the attributes associated with this
 *         mutex instance
 *  @param initial_lock (in) is the initial value of the mutex
 */
void _CORE_mutex_Initialize(
  CORE_mutex_Control           *the_mutex,
  CORE_mutex_Attributes        *the_mutex_attributes,
  uint32_t                      initial_lock
);

#ifndef __RTEMS_APPLICATION__
/**
 *  This routine attempts to receive a unit from the_mutex.
 *  If a unit is available or if the wait flag is FALSE, then the routine
 *  returns.  Otherwise, the calling task is blocked until a unit becomes
 *  available.
 *
 *  @param the_mutex (in) is the mutex to attempt to lock
 *  @param level_p (in) is the interrupt level holder
 *
 *  @note  For performance reasons, this routine is implemented as
 *         a macro that uses two support routines.
 */
RTEMS_INLINE_ROUTINE int _CORE_mutex_Seize_interrupt_trylock(
  CORE_mutex_Control  *the_mutex,
  ISR_Level           *level_p
);

/**
 *  This routine performs the blocking portion of a mutex obtain.
 *  It is an actual subroutine and is not implemented as something
 *  that may be inlined.
 *
 *  @param the_mutex (in) is the mutex to attempt to lock
 *  @param timeout (in) is the maximum number of ticks to block
 */
void _CORE_mutex_Seize_interrupt_blocking(
  CORE_mutex_Control  *the_mutex,
  Watchdog_Interval    timeout
);

/**
 *  This routine attempts to obtain the mutex.  If the mutex is available,
 *  then it will return immediately.  Otherwise, it will invoke the
 *  support routine @a _Core_mutex_Seize_interrupt_blocking.
 *
 *  @param _the_mutex (in) is the mutex to attempt to lock
 *  @param _id (in) is the Id of the owning API level Semaphore object
 *  @param _wait (in) is TRUE if the thread is willing to wait
 *  @param _timeout (in) is the maximum number of ticks to block
 *  @param _level (in) is a temporary variable used to contain the ISR
 *         disable level cookie
 */
#define _CORE_mutex_Seize( \
  _the_mutex, _id, _wait, _timeout, _level ) \
  do { \
    if ( _Thread_Dispatch_disable_level \
        && (_wait) \
        && (_System_state_Get() >= SYSTEM_STATE_BEGIN_MULTITASKING ) \
       ) { \
        _Internal_error_Occurred( \
           INTERNAL_ERROR_CORE, \
           FALSE, \
           18 /* called from wrong environment */); \
    } \
    if ( _CORE_mutex_Seize_interrupt_trylock( _the_mutex, &_level ) ) {  \
      if ( !_wait ) { \
        _ISR_Enable( _level ); \
        _Thread_Executing->Wait.return_code = \
          CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT; \
      } else { \
        _Thread_queue_Enter_critical_section( &(_the_mutex)->Wait_queue ); \
        _Thread_Executing->Wait.queue = &(_the_mutex)->Wait_queue; \
        _Thread_Executing->Wait.id    = _id; \
        _Thread_Disable_dispatch(); \
        _ISR_Enable( _level ); \
       _CORE_mutex_Seize_interrupt_blocking( _the_mutex, _timeout ); \
      } \
    } \
  } while (0)

/**
 *  This routine frees a unit to the mutex.  If a task was blocked waiting for
 *  a unit from this mutex, then that task will be readied and the unit
 *  given to that task.  Otherwise, the unit will be returned to the mutex.
 *
 *  @param the_mutex (in) is the mutex to surrender
 *  @param id (in) is the id of the RTEMS Object associated with this mutex
 *  @param api_mutex_mp_support (in) is the routine that will be called when
 *         unblocking a remote mutex
 *
 *  @return an indication of whether the routine succeeded or failed
 */
CORE_mutex_Status _CORE_mutex_Surrender(
  CORE_mutex_Control                *the_mutex,
  Objects_Id                         id,
  CORE_mutex_API_mp_support_callout  api_mutex_mp_support
);

/**
 *  This routine assists in the deletion of a mutex by flushing the associated
 *  wait queue.
 *
 *  @param the_mutex (in) is the mutex to flush
 *  @param remote_extract_callout (in) is the routine to invoke when a remote
 *         thread is extracted
 *  @param status (in) is the status value which each unblocked thread will
 *         return to its caller.
 */
void _CORE_mutex_Flush(
  CORE_mutex_Control         *the_mutex,
  Thread_queue_Flush_callout  remote_extract_callout,
  uint32_t                    status
);

#include <rtems/score/coremutex.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/*  end of include file */
