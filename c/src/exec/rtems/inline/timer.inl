/*  timer.inl
 *
 *  This file contains the static inline implementation of the inlined routines
 *  from the Timer Manager.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __TIMER_inl
#define __TIMER_inl

/*PAGE
 *
 *  _Timer_Allocate
 *
 */

STATIC INLINE Timer_Control *_Timer_Allocate( void )
{
  return (Timer_Control *) _Objects_Allocate( &_Timer_Information );
}

/*PAGE
 *
 *  _Timer_Free
 *
 */

STATIC INLINE void _Timer_Free (
  Timer_Control *the_timer
)
{
  _Objects_Free( &_Timer_Information, &the_timer->Object );
}

/*PAGE
 *
 *  _Timer_Get
 *
 */

STATIC INLINE Timer_Control *_Timer_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Timer_Control *)
    _Objects_Get( &_Timer_Information, id, location );
}

/*PAGE
 *
 *  _Timer_Is_interval_class
 *
 */

STATIC INLINE boolean _Timer_Is_interval_class (
  Timer_Classes the_class
)
{
  return ( the_class == TIMER_INTERVAL );
}

/*PAGE
 *
 *  _Timer_Is_time_of_day_class
 *
 */

STATIC INLINE boolean _Timer_Is_timer_of_day_class (
  Timer_Classes the_class
)
{
  return ( the_class == TIMER_TIME_OF_DAY );
}

/*PAGE
 *
 *  _Timer_Is_dormant_class
 *
 */

STATIC INLINE boolean _Timer_Is_dormant_class (
  Timer_Classes the_class
)
{
  return ( the_class == TIMER_DORMANT );
}

/*PAGE
 *
 *  _Timer_Is_null
 *
 */

STATIC INLINE boolean _Timer_Is_null (
  Timer_Control *the_timer
)
{
  return ( the_timer == NULL );
}

#endif
/* end of include file */
