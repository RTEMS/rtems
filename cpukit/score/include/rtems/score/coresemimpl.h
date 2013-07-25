/**
 * @file
 *
 * @brief Inlined Routines Associated with the SuperCore Semaphore
 *
 * This include file contains all of the inlined routines associated
 * with the SuperCore semaphore.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CORESEMIMPL_H
#define _RTEMS_SCORE_CORESEMIMPL_H

#include <rtems/score/coresem.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadqimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreSemaphore
 */
/**@{**/

#if defined(RTEMS_POSIX_API) || defined(RTEMS_MULTIPROCESSING)
  #define RTEMS_SCORE_CORESEM_ENABLE_SEIZE_BODY
#endif

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
 *  @brief Core semaphore last status value.
 *
 *  This is the last status value.
 */
#define CORE_SEMAPHORE_STATUS_LAST CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED

/**
 *  The following type defines the callout which the API provides
 *  to support global/multiprocessor operations on semaphores.
 */
typedef void ( *CORE_semaphore_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

/**
 *  @brief Initialize the semaphore based on the parameters passed.
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the CORE Semaphore Handler.
 *  This core object utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  This routine initializes the semaphore based on the parameters passed.
 *
 *  @param[in] the_semaphore is the semaphore to initialize
 *  @param[in] the_semaphore_attributes define the behavior of this instance
 *  @param[in] initial_value is the initial count of the semaphore
 */
void _CORE_semaphore_Initialize(
  CORE_semaphore_Control          *the_semaphore,
  const CORE_semaphore_Attributes *the_semaphore_attributes,
  uint32_t                         initial_value
);

#if defined(RTEMS_SCORE_CORESEM_ENABLE_SEIZE_BODY)
  /**
   *  This routine attempts to receive a unit from @a the_semaphore.
   *  If a unit is available or if the wait flag is false, then the routine
   *  returns.  Otherwise, the calling task is blocked until a unit becomes
   *  available.
   *
   *  @param[in] the_semaphore is the semaphore to seize
   *  @param[in,out] executing The currently executing thread.
   *  @param[in] id is the Id of the API level Semaphore object associated
   *         with this instance of a SuperCore Semaphore
   *  @param[in] wait indicates if the caller is willing to block
   *  @param[in] timeout is the number of ticks the calling thread is willing
   *         to wait if @a wait is true.
   */
  void _CORE_semaphore_Seize(
    CORE_semaphore_Control  *the_semaphore,
    Thread_Control          *executing,
    Objects_Id               id,
    bool                     wait,
    Watchdog_Interval        timeout
  );
#endif

/**
 *  @brief Surrender a unit to a semaphore.
 *
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
 *  @retval an indication of whether the routine succeeded or failed
 */
CORE_semaphore_Status _CORE_semaphore_Surrender(
  CORE_semaphore_Control                *the_semaphore,
  Objects_Id                             id,
  CORE_semaphore_API_mp_support_callout  api_semaphore_mp_support
);

/**
 *  @brief Core semaphore flush.
 *
 *  DESCRIPTION:
 *  This package is the implementation of the CORE Semaphore Handler.
 *  This core object utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
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

/**
 * This function returns true if the priority attribute is
 * enabled in the @a attribute_set and false otherwise.
 *
 * @param[in] the_attribute is the attribute set to test
 *
 * @return true if the priority attribute is enabled
 */
RTEMS_INLINE_ROUTINE bool _CORE_semaphore_Is_priority(
  const CORE_semaphore_Attributes *the_attribute
)
{
   return ( the_attribute->discipline == CORE_SEMAPHORE_DISCIPLINES_PRIORITY );
}

/**
 * This routine returns the current count associated with the semaphore.
 *
 * @param[in] the_semaphore is the semaphore to obtain the count of
 *
 * @return the current count of this semaphore
 */
RTEMS_INLINE_ROUTINE uint32_t  _CORE_semaphore_Get_count(
  CORE_semaphore_Control  *the_semaphore
)
{
  return the_semaphore->count;
}

/**
 * This routine attempts to receive a unit from the_semaphore.
 * If a unit is available or if the wait flag is false, then the routine
 * returns.  Otherwise, the calling task is blocked until a unit becomes
 * available.
 *
 * @param[in] the_semaphore is the semaphore to obtain
 * @param[in,out] executing The currently executing thread.
 * @param[in] id is the Id of the owning API level Semaphore object
 * @param[in] wait is true if the thread is willing to wait
 * @param[in] timeout is the maximum number of ticks to block
 * @param[in] level is a temporary variable used to contain the ISR
 *        disable level cookie
 *
 * @note There is currently no MACRO version of this routine.
 */
RTEMS_INLINE_ROUTINE void _CORE_semaphore_Seize_isr_disable(
  CORE_semaphore_Control  *the_semaphore,
  Thread_Control          *executing,
  Objects_Id               id,
  bool                     wait,
  Watchdog_Interval        timeout,
  ISR_Level                level
)
{
  /* disabled when you get here */

  executing->Wait.return_code = CORE_SEMAPHORE_STATUS_SUCCESSFUL;
  if ( the_semaphore->count != 0 ) {
    the_semaphore->count -= 1;
    _ISR_Enable( level );
    return;
  }

  if ( !wait ) {
    _ISR_Enable( level );
    executing->Wait.return_code = CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT;
    return;
  }

  _Thread_Disable_dispatch();
  _Thread_queue_Enter_critical_section( &the_semaphore->Wait_queue );
  executing->Wait.queue          = &the_semaphore->Wait_queue;
  executing->Wait.id             = id;
  _ISR_Enable( level );

  _Thread_queue_Enqueue( &the_semaphore->Wait_queue, timeout );
  _Thread_Enable_dispatch();
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
