/*  timer.inl
 *
 *  This file contains the macro implementation of the inlined routines
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

#define _Timer_Allocate() \
  (Timer_Control *) _Objects_Allocate( &_Timer_Information )

/*PAGE
 *
 *  _Timer_Free
 *
 */

#define _Timer_Free( _the_timer ) \
  _Objects_Free( &_Timer_Information, &(_the_timer)->Object )

/*PAGE
 *
 *  _Timer_Get
 *
 */

#define _Timer_Get( _id, _location ) \
  (Timer_Control *) \
    _Objects_Get( &_Timer_Information, (_id), (_location) )

/*PAGE
 *
 *  _Timer_Is_interval_class
 *
 */

#define _Timer_Is_interval_class( _the_class ) \
  ( (_the_class) == TIMER_INTERVAL )

/*PAGE
 *
 *  _Timer_Is_time_of_day_class
 *
 */

#define _Timer_Is_time_of_day_class( _the_class ) \
  ( (_the_class) == TIMER_TIME_OF_DAY )

/*PAGE
 *
 *  _Timer_Is_dormant_class
 *
 */

#define _Timer_Is_dormant_class( _the_class ) \
  ( (_the_class) == TIMER_DORMANT )

/*PAGE
 *
 *  _Timer_Is_null
 *
 */

#define _Timer_Is_null( _the_timer ) \
  ( (_the_timer) == NULL )

#endif
/* end of include file */
