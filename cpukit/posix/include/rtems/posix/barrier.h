/**
 * @file rtems/posix/barrier.h
 *
 *  This include file contains all the constants and structures associated
 *  with the POSIX Barrier Manager.
 *
 *  Directives provided are:
 *
 *   - create a barrier
 *   - delete a barrier
 *   - wait for a barrier
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_BARRIER_H
#define _RTEMS_POSIX_BARRIER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/corebarrier.h>

/**
 *  This type defines the control block used to manage each barrier.
 */

typedef struct {
  /** This is used to manage a barrier as an object. */
  Objects_Control          Object;
  /** This is used to implement the barrier. */
  CORE_barrier_Control     Barrier;
}   POSIX_Barrier_Control;

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

POSIX_EXTERN Objects_Information  _POSIX_Barrier_Information;

/**
 *  @brief _POSIX_Barrier_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 *
 *  @param[in] maximum_barriers is the total number of barriers allowed to
 *             concurrently be active in the system.
 */

void _POSIX_Barrier_Manager_initialization(void);

/**
 *  @brief _POSIX_Barrier_Translate_core_barrier_return_code (
 *
 *  This routine translates SuperCore Barrier status codes into the
 *  corresponding POSIX ones.
 *
 *
 *  @param[in] the_barrier_status is the SuperCore status.
 *
 *  @return the corresponding POSIX status
 */
int _POSIX_Barrier_Translate_core_barrier_return_code(
  CORE_barrier_Status  the_barrier_status
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/posix/barrier.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
