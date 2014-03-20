/**
 * @file
 * 
 * @brief POSIX Spinlock Support
 *
 * This include file contains all the constants and structures associated
 * with the POSIX Spinlock Manager.
 *
 *  Directives provided are:
 *
 *   - create a spinlock
 *   - delete a spinlock
 *   - wait for a spinlock
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_SPINLOCK_H
#define _RTEMS_POSIX_SPINLOCK_H

#include <rtems/score/object.h>
#include <rtems/score/corespinlock.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_SPINLOCK POSIX Spinlock Support
 *
 * @ingroup POSIXAPI
 *
 * @brief Constants and Structures Associated with the POSIX Spinlock Manager
 * 
 */
/**@{**/

/**
 * This type defines the control block used to manage each spinlock.
 */

typedef struct {
  /** This is used to manage a spinlock as an object. */
  Objects_Control          Object;
  /** This is used to implement the spinlock. */
  CORE_spinlock_Control    Spinlock;
}   POSIX_Spinlock_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
