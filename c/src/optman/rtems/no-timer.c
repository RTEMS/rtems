/*
 *  Timer Manager
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

#include <rtems/rtems/types.h>
#include <rtems/rtems/timer.h>

void _Timer_Manager_initialization(
  unsigned32 maximum_timers
)
{
}

rtems_status_code rtems_timer_create(
  rtems_name    name,
  Objects_Id   *id
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_timer_ident(
  rtems_name    name,
  Objects_Id   *id
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_timer_cancel(
  Objects_Id id
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_timer_delete(
  Objects_Id id
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_timer_fire_after(
  Objects_Id                         id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_timer_fire_when(
  Objects_Id                         id,
  rtems_time_of_day                 *wall_time,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_timer_reset(
  Objects_Id id
)
{
  return RTEMS_NOT_CONFIGURED;
}
