/*  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/tod.h>

uint32_t TOD_MILLISECONDS_TO_TICKS(
  uint32_t milliseconds
)
{
  uint32_t ticks;
  uint32_t milliseconds_per_tick;

  /**
   *  We should ensure the ticks not be truncated by integer division.  We
   *  need to have it be greater than or equal to the requested time.  It
   *  should not be shorter.
   */
  milliseconds_per_tick = rtems_configuration_get_milliseconds_per_tick();
  ticks                 = milliseconds / milliseconds_per_tick;
  if ( (milliseconds % milliseconds_per_tick) != 0 )
    ticks += 1;

  return ticks;
}
