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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_RWLOCK_H
#define _RTEMS_POSIX_RWLOCK_H

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

#include <rtems/score/object.h>
#include <rtems/score/corerwlock.h>

/**
 * This type defines the control block used to manage each RWLock.
 */

typedef struct {
  /** This is used to manage a RWLock as an object. */
  Objects_Control          Object;
  /** This is used to implement the RWLock. */
  CORE_RWLock_Control      RWLock;
}   POSIX_RWLock_Control;

/**
 * The following defines the information control block used to manage
 * this class of objects.
 */

POSIX_EXTERN Objects_Information  _POSIX_RWLock_Information;

/**
 * @brief POSIX RWLock manager initialization.
 *
 * This routine performs the initialization necessary for this manager.
 */

void _POSIX_RWLock_Manager_initialization(void);

/**
 * @brief POSIX translate core RWLock return code.
 *
 * This routine translates SuperCore RWLock status codes into the
 * corresponding POSIX ones.
 *
 *
 * @param[in] the_RWLock_status is the SuperCore status.
 *
 * @return the corresponding POSIX status
 * @retval 0 The status indicates that the operation completed successfully.
 * @retval EINVAL The status indicates that the thread was blocked waiting for
 * an operation to complete and the RWLock was deleted.
 * @retval EBUSY This status indicates that the RWLock was not 
 * immediately available.
 * @retval ETIMEDOUT This status indicates that the calling task was 
 * willing to block but the operation was unable to complete within
 * the time allotted because the resource never became available.
 */
int _POSIX_RWLock_Translate_core_RWLock_return_code(
  CORE_RWLock_Status  the_RWLock_status
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/posix/rwlock.inl>
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
