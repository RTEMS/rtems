/*  watchdog.inl
 *
 *  This file contains the static inline implementation of all inlined
 *  routines in the Watchdog Handler.
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

/*PAGE
 *
 *  _Watchdog_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the specified watchdog.  The watchdog is
 *  made inactive, the watchdog id and handler routine are set to the
 *  specified values.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Initialize(
  Watchdog_Control               *the_watchdog,
  Watchdog_Service_routine_entry  routine,
  Objects_Id                      id,
  void                           *user_data
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
 *  DESCRIPTION:
 *
 *  This routine returns TRUE if the watchdog timer is in the ACTIVE
 *  state, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Watchdog_Is_active(
  Watchdog_Control *the_watchdog
)
{

  return ( the_watchdog->state == WATCHDOG_ACTIVE );

}

/*PAGE
 *
 *  _Watchdog_Activate
 *
 *  DESCRIPTION:
 *
 *  This routine activates THE_WATCHDOG timer which is already
 *  on a watchdog chain.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Activate(
  Watchdog_Control *the_watchdog
)
{

  the_watchdog->state = WATCHDOG_ACTIVE;

}

/*PAGE
 *
 *  _Watchdog_Deactivate
 *
 *  DESCRIPTION:
 *
 *  This routine deactivates THE_WATCHDOG timer which will remain
 *  on a watchdog chain.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Deactivate(
  Watchdog_Control *the_watchdog
)
{

  the_watchdog->state = WATCHDOG_REMOVE_IT;

}

/*PAGE
 *
 *  _Watchdog_Tickle_ticks
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked at each clock tick to update the ticks
 *  watchdog chain.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Tickle_ticks( void )
{

  _Watchdog_Tickle( &_Watchdog_Ticks_chain );

}

/*PAGE
 *
 *  _Watchdog_Tickle_seconds
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked at each clock tick to update the seconds
 *  watchdog chain.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Tickle_seconds( void )
{

  _Watchdog_Tickle( &_Watchdog_Seconds_chain );

}

/*PAGE
 *
 *  _Watchdog_Insert_ticks
 *
 *  DESCRIPTION:
 *
 *  This routine inserts THE_WATCHDOG into the ticks watchdog chain
 *  for a time of UNITS ticks.  The INSERT_MODE indicates whether
 *  THE_WATCHDOG is to be activated automatically or later, explicitly
 *  by the caller.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Insert_ticks(
  Watchdog_Control      *the_watchdog,
  Watchdog_Interval      units
)
{

  the_watchdog->initial = units;

  _Watchdog_Insert( &_Watchdog_Ticks_chain, the_watchdog );

}

/*PAGE
 *
 *  _Watchdog_Insert_seconds
 *
 *  DESCRIPTION:
 *
 *  This routine inserts THE_WATCHDOG into the seconds watchdog chain
 *  for a time of UNITS seconds.  The INSERT_MODE indicates whether
 *  THE_WATCHDOG is to be activated automatically or later, explicitly
 *  by the caller.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Insert_seconds(
  Watchdog_Control      *the_watchdog,
  Watchdog_Interval      units
)
{

  the_watchdog->initial = units;

  _Watchdog_Insert( &_Watchdog_Seconds_chain, the_watchdog );

}

/*PAGE
 *
 *  _Watchdog_Adjust_seconds
 *
 *  DESCRIPTION:
 *
 *  This routine adjusts the seconds watchdog chain in the forward
 *  or backward DIRECTION for UNITS seconds.  This is invoked when the
 *  current time of day is changed.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Adjust_seconds(
  Watchdog_Adjust_directions direction,
  Watchdog_Interval          units
)
{

  _Watchdog_Adjust( &_Watchdog_Seconds_chain, direction, units );

}

/*PAGE
 *
 *  _Watchdog_Adjust_ticks
 *
 *  DESCRIPTION:
 *
 *  This routine adjusts the ticks watchdog chain in the forward
 *  or backward DIRECTION for UNITS ticks.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Adjust_ticks(
  Watchdog_Adjust_directions direction,
  Watchdog_Interval          units
)
{

  _Watchdog_Adjust( &_Watchdog_Ticks_chain, direction, units );

}

/*PAGE
 *
 *  _Watchdog_Reset
 *
 *  DESCRIPTION:
 *
 *  This routine resets THE_WATCHDOG timer to its state at INSERT
 *  time.  This routine is valid only on interval watchdog timers
 *  and is used to make an interval watchdog timer fire "every" so
 *  many ticks.
 */

RTEMS_INLINE_ROUTINE void _Watchdog_Reset(
  Watchdog_Control *the_watchdog
)
{

  (void) _Watchdog_Remove( the_watchdog );

  _Watchdog_Insert( &_Watchdog_Ticks_chain, the_watchdog );

}

/*PAGE
 *
 *  _Watchdog_Next
 *
 *  DESCRIPTION:
 *
 *  This routine returns a pointer to the watchdog timer following
 *  THE_WATCHDOG on the watchdog chain.
 */

RTEMS_INLINE_ROUTINE Watchdog_Control *_Watchdog_Next(
  Watchdog_Control *the_watchdog
)
{

  return ( (Watchdog_Control *) the_watchdog->Node.next );

}

/*PAGE
 *
 *  _Watchdog_Previous
 *
 *  DESCRIPTION:
 *
 *  This routine returns a pointer to the watchdog timer preceding
 *  THE_WATCHDOG on the watchdog chain.
 */

RTEMS_INLINE_ROUTINE Watchdog_Control *_Watchdog_Previous(
  Watchdog_Control *the_watchdog
)
{

  return ( (Watchdog_Control *) the_watchdog->Node.previous );

}

/*PAGE
 *
 *  _Watchdog_First
 *
 *  DESCRIPTION:
 *
 *  This routine returns a pointer to the first watchdog timer
 *  on the watchdog chain HEADER.
 */

RTEMS_INLINE_ROUTINE Watchdog_Control *_Watchdog_First(
  Chain_Control *header
)
{

  return ( (Watchdog_Control *) header->first );

}

/*PAGE
 *
 *  _Watchdog_Last
 *
 *  DESCRIPTION:
 *
 *  This routine returns a pointer to the last watchdog timer
 *  on the watchdog chain HEADER.
 */

RTEMS_INLINE_ROUTINE Watchdog_Control *_Watchdog_Last(
  Chain_Control *header
)
{

  return ( (Watchdog_Control *) header->last );

}

#endif
/* end of include file */
