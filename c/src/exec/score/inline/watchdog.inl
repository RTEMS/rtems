/*  watchdog.inl
 *
 *  This file contains the static inline implementation of all inlined
 *  routines in the Watchdog Handler.
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

#ifndef __WATCHDOG_inl
#define __WATCHDOG_inl

/*PAGE
 *
 *  _Watchdog_Initialize
 *
 */

STATIC INLINE void _Watchdog_Initialize(
  Watchdog_Control *the_watchdog,
  rtems_timer_service_routine_entry  routine,
  Objects_Id        id,
  void             *user_data
)
{
  the_watchdog->state     = WATCHDOG_INACTIVE;
  the_watchdog->routine   = routine;
  the_watchdog->id        = id;
  the_watchdog->user_data = user_data;
}

/*PAGE
 *
 *  _Watchdog_Is_active
 *
 */

STATIC INLINE boolean _Watchdog_Is_active(
  Watchdog_Control *the_watchdog
)
{

  return ( the_watchdog->state == WATCHDOG_ACTIVE );

}

/*PAGE
 *
 *  _Watchdog_Activate
 *
 */

STATIC INLINE void _Watchdog_Activate(
  Watchdog_Control *the_watchdog
)
{

  the_watchdog->state = WATCHDOG_ACTIVE;

}

/*PAGE
 *
 *  _Watchdog_Deactivate
 *
 */

STATIC INLINE void _Watchdog_Deactivate(
  Watchdog_Control *the_watchdog
)
{

  the_watchdog->state = WATCHDOG_REMOVE_IT;

}

/*PAGE
 *
 *  _Watchdog_Tickle_ticks
 *
 */

STATIC INLINE void _Watchdog_Tickle_ticks( void )
{

  _Watchdog_Tickle( &_Watchdog_Ticks_chain );

}

/*PAGE
 *
 *  _Watchdog_Tickle_seconds
 *
 */

STATIC INLINE void _Watchdog_Tickle_seconds( void )
{

  _Watchdog_Tickle( &_Watchdog_Seconds_chain );

}

/*PAGE
 *
 *  _Watchdog_Insert_ticks
 *
 */

STATIC INLINE void _Watchdog_Insert_ticks(
  Watchdog_Control      *the_watchdog,
  rtems_interval      units,
  Watchdog_Insert_modes  insert_mode
)
{

  the_watchdog->initial = units;

  _Watchdog_Insert( &_Watchdog_Ticks_chain, the_watchdog, insert_mode );

}

/*PAGE
 *
 *  _Watchdog_Insert_seconds
 *
 */

STATIC INLINE void _Watchdog_Insert_seconds(
  Watchdog_Control      *the_watchdog,
  rtems_interval      units,
  Watchdog_Insert_modes  insert_mode
)
{

  the_watchdog->initial = units;

  _Watchdog_Insert( &_Watchdog_Seconds_chain, the_watchdog, insert_mode );

}

/*PAGE
 *
 *  _Watchdog_Adjust_seconds
 *
 */

STATIC INLINE void _Watchdog_Adjust_seconds(
  Watchdog_Adjust_directions direction,
  rtems_interval          units
)
{

  _Watchdog_Adjust( &_Watchdog_Seconds_chain, direction, units );

}

/*PAGE
 *
 *  _Watchdog_Adjust_ticks
 *
 */

STATIC INLINE void _Watchdog_Adjust_ticks(
  Watchdog_Adjust_directions direction,
  rtems_interval          units
)
{

  _Watchdog_Adjust( &_Watchdog_Ticks_chain, direction, units );

}

/*PAGE
 *
 *  _Watchdog_Reset
 *
 */

STATIC INLINE void _Watchdog_Reset(
  Watchdog_Control *the_watchdog
)
{

  (void) _Watchdog_Remove( the_watchdog );

  _Watchdog_Insert(
    &_Watchdog_Ticks_chain,
    the_watchdog,
    WATCHDOG_ACTIVATE_NOW
  );

}

/*PAGE
 *
 *  _Watchdog_Next
 *
 */

STATIC INLINE Watchdog_Control *_Watchdog_Next(
  Watchdog_Control *the_watchdog
)
{

  return ( (Watchdog_Control *) the_watchdog->Node.next );

}

/*PAGE
 *
 *  _Watchdog_Previous
 *
 */

STATIC INLINE Watchdog_Control *_Watchdog_Previous(
  Watchdog_Control *the_watchdog
)
{

  return ( (Watchdog_Control *) the_watchdog->Node.previous );

}

/*PAGE
 *
 *  _Watchdog_First
 *
 */

STATIC INLINE Watchdog_Control *_Watchdog_First(
  Chain_Control *header
)
{

  return ( (Watchdog_Control *) header->first );

}

/*PAGE
 *
 *  _Watchdog_Last
 *
 */
STATIC INLINE Watchdog_Control *_Watchdog_Last(
  Chain_Control *header
)
{

  return ( (Watchdog_Control *) header->last );

}

#endif
/* end of include file */
