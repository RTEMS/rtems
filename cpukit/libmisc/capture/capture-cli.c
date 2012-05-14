/*
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/capture-cli.h>
#include <rtems/monitor.h>

#define RTEMS_CAPTURE_CLI_MAX_LOAD_TASKS (20)

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
rtems_capture_cli_open (int                          argc,
                        char**                       argv,
                        const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                        bool                         verbose __attribute__((unused)))
{
  uint32_t          size = 0;
  bool              enable = false;
  rtems_status_code sc;
  int               arg;

  if (argc <= 1)
  {
    fprintf (stdout, open_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      if (argv[arg][1] == 'i')
        enable = true;
      else
        fprintf (stdout, "warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      size = strtoul (argv[arg], 0, 0);

      if (size < 100)
      {
        fprintf (stdout, "error: size must be greater than or equal to 100\n");
        return;
      }
    }
  }

  sc = rtems_capture_open (size, capture_timestamp);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: open failed: %s\n", rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "capture engine opened.\n");

  if (!enable)
    return;

  sc = rtems_capture_control (enable);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: open enable failed: %s\n", rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "capture engine enabled.\n");
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
rtems_capture_cli_close (int                          argc __attribute__((unused)),
                         char**                       argv __attribute__((unused)),
                         const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                         bool                         verbose __attribute__((unused)))
{
  rtems_status_code sc;

  sc = rtems_capture_close ();

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: close failed: %s\n", rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "capture engine closed.\n");
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
rtems_capture_cli_enable (int                          argc __attribute__((unused)),
                          char**                       argv __attribute__((unused)),
                          const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                          bool                         verbose __attribute__((unused)))
{
  rtems_status_code sc;

  sc = rtems_capture_control (1);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: enable failed: %s\n", rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "capture engine enabled.\n");
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
rtems_capture_cli_disable (int                          argc __attribute__((unused)),
                           char**                       argv __attribute__((unused)),
                           const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                           bool                         verbose __attribute__((unused)))
{
  rtems_status_code sc;

  sc = rtems_capture_control (0);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: disable failed: %s\n", rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "capture engine disabled.\n");
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
rtems_capture_cli_task_list (int                          argc __attribute__((unused)),
                             char**                       argv __attribute__((unused)),
                             const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                             bool                         verbose __attribute__((unused)))
{
  rtems_task_priority   ceiling = rtems_capture_watch_get_ceiling ();
  rtems_task_priority   floor = rtems_capture_watch_get_floor ();
  rtems_capture_task_t* task = rtems_capture_get_task_list ();
  uint32_t              ticks;
  uint32_t              tick_offset;
  unsigned long long    total_time;
  int                   count = rtems_capture_task_count ();

  if (capture_timestamp)
    capture_timestamp (&ticks, &tick_offset);
  else
  {
    ticks = _Watchdog_Ticks_since_boot;
    tick_offset = 0;
  }

  fprintf (stdout, "total %i\n", count);

  while (task)
  {
    rtems_task_priority priority;
    int32_t             stack_used;
    int32_t             time_used;

    stack_used = rtems_capture_task_stack_usage (task);
    if (stack_used)
      stack_used = (stack_used * 100) / stack_used;

    if (stack_used > 100)
      stack_used = 100;

    total_time = (ticks * rtems_capture_task_time (task)) + tick_offset;

    time_used = (rtems_capture_task_time (task) * 100) / total_time;

    if (time_used > 100)
      time_used = 100;

    priority = rtems_capture_task_real_priority (task);

    fprintf (stdout, " ");
    rtems_monitor_dump_id (rtems_capture_task_id (task));
    fprintf (stdout, " ");
    rtems_monitor_dump_name (rtems_capture_task_name (task));
    fprintf (stdout, " ");
    rtems_monitor_dump_priority (rtems_capture_task_start_priority (task));
    fprintf (stdout, " ");
    rtems_monitor_dump_priority (rtems_capture_task_real_priority (task));
    fprintf (stdout, " ");
    rtems_monitor_dump_priority (rtems_capture_task_curr_priority (task));
    fprintf (stdout, " ");
    rtems_monitor_dump_state (rtems_capture_task_state (task));
    fprintf (stdout, " %c%c",
             rtems_capture_task_valid (task) ? 'a' : 'd',
             rtems_capture_task_flags (task) & RTEMS_CAPTURE_TRACED ? 't' : '-');

    if ((floor > ceiling) && (ceiling > priority))
      fprintf (stdout, "--");
    else
    {
      uint32_t flags = rtems_capture_task_control_flags (task);
      fprintf (stdout, "%c%c",
               rtems_capture_task_control (task) ?
               (flags & RTEMS_CAPTURE_WATCH ? 'w' : '+') : '-',
               rtems_capture_watch_global_on () ? 'g' : '-');
    }
    fprintf (stdout, " %3" PRId32 "%% %3" PRId32 "%% (%" PRIu32 ")\n",
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
rtems_capture_cli_task_load_thread (rtems_task_argument arg __attribute__((unused)))
{
  rtems_task_priority ceiling = rtems_capture_watch_get_ceiling ();
  rtems_task_priority floor = rtems_capture_watch_get_floor ();
  int                 last_count = 0;

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

    total_time = 0;

    while (task)
    {
      if (rtems_capture_task_valid (task))
      {
        unsigned long long l = rtems_capture_task_delta_time (task);

        count++;

        total_time += l;

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

    fprintf (stdout, "\x1b[H\x1b[J Press ENTER to exit.\n\n");
    fprintf (stdout,
             "     PID NAME RPRI CPRI STATE  %%CPU     %%STK FLGS    EXEC TIME\n");

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

      stack_used = rtems_capture_task_stack_usage (tasks[i]);
      if (stack_used)
        stack_used = (stack_used * 100) / stack_used;

      if (stack_used > 100)
        stack_used = 100;

      task_load = (int) ((load[i] * 100000) / total_time);

      priority = rtems_capture_task_real_priority (tasks[i]);

      fprintf (stdout, "\x1b[K");
      rtems_monitor_dump_id (rtems_capture_task_id (tasks[i]));
      fprintf (stdout, " ");
      rtems_monitor_dump_name (rtems_capture_task_name (tasks[i]));
      fprintf (stdout, "  ");
      rtems_monitor_dump_priority (priority);
      fprintf (stdout, "  ");
      rtems_monitor_dump_priority (rtems_capture_task_curr_priority (tasks[i]));
      fprintf (stdout, " ");
      k = rtems_monitor_dump_state (rtems_capture_task_state (tasks[i]));
      fprintf (stdout, "%*c %3i.%03i%% ", 6 - k, ' ',
               task_load / 1000, task_load % 1000);
      fprintf (stdout, "%3i%% %c%c", stack_used,
              rtems_capture_task_valid (tasks[i]) ? 'a' : 'd',
              rtems_capture_task_flags (tasks[i]) & RTEMS_CAPTURE_TRACED ? 't' : '-');

      if ((floor > ceiling) && (ceiling > priority))
        fprintf (stdout, "--");
      else
        fprintf (stdout, "%c%c",
                rtems_capture_task_control (tasks[i]) ?
                (rtems_capture_task_control_flags (tasks[i]) &
                 RTEMS_CAPTURE_WATCH ? 'w' : '+') : '-',
                rtems_capture_watch_global_on () ? 'g' : '-');

      fprintf (stdout, "   %qi\n", rtems_capture_task_time (tasks[i]));
    }

    if (count < RTEMS_CAPTURE_CLI_MAX_LOAD_TASKS)
    {
      j = RTEMS_CAPTURE_CLI_MAX_LOAD_TASKS - count;
      while (j > 0)
      {
        fprintf (stdout, "\x1b[K\n");
        j--;
      }
    }

    last_count = count;

    cli_load_thread_active = 0;

    rtems_task_wake_after (RTEMS_MICROSECONDS_TO_TICKS (5000000));
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
rtems_capture_cli_task_load (int                          argc __attribute__((unused)),
                             char**                       argv __attribute__((unused)),
                             const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                             bool                         verbose __attribute__((unused)))
{
  rtems_status_code   sc;
  rtems_task_priority priority;
  rtems_name          name;
  rtems_id            id;

  sc = rtems_task_set_priority (RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: cannot obtain the current priority: %s\n",
             rtems_status_text (sc));
    return;
  }

  name = rtems_build_name('C', 'P', 'l', 't');

  sc = rtems_task_create (name, priority, 4 * 1024,
                          RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
                          RTEMS_PREEMPT | RTEMS_TIMESLICE | RTEMS_NO_ASR,
                          &id);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: cannot create helper thread: %s\n",
             rtems_status_text (sc));
    return;
  }

  sc = rtems_task_start (id, rtems_capture_cli_task_load_thread, 0);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: cannot start helper thread: %s\n",
             rtems_status_text (sc));
    rtems_task_delete (id);
    return;
  }

  for (;;)
  {
    int c = getchar ();

    if ((c == '\r') || (c == '\n'))
    {
      int loops = 20;

      while (loops && cli_load_thread_active)
        rtems_task_wake_after (RTEMS_MICROSECONDS_TO_TICKS (100000));

      rtems_task_delete (id);

      fprintf (stdout, "load monitoring stopped.\n");

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
rtems_capture_cli_watch_list (int                          argc __attribute__((unused)),
                              char**                       argv __attribute__((unused)),
                              const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                              bool                         verbose __attribute__((unused)))
{
  rtems_capture_control_t* control = rtems_capture_get_control_list ();
  rtems_task_priority      ceiling = rtems_capture_watch_get_ceiling ();
  rtems_task_priority      floor = rtems_capture_watch_get_floor ();

  fprintf (stdout, "watch priority ceiling is %" PRId32 "\n", ceiling);
  fprintf (stdout, "watch priority floor is %" PRId32 "\n", floor);
  fprintf (stdout, "global watch is %s\n",
          rtems_capture_watch_global_on () ? "enabled" : "disabled");
  fprintf (stdout, "total %" PRId32 "\n", rtems_capture_control_count ());

  while (control)
  {
    uint32_t flags;
    int      f;
    int      fshowed;
    int      lf;

    fprintf (stdout, " ");
    rtems_monitor_dump_id (rtems_capture_control_id (control));
    fprintf (stdout, " ");
    rtems_monitor_dump_name (rtems_capture_control_name (control));
    flags = rtems_capture_control_flags (control);
    fprintf (stdout, " %c%c ",
             rtems_capture_watch_global_on () ? 'g' : '-',
             flags & RTEMS_CAPTURE_WATCH ? 'w' : '-');
    flags = rtems_capture_control_to_triggers (control);
    fprintf (stdout, " T:%c%c%c%c%c%c%c",
             flags & RTEMS_CAPTURE_SWITCH    ? 'S' : '-',
             flags & RTEMS_CAPTURE_CREATE ? 'C' : '-',
             flags & RTEMS_CAPTURE_START ? 'S' : '-',
             flags & RTEMS_CAPTURE_RESTART ? 'R' : '-',
             flags & RTEMS_CAPTURE_DELETE ? 'D' : '-',
             flags & RTEMS_CAPTURE_BEGIN ? 'B' : '-',
             flags & RTEMS_CAPTURE_EXITTED ? 'E' : '-');
    flags = rtems_capture_control_from_triggers (control);
    fprintf (stdout, " F:%c%c%c%c%c",
             flags & RTEMS_CAPTURE_SWITCH  ? 'S' : '-',
             flags & RTEMS_CAPTURE_CREATE  ? 'C' : '-',
             flags & RTEMS_CAPTURE_START   ? 'S' : '-',
             flags & RTEMS_CAPTURE_RESTART ? 'R' : '-',
             flags & RTEMS_CAPTURE_DELETE  ? 'D' : '-');

    for (f = 0, fshowed = 0, lf = 1; f < RTEMS_CAPTURE_TRIGGER_TASKS; f++)
    {
      if (rtems_capture_control_by_valid (control, f))
      {
        if (lf && ((fshowed % 3) == 0))
        {
          fprintf (stdout, "\n");
          lf = 0;
        }

        fprintf (stdout, "  %2i:", f);
        rtems_monitor_dump_name (rtems_capture_control_by_name (control, f));
        fprintf (stdout, "/");
        rtems_monitor_dump_id (rtems_capture_control_by_id (control, f));
        flags = rtems_capture_control_by_triggers (control, f);
        fprintf (stdout, ":%c%c%c%c%c",
                 flags & RTEMS_CAPTURE_SWITCH  ? 'S' : '-',
                 flags & RTEMS_CAPTURE_CREATE  ? 'C' : '-',
                 flags & RTEMS_CAPTURE_START   ? 'S' : '-',
                 flags & RTEMS_CAPTURE_RESTART ? 'R' : '-',
                 flags & RTEMS_CAPTURE_DELETE  ? 'D' : '-');
        fshowed++;
        lf = 1;
      }
    }

    if (lf)
      fprintf (stdout, "\n");

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

static bool
rtems_capture_cli_get_name_id (char*          arg,
                               bool*          valid_name,
                               bool*          valid_id,
                               rtems_name*    name,
                               rtems_id*      id)
{
  size_t l;
  size_t i;

  if (*valid_name && *valid_id)
  {
    fprintf (stdout, "error: too many arguments\n");
    return 0;
  }

  /*
   * See if the arg is all hex digits.
   */

  l = strlen (arg);

  for (i = 0; i < l; i++)
    if (!isxdigit ((unsigned char)arg[i]))
      break;

  if (i == l)
  {
    *id = strtoul (arg, 0, 16);
    *valid_id = true;
  }
  else
  {
    /*
     * This is a bit of hack but it should work on all platforms
     * as it is what the score does with names.
     *
     * @warning The extra assigns play with the byte order so do not
     *          remove unless the score has been updated.
     */
    rtems_name   rname;

    rname = rtems_build_name(arg[0], arg[1], arg[2], arg[3]);
    *name = rname;
    *valid_name = true;
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
rtems_capture_cli_watch_add (int                          argc,
                             char**                       argv,
                             const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                             bool                         verbose __attribute__((unused)))
{
  rtems_status_code sc;
  int               arg;
  rtems_name        name = 0;
  rtems_id          id = 0;
  bool              valid_name = false;
  bool              valid_id = false;

  if (argc <= 1)
  {
    fprintf (stdout, watch_add_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      fprintf (stdout, "warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      if (!rtems_capture_cli_get_name_id (argv[arg], &valid_name, &valid_id,
                                          &name, &id))
        return;
    }
  }

  if (!valid_name && !valid_id)
  {
    fprintf (stdout, "error: no valid name or task id located\n");
    return;
  }

  sc = rtems_capture_watch_add (name, id);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout,
             "error: watch add failed: %s\n", rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "watch added.\n");
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
rtems_capture_cli_watch_del (int                          argc,
                             char**                       argv,
                             const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                             bool                         verbose __attribute__((unused)))
{
  rtems_status_code sc;
  int               arg;
  rtems_name        name = 0;
  rtems_id          id = 0;
  bool              valid_name = false;
  bool              valid_id = false;

  if (argc <= 1)
  {
    fprintf (stdout, watch_del_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      fprintf (stdout, "warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      if (!rtems_capture_cli_get_name_id (argv[arg], &valid_name, &valid_id,
                                          &name, &id))
        return;
    }
  }

  if (!valid_name && !valid_id)
  {
    fprintf (stdout, "error: no valid name or task id located\n");
    return;
  }

  sc = rtems_capture_watch_del (name, id);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: watch delete failed: %s\n",
             rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "watch delete.\n");
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
rtems_capture_cli_watch_control (int                          argc,
                                 char**                       argv,
                                 const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                                 bool                         verbose __attribute__((unused)))
{
  rtems_status_code sc;
  int               arg;
  rtems_name        name = 0;
  rtems_id          id = 0;
  bool              valid_name = false;
  bool              valid_id = false;
  bool              enable = false;

  if (argc <= 2)
  {
    fprintf (stdout, watch_control_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      fprintf (stdout, "warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      if (strcmp (argv[arg], "on") == 0)
        enable = true;
      else if (strcmp (argv[arg], "off") == 0)
        enable = false;
      else if (!rtems_capture_cli_get_name_id (argv[arg], &valid_name,
                                               &valid_id, &name, &id))
        return;
    }
  }

  if (!valid_name && !valid_id)
  {
    fprintf (stdout, "error: no valid name or task id located\n");
    return;
  }

  sc = rtems_capture_watch_ctrl (name, id, enable);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: watch control failed: %s\n",
             rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "watch %s.\n", enable ? "enabled" : "disabled");
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
rtems_capture_cli_watch_global (int                          argc,
                                char**                       argv,
                                const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                                bool                         verbose __attribute__((unused)))
{
  rtems_status_code sc;
  int               arg;
  bool              enable = false;

  if (argc <= 1)
  {
    fprintf (stdout, watch_global_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      fprintf (stdout, "warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      if (strcmp (argv[arg], "on") == 0)
        enable = true;
      else if (strcmp (argv[arg], "off") == 0)
        enable = false;
    }
  }

  sc = rtems_capture_watch_global (enable);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: global watch failed: %s\n",
             rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "global watch %s.\n", enable ? "enabled" : "disabled");
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
rtems_capture_cli_watch_ceiling (int                          argc,
                                 char**                       argv,
                                 const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                                 bool                         verbose __attribute__((unused)))
{
  rtems_status_code   sc;
  int                 arg;
  rtems_task_priority priority = 0;

  if (argc <= 1)
  {
    fprintf (stdout, watch_ceiling_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      fprintf (stdout, "warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      priority = strtoul (argv[arg], 0, 0);
    }
  }

  sc = rtems_capture_watch_ceiling (priority);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: watch ceiling failed: %s\n",
             rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "watch ceiling is %" PRId32 ".\n", priority);
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
rtems_capture_cli_watch_floor (int                          argc,
                               char**                       argv,
                               const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                               bool                         verbose __attribute__((unused)))
{
  rtems_status_code   sc;
  int                 arg;
  rtems_task_priority priority = 0;

  if (argc <= 1)
  {
    fprintf (stdout, watch_floor_usage);
    return;
  }

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      fprintf (stdout, "warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      priority = strtoul (argv[arg], 0, 0);
    }
  }

  sc = rtems_capture_watch_floor (priority);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: watch floor failed: %s\n",
             rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "watch floor is %" PRId32 ".\n", priority);
}

/*
 * rtems_capture_cli_trigger_worker
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that sets or clears a trigger.
 *
 */

static char const *trigger_set_usage =
      "usage: %s [-?] type [to name/id] [from] [from name/id]\n";

static char const *trigger_set_types =
      "  You can say 'type TASK' or 'type TO from FROM'\n" \
      "  where TASK is the task the event is happening to\n" \
      "  or you can say the event TO this task FROM this task.\n" \
      "  No type defaults to 'switch'.\n" \
      "   switch  : context switch TASK or FROM or FROM->TO\n" \
      "   create  : create TASK, or create TO from FROM\n" \
      "   start   : start TASK, or start TO from FROM\n" \
      "   restart : restart TASK, or restart TO from FROM\n" \
      "   delete  : delete TASK or delete TO from FROM\n" \
      "   begin   : begin TASK\n" \
      "   exitted : exitted TASK\n";

/*
 * Structure to handle the parsing of the trigger command line.
 */
typedef struct rtems_capture_cli_triggers_s
{
  char const *            name;
  rtems_capture_trigger_t type;
  int                     to_only;
} rtems_capture_cli_triggers_t;

static rtems_capture_cli_triggers_t rtems_capture_cli_triggers[] =
{
  { "switch",  rtems_capture_switch,  0 }, /* must be first */
  { "create",  rtems_capture_create,  0 },
  { "start",   rtems_capture_start,   0 },
  { "restart", rtems_capture_restart, 0 },
  { "delete",  rtems_capture_delete,  0 },
  { "begin",   rtems_capture_begin,   1 },
  { "exitted", rtems_capture_exitted, 1 }
};

typedef enum rtems_capture_cli_trig_state_e
{
  trig_type,
  trig_to,
  trig_from_from,
  trig_from
} rtems_capture_cli_trig_state_t;

#define RTEMS_CAPTURE_CLI_TRIGGERS_NUM \
  (sizeof (rtems_capture_cli_triggers) / sizeof (rtems_capture_cli_triggers_t))

static void
rtems_capture_cli_trigger_worker (int set, int argc, char** argv)
{
  rtems_status_code            sc;
  int                          arg;
  int                          trigger = 0; /* switch */
  rtems_capture_trigger_mode_t trigger_mode = rtems_capture_from_any;
  bool                         trigger_set = false;
  bool                         is_from = false;
  rtems_name                   name = 0;
  rtems_id                     id = 0;
  bool                         valid_name = false;
  bool                         valid_id = false;
  rtems_name                   from_name = 0;
  rtems_id                     from_id = 0;
  bool                         from_valid_name = false;
  bool                         from_valid_id = false;
  rtems_name                   to_name = 0;
  rtems_id                     to_id = 0;
  bool                         to_valid_name = false;
  bool                         to_valid_id = false;

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      switch (argv[arg][1])
      {
        case '?':
          fprintf (stdout, trigger_set_usage, set ? "ctset" : "ctclear");
          fprintf (stdout, trigger_set_types);
          return;
        default:
          fprintf (stdout, "warning: option -%c ignored\n", argv[arg][1]);
          break;
      }
    }
    else
    {
      if (!trigger_set)
      {
        bool found = false;
        int  t;

        for (t = 0; t < RTEMS_CAPTURE_CLI_TRIGGERS_NUM; t++)
          if (strcmp (argv[arg], rtems_capture_cli_triggers[t].name) == 0)
          {
            trigger = t;
            found = true;
            break;
          }

        trigger_set = true;

        /*
         * If a trigger was not found assume the default and
         * assume the parameter is a task name or id.
         */
        if (found)
          continue;
      }

      if (strcmp (arg[argv], "from") == 0)
      {
        if (is_from)
          fprintf (stdout, "warning: extra 'from' ignored\n");

        is_from = 1;
        continue;
      }

      if (!rtems_capture_cli_get_name_id (argv[arg], &valid_name, &valid_id,
                                          &name, &id))
        return;

      if (valid_name)
      {
        if (is_from)
        {
          if (!from_valid_name && !from_valid_id)
          {
            from_valid_name = true;
            from_name       = name;
          }
          else
            fprintf (stdout, "warning: extra arguments ignored\n");
        }
        else if (!to_valid_name && !to_valid_id)
        {
          to_valid_name = true;
          to_name       = name;
        }
        else
          fprintf (stdout, "warning: extra arguments ignored\n");
      }

      if (valid_id)
      {
        if (is_from)
        {
          if (!from_valid_name && !from_valid_id)
          {
            from_valid_id = true;
            from_id       = id;
          }
          else
            fprintf (stdout, "warning: extra arguments ignored\n");
        }
        else if (!to_valid_name && !to_valid_id)
        {
          to_valid_id = true;
          to_id       = id;
        }
        else
          fprintf (stdout, "warning: extra arguments ignored\n");
      }
    }
  }

  if (is_from && rtems_capture_cli_triggers[trigger].to_only)
  {
    fprintf (stdout, "error: a %s trigger can be a TO trigger\n",
             rtems_capture_cli_triggers[trigger].name);
    return;
  }

  if (!to_valid_name && !to_valid_id && !from_valid_name && !from_valid_id)
  {
    fprintf (stdout, trigger_set_usage);
    return;
  }

  if (!is_from && !to_valid_name && !to_valid_id)
  {
    fprintf (stdout, "error: a %s trigger needs a TO name or id\n",
             rtems_capture_cli_triggers[trigger].name);
    return;
  }

  if (is_from && !from_valid_name && !from_valid_id)
  {
    fprintf (stdout, "error: a %s trigger needs a FROM name or id\n",
             rtems_capture_cli_triggers[trigger].name);
    return;
  }

  if ((from_valid_name || from_valid_id) && (to_valid_name || to_valid_id))
    trigger_mode = rtems_capture_from_to;
  else if (from_valid_name || from_valid_id)
    trigger_mode = rtems_capture_to_any;
  else if (to_valid_name || to_valid_id)
    trigger_mode = rtems_capture_from_any;

  if (set)
    sc = rtems_capture_set_trigger (from_name, from_id, to_name, to_id,
                                    trigger_mode,
                                    rtems_capture_cli_triggers[trigger].type);
  else
    sc = rtems_capture_clear_trigger (from_name, from_id, to_name, to_id,
                                      trigger_mode,
                                      rtems_capture_cli_triggers[trigger].type);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: %sing the trigger failed: %s\n",
             set ? "sett" : "clear", rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "trigger %s.\n", set ? "set" : "cleared");
}

