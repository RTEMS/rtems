/**
 *  @file
 *
 *  @brief RTEMS Get Timer Information
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/timerimpl.h>

rtems_status_code rtems_timer_get_information(
  rtems_id                 id,
  rtems_timer_information *the_info
)
{
  Timer_Control    *the_timer;
  ISR_lock_Context  lock_context;

  if ( !the_info )
    return RTEMS_INVALID_ADDRESS;

  the_timer = _Timer_Get( id, &lock_context );
  if ( the_timer != NULL ) {
    Per_CPU_Control *cpu;

    cpu = _Timer_Acquire_critical( the_timer, &lock_context );
    the_info->the_class  = the_timer->the_class;
    the_info->initial    = the_timer->initial;
    the_info->start_time = the_timer->start_time;
    the_info->stop_time  = the_timer->stop_time;
    _Timer_Release( cpu, &lock_context );
    return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INVALID_ID;
}
