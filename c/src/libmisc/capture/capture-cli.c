/*
  ------------------------------------------------------------------------
  $Id$
  ------------------------------------------------------------------------
  
  Copyright Objective Design Systems Pty Ltd, 2002
  All rights reserved Objective Design Systems Pty Ltd, 2002
  Chris Johns (ccj@acm.org)

  COPYRIGHT (c) 1989-1998.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution.

  This software with is provided ``as is'' and with NO WARRANTY.
  
  ------------------------------------------------------------------------

  RTEMS Performance Monitoring and Measurement Framework.

  This is the Target Interface Command Line Interface. You need
  start the RTEMS monitor.

*/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rtems.h>
#include <rtems/capture-cli.h>
#include <rtems/monitor.h>

#define RTEMS_CAPTURE_CLI_MAX_LOAD_TASKS (32)

/*
 * The user capture timestamper.
 */
static rtems_capture_timestamp capture_timestamp;

/*
 * Common variable to sync the load monitor task.
 */
static volatile int cli_load_thread_active;

/*
 * rtems_capture_cli_open
 *
 *  DESCRIPTION:
 *
 * This function opens the capture engine. We need the size of the
 * capture buffer.
 *
 */

static const char* open_usage = "usage: copen [-i] size\n";

static void
rtems_capture_cli_open (int argc, char **argv)
{
  rtems_unsigned32  size = 0;
  rtems_boolean     enable = 0;
  rtems_status_code sc;
  int               arg;

  if (argc <= 1)
  {
    printf (open_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      if (argv[arg][1] == 'i')
        enable = 1;
      else
        printf ("warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      size = strtoul (argv[arg], 0, 0);

      if (size < 100)
      {
        printf ("error: size must be greater than or equal to 100\n");
        return;
      }
    }
  }

  sc = rtems_capture_open (size, capture_timestamp);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: open failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("capture engine opened.\n");

  if (!enable)
    return;
  
  sc = rtems_capture_control (enable);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: open enable failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("capture engine enabled.\n");
}

/*
 * rtems_capture_cli_close
 *
 *  DESCRIPTION:
 *
 * This function closes the capture engine.
 *
 */

static void
rtems_capture_cli_close (int argc, char **argv)
{
  rtems_status_code sc;

  sc = rtems_capture_close ();

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: close failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("capture engine closed.\n");
}

/*
 * rtems_capture_cli_enable
 *
 *  DESCRIPTION:
 *
 * This function enables the capture engine.
 *
 */

static void
rtems_capture_cli_enable (int argc, char **argv)
{
  rtems_status_code sc;

  sc = rtems_capture_control (1);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: enable failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("capture engine enabled.\n");
}

/*
 * rtems_capture_cli_disable
 *
 *  DESCRIPTION:
 *
 * This function disables the capture engine.
 *
 */

static void
rtems_capture_cli_disable (int argc, char **argv)
{
  rtems_status_code sc;

  sc = rtems_capture_control (0);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: disable failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("capture engine disabled.\n");
}

/*
 * rtems_capture_cli_task_list
 *
 *  DESCRIPTION:
 *
 * This function lists the tasks the capture engine knows about.
 *
 */

static void
rtems_capture_cli_task_list (int argc, char **argv)
{
  rtems_task_priority   ceiling = rtems_capture_watch_get_ceiling ();
  rtems_task_priority   floor = rtems_capture_watch_get_floor ();
  rtems_capture_task_t* task = rtems_capture_get_task_list ();
  rtems_unsigned32      ticks;
  rtems_unsigned32      tick_offset;
  unsigned long long    total_time;
  int                   count = rtems_capture_task_count ();

  if (capture_timestamp)
    capture_timestamp (&ticks, &tick_offset);
  else
  {
    ticks = _Watchdog_Ticks_since_boot;
    tick_offset = 0;
  }

  total_time = (ticks * rtems_capture_task_time (task)) + tick_offset;

  printf ("total %i\n", count);

  while (task)
  {
    rtems_task_priority priority;
    int                 stack_used;
    int                 time_used;

    stack_used = rtems_capture_task_stack_usage (task) * 100;
    stack_used /= rtems_capture_task_stack_size (task);

    if (stack_used > 100)
      stack_used = 100;

    time_used = (rtems_capture_task_time (task) * 100) / total_time;

    if (time_used > 100)
      time_used = 100;

    priority = rtems_capture_task_real_priority (task);

    printf (" ");
    rtems_monitor_dump_id (rtems_capture_task_id (task));
    printf (" ");
    rtems_monitor_dump_name (rtems_capture_task_name (task));
    printf (" ");
    rtems_monitor_dump_priority (rtems_capture_task_start_priority (task));
    printf (" ");
    rtems_monitor_dump_priority (rtems_capture_task_real_priority (task));
    printf (" ");
    rtems_monitor_dump_priority (rtems_capture_task_curr_priority (task));
    printf (" ");
    rtems_monitor_dump_state (rtems_capture_task_state (task));
    printf (" %c%c%c%c%c",
            rtems_capture_task_valid (task) ? 'a' : 'd',
            rtems_capture_task_flags (task) & RTEMS_CAPTURE_TRACED ? 't' : '-',
            rtems_capture_task_control_flags (task) & RTEMS_CAPTURE_TO_ANY ? 'F' : '-',
            rtems_capture_task_control_flags (task) & RTEMS_CAPTURE_FROM_ANY ? 'T' : '-',
            rtems_capture_task_control_flags (task) & RTEMS_CAPTURE_FROM_TO ? 'E' : '-');
    if ((floor > ceiling) && (ceiling > priority))
      printf ("--");
    else
      printf ("%c%c",
              rtems_capture_task_control (task) ?
              (rtems_capture_task_control_flags (task) & RTEMS_CAPTURE_WATCH ? 'w' : '+') : '-',
              rtems_capture_watch_global_on () ? 'g' : '-');
    printf (" %3i%% %3i%% (%i)\n",
            stack_used, time_used, rtems_capture_task_ticks (task));

    task = rtems_capture_next_task (task);
  }
}

/*
 * rtems_capture_cli_task_load_thread
 *
 *  DESCRIPTION:
 *
 * This function displays the load of the tasks on an ANSI terminal.
 *
 */

static void
rtems_capture_cli_task_load_thread (rtems_task_argument arg)
{
  rtems_task_priority ceiling = rtems_capture_watch_get_ceiling ();
  rtems_task_priority floor = rtems_capture_watch_get_floor ();
  int                 last_count = 0;
  
  printf ("\x1b[2J Press ENTER to exit.\n\n");
  printf ("     PID NAME RPRI CPRI STATE  %%CPU     %%STK FLGS   EXEC TIME\n");

  for (;;)
  { 
    rtems_capture_task_t* tasks[RTEMS_CAPTURE_CLI_MAX_LOAD_TASKS + 1];
    unsigned long long    load[RTEMS_CAPTURE_CLI_MAX_LOAD_TASKS + 1];
    rtems_capture_task_t* task;
    unsigned long long    total_time;
    int                   count = 0;
    int                   i;
    int                   j;

    cli_load_thread_active = 1;
    
    /*
     * Iterate over the tasks and sort the highest load tasks
     * into our local arrays. We only handle a limited number of
     * tasks.
     */
   
    memset (tasks, 0, sizeof (tasks));
    memset (load, 0, sizeof (load));

    task = rtems_capture_get_task_list ();
  
    while (task)
    {
      if (rtems_capture_task_valid (task))
      {
        unsigned long long l = rtems_capture_task_delta_time (task);

        count++;

        for (i = 0; i < RTEMS_CAPTURE_CLI_MAX_LOAD_TASKS; i++)
        {
          if (tasks[i])
          {
            if ((l == 0) || (l < load[i]))
              continue;

            for (j = (RTEMS_CAPTURE_CLI_MAX_LOAD_TASKS - 1); j >= i; j--)
            {
              tasks[j + 1] = tasks[j];
              load[j + 1]  = load[j];
            }
          }

          tasks[i] = task;
          load[i]  = l;
          break;
        }
      }      
      task = rtems_capture_next_task (task);
    }

    printf ("\x1b[4;0H");

    total_time = 0;
    
    for (i = 0; i < RTEMS_CAPTURE_CLI_MAX_LOAD_TASKS; i++)
      total_time += load[i];

    if (count > last_count)
      j = count;
    else
      j = last_count;

    for (i = 0; i < RTEMS_CAPTURE_CLI_MAX_LOAD_TASKS; i++)
    {
      rtems_task_priority priority;
      int                 stack_used;
      int                 task_load;
      int                 k;

      if (!tasks[i])
        break;
      
      j--;

      stack_used = rtems_capture_task_stack_usage (tasks[i]) * 100;
      stack_used /= rtems_capture_task_stack_size (tasks[i]);

      if (stack_used > 100)
        stack_used = 100;

      task_load = (int) ((load[i] * 100000) / total_time);

      priority = rtems_capture_task_real_priority (tasks[i]);

      printf ("\x1b[K");
      rtems_monitor_dump_id (rtems_capture_task_id (tasks[i]));
      printf (" ");
      rtems_monitor_dump_name (rtems_capture_task_name (tasks[i]));
      printf ("  ");
      rtems_monitor_dump_priority (priority);
      printf ("  ");
      rtems_monitor_dump_priority (rtems_capture_task_curr_priority (tasks[i]));
      printf (" ");
      k = rtems_monitor_dump_state (rtems_capture_task_state (tasks[i]));
      printf ("%*c %3i.%03i%% ", 6 - k, ' ', task_load / 1000, task_load % 1000);
      printf ("%3i%% %c%c%c%c%c", stack_used,
              rtems_capture_task_valid (tasks[i]) ? 'a' : 'd',
              rtems_capture_task_flags (tasks[i]) & RTEMS_CAPTURE_TRACED ? 't' : '-',
              rtems_capture_task_control_flags (tasks[i]) & RTEMS_CAPTURE_TO_ANY ? 'F' : '-',
              rtems_capture_task_control_flags (tasks[i]) & RTEMS_CAPTURE_FROM_ANY ? 'T' : '-',
              rtems_capture_task_control_flags (tasks[i]) & RTEMS_CAPTURE_FROM_TO ? 'E' : '-');
      if ((floor > ceiling) && (ceiling > priority))
        printf ("--");
      else
        printf ("%c%c",
                rtems_capture_task_control (tasks[i]) ? 
                (rtems_capture_task_control_flags (tasks[i]) &
                 RTEMS_CAPTURE_WATCH ? 'w' : '+') : '-',
                rtems_capture_watch_global_on () ? 'g' : '-');
      
      printf ("   %qi\n", rtems_capture_task_time (tasks[i]));
    }

    while (j)
    {
      printf ("\x1b[K\n");
      j--;
    }

    last_count = count;

    cli_load_thread_active = 0;
    
    rtems_task_wake_after (TOD_MICROSECONDS_TO_TICKS (5000000));
  }
}

/*
 * rtems_capture_cli_task_load
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command.
 *
 */

static void
rtems_capture_cli_task_load (int argc, char **argv)
{
  rtems_status_code   sc;
  rtems_task_priority priority;
  rtems_name          name;
  rtems_id            id;
  
  sc = rtems_task_set_priority (RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: cannot obtain the current priority: %s\n", rtems_status_text (sc));
    return;
  }
  
  memcpy (&name, "CPlt", 4);
  
  sc = rtems_task_create (name, priority, 1024,
                          RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
                          RTEMS_PREEMPT | RTEMS_TIMESLICE | RTEMS_NO_ASR,
                          &id);
  
  if (sc != RTEMS_SUCCESSFUL) 
  {
    printf ("error: cannot create helper thread: %s\n", rtems_status_text (sc));
    return;
  }

  sc = rtems_task_start (id, rtems_capture_cli_task_load_thread, 0);
  
  if (sc != RTEMS_SUCCESSFUL) 
  {
    printf ("error: cannot start helper thread: %s\n", rtems_status_text (sc));
    rtems_task_delete (id);
    return;
  }

  for (;;)
  {
    char c = getchar ();

    if ((c == '\r') || (c == '\n'))
    {
      int loops = 20;
      
      while (loops && cli_load_thread_active)
        rtems_task_wake_after (TOD_MICROSECONDS_TO_TICKS (100000));

      rtems_task_delete (id);

      printf ("load monitoring stopped.\n");

      return;
    }
  }
}

/*
 * rtems_capture_cli_watch_list
 *
 *  DESCRIPTION:
 *
 * This function lists the controls in the capture engine.
 *
 */

static void
rtems_capture_cli_watch_list (int argc, char **argv)
{
  rtems_capture_control_t* control = rtems_capture_get_control_list ();
  rtems_task_priority      ceiling = rtems_capture_watch_get_ceiling ();
  rtems_task_priority      floor = rtems_capture_watch_get_floor ();
  
  printf ("watch priority ceiling is %i\n", ceiling);
  printf ("watch priority floor is %i\n", floor);
  printf ("global watch is %s\n",
          rtems_capture_watch_global_on () ? "enabled" : "disabled");
  printf ("total %d\n", rtems_capture_control_count ());

  while (control)
  {
    int f;
    int fshowed;
    int lf;
    
    printf (" ");
    rtems_monitor_dump_id (rtems_capture_control_id (control));
    printf (" ");
    rtems_monitor_dump_name (rtems_capture_control_name (control));
    printf (" %c%c%c%c%c",
            rtems_capture_control_flags (control) & RTEMS_CAPTURE_WATCH ? 'w' : '-',
            rtems_capture_watch_global_on () ? 'g' : '-',
            rtems_capture_control_flags (control) & RTEMS_CAPTURE_TO_ANY ? 'F' : '-',
            rtems_capture_control_flags (control) & RTEMS_CAPTURE_FROM_ANY ? 'T' : '-',
            rtems_capture_control_flags (control) & RTEMS_CAPTURE_FROM_TO ? 'E' : '-');

    for (f = 0, fshowed = 0, lf = 1; f < RTEMS_CAPTURE_TRIGGER_TASKS; f++)
    {
      if (lf && ((fshowed % 16) == 0))
      {
        printf ("\n");
        lf = 0;
      }
      
      /*
       * FIXME: name test.
       */
      if (rtems_capture_control_from_name (control, f))
      {
        printf ("  %2i:", f);
        rtems_monitor_dump_name (rtems_capture_control_from_name (control, f));
        printf ("/");
        rtems_monitor_dump_id (rtems_capture_control_from_id (control, f));
        fshowed++;
        lf = 1;
      }
    }

    if (lf)
      printf ("\n");

    control = rtems_capture_next_control (control);
  }
}

/*
 * rtems_capture_cli_get_name_id
 *
 *  DESCRIPTION:
 *
 * This function checks arguments for a name or an id.
 *
 */

static rtems_boolean
rtems_capture_cli_get_name_id (char*          arg,
                               rtems_boolean* valid_name,
                               rtems_boolean* valid_id,
                               rtems_name*    name,
                               rtems_id*      id)
{
  unsigned32 objclass;
  int             l;
  int             i;

  if (*valid_name && *valid_id)
  {
    printf ("error: too many arguments\n");
    return 0;
  }

  /*
   * See if the arg is all hex digits.
   */
  
  l = strlen (arg);

  for (i = 0; i < l; i++)
    if (!isxdigit (arg[i]))
      break;

  *id = strtoul (arg, 0, 16);
      
  objclass = _Objects_Get_class (*id);
  
  if ((i == l))
    *valid_id = 1;
  else
  {
    memcpy (name, arg, sizeof (rtems_name));
    *valid_name = 1;
  }

  return 1;
}

/*
 * rtems_capture_cli_watch_add
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that add a watch to the capture
 * engine.
 *
 */

static char const * watch_add_usage = "usage: cwadd [task name] [id]\n";

static void
rtems_capture_cli_watch_add (int argc, char **argv)
{
  rtems_status_code sc;
  int               arg;
  rtems_name        name = 0;
  rtems_id          id = 0;
  rtems_boolean     valid_name = 0;
  rtems_boolean     valid_id = 0;

  if (argc <= 1)
  {
    printf (watch_add_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      printf ("warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      if (!rtems_capture_cli_get_name_id (argv[arg], &valid_name, &valid_id, &name, &id))
        return;
    }
  }

  if (!valid_name && !valid_id)
  {
    printf("error: no valid name or task id located\n");
    return;
  }

  sc = rtems_capture_watch_add (name, id);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: watch add failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("watch added.\n");
}

/*
 * rtems_capture_cli_watch_del
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that deletes a watch from the capture
 * engine.
 *
 */

static char const * watch_del_usage = "usage: cwdel [task name] [id]\n";

static void
rtems_capture_cli_watch_del (int argc, char **argv)
{
  rtems_status_code sc;
  int               arg;
  rtems_name        name = 0;
  rtems_id          id = 0;
  rtems_boolean     valid_name = 0;
  rtems_boolean     valid_id = 0;

  if (argc <= 1)
  {
    printf (watch_del_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      printf ("warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      if (!rtems_capture_cli_get_name_id (argv[arg], &valid_name, &valid_id, &name, &id))
        return;
    }
  }

  if (!valid_name && !valid_id)
  {
    printf("error: no valid name or task id located\n");
    return;
  }

  sc = rtems_capture_watch_del (name, id);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: watch delete failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("watch delete.\n");
}

/*
 * rtems_capture_cli_watch_control
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that controls a watch.
 *
 */

static char const * watch_control_usage = "usage: cwctl [task name] [id] on/off\n";

static void
rtems_capture_cli_watch_control (int argc, char **argv)
{
  rtems_status_code sc;
  int               arg;
  rtems_name        name = 0;
  rtems_id          id = 0;
  rtems_boolean     valid_name = 0;
  rtems_boolean     valid_id = 0;
  rtems_boolean     enable = 0;

  if (argc <= 2)
  {
    printf (watch_control_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      printf ("warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      if (strcmp (argv[arg], "on") == 0)
        enable = 1;
      else if (strcmp (argv[arg], "off") == 0)
        enable = 0;
      else if (!rtems_capture_cli_get_name_id (argv[arg], &valid_name, &valid_id, &name, &id))
        return;
    }
  }

  if (!valid_name && !valid_id)
  {
    printf("error: no valid name or task id located\n");
    return;
  }

  sc = rtems_capture_watch_ctrl (name, id, enable);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: watch control failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("watch %s.\n", enable ? "enabled" : "disabled");
}

/*
 * rtems_capture_cli_watch_global
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that sets a global watch.
 *
 */

static char const * watch_global_usage = "usage: cwglob on/off\n";

static void
rtems_capture_cli_watch_global (int argc, char **argv)
{
  rtems_status_code sc;
  int               arg;
  rtems_boolean     enable = 0;

  if (argc <= 1)
  {
    printf (watch_global_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      printf ("warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      if (strcmp (argv[arg], "on") == 0)
        enable = 1;
      else if (strcmp (argv[arg], "off") == 0)
        enable = 0;
    }
  }

  sc = rtems_capture_watch_global (enable);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: global watch failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("global watch %s.\n", enable ? "enabled" : "disabled");
}

/*
 * rtems_capture_cli_watch_ceiling
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that sets watch ceiling.
 *
 */

static char const * watch_ceiling_usage = "usage: cwceil priority\n";

static void
rtems_capture_cli_watch_ceiling (int argc, char **argv)
{
  rtems_status_code   sc;
  int                 arg;
  rtems_task_priority priority = 0;

  if (argc <= 1)
  {
    printf (watch_ceiling_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      printf ("warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      priority = strtoul (argv[arg], 0, 0);
    }
  }

  sc = rtems_capture_watch_ceiling (priority);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: watch ceiling failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("watch ceiling is %i.\n", priority);
}

/*
 * rtems_capture_cli_watch_floor
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that sets watch floor.
 *
 */

static char const * watch_floor_usage = "usage: cwfloor priority\n";

static void
rtems_capture_cli_watch_floor (int argc, char **argv)
{
  rtems_status_code   sc;
  int                 arg;
  rtems_task_priority priority = 0;

  if (argc <= 1)
  {
    printf (watch_floor_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      printf ("warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      priority = strtoul (argv[arg], 0, 0);
    }
  }

  sc = rtems_capture_watch_floor (priority);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: watch floor failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("watch floor is %i.\n", priority);
}

/*
 * rtems_capture_cli_trigger_set
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that sets a trigger.
 *
 */

static char const *trigger_set_usage = "usage: ctrig type [from] [fromid] [to] [to id]\n";

static void
rtems_capture_cli_trigger_set (int argc, char **argv)
{
  rtems_status_code       sc;
  int                     arg;
  rtems_capture_trigger_t trigger = rtems_capture_from_to;
  rtems_boolean           trigger_set = 0;
  rtems_name              name = 0;
  rtems_id                id = 0;
  rtems_boolean           valid_name = 0;
  rtems_boolean           valid_id = 0;
  rtems_name              from_name = 0;
  rtems_id                from_id = 0;
  rtems_boolean           from_valid_name = 0;
  rtems_boolean           from_valid_id = 0;
  rtems_name              to_name = 0;
  rtems_id                to_id = 0;
  rtems_boolean           to_valid_name = 0;
  rtems_boolean           to_valid_id = 0;

  if (argc <= 2)
  {
    printf (trigger_set_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      printf ("warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      if (!trigger_set)
      {
        if (strcmp (argv[arg], "from") == 0)
          trigger = rtems_capture_to_any;
        else if (strcmp (argv[arg], "to") == 0)
          trigger = rtems_capture_from_any;
        else if (strcmp (argv[arg], "edge") == 0)
          trigger = rtems_capture_from_any;
        else
        {
          printf ("error: the first argument is the trigger type (from/to/edge)\n");
          return;
        }
        trigger_set = 1;
      }
      else
      {
        if (trigger == rtems_capture_to_any)
        {
          if (from_valid_name && from_valid_id)
            printf ("warning: extra arguments ignored\n");
          else if (!rtems_capture_cli_get_name_id (argv[arg], &from_valid_name, &from_valid_id,
                                           &from_name, &from_id))
            return;
        }
        else if (trigger == rtems_capture_from_any)
        {
          if (to_valid_name && to_valid_id)
            printf ("warning: extra arguments ignored\n");
          else if (!rtems_capture_cli_get_name_id (argv[arg], &to_valid_name, &to_valid_id,
                                           &to_name, &to_id))
            return;
        }
        else if (trigger == rtems_capture_from_to)
        {
          if (from_valid_name && from_valid_id && to_valid_name && to_valid_id)
            printf ("warning: extra arguments ignored\n");
          else
          {
            if (!rtems_capture_cli_get_name_id (argv[arg], &valid_name, &valid_id,
                                        &name, &id))
              return;

            if (valid_name)
            {
              if (!from_valid_name && !from_valid_id)
              {
                from_valid_name = 1;
                from_name       = name;
              }
              else if (to_valid_name)
                printf ("warning: extra arguments ignored\n");
              else
              {
                to_valid_name = 1;
                to_name       = name;
              }
            }
            if (valid_id)
            {
              if (!from_valid_id && !to_valid_name)
              {
                from_valid_id = 1;
                from_id       = id;
              }
              else if (to_valid_id)
                printf ("warning: extra arguments ignored\n");
              else
              {
                to_valid_id = 1;
                to_id       = id;
              }
            }
          }
        }
      }
    }
  }

  if ((trigger == rtems_capture_to_any) && !from_valid_name && !from_valid_id)
  {
    printf ("error: a from trigger need a to name or id\n");
    return;
  }

  if ((trigger == rtems_capture_from_any) && !to_valid_name && !to_valid_id)
  {
    printf ("error: a to trigger need a from name or id\n");
    return;
  }

  if ((trigger == rtems_capture_from_to) &&
      ((!from_valid_name && !from_valid_id) || (!to_valid_name && !to_valid_id)))
  {
    printf ("error: an edge trigger need a from and to name or id\n");
    return;
  }

  sc = rtems_capture_set_trigger (from_name, from_id, to_name, to_id, trigger);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: setting the trigger failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("trigger set.\n");
}

/*
 * rtems_capture_cli_trace_records
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that dumps trace records.
 *
 */

static void
rtems_capture_cli_trace_records (int argc, char **argv)
{
  rtems_status_code       sc;
  rtems_boolean           csv = 0;
  static int              dump_total = 32;
  int                     total;
  int                     count;
  rtems_unsigned32        read;
  rtems_capture_record_t* rec;
  int                     arg;
  
  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      if (argv[arg][1] == 'c')
        csv = 1;
      else if (argv[arg][1] == 'r')
      {
        int i;
        int l;

        arg++;
        if (arg == argc)
        {
          printf ("error: option -r requires number\n");
          return;
        }

        l = strlen (argv[arg]);

        for (i = 0; i < l; i++)
          if (!isdigit (argv[arg][i]))
          {
            printf ("error: option -r requires number and currently it is not\n");
            return;
          }
            
        dump_total = strtoul (argv[arg], 0, 0);
      }
      else
        printf ("warning: option -%c ignored\n", argv[arg][1]);
    }
  }

  total = dump_total;
  
  while (total)
  {
    sc = rtems_capture_read (0, 0, &read, &rec);

    if (sc != RTEMS_SUCCESSFUL)
    {
      printf ("error: trace read failed: %s\n", rtems_status_text (sc));
      rtems_capture_flush (0);
      return;
    }

    if (read == 0)
      break;
    
    for (count = 0; count < read; count++, rec++)
    {
      if (csv)
        printf ("%08x,%03d,%03d,%04x,%d,%d\n",
                (rtems_unsigned32) rec->task,
                (rec->events >> RTEMS_CAPTURE_REAL_PRIORITY_EVENT) & 0xff,
                (rec->events >> RTEMS_CAPTURE_CURR_PRIORITY_EVENT) & 0xff,
                (rec->events >> RTEMS_CAPTURE_EVENT_START),
                rec->ticks, rec->tick_offset);
      else
      {
        unsigned long long t;
        rtems_unsigned32   event;
        int                e;

        event = rec->events >> RTEMS_CAPTURE_EVENT_START;
        
        t  = rec->ticks;
        t *= rtems_capture_tick_time ();
        t += rec->tick_offset;

        for (e = RTEMS_CAPTURE_EVENT_START; e < RTEMS_CAPTURE_EVENT_END; e++)
        {
          if (event & 1)
          {
            printf ("%9li.%06li ", (unsigned long) (t / 1000000),
                    (unsigned long) (t % 1000000));
            rtems_monitor_dump_id (rtems_capture_task_id (rec->task));
            printf (" ");
            rtems_monitor_dump_name (rtems_capture_task_name (rec->task));
            printf (" %3i %3i %s\n",
                    (rec->events >> RTEMS_CAPTURE_REAL_PRIORITY_EVENT) & 0xff,
                    (rec->events >> RTEMS_CAPTURE_CURR_PRIORITY_EVENT) & 0xff,
                    rtems_capture_event_text (e));
          }
          event >>= 1;
        }
      }
    }

    if (read < total)
      total -= read;
    else
      total = 0;

    rtems_capture_release (read);
  }
}

/*
 * rtems_capture_cli_flush
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that flushes and primes the capture
 * engine.
 *
 */

static void
rtems_capture_cli_flush (int argc, char **argv)
{
  rtems_status_code sc;
  rtems_boolean     prime = 1;
  int               arg;

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      if (argv[arg][1] == 'n')
        prime = 0;
      else
        printf ("warning: option -%c ignored\n", argv[arg][1]);
    }
  }

  sc = rtems_capture_flush (prime);

  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: flush failed: %s\n", rtems_status_text (sc));
    return;
  }

  printf ("trace buffer flushed and %s.\n", prime ? "primed" : "not primed");
}

static rtems_monitor_command_entry_t rtems_capture_cli_cmds[] =
{
  {
    "copen",
    "usage: copen [-i] size\n",
    0,
    (void*) rtems_capture_cli_open,
    0,
    0
  },
  {
    "cclose",
    "usage: cclose\n",
    0,
    (void*) rtems_capture_cli_close,
    0,
    0
  },
  {
    "cenable",
    "usage: cenable\n",
    0,
    (void*) rtems_capture_cli_enable,
    0,
    0
  },
  {
    "cdisable",
    "usage: cdisable\n",
    0,
    (void*) rtems_capture_cli_disable,
    0,
    0
  },
  {
    "ctlist",
    "usage: ctlist \n",
    0,
    (void*) rtems_capture_cli_task_list,
    0,
    0
  },
  {
    "ctload",
    "usage: ctload \n",
    0,
    (void*) rtems_capture_cli_task_load,
    0,
    0
  },
  {
    "cwlist",
    "usage: cwlist\n",
    0,
    (void*) rtems_capture_cli_watch_list,
    0,
    0
  },
  {
    "cwadd",
    "usage: cwadd [task name] [id]\n",
    0,
    (void*) rtems_capture_cli_watch_add,
    0,
    0
  },
  {
    "cwdel",
    "usage: cwdel [task name] [id]\n",
    0,
    (void*) rtems_capture_cli_watch_del,
    0,
    0
  },
  {
    "cwctl",
    "usage: cwctl [task name] [id] on/off\n",
    0,
    (void*) rtems_capture_cli_watch_control,
    0,
    0
  },
  {
    "cwglob",
    "usage: cwglob on/off\n",
    0,
    (void*) rtems_capture_cli_watch_global,
    0,
    0
  },
  {
    "cwceil",
    "usage: cwceil priority\n",
    0,
    (void*) rtems_capture_cli_watch_ceiling,
    0,
    0
  },
  {
    "cwfloor",
    "usage: cwfloor priority\n",
    0,
    (void*) rtems_capture_cli_watch_floor,
    0,
    0
  },
  {
    "ctrace",
    "usage: ctrace [-c] [-r records]\n",
    0,
    (void*) rtems_capture_cli_trace_records,
    0,
    0
  },
  {
    "ctrig",
    "usage: ctrig type [from name] [from id] [to name] [to id]\n",
    0,
    (void*) rtems_capture_cli_trigger_set,
    0,
    0
  },
  {
    "cflush",
    "usage: cflush [-n]\n",
    0,
    (void*) rtems_capture_cli_flush,
    0,
    0
  }
};

/*
 * rtems_capture_cli_init
 *
 *  DESCRIPTION:
 *
 * This function initialises the command line interface to the capture
 * engine.
 *
 */

rtems_status_code
rtems_capture_cli_init (rtems_capture_timestamp timestamp)
{
  int cmd;
  
  capture_timestamp = timestamp;

  for (cmd = 0;
       cmd < sizeof (rtems_capture_cli_cmds) / sizeof (rtems_monitor_command_entry_t);
       cmd++)
      rtems_monitor_insert_cmd (&rtems_capture_cli_cmds[cmd]);

  return RTEMS_SUCCESSFUL;
}

