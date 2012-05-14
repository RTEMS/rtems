/**
 * @file rtems/rtems/barrier.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Barrier Manager.
 *
 *  Directives provided are:
 *
 *    - create a barrier
 *    - get an ID of a barrier
 *    - delete a barrier
 *    - wait for a barrier
 *    - signal a barrier
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_BARRIER_H
#define _RTEMS_RTEMS_BARRIER_H

/**
 *  @defgroup ClassicBarrier Barriers
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_BARRIER_EXTERN
#define RTEMS_BARRIER_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/status.h>
#include <rtems/score/object.h>
#include <rtems/score/corebarrier.h>

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
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
RTEMS_BARRIER_EXTERN Objects_Information  _Barrier_Information;

/**
 *  @brief _Barrier_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Barrier_Manager_initialization(void);

/**
 *  @brief rtems_barrier_create
 *
 *  This routine implements the rtems_barrier_create directive.  The
 *  barrier will have the name name.  The starting count for
 *  the barrier is count.  The attribute_set determines if
 *  the barrier is global or local and the thread queue
 *  discipline.  It returns the id of the created barrier in ID.
 *
 *  @param[in] name is the name of this barrier instance.
 *  @param[in] attribute_set specifies the attributes of this barrier instance.
 *  @param[in] maximum_waiters is the maximum number of threads which will
 *             be allowed to concurrently wait at the barrier.
 *  @param[out] id will contain the id of this barrier.
 *
 *  @return a status code indicating success or the reason for failure.
 */
rtems_status_code rtems_barrier_create(
  rtems_name           name,
  rtems_attribute      attribute_set,
  uint32_t             maximum_waiters,
  rtems_id            *id
);

/**
 *  @brief rtems_barrier_ident
 *
 *  This routine implements the rtems_barrier_ident directive.
 *  This directive returns the barrier ID associated with name.
 *  If more than one barrier is named name, then the barrier
 *  to which the ID belongs is arbitrary.  node indicates the
 *  extent of the search for the ID of the barrier named name.
 *  The search can be limited to a particular node or allowed to
 *  encompass all nodes.
 *
 *  @param[in] name is the name of this barrier instance.
 *  @param[out] id will contain the id of this barrier.
 *
 *  @return a status code indicating success or the reason for failure.
 */
rtems_status_code rtems_barrier_ident(
  rtems_name    name,
  rtems_id     *id
);

/**
 *  @brief rtems_barrier_delete
 *
 *  This routine implements the rtems_barrier_delete directive.  The
 *  barrier indicated by @a id is deleted.
 *
 *  @param[in] id indicates the barrier to delete
 *
 *  @return a status code indicating success or the reason for failure.
 */
rtems_status_code rtems_barrier_delete(
  rtems_id   id
);

/**
 *  @brief rtems_barrier_wait
 *
 *  This routine implements the rtems_barrier_wait directive.  It
 *  attempts to wait at the barrier associated with @a id.  The calling task
 *  may block waiting for the barrier with an optional timeout of @a timeout
 *  clock ticks.
 *
 *  @param[in] id indicates the barrier to wait at.
 *  @param[in] timeout is the maximum length of time in ticks the calling
 *             thread is willing to block.
 *
 *  @return a status code indicating success or the reason for failure.
 */
rtems_status_code rtems_barrier_wait(
  rtems_id       id,
  rtems_interval timeout
);

/**
 *  @brief rtems_barrier_release
 *
 *  This routine implements the rtems_barrier_release directive.  It
 *  unblocks all of the threads waiting on the barrier associated with
 *  @a id.  The number of threads unblocked is returned in @a released.
 *
 *
 *  @param[in] id indicates the barrier to wait at.
 *  @param[out] released will contain the number of threads unblocked.
 *
 *  @return a status code indicating success or the reason for failure.
 */
rtems_status_code rtems_barrier_release(
  rtems_id  id,
  uint32_t *released
);

/**
 *  @brief Translate SuperCore Barrier Status Code to RTEMS Status Code
 *
 *  This function returns a RTEMS status code based on the barrier
 *  status code specified.
 *
 *  @param[in] the_status is the SuperCore Barrier status to translate.
 *
 *  @return a status code indicating success or the reason for failure.
 */
rtems_status_code _Barrier_Translate_core_barrier_return_code (
  CORE_barrier_Status  the_status
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/barrier.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/*  end of include file */
