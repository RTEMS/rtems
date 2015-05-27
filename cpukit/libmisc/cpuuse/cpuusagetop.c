/**
 * @file
 *
 * @brief CPU Usage Top
 * @ingroup libmisc_cpuuse CPU Usage
 */

/*
 *  COPYRIGHT (c) 2015. Chris Johns <chrisj@rtems.org>
 *
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 * Based on the old capture engine ct-load.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#include <rtems/cpuuse.h>
#include <rtems/malloc.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/protectedheap.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/score/wkspace.h>

/*
 * Common variable to sync the load monitor task.
 */
typedef struct
{
  void*                  context;
  rtems_printk_plugin_t  print;
} rtems_cpu_usage_plugin;

/*
 * Use a struct for all data to allow more than one top and to support the
 * thread iterator.
 */
typedef struct
{
  volatile bool          thread_run;
  volatile bool          thread_active;
  volatile bool          single_page;
  volatile uint32_t      sort_order;
  volatile uint32_t      poll_rate_usecs;
  volatile uint32_t      show;
  rtems_cpu_usage_plugin plugin;
  Thread_CPU_usage_t     zero;
  Timestamp_Control      uptime;
  Timestamp_Control      last_uptime;
  Timestamp_Control      period;
  int                    task_count;        /* Number of tasks. */
  int                    last_task_count;   /* Number of tasks in the previous sample. */
  int                    task_size;         /* The size of the arrays */
  Thread_Control**       tasks;             /* List of tasks in this sample. */
  Thread_Control**       last_tasks;        /* List of tasks in the last sample. */
  Thread_CPU_usage_t*    usage;             /* Usage of task's in this sample. */
  Thread_CPU_usage_t*    last_usage;        /* Usage of task's in the last sample. */
  Thread_CPU_usage_t*    current_usage;     /* Current usage for this sample. */
  Timestamp_Control      total;             /* Total run run, should equal the uptime. */
  Timestamp_Control      idle;              /* Time spent in idle. */
  Timestamp_Control      current;           /* Current time run in this period. */
  Timestamp_Control      current_idle;      /* Current time in idle this period. */
  uint32_t               stack_size;        /* Size of stack allocated. */
} rtems_cpu_usage_data;

/*
 * Sort orders.
 */
#define RTEMS_TOP_SORT_ID            (0)
#define RTEMS_TOP_SORT_REAL_PRI      (1)
#define RTEMS_TOP_SORT_CURRENT_PRI   (2)
#define RTEMS_TOP_SORT_TOTAL         (3)
#define RTEMS_TOP_SORT_CURRENT       (4)
#define RTEMS_TOP_SORT_MAX           (4)

/*
 * Private version of the iterator with an arg. This will be moved
 * to the public version in 5.0.
 */

typedef void (*rtems_per_thread_routine_2)( Thread_Control *, void* );

void rtems_iterate_over_all_threads_2(rtems_per_thread_routine_2 routine,
                                      void*                      arg);

void rtems_iterate_over_all_threads_2(rtems_per_thread_routine_2 routine,
                                      void*                      arg)
{
  uint32_t             i;
  uint32_t             api_index;
  Thread_Control      *the_thread;
  Objects_Information *information;

  if ( !routine )
    return;

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
          (*routine)(the_thread, arg);
      }
    }
  }
}

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

#define CPU_usage_Equal_to( _lhs, _rhs ) \
	_Timestamp_Equal_to( _lhs, _rhs )

#define CPU_usage_Set_to_zero( _time ) \
       _Timestamp_Set_to_zero( _time )

#define CPU_usage_Less_than( _lhs, _rhs ) \
      _Timestamp_Less_than( _lhs, _rhs )

static void
print_memsize(rtems_cpu_usage_data* data, const uint32_t size, const char* label)
{
  if (size > (1024 * 1024))
    (*data->plugin.print)(data->plugin.context, "%4" PRIu32 "M %s",
                          size / (1024 * 1024), label);
  else if (size > 1024)
    (*data->plugin.print)(data->plugin.context, "%4" PRIu32 "K %s",
                          size / 1024, label);
  else
    (*data->plugin.print)(data->plugin.context, "%4" PRIu32 " %s",
                          size, label);
}

