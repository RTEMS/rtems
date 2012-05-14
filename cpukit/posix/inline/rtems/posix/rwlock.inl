/**
 * @file rtems/posix/rwlock.inl
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_RWLOCK_H
# error "Never use <rtems/posix/rwlock.inl> directly; include <rtems/posix/rwlock.h> instead."
#endif

#ifndef _RTEMS_POSIX_RWLOCK_INL
#define _RTEMS_POSIX_RWLOCK_INL

#include <pthread.h>

/**
 *  @brief _POSIX_RWLock_Allocate
 *
 *  This function allocates a RWLock control block from
 *  the inactive chain of free RWLock control blocks.
 */
RTEMS_INLINE_ROUTINE POSIX_RWLock_Control *_POSIX_RWLock_Allocate( void )
{
  return (POSIX_RWLock_Control *) 
    _Objects_Allocate( &_POSIX_RWLock_Information );
}

/**
 *  @brief _POSIX_RWLock_Free
 *
 *  This routine frees a RWLock control block to the
 *  inactive chain of free RWLock control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_RWLock_Free (
  POSIX_RWLock_Control *the_RWLock
)
{
  _Objects_Free( &_POSIX_RWLock_Information, &the_RWLock->Object );
}

/**
 *  @brief _POSIX_RWLock_Get
 *
 *  This function maps RWLock IDs to RWLock control blocks.
 *  If ID corresponds to a local RWLock, then it returns
 *  the_RWLock control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the RWLock ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_RWLock is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_RWLock is undefined.
 */
RTEMS_INLINE_ROUTINE POSIX_RWLock_Control *_POSIX_RWLock_Get (
  pthread_rwlock_t *RWLock,
  Objects_Locations *location
)
{
  return (POSIX_RWLock_Control *) _Objects_Get(
      &_POSIX_RWLock_Information,
      (Objects_Id) *RWLock,
      location
  );
}

/**
 *  @brief _POSIX_RWLock_Is_null
 *
 *  This function returns TRUE if the_RWLock is NULL and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _POSIX_RWLock_Is_null (
  POSIX_RWLock_Control *the_RWLock
)
{
  return ( the_RWLock == NULL );
}

#endif
/*  end of include file */
