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
#include <rtems/score/thread.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>

static void CPU_usage_Per_thread_handler(
  Thread_Control *the_thread
)
{
  #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
    _Timestamp_Set_to_zero( &the_thread->cpu_time_used );
  #else
    the_thread->cpu_time_used = 0;
  #endif
}

/*
 *  rtems_cpu_usage_reset
 */
void rtems_cpu_usage_reset( void )
{
  #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
    uint32_t cpu_count;
    uint32_t cpu_index;

    _TOD_Get_uptime( &CPU_usage_Uptime_at_last_reset );

    cpu_count = rtems_get_processor_count();
    for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
      Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

      cpu->time_of_last_context_switch = CPU_usage_Uptime_at_last_reset;
    }
  #else
    CPU_usage_Ticks_at_last_reset = _Watchdog_Ticks_since_boot;
  #endif

  rtems_iterate_over_all_threads(CPU_usage_Per_thread_handler);
}
