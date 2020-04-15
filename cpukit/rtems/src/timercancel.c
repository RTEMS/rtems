/*
 *  Timer Manager - rtems_timer_cancel directive
 *
 *
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

rtems_status_code rtems_timer_cancel(
  rtems_id id
)
{
  Timer_Control    *the_timer;
  ISR_lock_Context  lock_context;

  the_timer = _Timer_Get( id, &lock_context );
  if ( the_timer != NULL ) {
    Per_CPU_Control *cpu;

    cpu = _Timer_Acquire_critical( the_timer, &lock_context );
    _Timer_Cancel( cpu, the_timer );
    _Timer_Release( cpu, &lock_context );
    return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INVALID_ID;
}
