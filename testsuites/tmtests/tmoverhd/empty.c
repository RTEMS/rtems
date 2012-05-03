/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_timer_service_routine Timer_handler(
  rtems_id argument
)
{
}

rtems_asr Isr_handler(
  rtems_signal_set signals
)
{
}

rtems_asr Asr_handler(
  rtems_signal_set signals
)
{
}

rtems_task task_func(void)
{
}

void null_func(void)
{
}

rtems_status_code Empty_directive(void)
{
  return( RTEMS_SUCCESSFUL );
}
