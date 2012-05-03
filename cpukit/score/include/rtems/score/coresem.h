/**
 *  @file  rtems/score/coresem.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Counting Semaphore Handler.  A counting semaphore is the
 *  standard Dijkstra binary semaphore used to provide synchronization
 *  and mutual exclusion capabilities.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CORESEM_H
#define _RTEMS_SCORE_CORESEM_H

/**
 *  @defgroup ScoreSemaphore Semaphore Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  Semaphore services used in all of the APIs supported by RTEMS.
 */
/**@{*/

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(RTEMS_POSIX_API) || defined(RTEMS_MULTIPROCESSING)
  #define RTEMS_SCORE_CORESEM_ENABLE_SEIZE_BODY
#endif

/**
 *  The following type defines the callout which the API provides
 *  to support global/multiprocessor operations on semaphores.
 */
typedef void ( *CORE_semaphore_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

/**
 *  Blocking disciplines for a semaphore.
 */
typedef enum {
  /** This specifies that threads will wait for the semaphore in FIFO order. */
  CORE_SEMAPHORE_DISCIPLINES_FIFO,
  /** This specifies that threads will wait for the semaphore in
   *  priority order.
   */
  CORE_SEMAPHORE_DISCIPLINES_PRIORITY
}   CORE_semaphore_Disciplines;

/**
 *  Core Semaphore handler return statuses.
 */
typedef enum {
  /** This status indicates that the operation completed successfully. */
  CORE_SEMAPHORE_STATUS_SUCCESSFUL,
  /** This status indicates that the calling task did not want to block
   *  and the operation was unable to complete immediately because the
   *  resource was unavailable.
   */
  CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT,
  /** This status indicates that the thread was blocked waiting for an
   *  operation to complete and the semaphore was deleted.
   */
  CORE_SEMAPHORE_WAS_DELETED,
  /** This status indicates that the calling task was willing to block
   *  but the operation was unable to complete within the time allotted
   *  because the resource never became available.
   */
  CORE_SEMAPHORE_TIMEOUT,
  /** This status indicates that an attempt was made to unlock the semaphore
   *  and this would have made its count greater than that allowed.
   */
  CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED
}   CORE_semaphore_Status;

/**
 *  @brief Core Semaphore Last Status
 *
 *  This is the last status value.
 */
#define CORE_SEMAPHORE_STATUS_LAST CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED

/**
 *  The following defines the control block used to manage the
 *  attributes of each semaphore.
 */
typedef struct {
  /** This element indicates the maximum count this semaphore may have. */
  uint32_t                    maximum_count;
  /** This field indicates whether threads waiting on the semaphore block in
   *  FIFO or priority order.
   */
  CORE_semaphore_Disciplines  discipline;
}   CORE_semaphore_Attributes;

/**
 *  The following defines the control block used to manage each
 *  counting semaphore.
 */
typedef struct {
  /** This field is the Waiting Queue used to manage the set of tasks
   *  which are blocked waiting to obtain the semaphore.
   */
  Thread_queue_Control        Wait_queue;
  /** This element is the set of attributes which define this instance's
   *  behavior.
   */
  CORE_semaphore_Attributes   Attributes;
  /** This element contains the current count of this semaphore. */
  uint32_t                    count;
}   CORE_semaphore_Control;

/**
 *  This routine initializes the semaphore based on the parameters passed.
 *
 *  @param[in] the_semaphore is the semaphore to initialize
 *  @param[in] the_semaphore_attributes define the behavior of this instance
 *  @param[in] initial_value is the initial count of the semaphore
 */
void _CORE_semaphore_Initialize(
  CORE_semaphore_Control       *the_semaphore,
  CORE_semaphore_Attributes    *the_semaphore_attributes,
  uint32_t                      initial_value
);

#if defined(RTEMS_SCORE_CORESEM_ENABLE_SEIZE_BODY)
  /**
   *  This routine attempts to receive a unit from @a the_semaphore.
   *  If a unit is available or if the wait flag is false, then the routine
   *  returns.  Otherwise, the calling task is blocked until a unit becomes
   *  available.
   *
   *  @param[in] the_semaphore is the semaphore to seize
   *  @param[in] id is the Id of the API level Semaphore object associated
   *         with this instance of a SuperCore Semaphore
   *  @param[in] wait indicates if the caller is willing to block
   *  @param[in] timeout is the number of ticks the calling thread is willing
   *         to wait if @a wait is true.
   */
  void _CORE_semaphore_Seize(
    CORE_semaphore_Control  *the_semaphore,
    Objects_Id               id,
    bool                     wait,
    Watchdog_Interval        timeout
  );
#endif

/**
 *  This routine frees a unit to the semaphore.  If a task was blocked waiting
 *  for a unit from this semaphore, then that task will be readied and the unit
 *  given to that task.  Otherwise, the unit will be returned to the semaphore.
 *
 *  @param[in] the_semaphore is the semaphore to surrender
 *  @param[in] id is the Id of the API level Semaphore object associated
 *         with this instance of a SuperCore Semaphore
 *  @param[in] api_semaphore_mp_support is the routine to invoke if the
 *         thread unblocked is remote
 *
 *  @return an indication of whether the routine succeeded or failed
 */
CORE_semaphore_Status _CORE_semaphore_Surrender(
  CORE_semaphore_Control                *the_semaphore,
  Objects_Id                             id,
  CORE_semaphore_API_mp_support_callout  api_semaphore_mp_support
);

/**
 *  This routine assists in the deletion of a semaphore by flushing the
 *  associated wait queue.
 *
 *  @param[in] the_semaphore is the semaphore to flush
 *  @param[in] remote_extract_callout is the routine to invoke if the
 *         thread unblocked is remote
 *  @param[in] status is the status to be returned to the unblocked thread
 */
void _CORE_semaphore_Flush(
  CORE_semaphore_Control         *the_semaphore,
  Thread_queue_Flush_callout      remote_extract_callout,
  uint32_t                        status
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/coresem.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/*  end of include file */