static int
print_time(rtems_cpu_usage_data*    data,
           const Timestamp_Control* time,
           const int                length)
{
  uint32_t secs = _Timestamp_Get_seconds( time );
  uint32_t usecs = _Timestamp_Get_nanoseconds( time ) / TOD_NANOSECONDS_PER_MICROSECOND;
  int      len = 0;

  if (secs > 60)
  {
    uint32_t mins = secs / 60;
    if (mins > 60)
    {
      uint32_t hours = mins / 60;
      if (hours > 24)
      {
        len += (*data->plugin.print)(data->plugin.context, "%" PRIu32 "d", hours / 24);
        hours %= 24;
      }
      len += (*data->plugin.print)(data->plugin.context, "%" PRIu32 "hr", hours);
      mins %= 60;
    }
    len += (*data->plugin.print)(data->plugin.context, "%" PRIu32 "m", mins);
    secs %= 60;
  }
  len += (*data->plugin.print)(data->plugin.context, "%" PRIu32 ".%06" PRIu32, secs, usecs);

  if (len < length)
    (*data->plugin.print)(data->plugin.context, "%*c", length - len, ' ');

  return len;
}

/*
 * Count the number of tasks.
 */
static void
task_counter(Thread_Control *thrad, void* arg)
{
  rtems_cpu_usage_data* data = (rtems_cpu_usage_data*) arg;
  ++data->task_count;
}

/*
 * Create the sorted table with the current and total usage.
 */
static void
task_usage(Thread_Control* thread, void* arg)
{
  rtems_cpu_usage_data* data = (rtems_cpu_usage_data*) arg;
  Thread_CPU_usage_t    usage = thread->cpu_time_used;
  Thread_CPU_usage_t    current = data->zero;
  int                   j;

  data->stack_size += thread->Start.Initial_stack.size;

  for (j = 0; j < data->last_task_count; j++)
  {
    if (thread == data->last_tasks[j])
    {
      _Timestamp_Subtract(&data->last_usage[j], &usage, &current);
      break;
    }
  }

  /*
   * When not using nanosecond CPU usage resolution, we have to count the
   * number of "ticks" we gave credit for to give the user a rough guideline as
   * to what each number means proportionally.
   */
  _Timestamp_Add_to(&data->total, &usage);
  _Timestamp_Add_to(&data->current, &current);

  if (thread->Object.id == 0x09010001)
  {
    data->idle = usage;
    data->current_idle = current;
  }

  /*
   * Create the tasks to display soring as we create.
   */
  for (j = 0; j < data->task_count; j++)
  {
    if (data->tasks[j])
    {
      int k;

      /*
       * Sort on the current load.
       */
      switch (data->sort_order)
      {
        default:
          data->sort_order = RTEMS_TOP_SORT_CURRENT;
          /* drop through */
        case RTEMS_TOP_SORT_CURRENT:
          if (CPU_usage_Equal_to(&current, &data->zero) ||
              CPU_usage_Less_than(&current, &data->current_usage[j]))
            continue;
        case RTEMS_TOP_SORT_TOTAL:
          if (CPU_usage_Equal_to(&usage, &data->zero) ||
              CPU_usage_Less_than(&usage, &data->usage[j]))
            continue;
        case RTEMS_TOP_SORT_REAL_PRI:
          if (thread->real_priority > data->tasks[j]->real_priority)
            continue;
        case RTEMS_TOP_SORT_CURRENT_PRI:
          if (thread->current_priority > data->tasks[j]->current_priority)
            continue;
        case RTEMS_TOP_SORT_ID:
          if (thread->Object.id < data->tasks[j]->Object.id)
            continue;
      }

      for (k = (data->task_count - 1); k >= j; k--)
      {
        data->tasks[k + 1] = data->tasks[k];
        data->usage[k + 1]  = data->usage[k];
        data->current_usage[k + 1]  = data->current_usage[k];
      }
    }
    data->tasks[j] = thread;
    data->usage[j] = usage;
    data->current_usage[j] = current;
    break;
  }
}

