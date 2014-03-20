/**
 * @file
 * 
 * @brief Constants and Structures Associated with the POSIX Barrier Manager
 *
 * This include file contains all the constants and structures associated
 * with the POSIX Barrier Manager.
 *
 * Directives provided are:
 *
 *  - create a barrier
 *  - delete a barrier
 *  - wait for a barrier
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_BARRIER_H
#define _RTEMS_POSIX_BARRIER_H

#include <rtems/score/object.h>
#include <rtems/score/corebarrier.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIXBarrier POSIX Barriers
 *
 * @ingroup POSIXAPI
 *
 * This encapsulates functionality which implements the RTEMS API
 * Barrier Manager.
 * 
 */
/**@{**/

/**
 * This type defines the control block used to manage each barrier.
 */

typedef struct {
  /** This is used to manage a barrier as an object. */
  Objects_Control          Object;
  /** This is used to implement the barrier. */
  CORE_barrier_Control     Barrier;
}   POSIX_Barrier_Control;

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/*  end of include file */
