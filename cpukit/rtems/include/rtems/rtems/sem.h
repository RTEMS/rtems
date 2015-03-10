/**
 * @file
 *
 * @ingroup ClassicSem
 *
 * @brief Classic Semaphores API
 *
 * This include file contains all the constants and structures associated
 * with the Semaphore Manager. This manager utilizes standard Dijkstra
 * counting semaphores to provide synchronization and mutual exclusion
 * capabilities.
 *
 * Directives provided are:
 *
 * - create a semaphore
 * - get an ID of a semaphore
 * - delete a semaphore
 * - acquire a semaphore
 * - release a semaphore
 * - flush a semaphore
 * - set ceiling priority for a semaphore
 */

/*
 * COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SEM_H
#define _RTEMS_RTEMS_SEM_H

#include <rtems/rtems/types.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/object.h>
#include <rtems/score/coresem.h>
#include <rtems/score/mrsp.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicSem Semaphores
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality related to the Classic API
 *  Semaphore Manager.
 */
/**@{*/

/**
 *  The following defines the control block used to manage each semaphore.
 */
typedef struct {
  /** This field is the object management portion of a Semaphore instance. */
  Objects_Control          Object;

  /**
   *  This is the Classic API attribute provided to the create directive.
   *  It is translated into behavioral attributes on the SuperCore Semaphore
   *  or Mutex instance.
   */
  rtems_attribute          attribute_set;

  /**
   *  This contains the memory associated with the SuperCore Semaphore or
   *  Mutex instance that provides the primary functionality of each
   *  Classic API Semaphore instance.  The structure used is dependent
   *  on the attributes specified by the user on the create directive.
   *
   *  @note Only one of these has meaning in a particular Classic API
   *        Semaphore instance.
   */
  union {
    /**
     *  This is the SuperCore Mutex instance associated with this Classic
     *  API Semaphore instance.
     */
    CORE_mutex_Control     mutex;

    /**
     *  This is the SuperCore Semaphore instance associated with this Classic
     *  API Semaphore instance.
     */
    CORE_semaphore_Control semaphore;

#if defined(RTEMS_SMP)
    MRSP_Control mrsp;
#endif
  } Core_control;
}   Semaphore_Control;

/**
 *  @brief rtems_semaphore_create
 *
 *  This routine implements the rtems_semaphore_create directive.  The
 *  semaphore will have the name name.  The starting count for
 *  the semaphore is count.  The attribute_set determines if
 *  the semaphore is global or local and the thread queue
 *  discipline.  It returns the id of the created semaphore in ID.
 *
 *  Semaphore Manager
 */
rtems_status_code rtems_semaphore_create(
  rtems_name           name,
  uint32_t             count,
  rtems_attribute      attribute_set,
  rtems_task_priority  priority_ceiling,
  rtems_id            *id
);

/**
 * @brief RTEMS Semaphore Name to Id
 *
 * This routine implements the rtems_semaphore_ident directive.
 * This directive returns the semaphore ID associated with name.
 * If more than one semaphore is named name, then the semaphore
 * to which the ID belongs is arbitrary. node indicates the
 * extent of the search for the ID of the semaphore named name.
 * The search can be limited to a particular node or allowed to
 * encompass all nodes.
 *
 * @param[in] name is the user defined semaphore name
 * @param[in] node is(are) the node(s) to be searched
 * @param[in] id is the pointer to semaphore id
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful and
 * *id filled in with the semaphore id
 */
rtems_status_code rtems_semaphore_ident(
  rtems_name    name,
  uint32_t      node,
  rtems_id     *id
);

/**
 * @brief RTEMS Delete Semaphore
 *
 * This routine implements the rtems_semaphore_delete directive. The
 * semaphore indicated by ID is deleted.
 *
 * @param[in] id is the semaphore id
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_semaphore_delete(
  rtems_id   id
);

/**
 * @brief RTEMS Obtain Semaphore
 *
 * This routine implements the rtems_semaphore_obtain directive. It
 * attempts to obtain a unit from the semaphore associated with ID.
 * If a unit can be allocated, the calling task will return immediately.
 * If no unit is available, then the task may return immediately or
 * block waiting for a unit with an optional timeout of timeout
 * clock ticks. Whether the task blocks or returns immediately
 * is based on the RTEMS_NO_WAIT option in the option_set.
 *
 * @param[in] id is the semaphore id
 * @param[in] option_set is the wait option
 * @param[in] timeout is the number of ticks to wait (0 means wait forever)
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_semaphore_obtain(
  rtems_id       id,
  rtems_option   option_set,
  rtems_interval timeout
);

/**
 *  @brief RTEMS Semaphore Release
 *
 *  Semaphore Manager
 *
 *  This routine implements the rtems_semaphore_release directive.  It
 *  frees a unit to the semaphore associated with ID.  If a task was
 *  blocked waiting for a unit from this semaphore, then that task will
 *  be readied and the unit given to that task.  Otherwise, the unit
 *  will be returned to the semaphore.
 */
rtems_status_code rtems_semaphore_release(
  rtems_id   id
);

/**
 * @brief RTEMS Semaphore Flush
 *
 * This method is the implementation of the flush directive
 * of the Semaphore Manager.
 *
 * This directive allows a thread to flush the threads
 * pending on the semaphore.
 *
 * @param[in] id is the semaphore id
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 */
rtems_status_code rtems_semaphore_flush(
  rtems_id	   id
);

/**
 * @brief Sets the priority value with respect to the specified scheduler of a
 * semaphore.
 *
 * The special priority value @ref RTEMS_CURRENT_PRIORITY can be used to get
 * the current priority value without changing it.
 *
 * The interpretation of the priority value depends on the protocol of the
 * semaphore object.
 *
 * - The Multiprocessor Resource Sharing Protocol needs a ceiling priority per
 *   scheduler instance.  This operation can be used to specify these priority
 *   values.
 * - For the Priority Ceiling Protocol the ceiling priority is used with this
 *   operation.
 * - For other protocols this operation is not defined.
 *
 * @param[in] semaphore_id Identifier of the semaphore.
 * @param[in] scheduler_id Identifier of the scheduler.
 * @param[in] new_priority The new priority value.  Use
 * @ref RTEMS_CURRENT_PRIORITY to not set a new priority and only get the
 * current priority.
 * @param[out] old_priority Reference to store the old priority value.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Invalid semaphore or scheduler identifier.
 * @retval RTEMS_INVALID_ADDRESS The old priority reference is @c NULL.
 * @retval RTEMS_INVALID_PRIORITY The new priority value is invalid.
 * @retval RTEMS_NOT_DEFINED The set priority operation is not defined for the
 * protocol of this semaphore object.
 * @retval RTEMS_ILLEGAL_ON_REMOTE_OBJECT Not supported for remote semaphores.
 *
 * @see rtems_scheduler_ident() and rtems_task_set_priority().
 */
rtems_status_code rtems_semaphore_set_priority(
  rtems_id             semaphore_id,
  rtems_id             scheduler_id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
