/*  watchdog.inl
 *
 *  This file contains the macro implementation of all inlined routines
 *  in the Watchdog Handler.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __WATCHDOG_inl
#define __WATCHDOG_inl

#include <rtems/score/object.h>

/*PAGE
 *
 *  _Watchdog_Initialize
 *
 */

#define _Watchdog_Initialize( _the_watchdog, _routine, _id, _user_data )  \
  { \
    (_the_watchdog)->state     = WATCHDOG_INACTIVE; \
    (_the_watchdog)->routine   = (_routine); \
    (_the_watchdog)->id        = (_id); \
    (_the_watchdog)->user_data = (_user_data); \
  }

/*PAGE
 *
 *  _Watchdog_Is_active
 *
 */

#define _Watchdog_Is_active( _the_watchdog ) \
  ( (_the_watchdog)->state == WATCHDOG_ACTIVE )

/*PAGE
 *
 *  _Watchdog_Activate
 *
 */

#define _Watchdog_Activate( _the_watchdog ) \
  (_the_watchdog)->state = WATCHDOG_ACTIVE

/*PAGE
 *
 *  _Watchdog_Deactivate
 *
 */

#define _Watchdog_Deactivate( _the_watchdog ) \
  (_the_watchdog)->state = WATCHDOG_REMOVE_IT

/*PAGE
 *
 *  _Watchdog_Tickle_ticks
 *
 */

#define _Watchdog_Tickle_ticks() \
  _Watchdog_Tickle( &_Watchdog_Ticks_chain )

/*PAGE
 *
 *  _Watchdog_Tickle_seconds
 *
 */

#define _Watchdog_Tickle_seconds() \
  _Watchdog_Tickle( &_Watchdog_Seconds_chain )

/*PAGE
 *
 *  _Watchdog_Insert_ticks
 *
 */

#define _Watchdog_Insert_ticks( _the_watchdog, _units ) \
  do { \
    (_the_watchdog)->initial = (_units); \
    _Watchdog_Insert( &_Watchdog_Ticks_chain, (_the_watchdog) ); \
  } while ( 0 )

/*PAGE
 *
 *  _Watchdog_Insert_seconds
 *
 */

#define _Watchdog_Insert_seconds( _the_watchdog, _units ) \
  do { \
    (_the_watchdog)->initial = (_units); \
    _Watchdog_Insert( &_Watchdog_Seconds_chain, (_the_watchdog) ); \
  } while ( 0 )

/*PAGE
 *
 *  _Watchdog_Adjust_seconds
 *
 */

#define _Watchdog_Adjust_seconds( _direction, _units ) \
  _Watchdog_Adjust( &_Watchdog_Seconds_chain, (_direction), (_units) )

/*PAGE
 *
 *  _Watchdog_Adjust_ticks
 *
 */

#define _Watchdog_Adjust_ticks( _direction, _units ) \
  _Watchdog_Adjust( &_Watchdog_Ticks_chain, (_direction), (_units) )

/*PAGE
 *
 *  _Watchdog_Reset
 *
 */

#define _Watchdog_Reset( _the_watchdog ) \
   { \
     (void) _Watchdog_Remove( (_the_watchdog) ); \
     _Watchdog_Insert( &_Watchdog_Ticks_chain, (_the_watchdog) ); \
   }

/*PAGE
 *
 *  _Watchdog_Next
 *
 */

#define _Watchdog_Next( _watchdog ) \
  ((Watchdog_Control *) (_watchdog)->Node.next)

/*PAGE
 *
 *  _Watchdog_Previous
 *
 */

#define _Watchdog_Previous( _watchdog ) \
  ((Watchdog_Control *) (_watchdog)->Node.previous)

/*PAGE
 *
 *  _Watchdog_First
 *
 */

#define _Watchdog_First( _header ) \
  ((Watchdog_Control *) (_header)->first)

/*PAGE
 *
 *  _Watchdog_Last
 *
 */

#define _Watchdog_Last( _header ) \
  ((Watchdog_Control *) (_header)->last)

#endif
/* end of include file */
