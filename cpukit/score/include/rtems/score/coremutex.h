/**
 *  @file  rtems/score/coremutex.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Mutex Handler.  A mutex is an enhanced version of the standard
 *  Dijkstra binary semaphore used to provide synchronization and mutual
 *  exclusion capabilities.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREMUTEX_H
#define _RTEMS_SCORE_COREMUTEX_H


/**
 *  @defgroup ScoreMutex Mutex Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
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
 *  @brief MP Support Callback Prototype
 *
 *  The following type defines the callout which the API provides
 *  to support global/multiprocessor operations on mutexes.
 */
typedef void ( *CORE_mutex_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

/**
 *  @brief Blocking Disciplines Enumerated Type
 *
 *  This enumerated type defines the blocking disciplines for a mutex.
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
 *  @brief Mutex method return statuses
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
 *  @brief Core Mutex Last Status
 *
 *  This is the last status value.
 */
#define CORE_MUTEX_STATUS_LAST CORE_MUTEX_STATUS_CEILING_VIOLATED

/**
 *  @brief Mutex Lock Nesting Behavior Enumeration
 *
 *  This enumerated type defines the possible behaviors for
 *  lock nesting.
 */
typedef enum {
  /**
   *    This sequence has no blocking or errors:
   *
   *         + lock(m)
   *         + lock(m)
   *         + unlock(m)
   *         + unlock(m)
   */
  CORE_MUTEX_NESTING_ACQUIRES,
#if defined(RTEMS_POSIX_API)
  /**
   *    This sequence returns an error at the indicated point:
   *
   *        + lock(m)
   *        + lock(m)   - already locked error
   *        + unlock(m)
   */
  CORE_MUTEX_NESTING_IS_ERROR,
#endif
  /**
   *    This sequence performs as indicated:
   *        + lock(m)
   *        + lock(m)   - deadlocks or timeouts
   *        + unlock(m) - releases
   */
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
 *  @brief Core Mutex Attributes
 *
 *  The following defines the control block used to manage the
 *  attributes of each mutex.
 */
typedef struct {
  /** This field determines what the behavior of this mutex instance will
   *  be when attempting to acquire the mutex when it is already locked.
   */
  CORE_mutex_Nesting_behaviors lock_nesting_behavior;
  /** When this field is true, then only the thread that locked the mutex
   *  is allowed to unlock it.
   */
  bool                         only_owner_release;
  /** This field indicates whether threads waiting on the mutex block in
   *  FIFO or priority order.
   */
  CORE_mutex_Disciplines       discipline;
  /** This field contains the ceiling priority to be used if that protocol
   *  is selected.
   */
  Priority_Control             priority_ceiling;
}   CORE_mutex_Attributes;

#ifdef __RTEMS_STRICT_ORDER_MUTEX__
/*@brief Core Mutex Lock_Chain Struct
 *
 * The following defines the control block used to manage lock chain of
 * priority inheritance mutex.
 */
  typedef struct{
    /** This field is a chian of locked mutex by a thread,new mutex will
     *  be added to the head of queue, and the mutex which will be released
     *  must be the head of queue.
     */
    Chain_Node                lock_queue;
    /** This field is the priority of thread before locking this mutex
     *
     */
    Priority_Control          priority_before;
  }  CORE_mutex_order_list;
#endif

/**
 *  @brief Core Mutex Control Structure
 *
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
#ifdef __RTEMS_STRICT_ORDER_MUTEX__
  /** This field is used to manipulate the priority inheritance mutex queue*/
  CORE_mutex_order_list   queue;
#endif

}   CORE_mutex_Control;

/**
 *  @brief Initialize a Core Mutex
 *
 *  This routine initializes the mutex based on the parameters passed.
 *
 *  @param[in] the_mutex is the mutex to initalize
 *  @param[in] the_mutex_attributes is the attributes associated with this
 *         mutex instance
 *  @param[in] initial_lock is the initial value of the mutex
 *
 *  @return This method returns CORE_MUTEX_STATUS_SUCCESSFUL if successful.
 */
CORE_mutex_Status _CORE_mutex_Initialize(
  CORE_mutex_Control           *the_mutex,
  CORE_mutex_Attributes        *the_mutex_attributes,
  uint32_t                      initial_lock
);

#ifndef __RTEMS_APPLICATION__
/**
 *  @brief Seize Mutex with Quick Success Path
 *
 *  This routine attempts to receive a unit from the_mutex.
 *  If a unit is available or if the wait flag is false, then the routine
 *  returns.  Otherwise, the calling task is blocked until a unit becomes
 *  available.
 *
 *  @param[in] the_mutex is the mutex to attempt to lock
 *  @param[in] level_p is the interrupt level holder
 *
 *  @return This routine returns 0 if "trylock" can resolve whether or not
 *  the mutex is immediately obtained or there was an error attempting to
 *  get it.  It returns 1 to indicate that the caller cannot obtain
 *  the mutex and will have to block to do so.
 *
 *  @note  For performance reasons, this routine is implemented as
 *         a macro that uses two support routines.
 */

RTEMS_INLINE_ROUTINE int _CORE_mutex_Seize_interrupt_trylock_body(
  CORE_mutex_Control  *the_mutex,
  ISR_Level           *level_p
);

#if defined(__RTEMS_DO_NOT_INLINE_CORE_MUTEX_SEIZE__)
  /**
   *  When doing test coverage analysis or trying to minimize the code
   *  space for RTEMS, it is often helpful to not inline this method
   *  multiple times.  It is fairly large and has a high branch complexity
   *  which makes it harder to get full binary test coverage.
   *
   *  @param[in] the_mutex will attempt to lock
   *  @param[in] level_p is the interrupt level holder
   */
  int _CORE_mutex_Seize_interrupt_trylock(
    CORE_mutex_Control  *the_mutex,
    ISR_Level           *level_p
  );
#else
  /**
   *  The default is to favor speed and inlining this definitely saves
   *  a few instructions.  This is very important for mutex performance.
   *
   *  @param[in] _mutex will attempt to lock
   *  @param[in] _level_p is the interrupt level holder
   */
  #define _CORE_mutex_Seize_interrupt_trylock( _mutex, _level_p ) \
     _CORE_mutex_Seize_interrupt_trylock_body( _mutex, _level_p )
#endif

/**
 *  @brief Seize Mutex with Blocking
 *
 *  This routine performs the blocking portion of a mutex obtain.
 *  It is an actual subroutine and is not implemented as something
 *  that may be inlined.
 *
 *  @param[in] the_mutex is the mutex to attempt to lock
 *  @param[in] timeout is the maximum number of ticks to block
 */
void _CORE_mutex_Seize_interrupt_blocking(
  CORE_mutex_Control  *the_mutex,
  Watchdog_Interval    timeout
);


/**
 *  @brief Sieze Interrupt Wrapper
 *
 *  This macro is to verify that a mutex blocking seize is
 *  performed from a safe system state.  For example, one 
 *  cannot block inside an isr.
 *
 *  @return this method returns true if dispatch is in an unsafe state.
 */
#ifdef RTEMS_SMP
  #define _CORE_mutex_Check_dispatch_for_seize(_wait) 0
#else
  #define _CORE_mutex_Check_dispatch_for_seize(_wait) \
      (_Thread_Dispatch_in_critical_section() \
        && (_wait) \
        && (_System_state_Get() >= SYSTEM_STATE_BEGIN_MULTITASKING))
#endif

/**
 *  @brief Sieze Interrupt Wrapper
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
#define _CORE_mutex_Seize_body( \
  _the_mutex, _id, _wait, _timeout, _level ) \
  do { \
    if ( _CORE_mutex_Check_dispatch_for_seize(_wait) ) { \
        _Internal_error_Occurred( \
           INTERNAL_ERROR_CORE, \
           false, \
           INTERNAL_ERROR_MUTEX_OBTAIN_FROM_BAD_STATE \
           ); \
    } \
    if ( _CORE_mutex_Seize_interrupt_trylock( _the_mutex, &(_level) ) ) {  \
      if ( !(_wait) ) { \
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
    Objects_Id           _id,
    bool                 _wait,
    Watchdog_Interval    _timeout,
    ISR_Level            _level
  );
#else
  #define _CORE_mutex_Seize( _the_mutex, _id, _wait, _timeout, _level ) \
     _CORE_mutex_Seize_body( _the_mutex, _id, _wait, _timeout, _level )
#endif

/**
 *  @brief Surrender the Mutex
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
 *  @return an indication of whether the routine succeeded or failed
 */
CORE_mutex_Status _CORE_mutex_Surrender(
  CORE_mutex_Control                *the_mutex,
  Objects_Id                         id,
  CORE_mutex_API_mp_support_callout  api_mutex_mp_support
);

/**
 *  @brief Flush all waiting threads
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

#include <rtems/score/coremutex.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/*  end of include file */
