/**
 * @file rtems/rtems/barrier.inl
 *
 * This file contains the static inlin implementation of the inlined
 * routines from the Barrier Manager.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_BARRIER_H
# error "Never use <rtems/rtems/barrier.inl> directly; include <rtems/rtems/barrier.h> instead."
#endif

#ifndef _RTEMS_RTEMS_BARRIER_INL
#define _RTEMS_RTEMS_BARRIER_INL

/**
 *  @addtogroup ClassicBarrier
 *  @{
 */

/**
 *  @brief _Barrier_Allocate
 *
 *  This function allocates a barrier control block from
 *  the inactive chain of free barrier control blocks.
 */
RTEMS_INLINE_ROUTINE Barrier_Control *_Barrier_Allocate( void )
{
  return (Barrier_Control *) _Objects_Allocate( &_Barrier_Information );
}

/**
 *  @brief _Barrier_Free
 *
 *  This routine frees a barrier control block to the
 *  inactive chain of free barrier control blocks.
 */
RTEMS_INLINE_ROUTINE void _Barrier_Free (
  Barrier_Control *the_barrier
)
{
  _Objects_Free( &_Barrier_Information, &the_barrier->Object );
}

/**
 *  @brief _Barrier_Get
 *
 *  This function maps barrier IDs to barrier control blocks.
 *  If ID corresponds to a local barrier, then it returns
 *  the_barrier control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the barrier ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_barrier is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_barrier is undefined.
 */
RTEMS_INLINE_ROUTINE Barrier_Control *_Barrier_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Barrier_Control *)
    _Objects_Get( &_Barrier_Information, id, location );
}

/**
 *  @brief _Barrier_Is_null
 *
 *  This function returns TRUE if the_barrier is NULL and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Barrier_Is_null (
  Barrier_Control *the_barrier
)
{
  return ( the_barrier == NULL );
}

/**@}*/

#endif
/*  end of include file */
