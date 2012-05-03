/**
 *  @file  score/src/ts64toticks.c
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

#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/timestamp.h>
#include <rtems/score/tod.h>

#if CPU_TIMESTAMP_USE_INT64 == TRUE
uint32_t _Timestamp64_To_ticks(
  const Timestamp64_Control *time
)
{
  uint32_t ticks;

  ticks = *time / rtems_configuration_get_nanoseconds_per_tick();
  if ( ticks )
    return ticks;
  return 1;
}
#endif
