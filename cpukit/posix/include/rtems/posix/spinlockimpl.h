/**
 * @file
 * 
 * @brief Inlined Routines from the POSIX Spinlock Manager
 *
 * This file contains the static inlin implementation of the inlined
 * routines from the POSIX Spinlock Manager.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_SPINLOCKIMPL_H
#define _RTEMS_POSIX_SPINLOCKIMPL_H

#include <rtems/posix/spinlock.h>
#include <rtems/score/corespinlockimpl.h>
#include <rtems/score/objectimpl.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The following defines the information control block used to manage
 * this class of objects.
 */

POSIX_EXTERN Objects_Information  _POSIX_Spinlock_Information;

/**
 * @brief POSIX spinlock manager initialization.
 *
 * This routine performs the initialization necessary for this manager.
 */

void _POSIX_Spinlock_Manager_initialization(void);

/**
 * @brief Translate core spinlock status code.
 *
 * This routine translates SuperCore Spinlock status codes into the
 * corresponding POSIX ones.
 * 
 * @param[in] the_spinlock_status is the SuperCore status.
 *
 * @return the corresponding POSIX status
 */
int _POSIX_Spinlock_Translate_core_spinlock_return_code(
  CORE_spinlock_Status  the_spinlock_status
);

/**
 * @brief Allocate a spinlock control block.
 *
 * This function allocates a spinlock control block from
 * the inactive chain of free spinlock control blocks.
 */
RTEMS_INLINE_ROUTINE POSIX_Spinlock_Control *_POSIX_Spinlock_Allocate( void )
{
  return (POSIX_Spinlock_Control *) 
    _Objects_Allocate( &_POSIX_Spinlock_Information );
}

/**
 * @brief Free a spinlock control block.
 *
 * This routine frees a spinlock control block to the
 * inactive chain of free spinlock control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Spinlock_Free (
  POSIX_Spinlock_Control *the_spinlock
)
{
  _Objects_Free( &_POSIX_Spinlock_Information, &the_spinlock->Object );
}

/**
 * @brief Get a spinlock control block.
 *
 * This function maps spinlock IDs to spinlock control blocks.
 * If ID corresponds to a local spinlock, then it returns
 * the_spinlock control pointer which maps to ID and location
 * is set to OBJECTS_LOCAL.  if the spinlock ID is global and
 * resides on a remote node, then location is set to OBJECTS_REMOTE,
 * and the_spinlock is undefined.  Otherwise, location is set
 * to OBJECTS_ERROR and the_spinlock is undefined.
 */
RTEMS_INLINE_ROUTINE POSIX_Spinlock_Control *_POSIX_Spinlock_Get (
  pthread_spinlock_t *spinlock,
  Objects_Locations *location
)
{
  return (POSIX_Spinlock_Control *) _Objects_Get(
      &_POSIX_Spinlock_Information,
      (Objects_Id) *spinlock,
      location
  );
}

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
