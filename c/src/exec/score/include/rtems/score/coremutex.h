/*  mutex.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Mutex Handler.  A mutex is an enhanced version of the standard
 *  Dijkstra binary semaphore used to provide synchronization and mutual 
 *  exclusion capabilities.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */
 
#ifndef __RTEMS_CORE_MUTEX_h
#define __RTEMS_CORE_MUTEX_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/core/thread.h>
#include <rtems/core/threadq.h>
#include <rtems/core/priority.h>
#include <rtems/core/watchdog.h>
 
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
  CORE_MUTEX_TIMEOUT
}   CORE_mutex_Status;

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
  boolean                 allow_nesting;
  CORE_mutex_Disciplines  discipline;
  Priority_Control        priority_ceiling;
}   CORE_mutex_Attributes;
 
/*
 *  The following defines the control block used to manage each mutex.
 */
 
typedef struct {
  Thread_queue_Control    Wait_queue;
  CORE_mutex_Attributes   Attributes;
  unsigned32              lock;
  unsigned32              nest_count;
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
  Objects_Classes               the_class,
  CORE_mutex_Attributes        *the_mutex_attributes,
  unsigned32                    initial_lock,
  Thread_queue_Extract_callout  proxy_extract_callout
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
 */

void _CORE_mutex_Seize(
  CORE_mutex_Control  *the_mutex,
  Objects_Id           id,
  boolean              wait,
  Watchdog_Interval    timeout
);
 
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
 
/*
 *  _CORE_mutex_Is_locked
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex specified is locked and FALSE
 *  otherwise.
 */
 
STATIC INLINE boolean _CORE_mutex_Is_locked(
  CORE_mutex_Control  *the_mutex
);

/*
 *  _CORE_mutex_Is_fifo
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex's wait discipline is FIFO and FALSE
 *  otherwise.
 */
 
STATIC INLINE boolean _CORE_mutex_Is_fifo(
  CORE_mutex_Attributes *the_attribute
);

/*
 *  _CORE_mutex_Is_priority
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex's wait discipline is PRIORITY and
 *  FALSE otherwise.
 */

STATIC INLINE boolean _CORE_mutex_Is_priority(
  CORE_mutex_Attributes *the_attribute
);

/*
 *  _CORE_mutex_Is_inherit_priority
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex's wait discipline is
 *  INHERIT_PRIORITY and FALSE otherwise.
 */

STATIC INLINE boolean _CORE_mutex_Is_inherit_priority(
  CORE_mutex_Attributes *the_attribute
);

/*
 *  _CORE_mutex_Is_priority_ceiling
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex's wait discipline is
 *  PRIORITY_CEILING and FALSE otherwise.
 */

STATIC INLINE boolean _CORE_mutex_Is_priority_ceiling(
  CORE_mutex_Attributes *the_attribute
);

/*
 *  _CORE_mutex_Is_nesting_allowed
 *
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the mutex allows a task to obtain a
 *  semaphore more than once and nest.
 */
 
STATIC INLINE boolean _CORE_mutex_Is_nesting_allowed(
  CORE_mutex_Attributes *the_attribute
);

#include <rtems/core/coremutex.inl>

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */

