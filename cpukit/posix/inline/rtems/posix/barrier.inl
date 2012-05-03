/**
 * @file rtems/posix/barrier.inl
 *
 *  This file contains the static inlin implementation of the inlined
 *  routines from the POSIX Barrier Manager.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_BARRIER_H
# error "Never use <rtems/posix/barrier.inl> directly; include <rtems/posix/barrier.h> instead."
#endif

#ifndef _RTEMS_POSIX_BARRIER_INL
#define _RTEMS_POSIX_BARRIER_INL

#include <pthread.h>

/**
 *  @brief _POSIX_Barrier_Allocate
 *
 *  This function allocates a barrier control block from
 *  the inactive chain of free barrier control blocks.
 */
RTEMS_INLINE_ROUTINE POSIX_Barrier_Control *_POSIX_Barrier_Allocate( void )
{
  return (POSIX_Barrier_Control *) 
    _Objects_Allocate( &_POSIX_Barrier_Information );
}

/**
 *  @brief _POSIX_Barrier_Free
 *
 *  This routine frees a barrier control block to the
 *  inactive chain of free barrier control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Barrier_Free (
  POSIX_Barrier_Control *the_barrier
)
{
  _Objects_Free( &_POSIX_Barrier_Information, &the_barrier->Object );
}

/**
 *  @brief _POSIX_Barrier_Get
 *
 *  This function maps barrier IDs to barrier control blocks.
 *  If ID corresponds to a local barrier, then it returns
 *  the_barrier control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the barrier ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_barrier is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_barrier is undefined.
 */
RTEMS_INLINE_ROUTINE POSIX_Barrier_Control *_POSIX_Barrier_Get (
  pthread_barrier_t *barrier,
  Objects_Locations *location
)
{
  return (POSIX_Barrier_Control *) _Objects_Get(
      &_POSIX_Barrier_Information,
      (Objects_Id) *barrier,
      location
  );
}

/**
 *  @brief _POSIX_Barrier_Is_null
 *
 *  This function returns TRUE if the_barrier is NULL and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _POSIX_Barrier_Is_null (
  POSIX_Barrier_Control *the_barrier
)
{
  return ( the_barrier == NULL );
}

#endif
/*  end of include file */
