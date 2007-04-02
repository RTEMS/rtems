/** 
 *  @file  rtems/score/tod.inl
 *
 *  This file contains the static inline implementation of the inlined routines
 *  from the Time of Day Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_TOD_INL
#define _RTEMS_SCORE_TOD_INL

#include <rtems/score/isr.h>

/**
 *  @addtogroup ScoreTOD 
 *  @{
 */

/**
 *
 *  This routines adds two timespecs.  The second argument is added
 *  to the first.
 */

RTEMS_INLINE_ROUTINE uint32_t _TOD_Add_timespec(
  struct timespec *time,
  struct timespec *add
)
{
  uint32_t seconds = 0;

  
  /* Add the basics */
  time->tv_sec += add->tv_sec;
  time->tv_nsec += add->tv_nsec;

  /* Now adjust it so nanoseconds is in range */
  while ( time->tv_nsec >= TOD_NANOSECONDS_PER_SECOND ) {
    time->tv_nsec -= TOD_NANOSECONDS_PER_SECOND;
    time->tv_sec++;
    seconds++;
  }

  return seconds;
}

/**
 *  This routine increments the ticks field of the current time of
 *  day at each clock tick.
 */

RTEMS_INLINE_ROUTINE void _TOD_Tickle_ticks( void )
{
  struct timespec tick;
  uint32_t        seconds;

  /* Convert the tick quantum to a timespec */
  tick.tv_nsec = _TOD_Microseconds_per_tick * 1000;
  tick.tv_sec  = 0;

  /* Update the counter of ticks since boot */
  _Watchdog_Ticks_since_boot += 1;

  /* Update the timespec format uptime */
  (void) _TOD_Add_timespec( &_TOD_Uptime, &tick );
  /* we do not care how much the uptime changed */

  /* Update the timespec format TOD */
  seconds = _TOD_Add_timespec( &_TOD_Now, &tick );
  while ( seconds ) {
    _Watchdog_Tickle_seconds();
    seconds--;
  }
}

/**
 *  This routine deactivates updating of the current time of day.
 */

RTEMS_INLINE_ROUTINE void _TOD_Deactivate( void )
{
  /* XXX do we need something now that we are using timespec for TOD */
}

/**
 *  This routine activates updating of the current time of day.
 */

RTEMS_INLINE_ROUTINE void _TOD_Activate(
  Watchdog_Interval ticks
)
{
  /* XXX do we need something now that we are using timespec for TOD */
}

/**
 *  This routine returns a timeval based upon the internal timespec format TOD.
 */

RTEMS_INLINE_ROUTINE void _TOD_Get_timeval(
  struct timeval *time
)
{
  ISR_Level level;
  struct timespec now;

  _ISR_Disable(level);
    _TOD_Get( &now );
  _ISR_Enable(level);

  time->tv_sec  = now.tv_sec;
  time->tv_usec = now.tv_nsec / TOD_NANOSECONDS_PER_MICROSECOND;
}

/**@}*/

#endif
/* end of include file */
