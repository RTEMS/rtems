/*
 *  CPU Usage Reporter
 *
 *  COPYRIGHT (c) 1989-2007
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/score/timestamp.h>

#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

#include <rtems/cpuuse.h>

static void CPU_usage_Per_thread_handler(
  Thread_Control *the_thread
)
{
  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    _Timestamp_Set_to_zero( &the_thread->cpu_time_used );
  #else
    the_thread->cpu_time_used = 0;
  #endif
}


/*
 * External data that is shared by cpu usage code but not declared in .h files.
 */
#ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
  extern Timestamp_Control CPU_usage_Uptime_at_last_reset;
#else
  extern uint32_t   CPU_usage_Ticks_at_last_reset;
#endif

/*
 *  rtems_cpu_usage_reset
 */
void rtems_cpu_usage_reset( void )
{
  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    _TOD_Get_uptime( &CPU_usage_Uptime_at_last_reset );
    _Thread_Time_of_last_context_switch = CPU_usage_Uptime_at_last_reset;
  #else
    CPU_usage_Ticks_at_last_reset = _Watchdog_Ticks_since_boot;
  #endif

  rtems_iterate_over_all_threads(CPU_usage_Per_thread_handler);
}