/*
 * rtems_cpuusage_top_thread
 *
 * This function displays the load of the tasks on an ANSI terminal.
 */

static void
rtems_cpuusage_top_thread (rtems_task_argument arg)
{
  rtems_cpu_usage_data*  data = (rtems_cpu_usage_data*) arg;
  char                   name[13];
  int                    i;
  Heap_Information_block wksp;
  uint32_t               ival, fval;
  int                    task_count;
  rtems_event_set        out;
  rtems_status_code      sc;
  bool                   first_time = true;

  data->thread_active = true;

  _TOD_Get_uptime(&data->last_uptime);

  CPU_usage_Set_to_zero(&data->zero);

  while (data->thread_run)
  {
    Timestamp_Control uptime_at_last_reset = CPU_usage_Uptime_at_last_reset;
    size_t            tasks_size;
    size_t            usage_size;
    Timestamp_Control load;

    data->task_count = 0;
    rtems_iterate_over_all_threads_2(task_counter, data);

    tasks_size = sizeof(Thread_Control*) * (data->task_count + 1);
    usage_size = sizeof(Thread_CPU_usage_t) * (data->task_count + 1);

    if (data->task_count > data->task_size)
    {
      data->tasks = realloc(data->tasks, tasks_size);
      data->usage = realloc(data->usage, usage_size);
      data->current_usage = realloc(data->current_usage, usage_size);
      if ((data->tasks == NULL) || (data->usage == NULL) || (data->current_usage == NULL))
      {
        (*data->plugin.print)(data->plugin.context, "top worker: error: no memory\n");
        data->thread_run = false;
        break;
      }
    }

    memset(data->tasks, 0, tasks_size);
    memset(data->usage, 0, usage_size);
    memset(data->current_usage, 0, usage_size);

    _Timestamp_Set_to_zero(&data->total);
    _Timestamp_Set_to_zero(&data->current);
    data->stack_size = 0;

    _TOD_Get_uptime(&data->uptime);
    _Timestamp_Subtract(&uptime_at_last_reset, &data->uptime, &data->uptime);
    _Timestamp_Subtract(&data->last_uptime, &data->uptime, &data->period);
    data->last_uptime = data->uptime;

    rtems_iterate_over_all_threads_2(task_usage, data);

    if (data->task_count > data->task_size)
    {
      data->last_tasks = realloc(data->last_tasks, tasks_size);
      data->last_usage = realloc(data->last_usage, usage_size);
      if ((data->last_tasks == NULL) || (data->last_usage == NULL))
      {
        (*data->plugin.print)(data->plugin.context, "top worker: error: no memory\n");
        data->thread_run = false;
        break;
      }
      data->task_size = data->task_count;
    }

    memcpy(data->last_tasks, data->tasks, tasks_size);
    memcpy(data->last_usage, data->usage, usage_size);
    data->last_task_count = data->task_count;

    /*
     * We need to loop again to get suitable current usage values as we need a
     * last sample to work.
     */
    if (first_time)
    {
      rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(500));
      first_time = false;
      continue;
    }

    _Protected_heap_Get_information(&_Workspace_Area, &wksp);

    if (data->single_page)
      (*data->plugin.print)(data->plugin.context,
                            "\x1b[H\x1b[J"
                            " ENTER:Exit  SPACE:Refresh"
                            "  S:Scroll  A:All  <>:Order  +/-:Lines\n");
    (*data->plugin.print)(data->plugin.context,"\n");

    /*
     * Uptime and period of this sample.
     */
    (*data->plugin.print)(data->plugin.context, "Uptime: ");
    print_time(data, &data->uptime, 20);
    (*data->plugin.print)(data->plugin.context, " Period: ");
    print_time(data, &data->period, 20);

    /*
     * Task count, load and idle levels.
     */
    (*data->plugin.print)(data->plugin.context, "\nTasks: %4i  ", data->task_count);

    _Timestamp_Subtract(&data->idle, &data->total, &load);
    _Timestamp_Divide(&load, &data->uptime, &ival, &fval);
    (*data->plugin.print)(data->plugin.context,
                          "Load Average: %4" PRIu32 ".%03" PRIu32 "%%", ival, fval);
    _Timestamp_Subtract(&data->current_idle, &data->current, &load);
    _Timestamp_Divide(&load, &data->period, &ival, &fval);
    (*data->plugin.print)(data->plugin.context,
                          "  Load: %4" PRIu32 ".%03" PRIu32 "%%", ival, fval);
    _Timestamp_Divide(&data->current_idle, &data->period, &ival, &fval);
    (*data->plugin.print)(data->plugin.context,
                          "  Idle: %4" PRIu32 ".%03" PRIu32 "%%", ival, fval);

    /*
     * Memory usage.
     */
    if (rtems_configuration_get_unified_work_area())
    {
      (*data->plugin.print)(data->plugin.context, "\nMem: ");
      print_memsize(data, wksp.Free.total, "free");
      print_memsize(data, wksp.Used.total, "used");
    }
    else
    {
      region_information_block libc_heap;
      malloc_info(&libc_heap);
      (*data->plugin.print)(data->plugin.context, "\nMem: Wksp: ");
      print_memsize(data, wksp.Free.total, "free");
      print_memsize(data, wksp.Used.total, "used  Heap: ");
      print_memsize(data, libc_heap.Free.total, "free");
      print_memsize(data, libc_heap.Used.total, "used");
    }

    print_memsize(data, data->stack_size, "stack\n");

    (*data->plugin.print)(data->plugin.context,
       "\n"
        " ID         | NAME                | RPRI | CPRI   | TIME                | TOTAL   | CURRENT\n"
        "-%s---------+---------------------+-%s-----%s-----+---------------------+-%s------+--%s----\n",
       data->sort_order == RTEMS_TOP_SORT_ID ? "^^" : "--",
       data->sort_order == RTEMS_TOP_SORT_REAL_PRI ? "^^" : "--",
       data->sort_order == RTEMS_TOP_SORT_CURRENT_PRI ? "^^" : "--",
                          data->sort_order == RTEMS_TOP_SORT_TOTAL ? "^^" : "--",
       data->sort_order == RTEMS_TOP_SORT_CURRENT ? "^^" : "--"
    );

    task_count = 0;

    for (i = 0; i < data->task_count; i++)
    {
      Thread_Control*   thread = data->tasks[i];
      Timestamp_Control last;
      Timestamp_Control usage;
      Timestamp_Control current_usage;

      if (thread == NULL)
        break;

      if (data->single_page && (data->show != 0) && (i >= data->show))
        break;

      /*
       * We need to count the number displayed to clear the remainder of the
       * the display.
       */
      ++task_count;

      /*
       * If the API os POSIX print the entry point.
       */
      rtems_object_get_name(thread->Object.id, sizeof(name), name);
      if (name[0] == '\0')
        snprintf(name, sizeof(name) - 1, "(%p)", thread->Start.entry_point);

      (*data->plugin.print)(data->plugin.context,
                            " 0x%08" PRIx32 " | %-19s |  %3" PRId32 " |  %3" PRId32 "   | ",
                            thread->Object.id,
                            name,
                            thread->real_priority,
                            thread->current_priority);

      usage = data->usage[i];
      current_usage = data->current_usage[i];

      /*
       * If this is the currently executing thread, account for time since
       * the last context switch.
       */
      if (_Thread_Get_time_of_last_context_switch(thread, &last))
      {
        Timestamp_Control used;
        Timestamp_Control now;

        /*
         * Get the current uptime and assume we are not pre-empted to
         * measure the time from the last switch this thread and now.
         */
        _TOD_Get_uptime(&now);
        _Timestamp_Subtract(&last, &now, &used);
        _Timestamp_Add_to(&usage, &used);
        _Timestamp_Add_to(&current_usage, &used);
      }

      /*
       * Print the information
       */
      print_time(data, &usage, 19);
      _Timestamp_Divide(&usage, &data->total, &ival, &fval);
      (*data->plugin.print)(data->plugin.context,
                            " |%4" PRIu32 ".%03" PRIu32, ival, fval);
      _Timestamp_Divide(&current_usage, &data->period, &ival, &fval);
      (*data->plugin.print)(data->plugin.context,
                            " |%4" PRIu32 ".%03" PRIu32 "\n", ival, fval);
    }

    if (data->single_page && (data->show != 0) && (task_count < data->show))
    {
      i = data->show - task_count;
      while (i > 0)
      {
        (*data->plugin.print)(data->plugin.context, "\x1b[K\n");
        i--;
      }
    }

    sc = rtems_event_receive(RTEMS_EVENT_1,
                             RTEMS_EVENT_ANY,
                             RTEMS_MILLISECONDS_TO_TICKS (data->poll_rate_usecs),
                             &out);
    if ((sc != RTEMS_SUCCESSFUL) && (sc != RTEMS_TIMEOUT))
    {
      (*data->plugin.print)(data->plugin.context,
                            "error: event receive: %s\n", rtems_status_text(sc));
      break;
    }
  }

  free(data->tasks);
  free(data->last_tasks);
  free(data->last_usage);
  free(data->current_usage);

  data->thread_active = false;

  rtems_task_delete (RTEMS_SELF);
}

