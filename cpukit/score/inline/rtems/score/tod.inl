/*  tod.inl
 *
 *  This file contains the static inline implementation of the inlined routines
 *  from the Time of Day Handler.
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

#ifndef __TIME_OF_DAY_inl
#define __TIME_OF_DAY_inl

/*PAGE
 *
 *  _TOD_Tickle_ticks
 *
 *  DESCRIPTION:
 *
 *  This routine increments the ticks field of the current time of
 *  day at each clock tick.
 */

RTEMS_INLINE_ROUTINE void _TOD_Tickle_ticks( void )
{
  _TOD_Current.ticks += 1;
  _Watchdog_Ticks_since_boot += 1;
}

/*PAGE
 *
 *  _TOD_Deactivate
 *
 *  DESCRIPTION:
 *
 *  This routine deactivates updating of the current time of day.
 */

RTEMS_INLINE_ROUTINE void _TOD_Deactivate( void )
{
  _Watchdog_Remove( &_TOD_Seconds_watchdog );
}

/*PAGE
 *
 *  _TOD_Activate
 *
 *  DESCRIPTION:
 *
 *  This routine activates updating of the current time of day.
 */

RTEMS_INLINE_ROUTINE void _TOD_Activate(
  Watchdog_Interval ticks
)
{
  _Watchdog_Insert_ticks( &_TOD_Seconds_watchdog, ticks );
}

#endif
/* end of include file */