/*
 * rtems_capture_cli_trigger_set
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that sets a trigger.
 *
 */

static void
rtems_capture_cli_trigger_set (int                          argc,
                               char**                       argv,
                               const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                               bool                         verbose __attribute__((unused)))
{
  rtems_capture_cli_trigger_worker (1, argc, argv);
}

/*
 * rtems_capture_cli_trigger_clear
 *
 *  DESCRIPTION:
 *
 * This function is a monitor command that clears a trigger.
 *
 */

static void
rtems_capture_cli_trigger_clear (int                          argc,
                                 char**                       argv,
                                 const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                                 bool                         verbose __attribute__((unused)))
{
  rtems_capture_cli_trigger_worker (0, argc, argv);
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
rtems_capture_cli_trace_records (int                          argc,
                                 char**                       argv,
                                 const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                                 bool                         verbose __attribute__((unused)))
{
  rtems_status_code       sc;
  bool                    csv = false;
  static int              dump_total = 22;
  int                     total;
  int                     count;
  uint32_t                read;
  rtems_capture_record_t* rec;
  int                     arg;

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      if (argv[arg][1] == 'c')
        csv = true;
      else
        fprintf (stdout, "warning: option -%c ignored\n", argv[arg][1]);
    }
    else
    {
      size_t i;
      size_t l;

      l = strlen (argv[arg]);

      for (i = 0; i < l; i++)
        if (!isdigit ((unsigned char)argv[arg][i]))
        {
          fprintf (stdout, "error: not a number\n");
          return;
        }

      dump_total = strtoul (argv[arg], 0, 0);
    }
  }

  total = dump_total;

  while (total)
  {
    sc = rtems_capture_read (0, 0, &read, &rec);

    if (sc != RTEMS_SUCCESSFUL)
    {
      fprintf (stdout, "error: trace read failed: %s\n", rtems_status_text (sc));
      rtems_capture_flush (0);
      return;
    }

    /*
     * If we have no records then just exist. We still need to release
     * the reader lock.
     */

    if (read == 0)
    {
      rtems_capture_release (read);
      break;
    }

    count = total < read ? total : read;

    while (count--)
    {
      if (csv)
        fprintf (stdout, "%08" PRIxPTR ",%03" PRIu32
                   ",%03" PRIu32 ",%04" PRIx32 ",%" PRId32 ",%" PRId32 "\n",
                (uintptr_t) rec->task,
                (rec->events >> RTEMS_CAPTURE_REAL_PRIORITY_EVENT) & 0xff,
                (rec->events >> RTEMS_CAPTURE_CURR_PRIORITY_EVENT) & 0xff,
                (rec->events >> RTEMS_CAPTURE_EVENT_START),
                rec->ticks, rec->tick_offset);
      else
      {
        unsigned long long t;
        uint32_t     event;
        int                e;

        event = rec->events >> RTEMS_CAPTURE_EVENT_START;

        t  = rec->ticks;
        t *= rtems_capture_tick_time ();
        t += rec->tick_offset;

        for (e = RTEMS_CAPTURE_EVENT_START; e < RTEMS_CAPTURE_EVENT_END; e++)
        {
          if (event & 1)
          {
            fprintf (stdout, "%9li.%06li ", (unsigned long) (t / 1000000),
                    (unsigned long) (t % 1000000));
            rtems_monitor_dump_id (rtems_capture_task_id (rec->task));
            fprintf (stdout, " ");
            rtems_monitor_dump_name (rtems_capture_task_name (rec->task));
            fprintf (stdout, " %3" PRId32 " %3" PRId32 " %s\n",
                    (rec->events >> RTEMS_CAPTURE_REAL_PRIORITY_EVENT) & 0xff,
                    (rec->events >> RTEMS_CAPTURE_CURR_PRIORITY_EVENT) & 0xff,
                    rtems_capture_event_text (e));
          }
          event >>= 1;
        }
      }
      rec++;
    }

    count = total < read ? total : read;

    if (count < total)
      total -= count;
    else
      total = 0;

    rtems_capture_release (count);
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
rtems_capture_cli_flush (int                          argc,
                         char**                       argv,
                         const rtems_monitor_command_arg_t* command_arg __attribute__((unused)),
                         bool                         verbose __attribute__((unused)))
{
  rtems_status_code sc;
  bool              prime = true;
  int               arg;

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      if (argv[arg][1] == 'n')
        prime = false;
      else
        fprintf (stdout, "warning: option -%c ignored\n", argv[arg][1]);
    }
  }

  sc = rtems_capture_flush (prime);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: flush failed: %s\n", rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "trace buffer flushed and %s.\n",
           prime ? "primed" : "not primed");
}

