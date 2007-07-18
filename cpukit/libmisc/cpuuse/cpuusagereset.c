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

#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

#include <rtems/cpuuse.h>

static void CPU_usage_Per_thread_handler(
  Thread_Control *the_thread
)
{
  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    the_thread->cpu_time_used.tv_sec  = 0;
    the_thread->cpu_time_used.tv_nsec = 0;
  #else
    the_thread->ticks_executed = 0;
  #endif
}

/*
 *  rtems_cpu_usage_reset
 */
void rtems_cpu_usage_reset( void )
{
  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    extern struct timespec CPU_usage_Uptime_at_last_reset;
    
    _TOD_Get_uptime( &CPU_usage_Uptime_at_last_reset );
  #else
    extern uint32_t   CPU_usage_Ticks_at_last_reset;

    CPU_usage_Ticks_at_last_reset = _Watchdog_Ticks_since_boot;

  #endif

  rtems_iterate_over_all_threads(CPU_usage_Per_thread_handler);
}
