/*
 *  CPU Usage Reporter
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#include <rtems/cpuuse.h>

#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  static bool is_executing_on_a_core(
    Thread_Control    *the_thread,
    Timestamp_Control *time_of_context_switch
  )
  {
    #ifndef RTEMS_SMP
      if ( _Thread_Executing->Object.id == the_thread->Object.id ) {
        *time_of_context_switch = _Thread_Time_of_last_context_switch;
        return true;
      }
    #else
      int  cpu;
      for ( cpu=0 ; cpu < rtems_smp_get_number_of_processors() ; cpu++ ) {
        Per_CPU_Control *p = &_Per_CPU_Information[cpu];
        if ( p->executing->Object.id == the_thread->Object.id ) {
          *time_of_context_switch = p->time_of_last_context_switch;
          return true;
        }
      }
    #endif
    return false;
  }
#endif

/*
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
  #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
    Timestamp_Control  uptime, total, ran, uptime_at_last_reset;
    uint32_t seconds, nanoseconds;
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
  #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
    _Timestamp_Set_to_zero( &total );
    uptime_at_last_reset = CPU_usage_Uptime_at_last_reset;
  #else
    for ( api_index = 1 ; api_index <= OBJECTS_APIS_LAST ; api_index++ ) {
      #if !defined(RTEMS_POSIX_API) || defined(RTEMS_DEBUG)
        if ( !_Objects_Information_table[ api_index ] )
          continue;
      #endif

      information = _Objects_Information_table[ api_index ][ 1 ];
      if ( information ) {
        for ( i=1 ; i <= information->maximum ; i++ ) {
          the_thread = (Thread_Control *)information->local_table[ i ];

          if ( the_thread )
            total_units += the_thread->cpu_time_used;
        }
      }
    }
  #endif

  (*print)(
     context,
     "-------------------------------------------------------------------------------\n"
     "                              CPU USAGE BY THREAD\n"
     "------------+----------------------------------------+---------------+---------\n"
     #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
       " ID         | NAME                                   | SECONDS       | PERCENT\n"
     #else
       " ID         | NAME                                   | TICKS         | PERCENT\n"
     #endif
     "------------+----------------------------------------+---------------+---------\n"
  );

  for ( api_index = 1 ; api_index <= OBJECTS_APIS_LAST ; api_index++ ) {
    #if !defined(RTEMS_POSIX_API) || defined(RTEMS_DEBUG)
      if ( !_Objects_Information_table[ api_index ] )
        continue;
    #endif

    information = _Objects_Information_table[ api_index ][ 1 ];
    if ( information ) {
      for ( i=1 ; i <= information->maximum ; i++ ) {
        the_thread = (Thread_Control *)information->local_table[ i ];

        if ( !the_thread )
          continue;

        rtems_object_get_name( the_thread->Object.id, sizeof(name), name );

        (*print)(
          context,
          " 0x%08" PRIx32 " | %-38s |",
          the_thread->Object.id,
          name
        );

        #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
        {
          Timestamp_Control last;

          /*
           * If this is the currently executing thread, account for time
           * since the last context switch.
           */
          ran = the_thread->cpu_time_used;
          if ( is_executing_on_a_core( the_thread, &last ) ) {
            Timestamp_Control used;
            _TOD_Get_uptime( &uptime );
            _Timestamp_Subtract( &last, &uptime, &used );
            _Timestamp_Add_to( &ran, &used );
          } else {
            _TOD_Get_uptime( &uptime );
          }
          _Timestamp_Subtract( &uptime_at_last_reset, &uptime, &total );
          _Timestamp_Divide( &ran, &total, &ival, &fval );

          /*
           * Print the information
           */

          seconds = _Timestamp_Get_seconds( &ran );
          nanoseconds = _Timestamp_Get_nanoseconds( &ran ) /
            TOD_NANOSECONDS_PER_MICROSECOND;
          (*print)( context,
            "%7" PRIu32 ".%06" PRIu32 " |%4" PRIu32 ".%03" PRIu32 "\n",
            seconds, nanoseconds,
            ival, fval
          );
        }
        #else
         if (total_units) {
            uint64_t ival_64;

            ival_64 = the_thread->cpu_time_used;
            ival_64 *= 100000;
            ival = ival_64 / total_units;
          } else {
            ival = 0;
          }

          fval = ival % 1000;
          ival /= 1000;
          (*print)( context,
            "%14" PRIu32 " |%4" PRIu32 ".%03" PRIu32 "\n",
            the_thread->cpu_time_used,
            ival,
            fval
          );
        #endif
      }
    }
  }

  #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
    seconds = _Timestamp_Get_seconds( &total );
    nanoseconds = _Timestamp_Get_nanoseconds( &total ) /
      TOD_NANOSECONDS_PER_MICROSECOND;
    (*print)(
       context,
       "------------+----------------------------------------+---------------+---------\n"
       " TIME SINCE LAST CPU USAGE RESET IN SECONDS:                    %7" PRIu32 ".%06" PRIu32 "\n"
       "-------------------------------------------------------------------------------\n",
       seconds, nanoseconds
    );
  #else
    (*print)(
       context,
       "------------+----------------------------------------+---------------+---------\n"
       " TICKS SINCE LAST SYSTEM RESET:                                 %14" PRIu32 "\n"
       " TOTAL UNITS:                                                   %14" PRIu32 "\n"
       "-------------------------------------------------------------------------------\n",
       _Watchdog_Ticks_since_boot - CPU_usage_Ticks_at_last_reset,
       total_units
    );
  #endif
}

void rtems_cpu_usage_report( void )
{
  rtems_cpu_usage_report_with_plugin( NULL, printk_plugin );
}