void rtems_cpu_usage_top_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  print
)
{
  rtems_status_code      sc;
  rtems_task_priority    priority;
  rtems_name             name;
  rtems_id               id;
  rtems_cpu_usage_data   data;
  int                    show_lines = 25;

  if ( !print )
    return;

  memset(&data, 0, sizeof(data));

  data.thread_run = true;
  data.single_page = true;
  data.sort_order = RTEMS_TOP_SORT_CURRENT;
  data.poll_rate_usecs = 3000;
  data.show = show_lines;
  data.plugin.context = context;
  data.plugin.print = print;

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
    id, rtems_cpuusage_top_thread, (rtems_task_argument) &data
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

  while (true)
  {
    int c = getchar ();

    if ((c == '\r') || (c == '\n') || (c == 'q') || (c == 'Q'))
    {
      int loops = 50;

      data.thread_run = false;

      rtems_event_send(id, RTEMS_EVENT_1);

      while (loops && data.thread_active)
        rtems_task_wake_after (RTEMS_MICROSECONDS_TO_TICKS (100000));

      (*print)(context, "load monitoring stopped.\n");
      return;
    }
    else if (c == '<')
    {
      if (data.sort_order == 0)
        data.sort_order = RTEMS_TOP_SORT_MAX;
      else
        --data.sort_order;
      rtems_event_send(id, RTEMS_EVENT_1);
    }
    else if (c == '>')
    {
      if (data.sort_order >= RTEMS_TOP_SORT_MAX)
        data.sort_order = 0;
      else
        ++data.sort_order;
      rtems_event_send(id, RTEMS_EVENT_1);
    }
    else if ((c == 's') || (c == 'S'))
    {
      data.single_page = !data.single_page;
      rtems_event_send(id, RTEMS_EVENT_1);
    }
    else if ((c == 'a') || (c == 'A'))
    {
      if (data.show == 0)
        data.show = show_lines;
      else
        data.show = 0;
      rtems_event_send(id, RTEMS_EVENT_1);
    }
    else if (c == '+')
    {
      ++show_lines;
      if (data.show != 0)
        data.show = show_lines;
    }
    else if (c == '-')
    {
      if (show_lines > 5)
        --show_lines;
      if (data.show != 0)
        data.show = show_lines;
    }
    else if (c == ' ')
    {
      rtems_event_send(id, RTEMS_EVENT_1);
    }
  }
}

void rtems_cpu_usage_top( void )
{
  rtems_cpu_usage_top_with_plugin( NULL, printk_plugin );
}
