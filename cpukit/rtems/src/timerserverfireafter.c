/**
 *  @file
 *
 *  @brief RTEMS Timer Server Fire After
 *  @ingroup ClassicTimer
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/timerimpl.h>

rtems_status_code rtems_timer_server_fire_after(
  rtems_id                           id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
)
{
  Timer_server_Control *timer_server;

  timer_server = _Timer_server;

  if ( !timer_server )
    return RTEMS_INCORRECT_STATE;

  return _Timer_Fire_after(
    id,
    ticks,
    routine,
    user_data,
    TIMER_INTERVAL_ON_TASK,
    _Timer_server_Routine_adaptor
  );
}
