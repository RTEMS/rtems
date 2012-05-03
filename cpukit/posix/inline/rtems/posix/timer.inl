/**
 * @file rtems/posix/timer.inl
 *
 * This file contains the static inline implementation of the inlined routines
 * from the POSIX Timer Manager.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_TIMER_H
# error "Never use <rtems/posix/timer.inl> directly; include <rtems/posix/timer.h> instead."
#endif

#ifndef _RTEMS_POSIX_TIMER_INL
#define _RTEMS_POSIX_TIMER_INL

/*
 *  _POSIX_Timer_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a timer control block from
 *  the inactive chain of free timer control blocks.
 */

RTEMS_INLINE_ROUTINE POSIX_Timer_Control *_POSIX_Timer_Allocate( void )
{
  return (POSIX_Timer_Control *) _Objects_Allocate( &_POSIX_Timer_Information );
}

/*
 *  _POSIX_Timer_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a timer control block to the
 *  inactive chain of free timer control blocks.
 */

RTEMS_INLINE_ROUTINE void _POSIX_Timer_Free (
  POSIX_Timer_Control *the_timer
)
{
  _Objects_Free( &_POSIX_Timer_Information, &the_timer->Object );
}

/*
 *  _POSIX_Timer_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps timer IDs to timer control blocks.
 *  If ID corresponds to a local timer, then it returns
 *  the timer control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the returned value is undefined.
 */

RTEMS_INLINE_ROUTINE POSIX_Timer_Control *_POSIX_Timer_Get (
  timer_t            id,
  Objects_Locations *location
)
{
  return (POSIX_Timer_Control *)
    _Objects_Get( &_POSIX_Timer_Information, (Objects_Id) id, location );
}

/*
 *  _POSIX_Timer_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_timer is NULL and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE bool _POSIX_Timer_Is_null (
  POSIX_Timer_Control *the_timer
)
{
  return (the_timer == NULL);
}

#endif
/* end of include file */
