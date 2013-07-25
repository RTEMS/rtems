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

#include <rtems/score/tod.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/watchdogimpl.h>

Timestamp_Control *_TOD_Get_with_nanoseconds(
  Timestamp_Control *snapshot,
  const Timestamp_Control *clock
)
{
  ISR_Level         level;
  Timestamp_Control offset;
  Timestamp_Control now;
  uint32_t          nanoseconds;

  _ISR_Disable( level );
    nanoseconds = (*_Watchdog_Nanoseconds_since_tick_handler)();
    now = *clock;
  _ISR_Enable( level );

  _Timestamp_Set( &offset, 0, nanoseconds );
  _Timestamp_Add_to( &now, &offset );

  *snapshot = now;

  return snapshot;
}
