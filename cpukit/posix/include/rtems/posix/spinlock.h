/**
 * @file rtems/posix/spinlock.h
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_SPINLOCK_H
#define _RTEMS_POSIX_SPINLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/corespinlock.h>

/**
 *  This type defines the control block used to manage each spinlock.
 */

typedef struct {
  /** This is used to manage a spinlock as an object. */
  Objects_Control          Object;
  /** This is used to implement the spinlock. */
  CORE_spinlock_Control    Spinlock;
}   POSIX_Spinlock_Control;

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

POSIX_EXTERN Objects_Information  _POSIX_Spinlock_Information;

/**
 *  @brief _POSIX_Spinlock_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 *
 *  @param[in] maximum_spinlocks is the total number of spinlocks allowed to
 *             concurrently be active in the system.
 */

void _POSIX_Spinlock_Manager_initialization(void);

/**
 *  @brief _POSIX_Spinlock_Translate_core_spinlock_return_code (
 *
 *  This routine translates SuperCore Spinlock status codes into the
 *  corresponding POSIX ones.
 *
 *
 *  @param[in] the_spinlock_status is the SuperCore status.
 *
 *  @return the corresponding POSIX status
 */
int _POSIX_Spinlock_Translate_core_spinlock_return_code(
  CORE_spinlock_Status  the_spinlock_status
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/posix/spinlock.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
