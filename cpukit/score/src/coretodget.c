/**
 *  @file
 *
 *  @brief Returns a Current TOD with Nanosecond Granularity
 *  @ingroup ScoreTODConstants
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/todimpl.h>
#include <rtems/score/isrlevel.h>

Timestamp_Control *_TOD_Get_with_nanoseconds(
  Timestamp_Control *snapshot,
  const Timestamp_Control *clock
)
{
  TOD_Control      *tod = &_TOD;
  ISR_Level         level;
  Timestamp_Control offset;
  Timestamp_Control now;
  uint32_t          nanoseconds;

  _TOD_Acquire( tod, level );
    nanoseconds = ( *tod->nanoseconds_since_last_tick )();
    now = *clock;
  _TOD_Release( tod, level );

  _Timestamp_Set( &offset, 0, nanoseconds );
  _Timestamp_Add_to( &now, &offset );

  *snapshot = now;

  return snapshot;
}
