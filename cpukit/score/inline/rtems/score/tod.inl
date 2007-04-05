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
 *  This routine deactivates updating of the current time of day.
 */

RTEMS_INLINE_ROUTINE void _TOD_Deactivate( void )
{
  /* XXX do we need something now that we are using timespec for TOD */
}

/**
 *  This routine activates updating of the current time of day.
 */

RTEMS_INLINE_ROUTINE void _TOD_Activate()
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
