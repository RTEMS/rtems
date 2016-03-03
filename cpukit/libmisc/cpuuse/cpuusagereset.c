/**
 * @file
 *
 * @brief CPU Usage Reset
 * @ingroup libmisc_cpuuse CPU Usage
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

static void CPU_usage_Per_thread_handler(
  Thread_Control *the_thread
)
{
  ISR_lock_Context lock_context;

  _Scheduler_Acquire( the_thread, &lock_context );
  _Timestamp_Set_to_zero( &the_thread->cpu_time_used );
  _Scheduler_Release( the_thread, &lock_context );
}

/*
 *  rtems_cpu_usage_reset
 */
void rtems_cpu_usage_reset( void )
{
  uint32_t cpu_count;
  uint32_t cpu_index;

  _TOD_Get_uptime( &CPU_usage_Uptime_at_last_reset );

  cpu_count = rtems_get_processor_count();
  for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
    Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

    cpu->cpu_usage_timestamp = CPU_usage_Uptime_at_last_reset;
  }

  rtems_iterate_over_all_threads(CPU_usage_Per_thread_handler);
}
