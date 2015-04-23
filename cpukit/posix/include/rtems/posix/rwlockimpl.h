/**
 * @file
 * 
 * @brief Inlined Routines from the POSIX RWLock Manager
 *
 * This file contains the static inlin implementation of the inlined
 * routines from the POSIX RWLock Manager.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_RWLOCKIMPL_H
#define _RTEMS_POSIX_RWLOCKIMPL_H

#include <rtems/posix/rwlock.h>
#include <rtems/score/corerwlockimpl.h>
#include <rtems/score/objectimpl.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * @brief Allocate a RWLock control block.
 *
 * This function allocates a RWLock control block from
 * the inactive chain of free RWLock control blocks.
 */
RTEMS_INLINE_ROUTINE POSIX_RWLock_Control *_POSIX_RWLock_Allocate( void )
{
  return (POSIX_RWLock_Control *) 
    _Objects_Allocate( &_POSIX_RWLock_Information );
}

/**
 * @brief Free a RWLock control block.
 *
 * This routine frees a RWLock control block to the
 * inactive chain of free RWLock control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_RWLock_Free (
  POSIX_RWLock_Control *the_RWLock
)
{
  _CORE_RWLock_Destroy( &the_RWLock->RWLock );
  _Objects_Free( &_POSIX_RWLock_Information, &the_RWLock->Object );
}

POSIX_RWLock_Control *_POSIX_RWLock_Get(
  pthread_rwlock_t  *rwlock,
  Objects_Locations *location
);

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
