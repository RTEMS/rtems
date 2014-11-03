/**
 * @file
 *
 * @brief CPU Usage Top
 * @ingroup libmisc_cpuuse CPU Usage
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
#include <rtems/score/objectimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>

/*
 * Common variable to sync the load monitor task.
 */
static volatile int rtems_cpuusage_top_thread_active;

typedef struct {
  void                  *context;
  rtems_printk_plugin_t  print;
}rtems_cpu_usage_plugin_t;

#define RTEMS_CPUUSAGE_TOP_MAX_LOAD_TASKS (20)


static inline bool equal_to_uint32_t( uint32_t * lhs, uint32_t * rhs )
{
   if ( *lhs == *rhs )
     return true;
   else 
     return false;
}

static inline bool less_than_uint32_t( uint32_t * lhs, uint32_t * rhs )
{
   if ( *lhs < *rhs )
    return true;
   else
    return false;
}

#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #define _Thread_CPU_usage_Equal_to( _lhs, _rhs ) \
          _Timestamp_Equal_to( _lhs, _rhs )
#else
  #define _Thread_CPU_usage_Equal_to( _lhs, _rhs ) \
          equal_to_uint32_t( _lhs, _rhs )
#endif

#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
#define  _Thread_CPU_usage_Set_to_zero( _time ) \
         _Timestamp_Set_to_zero( _time )
#else
#define  _Thread_CPU_usage_Set_to_zero( _time ) \
       do { \
         *_time = 0; \
       } while (0)
#endif

#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
#define _Thread_CPU_usage_Less_than( _lhs, _rhs ) \
        _Timestamp_Less_than( _lhs, _rhs )
#else
#define _Thread_CPU_usage_Less_than( _lhs, _rhs ) \
         less_than_uint32_t( _lhs, _rhs )
#endif

/*
 * rtems_cpuusage_top_thread
 *
 * This function displays the load of the tasks on an ANSI terminal.
 */

