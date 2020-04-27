/**
 * @file
 *
 * @ingroup libmisc_cpuuse CPU Usage
 *
 * @brief CPU Usage Reset
 */

/*
 *  COPYRIGHT (c) 1989-2009
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/cpuuse.h>
#include <rtems/score/percpu.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/watchdogimpl.h>

#include "cpuuseimpl.h"

static bool CPU_usage_Per_thread_handler(
  Thread_Control *the_thread,
  void           *arg
)
{
  const Scheduler_Control *scheduler;
  ISR_lock_Context         state_lock_context;
  ISR_lock_Context         scheduler_lock_context;

  _Thread_State_acquire( the_thread, &state_lock_context );
  scheduler = _Thread_Scheduler_get_home( the_thread );
  _Scheduler_Acquire_critical( scheduler, &scheduler_lock_context );

  _Timestamp_Set_to_zero( &the_thread->cpu_time_used );

  _Scheduler_Release_critical( scheduler, &scheduler_lock_context );
  _Thread_State_release( the_thread, &state_lock_context );
  return false;
}

/*
 *  rtems_cpu_usage_reset
 */
void rtems_cpu_usage_reset( void )
{
  uint32_t cpu_count;
  uint32_t cpu_index;

  _TOD_Get_uptime( &CPU_usage_Uptime_at_last_reset );

  cpu_count = rtems_scheduler_get_processor_maximum();
  for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
    Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

    cpu->cpu_usage_timestamp = CPU_usage_Uptime_at_last_reset;
  }

  rtems_task_iterate(CPU_usage_Per_thread_handler, NULL);
}
