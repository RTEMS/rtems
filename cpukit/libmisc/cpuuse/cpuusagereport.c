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

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#include <rtems/cpuuse.h>
#include <rtems/bspIo.h>

#if defined(RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS)
  #include <rtems/score/timespec.h>
#endif

#ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
  extern struct timespec    CPU_usage_Uptime_at_last_reset;
#else
  extern uint32_t           CPU_usage_Ticks_at_last_reset;
#endif

/*PAGE
 *
 *  rtems_cpu_usage_report
 */

void rtems_cpu_usage_report_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  print
)
{
  uint32_t             i;
  uint32_t             api_index;
  Thread_Control      *the_thread;
  Objects_Information *information;
  char                 name[13];
  uint32_t             ival, fval;
  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    struct timespec    uptime, total, ran;
  #else
    uint32_t           total_units = 0;
  #endif

  if ( !print )
    return;

  /*
   *  When not using nanosecond CPU usage resolution, we have to count
   *  the number of "ticks" we gave credit for to give the user a rough
   *  guideline as to what each number means proportionally.
   */
  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    _TOD_Get_uptime( &uptime );
    _Timespec_Subtract( &CPU_usage_Uptime_at_last_reset, &uptime, &total );
  #else
    for ( api_index = 1 ; api_index <= OBJECTS_APIS_LAST ; api_index++ ) {
      if ( !_Objects_Information_table[ api_index ] )
        continue;
      information = _Objects_Information_table[ api_index ][ 1 ];
      if ( information ) {
        for ( i=1 ; i <= information->maximum ; i++ ) {
          the_thread = (Thread_Control *)information->local_table[ i ];

          if ( the_thread )
            total_units += the_thread->ticks_executed;
        }
      }
    }
  #endif
  
  (*print)( context, "CPU Usage by thread\n"
  #if defined(RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS)
     "   ID            NAME         SECONDS   PERCENT\n"
  #else
     "   ID            NAME         TICKS   PERCENT\n"
  #endif
  );

  for ( api_index = 1 ;
        api_index <= OBJECTS_APIS_LAST ;
        api_index++ ) {
    if ( !_Objects_Information_table[ api_index ] )
      continue;
    information = _Objects_Information_table[ api_index ][ 1 ];
    if ( information ) {
      for ( i=1 ; i <= information->maximum ; i++ ) {
        the_thread = (Thread_Control *)information->local_table[ i ];

        if ( !the_thread )
          continue;

        rtems_object_get_name( the_thread->Object.id, sizeof(name), name );
 
        (*print)(
          context,
          "0x%08" PRIx32 "   %-12s   ",
          the_thread->Object.id,
          name
        );

        #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
          /*
           * If this is the currently executing thread, account for time
           * since the last context switch.
           */
          ran = the_thread->cpu_time_used;
          if ( _Thread_Executing->Object.id == the_thread->Object.id ) {
            struct timespec used;
            _Timespec_Subtract(
              &_Thread_Time_of_last_context_switch, &uptime, &used
            );
            _Timespec_Add_to( &ran, &used );
          };
          _Timespec_Divide( &ran, &total, &ival, &fval );

          /*
           * Print the information
           */

          (*print)( context,
            "%3" PRId32 ".%06" PRId32 "  %3" PRId32 ".%03" PRId32 "\n",
            ran.tv_sec, ran.tv_nsec / TOD_NANOSECONDS_PER_MICROSECOND,
            ival, fval
          );
        #else
          ival = (total_units) ?
                   the_thread->ticks_executed * 10000 / total_units : 0;
          fval = ival % 100;
          ival /= 100;
          (*print)( context,
            "%8" PRId32 "  %3" PRId32 ".%02" PRId32"\n",
            the_thread->ticks_executed,
            ival,
            fval
          );
        #endif
      }
    }
  }

  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    (*print)( context, "Time since last CPU Usage reset %" PRId32
            ".%06" PRId32 " seconds\n",
       total.tv_sec,
       total.tv_nsec / TOD_NANOSECONDS_PER_MICROSECOND
    );
  #else
    (*print)( context,
      "Ticks since last reset = %" PRId32 "\n",
      _Watchdog_Ticks_since_boot - CPU_usage_Ticks_at_last_reset
    );
    (*print)( context, "Total Units = %" PRId32 "\n", total_units );
  #endif
}

void rtems_cpu_usage_report( void )
{
  rtems_cpu_usage_report_with_plugin( NULL, printk_plugin );
}
