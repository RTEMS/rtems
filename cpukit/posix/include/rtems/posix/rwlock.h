/**
 * @file rtems/posix/rwlock.h
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_RWLOCK_H
#define _RTEMS_POSIX_RWLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/corerwlock.h>

/**
 *  This type defines the control block used to manage each RWLock.
 */

typedef struct {
  /** This is used to manage a RWLock as an object. */
  Objects_Control          Object;
  /** This is used to implement the RWLock. */
  CORE_RWLock_Control      RWLock;
}   POSIX_RWLock_Control;

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

POSIX_EXTERN Objects_Information  _POSIX_RWLock_Information;

/**
 *  @brief _POSIX_RWLock_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 *
 *  @param[in] maximum_rwlocks is the total number of RWLocks allowed to
 *             concurrently be active in the system.
 */

void _POSIX_RWLock_Manager_initialization(void);

/**
 *  @brief _POSIX_RWLock_Translate_core_RWLock_return_code (
 *
 *  This routine translates SuperCore RWLock status codes into the
 *  corresponding POSIX ones.
 *
 *
 *  @param[in] the_RWLock_status is the SuperCore status.
 *
 *  @return the corresponding POSIX status
 */
int _POSIX_RWLock_Translate_core_RWLock_return_code(
  CORE_RWLock_Status  the_RWLock_status
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/posix/rwlock.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
