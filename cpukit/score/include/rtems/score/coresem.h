/*  core_sem.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Counting Semaphore Handler.  A counting semaphore is the
 *  standard Dijkstra binary semaphore used to provide synchronization
 *  and mutual exclusion capabilities.
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
 
#ifndef __RTEMS_CORE_COUNTING_SEMAPHORE_h
#define __RTEMS_CORE_COUNTING_SEMAPHORE_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>
 
/*
 *  The following type defines the callout which the API provides
 *  to support global/multiprocessor operations on semaphores.
 */
 
typedef void ( *CORE_semaphore_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

/*
 *  Blocking disciplines for a semaphore.
 */

typedef enum {
  CORE_SEMAPHORE_DISCIPLINES_FIFO,
  CORE_SEMAPHORE_DISCIPLINES_PRIORITY
}   CORE_semaphore_Disciplines;

/*
 *  Core Semaphore handler return statuses.
 */
 
typedef enum {
  CORE_SEMAPHORE_STATUS_SUCCESSFUL,
  CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT,
  CORE_SEMAPHORE_WAS_DELETED,
  CORE_SEMAPHORE_TIMEOUT,
  CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED
}   CORE_semaphore_Status;

/*
 *  The following defines the control block used to manage the 
 *  attributes of each semaphore.
 */

typedef struct {
  unsigned32                  maximum_count;
  CORE_semaphore_Disciplines  discipline;
}   CORE_semaphore_Attributes;
 
/*
 *  The following defines the control block used to manage each 
 *  counting semaphore.
 */
 
typedef struct {
  Thread_queue_Control        Wait_queue;
  CORE_semaphore_Attributes   Attributes;
  unsigned32                  count;
}   CORE_semaphore_Control;

/*
 *  _CORE_semaphore_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the semaphore based on the parameters passed.
 */

void _CORE_semaphore_Initialize(
  CORE_semaphore_Control       *the_semaphore,
  CORE_semaphore_Attributes    *the_semaphore_attributes,
  unsigned32                    initial_value
);
 
/*
 *  _CORE_semaphore_Seize
 *
 *  DESCRIPTION:
 *
 *  This routine attempts to receive a unit from the_semaphore.
 *  If a unit is available or if the wait flag is FALSE, then the routine
 *  returns.  Otherwise, the calling task is blocked until a unit becomes
 *  available.
 */

void _CORE_semaphore_Seize(
  CORE_semaphore_Control  *the_semaphore,
  Objects_Id               id,
  boolean                  wait,
  Watchdog_Interval        timeout
);
 
/*
 *  _CORE_semaphore_Surrender
 *
 *  DESCRIPTION:
 *
 *  This routine frees a unit to the semaphore.  If a task was blocked waiting
 *  for a unit from this semaphore, then that task will be readied and the unit
 *  given to that task.  Otherwise, the unit will be returned to the semaphore.
 */

CORE_semaphore_Status _CORE_semaphore_Surrender(
  CORE_semaphore_Control                *the_semaphore,
  Objects_Id                             id,
  CORE_semaphore_API_mp_support_callout  api_semaphore_mp_support
);
 
/*
 *  _CORE_semaphore_Flush
 *
 *  DESCRIPTION:
 *
 *  This routine assists in the deletion of a semaphore by flushing the
 *  associated wait queue.
 */

void _CORE_semaphore_Flush(
  CORE_semaphore_Control         *the_semaphore,
  Thread_queue_Flush_callout      remote_extract_callout,
  unsigned32                      status
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/coresem.inl>
#endif

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */

