/**
 * @file rtems/posix/spinlock.inl
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_SPINLOCK_H
# error "Never use <rtems/posix/spinlock.inl> directly; include <rtems/posix/spinlock.h> instead."
#endif

#ifndef _RTEMS_POSIX_SPINLOCK_INL
#define _RTEMS_POSIX_SPINLOCK_INL

#include <pthread.h>

/**
 *  @brief _POSIX_Spinlock_Allocate
 *
 *  This function allocates a spinlock control block from
 *  the inactive chain of free spinlock control blocks.
 */
RTEMS_INLINE_ROUTINE POSIX_Spinlock_Control *_POSIX_Spinlock_Allocate( void )
{
  return (POSIX_Spinlock_Control *) 
    _Objects_Allocate( &_POSIX_Spinlock_Information );
}

/**
 *  @brief _POSIX_Spinlock_Free
 *
 *  This routine frees a spinlock control block to the
 *  inactive chain of free spinlock control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Spinlock_Free (
  POSIX_Spinlock_Control *the_spinlock
)
{
  _Objects_Free( &_POSIX_Spinlock_Information, &the_spinlock->Object );
}

/**
 *  @brief _POSIX_Spinlock_Get
 *
 *  This function maps spinlock IDs to spinlock control blocks.
 *  If ID corresponds to a local spinlock, then it returns
 *  the_spinlock control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the spinlock ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_spinlock is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_spinlock is undefined.
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

/**
 *  @brief _POSIX_Spinlock_Is_null
 *
 *  This function returns TRUE if the_spinlock is NULL and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _POSIX_Spinlock_Is_null (
  POSIX_Spinlock_Control *the_spinlock
)
{
  return ( the_spinlock == NULL );
}

#endif
/*  end of include file */
