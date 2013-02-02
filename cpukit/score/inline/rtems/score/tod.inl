/**
 * @file
 *
 * @brief Inlined Routines from the Time of Day Handle
 *
 * This file contains the static inline implementation of the inlined routines
 * from the Time of Day Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TOD_H
# error "Never use <rtems/score/tod.inl> directly; include <rtems/score/tod.h> instead."
#endif

#ifndef _RTEMS_SCORE_TOD_INL
#define _RTEMS_SCORE_TOD_INL

#include <sys/time.h> /* struct timeval */

#include <rtems/score/isr.h>

/**
 * @addtogroup ScoreTOD
 */
/**@{**/

/**
 * This routine deactivates updating of the current time of day.
 */

RTEMS_INLINE_ROUTINE void _TOD_Deactivate( void )
{
  /* XXX do we need something now that we are using timespec for TOD */
}

/**
 * This routine activates updating of the current time of day.
 */

RTEMS_INLINE_ROUTINE void _TOD_Activate( void )
{
  /* XXX do we need something now that we are using timespec for TOD */
}

/**
 * This routine returns a timeval based upon the internal timespec format TOD.
 */

RTEMS_INLINE_ROUTINE void _TOD_Get_timeval(
  struct timeval *time
)
{
  Timestamp_Control  snapshot_as_timestamp;
  Timestamp_Control *snapshot_as_timestamp_ptr;

  snapshot_as_timestamp_ptr =
    _TOD_Get_with_nanoseconds( &snapshot_as_timestamp, &_TOD.now );
  _Timestamp_To_timeval( snapshot_as_timestamp_ptr, time );
}

/** @} */

#endif
/* end of include file */
