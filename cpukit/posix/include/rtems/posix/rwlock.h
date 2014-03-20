/**
 * @file
 * 
 * @brief Constants and Structures Associated with the POSIX RWLock Manager
 *
 * This include file contains all the constants and structures associated
 * with the POSIX RWLock Manager.
 *
 * Directives provided are:
 *
 *   - create a RWLock
 *   - delete a RWLock
 *   - wait for a RWLock
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_RWLOCK_H
#define _RTEMS_POSIX_RWLOCK_H

#include <rtems/score/object.h>
#include <rtems/score/corerwlock.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_RWLOCK POSIX RWLock Manager
 *
 * @ingroup POSIXAPI
 *
 * @brief Constants and Structures Associated with the POSIX RWLock Manager
 * 
 */
/**@{**/

/**
 * This type defines the control block used to manage each RWLock.
 */

typedef struct {
  /** This is used to manage a RWLock as an object. */
  Objects_Control          Object;
  /** This is used to implement the RWLock. */
  CORE_RWLock_Control      RWLock;
}   POSIX_RWLock_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
