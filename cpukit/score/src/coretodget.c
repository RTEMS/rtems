/**
 * @file
 *
 * @brief Returns a Current TOD with Nanosecond Granularity
 * @ingroup ScoreTOD
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
  ISR_lock_Context  lock_context;
  Timestamp_Control offset;
  Timestamp_Control now;
  uint32_t          nanoseconds;

  _TOD_Acquire( tod, &lock_context );
    nanoseconds = ( *tod->nanoseconds_since_last_tick )();
    now = *clock;
  _TOD_Release( tod, &lock_context );

  _Timestamp_Set( &offset, 0, nanoseconds );
  _Timestamp_Add_to( &now, &offset );

  *snapshot = now;

  return snapshot;
}
