/**
 * @file
 *
 * @brief Initializes the Time of Day Handler
 *
 * @ingroup ScoreTODConstants
 */

/*  COPYRIGHT (c) 1989-2008.
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

static uint32_t _TOD_Nanoseconds_since_tick_default_handler( void )
{
  return 0;
}

void _TOD_Handler_initialization(void)
{
  TOD_Control *tod = &_TOD;

  _ISR_lock_Initialize( &tod->lock );

  _Timestamp_Set( &tod->now, TOD_SECONDS_1970_THROUGH_1988, 0 );

  _Timestamp_Set_to_zero( &tod->uptime );

  tod->nanoseconds_since_last_tick =
    _TOD_Nanoseconds_since_tick_default_handler;

  /* TOD has not been set */
  tod->is_set = false;
}
