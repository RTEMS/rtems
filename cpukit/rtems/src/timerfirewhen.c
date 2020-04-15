/**
 *  @file
 *
 *  @brief RTEMS Timer Fire When
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

rtems_status_code rtems_timer_fire_when(
  rtems_id                            id,
  rtems_time_of_day                  *wall_time,
  rtems_timer_service_routine_entry   routine,
  void                               *user_data
)
{
  return _Timer_Fire_when(
    id,
    wall_time,
    routine,
    user_data,
    TIMER_TIME_OF_DAY,
    _Timer_Routine_adaptor
  );
}
