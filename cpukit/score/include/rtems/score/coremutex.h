/*  mutex.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Mutex Handler.  A mutex is an enhanced version of the standard
 *  Dijkstra binary semaphore used to provide synchronization and mutual 
 *  exclusion capabilities.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 
#ifndef __RTEMS_CORE_MUTEX_h
#define __RTEMS_CORE_MUTEX_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/interr.h>
#include <rtems/score/sysstate.h>
 
/*
 *  The following type defines the callout which the API provides
 *  to support global/multiprocessor operations on mutexes.
 */
 
typedef void ( *CORE_mutex_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

/*
 *  Blocking disciplines for a mutex.
 */

typedef enum {
  CORE_MUTEX_DISCIPLINES_FIFO,
  CORE_MUTEX_DISCIPLINES_PRIORITY,
  CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT,
  CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING
}   CORE_mutex_Disciplines;

/*
 *  Mutex handler return statuses.
 */
 
typedef enum {
  CORE_MUTEX_STATUS_SUCCESSFUL,
  CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT,
  CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED,
  CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE,
  CORE_MUTEX_WAS_DELETED,
  CORE_MUTEX_TIMEOUT,
  CORE_MUTEX_STATUS_CEILING_VIOLATED
}   CORE_mutex_Status;

/*
 *  Mutex lock nesting behavior
 *
 *  CORE_MUTEX_NESTING_ACQUIRES:
 *    This sequence has no blocking or errors:
 *         lock(m)
 *         lock(m)
 *         unlock(m)
 *         unlock(m)
 *
 *  CORE_MUTEX_NESTING_IS_ERROR
 *    This sequence returns an error at the indicated point:
 *        lock(m)
 *        lock(m)   - already locked error
 *        unlock(m)
 *
 *  CORE_MUTEX_NESTING_BLOCKS
 *    This sequence performs as indicated:
 *        lock(m)
 *        lock(m)   - deadlocks or timeouts
 *        unlock(m) - releases
 */

typedef enum {
  CORE_MUTEX_NESTING_ACQUIRES,
  CORE_MUTEX_NESTING_IS_ERROR,
  CORE_MUTEX_NESTING_BLOCKS
}  CORE_mutex_Nesting_behaviors;
 
/*
 *  Locked and unlocked values
 */

#define CORE_MUTEX_UNLOCKED 1
#define CORE_MUTEX_LOCKED   0

/*
 *  The following defines the control block used to manage the 
 *  attributes of each mutex.
 */

typedef struct {
  CORE_mutex_Nesting_behaviors lock_nesting_behavior;
  boolean                      only_owner_release;
  CORE_mutex_Disciplines       discipline;
  Priority_Control             priority_ceiling;
}   CORE_mutex_Attributes;
 
/*
 *  The following defines the control block used to manage each mutex.
 */
 
typedef struct {
  Thread_queue_Control    Wait_queue;
  CORE_mutex_Attributes   Attributes;
  unsigned32              lock;
  unsigned32              nest_count;
  unsigned32              blocked_count;
  Thread_Control         *holder;
  Objects_Id              holder_id;
}   CORE_mutex_Control;

/*
 *  _CORE_mutex_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the mutex based on the parameters passed.
 */

void _CORE_mutex_Initialize(
  CORE_mutex_Control           *the_mutex,
  CORE_mutex_Attributes        *the_mutex_attributes,
  unsigned32                    initial_lock
);
 
/*
 *  _CORE_mutex_Seize
 *
 *  DESCRIPTION:
 *
 *  This routine attempts to receive a unit from the_mutex.
 *  If a unit is available or if the wait flag is FALSE, then the routine
 *  returns.  Otherwise, the calling task is blocked until a unit becomes
 *  available.
 *
 *  NOTE:  For performance reasons, this routine is implemented as
 *         a macro that uses two support routines.
 */


#ifndef __RTEMS_APPLICATION__
RTEMS_INLINE_ROUTINE int _CORE_mutex_Seize_interrupt_trylock(
  CORE_mutex_Control  *the_mutex,
  ISR_Level           *level_p
);

void _CORE_mutex_Seize_interrupt_blocking(
  CORE_mutex_Control  *the_mutex,
  Watchdog_Interval    timeout
);

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

/*
 *  _CORE_mutex_Surrender
 *
 *  DESCRIPTION:
 *
 *  This routine frees a unit to the mutex.  If a task was blocked waiting for
 *  a unit from this mutex, then that task will be readied and the unit
 *  given to that task.  Otherwise, the unit will be returned to the mutex.
 */

CORE_mutex_Status _CORE_mutex_Surrender(
  CORE_mutex_Control                *the_mutex,
  Objects_Id                         id,
  CORE_mutex_API_mp_support_callout  api_mutex_mp_support
);
 
/*
 *  _CORE_mutex_Flush
 *
 *  DESCRIPTION:
 *
 *  This routine assists in the deletion of a mutex by flushing the associated
 *  wait queue.
 */
 
void _CORE_mutex_Flush(
  CORE_mutex_Control         *the_mutex,
  Thread_queue_Flush_callout  remote_extract_callout,
  unsigned32                  status
);
 
#include <rtems/score/coremutex.inl>
#endif

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */

