/**
 * @file rtems/rtems/barrier.h
 *
 * @defgroup ClassicBarrier Barriers
 *
 * @ingroup ClassicRTEMS
 * @brief Classic API Barrier Manager
 *
 * This include file contains all the constants and structures associated
 * with the Barrier Manager.
 *
 * Directives provided are:
 *
 * - create a barrier
 * - get an ID of a barrier
 * - delete a barrier
 * - wait for a barrier
 * - signal a barrier
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_BARRIER_H
#define _RTEMS_RTEMS_BARRIER_H

#include <rtems/rtems/types.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/object.h>
#include <rtems/score/corebarrier.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicBarrier Barriers
 *
 * @ingroup ClassicRTEMS
 *
 * This encapsulates functionality which implements the Classic API
 * Barrier Manager.
 */
/**@{*/

/**
 *  This type defines the control block used to manage each barrier.
 */
typedef struct {
  /** This is used to manage a barrier as an object. */
  Objects_Control          Object;
  /** This is used to specify the attributes of a barrier. */
  rtems_attribute          attribute_set;
  /** This is used to implement the barrier. */
  CORE_barrier_Control     Barrier;
}   Barrier_Control;

/**
 * @brief RTEMS Create Barrier
 *
 * Barrier Manager -- Create a Barrier Instance
 *
 * This routine implements the rtems_barrier_create directive.  The
 * barrier will have the name name. The starting count for
 * the barrier is count. The attribute_set determines if
 * the barrier is global or local and the thread queue
 * discipline. It returns the id of the created barrier in ID.
 *
 * @param[in] name is the name of this barrier instance.
 * @param[in] attribute_set specifies the attributes of this barrier instance.
 * @param[in] maximum_waiters is the maximum number of threads which will
 *            be allowed to concurrently wait at the barrier.
 * @param[out] id will contain the id of this barrier.
 *
 * @retval a status code indicating success or the reason for failure.
 */
rtems_status_code rtems_barrier_create(
  rtems_name           name,
  rtems_attribute      attribute_set,
  uint32_t             maximum_waiters,
  rtems_id            *id
);

/**
 * @brief RTEMS Barrier name to Id
 *
 * This routine implements the rtems_barrier_ident directive.
 * This directive returns the barrier ID associated with name.
 * If more than one barrier is named name, then the barrier
 * to which the ID belongs is arbitrary. node indicates the
 * extent of the search for the ID of the barrier named name.
 * The search can be limited to a particular node or allowed to
 * encompass all nodes.
 *
 * @param[in] name is the name of this barrier instance.
 * @param[out] id will contain the id of this barrier.
 *
 * @retval a status code indicating success or the reason for failure.
 */
rtems_status_code rtems_barrier_ident(
  rtems_name    name,
  rtems_id     *id
);

/**
 * @brief RTEMS Delete Barrier
 *
 * This routine implements the rtems_barrier_delete directive. The
 * barrier indicated by @a id is deleted. The barrier is freed back to the
 * inactive barrier chain.
 *
 *
 * @param[in] id indicates the barrier to delete
 *
 * @retval a status code indicating success or the reason for failure.
 */
rtems_status_code rtems_barrier_delete(
  rtems_id   id
);

/**
 * @brief RTEMS Barrier Wait
 *
 * This routine implements the rtems_barrier_wait directive. It
 * attempts to wait at the barrier associated with @a id. The calling task
 * may block waiting for the barrier with an optional timeout of @a timeout
 * clock ticks.
 *
 * @param[in] id indicates the barrier to wait at.
 * @param[in] timeout is the maximum length of time in ticks the calling
 *            thread is willing to block.
 *
 * @retval a status code indicating success or the reason for failure.
 */
rtems_status_code rtems_barrier_wait(
  rtems_id       id,
  rtems_interval timeout
);

/**
 * @brief RTEMS Barrier Release
 *
 * Barrier Manager -- Release Tasks Waitng at a Barrier
 *
 * This routine implements the rtems_barrier_release directive. It
 * unblocks all of the threads waiting on the barrier associated with
 * @a id. The number of threads unblocked is returned in @a released.
 *
 *
 * @param[in] id indicates the barrier to wait at.
 * @param[out] released will contain the number of threads unblocked.
 *
 * @retval a status code indicating success or the reason for failure.
 */
rtems_status_code rtems_barrier_release(
  rtems_id  id,
  uint32_t *released
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