static rtems_monitor_command_entry_t rtems_capture_cli_cmds[] =
{
  {
    "copen",
    "usage: copen [-i] size\n",
    0,
    rtems_capture_cli_open,
    { 0 },
    0
  },
  {
    "cclose",
    "usage: cclose\n",
    0,
    rtems_capture_cli_close,
    { 0 },
    0
  },
  {
    "cenable",
    "usage: cenable\n",
    0,
    rtems_capture_cli_enable,
    { 0 },
    0
  },
  {
    "cdisable",
    "usage: cdisable\n",
    0,
    rtems_capture_cli_disable,
    { 0 },
    0
  },
  {
    "ctlist",
    "usage: ctlist \n",
    0,
     rtems_capture_cli_task_list,
    { 0 },
    0
  },
  {
    "ctload",
    "usage: ctload \n",
    0,
    rtems_capture_cli_task_load,
    { 0 },
    0
  },
  {
    "cwlist",
    "usage: cwlist\n",
    0,
    rtems_capture_cli_watch_list,
    { 0 },
    0
  },
  {
    "cwadd",
    "usage: cwadd [task name] [id]\n",
    0,
    rtems_capture_cli_watch_add,
    { 0 },
    0
  },
  {
    "cwdel",
    "usage: cwdel [task name] [id]\n",
    0,
    rtems_capture_cli_watch_del,
    { 0 },
    0
  },
  {
    "cwctl",
    "usage: cwctl [task name] [id] on/off\n",
    0,
    rtems_capture_cli_watch_control,
    { 0 },
    0
  },
  {
    "cwglob",
    "usage: cwglob on/off\n",
    0,
    rtems_capture_cli_watch_global,
    { 0 },
    0
  },
  {
    "cwceil",
    "usage: cwceil priority\n",
    0,
    rtems_capture_cli_watch_ceiling,
    { 0 },
    0
  },
  {
    "cwfloor",
    "usage: cwfloor priority\n",
    0,
    rtems_capture_cli_watch_floor,
    { 0 },
    0
  },
  {
    "ctrace",
    "usage: ctrace [-c] [-r records]\n",
    0,
    rtems_capture_cli_trace_records,
    { 0 },
    0
  },
  {
    "ctset",
    "usage: ctset -h\n",
    0,
    rtems_capture_cli_trigger_set,
    { 0 },
    0
  },
  {
    "ctclear",
    "usage: ctclear -?\n",
    0,
    rtems_capture_cli_trigger_clear,
    { 0 },
    0
  },
  {
    "cflush",
    "usage: cflush [-n]\n",
    0,
    rtems_capture_cli_flush,
    { 0 },
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
  size_t cmd;

  capture_timestamp = timestamp;

  for (cmd = 0;
       cmd < sizeof (rtems_capture_cli_cmds) / sizeof (rtems_monitor_command_entry_t);
       cmd++)
      rtems_monitor_insert_cmd (&rtems_capture_cli_cmds[cmd]);

  return RTEMS_SUCCESSFUL;
}