static void
rtems_cpuusage_top_thread (rtems_task_argument arg)
{
  uint32_t                  api_index;
  Thread_Control*           the_thread;
  int                       i;
  int                       j;
  int                       k;
  Objects_Information*      information;
  char                      name[13];
  int                       task_count = 0;
  uint32_t                  seconds, nanoseconds;
  rtems_cpu_usage_plugin_t* plugin = (rtems_cpu_usage_plugin_t*)arg;
  Thread_Control*           load_tasks[RTEMS_CPUUSAGE_TOP_MAX_LOAD_TASKS + 1];
  Thread_CPU_usage_t        load[RTEMS_CPUUSAGE_TOP_MAX_LOAD_TASKS + 1];
  Thread_CPU_usage_t        zero;
  Timestamp_Control         uptime;
  uint32_t                  ival, fval;

  while (true) {
    #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
      Timestamp_Control  total, ran, uptime_at_last_reset;
    #else
      uint32_t           total_units = 0;
    #endif

    rtems_cpuusage_top_thread_active = 1;

    _Thread_CPU_usage_Set_to_zero( &zero);
    memset (load_tasks, 0, sizeof (load_tasks));
    for (i=0; i< (RTEMS_CPUUSAGE_TOP_MAX_LOAD_TASKS + 1); i++)
      _Thread_CPU_usage_Set_to_zero( &load[i] );

   /*
     * Iterate over the tasks and sort the highest load tasks
     * into our local arrays. We only handle a limited number of
     * tasks.
     */
    for ( api_index = 1 ; api_index <= OBJECTS_APIS_LAST ; api_index++ ) {
      #if !defined(RTEMS_POSIX_API) || defined(RTEMS_DEBUG)
        if ( !_Objects_Information_table[ api_index ] )
          continue;
      #endif

      information = _Objects_Information_table[ api_index ][ 1 ];
      if ( information ) {
        for ( i=1 ; i <= information->maximum ; i++ ) {
          the_thread = (Thread_Control *)information->local_table[ i ];
          if ( the_thread ) {
            Thread_CPU_usage_t usage = the_thread->cpu_time_used;

            /*
             *  When not using nanosecond CPU usage resolution, we have to count
             *  the number of "ticks" we gave credit for to give the user a rough
             *  guideline as to what each number means proportionally.
             */
            #ifdef __RTEMS_USE_TICKS_FOR_STATISTICS__
              total_units += usage;
            #endif

            /* Count the number of tasks and sort this load value */
            task_count++;
            for (j = 0; j < RTEMS_CPUUSAGE_TOP_MAX_LOAD_TASKS; j++) {
              if (load_tasks[j]) {
                if ( _Thread_CPU_usage_Equal_to( &usage, &zero) || 
                     _Thread_CPU_usage_Less_than( &usage, &load[j]))
                  continue;
                for (k = (RTEMS_CPUUSAGE_TOP_MAX_LOAD_TASKS - 1); k >= j; k--){
                  load_tasks[k + 1] = load_tasks[k];
                  load[k + 1]  = load[k];
                }
              }
              load_tasks[j] = the_thread;
              load[j]  = usage;
              break;
            }
          }
        }
      }
    }

    #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
      _Timestamp_Set_to_zero( &total );
      uptime_at_last_reset = CPU_usage_Uptime_at_last_reset;
    #endif

    _TOD_Get_uptime( &uptime );
    seconds = _Timestamp_Get_seconds( &uptime );
    nanoseconds = _Timestamp_Get_nanoseconds( &uptime ) /
                  TOD_NANOSECONDS_PER_MICROSECOND;
    (*plugin->print)(plugin->context, "\x1b[H\x1b[J Press ENTER to exit.\n\n");
    (*plugin->print)(plugin->context, "uptime: ");
    (*plugin->print)(plugin->context,
      "%7" PRIu32 ".%06" PRIu32 "\n",  seconds, nanoseconds
    );

    (*plugin->print)(
       plugin->context,
       "-------------------------------------------------------------------------------\n"
       "                              CPU USAGE BY THREAD\n"
       "------------+---------------------+---------------+---------------+------------\n"
       #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
        " ID         | NAME                | RPRI | CPRI   | SECONDS       | PERCENT\n"
       #else
        " ID         | NAME                | RPRI | CPRI   | TICKS         | PERCENT\n"
       #endif
       "------------+---------------------+---------------+---------------+------------\n"
    );

    for (i = 0; i < RTEMS_CPUUSAGE_TOP_MAX_LOAD_TASKS; i++) {

      if (!load_tasks[i])
        break;

      /*
       * If this is the currently executing thread, account for time
       * since the last context switch.
       */
      the_thread = load_tasks[i];

      rtems_object_get_name( the_thread->Object.id, sizeof(name), name );
      (*plugin->print)(
        plugin->context,
        " 0x%08" PRIx32 " | %-19s |  %3" PRId32 " |  %3" PRId32 "   |",
        the_thread->Object.id,
        name,
        the_thread->real_priority,
        the_thread->current_priority
      );

      #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
      {
        Timestamp_Control last;

        /*
         * If this is the currently executing thread, account for time
         * since the last context switch.
         */
        ran = load[i];
        if ( _Thread_Get_time_of_last_context_switch( the_thread, &last ) ) {
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
       (*plugin->print)( plugin->context,
          "%7" PRIu32 ".%06" PRIu32 " |%4" PRIu32 ".%03" PRIu32 "\n",
          seconds, nanoseconds,
            ival, fval
        );
      }
      #else
        if (total_units) {
          uint64_t ival_64;

          ival_64 = load[i];
          ival_64 *= 100000;
          ival = ival_64 / total_units;
        } else {
          ival = 0;
        }

        fval = ival % 1000;
        ival /= 1000;
       (*plugin->print)( plugin->context,
          "%14" PRIu32 " |%4" PRIu32 ".%03" PRIu32 "\n",
          load[i],
          ival,
          fval
        );
      #endif
    }

    if (task_count < RTEMS_CPUUSAGE_TOP_MAX_LOAD_TASKS)
    {
      j = RTEMS_CPUUSAGE_TOP_MAX_LOAD_TASKS - task_count;
      while (j > 0)
      {
       (*plugin->print)( plugin->context, "\x1b[K\n");
        j--;
      }
    }

    rtems_cpuusage_top_thread_active = 0;

    rtems_task_wake_after (RTEMS_MICROSECONDS_TO_TICKS (5000000));
  }
}

void rtems_cpu_usage_top_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  print
)
{
  rtems_status_code   sc;
  rtems_task_priority priority;
  rtems_name          name;
  rtems_id            id;
  rtems_cpu_usage_plugin_t  plugin;

  if ( !print )
    return;

  plugin.context = context;
  plugin.print   = print;

  sc = rtems_task_set_priority (RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority);

  if (sc != RTEMS_SUCCESSFUL)
  {
    (*print)(
       context,
       "error: cannot obtain the current priority: %s\n",
       rtems_status_text (sc)
    );
    return;
  }

  name = rtems_build_name('C', 'P', 'l', 't');

  sc = rtems_task_create (name, priority, 4 * 1024,
                          RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
                          RTEMS_PREEMPT | RTEMS_TIMESLICE | RTEMS_NO_ASR,
                          &id);

  if (sc != RTEMS_SUCCESSFUL)
  {
    (*print)(
       context,
       "error: cannot create helper thread: %s\n",
       rtems_status_text (sc)
    );
    return;
  }

  sc = rtems_task_start (
    id, rtems_cpuusage_top_thread, (rtems_task_argument)&plugin
  );
  if (sc != RTEMS_SUCCESSFUL)
  {
    (*print)(
       context,
       "error: cannot start helper thread: %s\n",
       rtems_status_text (sc)
    );
    rtems_task_delete (id);
    return;
  }

  for (;;)
  {
    int c = getchar ();

    if ((c == '\r') || (c == '\n'))
    {
      int loops = 20;

      while (loops && rtems_cpuusage_top_thread_active)
        rtems_task_wake_after (RTEMS_MICROSECONDS_TO_TICKS (100000));

      rtems_task_delete (id);

      (*print)(context, "load monitoring stopped.\n");
      return;
    }
  }
}

void rtems_cpu_usage_top( void )
{
  rtems_cpu_usage_top_with_plugin( NULL, printk_plugin );
}
