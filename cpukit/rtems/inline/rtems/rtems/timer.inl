/**
 * @file rtems/rtems/timer.inl
 *
 * This file contains the static inline implementation of the inlined routines
 * from the Timer Manager.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TIMER_H
# error "Never use <rtems/rtems/timer.inl> directly; include <rtems/rtems/timer.h> instead."
#endif

#ifndef _RTEMS_RTEMS_TIMER_INL
#define _RTEMS_RTEMS_TIMER_INL

/**
 *  @addtogroup ClassicTimer
 *  @{
 */

/**
 *  @brief Timer_Allocate
 *
 *  This function allocates a timer control block from
 *  the inactive chain of free timer control blocks.
 */
RTEMS_INLINE_ROUTINE Timer_Control *_Timer_Allocate( void )
{
  return (Timer_Control *) _Objects_Allocate( &_Timer_Information );
}

/**
 *  @brief Timer_Free
 *
 *  This routine frees a timer control block to the
 *  inactive chain of free timer control blocks.
 */
RTEMS_INLINE_ROUTINE void _Timer_Free (
  Timer_Control *the_timer
)
{
  _Objects_Free( &_Timer_Information, &the_timer->Object );
}

/**
 *  @brief Timer_Get
 *
 *  This function maps timer IDs to timer control blocks.
 *  If ID corresponds to a local timer, then it returns
 *  the timer control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the returned value is undefined.
 */
RTEMS_INLINE_ROUTINE Timer_Control *_Timer_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Timer_Control *)
    _Objects_Get( &_Timer_Information, id, location );
}

/**
 *  @brief Timer_Is_interval_class
 *
 *  This function returns TRUE if the class is that of an INTERVAL
 *  timer, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Timer_Is_interval_class (
  Timer_Classes the_class
)
{
  return (the_class == TIMER_INTERVAL) || (the_class == TIMER_INTERVAL_ON_TASK);
}

/**
 *  @brief Timer_Is_time_of_day_class
 *
 *  This function returns TRUE if the class is that of an INTERVAL
 *  timer, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Timer_Is_timer_of_day_class (
  Timer_Classes the_class
)
{
  return ( the_class == TIMER_TIME_OF_DAY );
}

/**
 *  @brief Timer_Is_dormant_class
 *
 *  This function returns TRUE if the class is that of a DORMANT
 *  timer, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Timer_Is_dormant_class (
  Timer_Classes the_class
)
{
  return ( the_class == TIMER_DORMANT );
}

/**
 *  @brief Timer_Is_null
 *
 *  This function returns TRUE if the_timer is NULL and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Timer_Is_null (
  Timer_Control *the_timer
)
{
  return ( the_timer == NULL );
}

/**@}*/

#endif
/* end of include file */
