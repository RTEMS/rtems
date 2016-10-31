/*
  ------------------------------------------------------------------------

  Copyright 2002, 2015 Chris Johns <chrisj@rtems.org>
  All rights reserved.

  COPYRIGHT (c) 2014.
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
#include <rtems/captureimpl.h>
#include <rtems/monitor.h>
#include <rtems/cpuuse.h>
#
#define RC_UNUSED RTEMS_UNUSED

#define RTEMS_CAPTURE_CLI_MAX_LOAD_TASKS (20)

/*
 * The user capture timestamper.
 */
static rtems_capture_timestamp capture_timestamp;

/*
 * rtems_capture_cli_open
 *
 * This function opens the capture engine. We need the size of the
 * capture buffer.
 */

static const char* open_usage = "usage: copen [-i] size\n";

static void
rtems_capture_cli_open (int                                argc,
                        char**                             argv,
                        const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                        bool                               verbose RC_UNUSED)
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

  sc = rtems_capture_set_control (enable);

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
 * This function closes the capture engine.
 */

static void
rtems_capture_cli_close (int                                argc RC_UNUSED,
                         char**                             argv RC_UNUSED,
                         const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                         bool                               verbose RC_UNUSED)
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
 * This function enables the capture engine.
 */

static void
rtems_capture_cli_enable (int                                argc RC_UNUSED,
                          char**                             argv RC_UNUSED,
                          const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                          bool                               verbose RC_UNUSED)
{
  rtems_status_code sc;

  sc = rtems_capture_set_control (true);

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
 * This function disables the capture engine.
 */

static void
rtems_capture_cli_disable (int                                argc RC_UNUSED,
                           char**                             argv RC_UNUSED,
                           const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                           bool                               verbose RC_UNUSED)
{
  rtems_status_code sc;

  sc = rtems_capture_set_control (false);

  if (sc != RTEMS_SUCCESSFUL)
  {
    fprintf (stdout, "error: disable failed: %s\n", rtems_status_text (sc));
    return;
  }

  fprintf (stdout, "capture engine disabled.\n");
}

static bool
rtems_capture_cli_print_task (rtems_tcb *tcb, void *arg)
{
  rtems_task_priority   ceiling = rtems_capture_watch_get_ceiling ();
  rtems_task_priority   floor = rtems_capture_watch_get_floor ();
  rtems_task_priority   priority;
  int                   length;
  uint32_t              flags = rtems_capture_task_control_flags (tcb);

  priority = rtems_capture_task_real_priority (tcb);

  fprintf (stdout, " ");
  rtems_monitor_dump_id (rtems_capture_task_id (tcb));
  fprintf (stdout, " ");
  if (rtems_capture_task_api (rtems_capture_task_id (tcb)) != OBJECTS_POSIX_API)
  {
    rtems_monitor_dump_name (rtems_capture_task_id (tcb));
    fprintf (stdout, " ");
  }
  else
  {
    fprintf (stdout, "     ");
  }
  rtems_monitor_dump_priority (rtems_capture_task_start_priority (tcb));
  fprintf (stdout, " ");
  rtems_monitor_dump_priority (rtems_capture_task_real_priority (tcb));
  fprintf (stdout, " ");
  rtems_monitor_dump_priority (rtems_capture_task_curr_priority (tcb));
  fprintf (stdout, " ");
  length = rtems_monitor_dump_state (rtems_capture_task_state (tcb));
  fprintf (stdout, "%*c", 14 - length, ' ');
  fprintf (stdout, " %c%c",
           'a',
           flags & RTEMS_CAPTURE_TRACED ? 't' : '-');

  if ((floor > ceiling) && (ceiling > priority))
    fprintf (stdout, "--");
  else
  {
    fprintf (stdout, "%c%c",
             rtems_capture_task_control (tcb) ?
             (flags & RTEMS_CAPTURE_WATCH ? 'w' : '+') : '-',
             rtems_capture_watch_global_on () ? 'g' : '-');
  }
  fprintf (stdout, "\n");
  return false;
}

/*
 * rtems_capture_cli_count_tasks
 *
 * This function is called for each tcb and counts the
 * number of tasks.
 */

static bool
rtems_capture_cli_count_tasks (rtems_tcb *tcb, void *arg)
{
  uint32_t *task_count = arg;
  ++(*task_count);
  return false;
}


/*
 * rtems_capture_cli_task_list
 *
 * This function lists the tasks the capture engine knows about.
 */

static void
rtems_capture_cli_task_list (int                                argc RC_UNUSED,
                             char**                             argv RC_UNUSED,
                             const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                             bool                               verbose RC_UNUSED)
{
  rtems_capture_time uptime;
  uint32_t           task_count;

  rtems_capture_get_time (&uptime);

  task_count = 0;
  rtems_task_iterate (rtems_capture_cli_count_tasks, &task_count);

  fprintf (stdout, "uptime: ");
  rtems_capture_print_timestamp (uptime);
  fprintf (stdout, "\ntotal %" PRIu32 "\n", task_count);
  rtems_task_iterate (rtems_capture_cli_print_task, NULL);
}

/*
 * rtems_capture_cli_watch_list
 *
 * This function lists the controls in the capture engine.
 */

static void
rtems_capture_cli_watch_list (int                                argc RC_UNUSED,
                              char**                             argv RC_UNUSED,
                              const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                              bool                               verbose RC_UNUSED)
{
  rtems_capture_print_watch_list();
}

/*
 * rtems_capture_cli_get_name_id
 *
 * This function checks arguments for a name or an id.
 */

static bool
rtems_capture_cli_get_name_id (char*       arg,
                               bool*       valid_name,
                               bool*       valid_id,
                               rtems_name* name,
                               rtems_id*   id)
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
 * This function is a monitor command that add a watch to the capture
 * engine.
 */

static char const * watch_add_usage = "usage: cwadd [task name] [id]\n";

static void
rtems_capture_cli_watch_add (int                                argc,
                             char**                             argv,
                             const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                             bool                               verbose RC_UNUSED)
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
 * This function is a monitor command that deletes a watch from the capture
 * engine.
 */

static char const * watch_del_usage = "usage: cwdel [task name] [id]\n";

static void
rtems_capture_cli_watch_del (int                                argc,
                             char**                             argv,
                             const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                             bool                               verbose RC_UNUSED)
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
 * This function is a monitor command that controls a watch.
 */

static char const * watch_control_usage = "usage: cwctl [task name] [id] on/off\n";

static void
rtems_capture_cli_watch_control (int                                argc,
                                 char**                             argv,
                                 const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                                 bool                               verbose RC_UNUSED)
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
 * This function is a monitor command that sets a global watch.
 */

static char const * watch_global_usage = "usage: cwglob on/off\n";

static void
rtems_capture_cli_watch_global (int                                argc,
                                char**                             argv,
                                const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                                bool                               verbose RC_UNUSED)
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
 * This function is a monitor command that sets watch ceiling.
 */

static char const * watch_ceiling_usage = "usage: cwceil priority\n";

static void
rtems_capture_cli_watch_ceiling (int                                argc,
                                 char**                             argv,
                                 const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                                 bool                               verbose RC_UNUSED)
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
 * This function is a monitor command that sets watch floor.
 */

static char const * watch_floor_usage = "usage: cwfloor priority\n";

static void
rtems_capture_cli_watch_floor (int                                argc,
                               char**                             argv,
                               const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                               bool                               verbose RC_UNUSED)
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
 * This function is a monitor command that sets or clears a trigger.
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
typedef struct rtems_capture_cli_triggers
{
  char const *          name;
  rtems_capture_trigger type;
  int                   to_only;
} rtems_capture_cli_triggers;

static const rtems_capture_cli_triggers rtems_capture_cli_trigger[] =
{
  { "switch",  rtems_capture_switch,  0 }, /* must be first */
  { "create",  rtems_capture_create,  0 },
  { "start",   rtems_capture_start,   0 },
  { "restart", rtems_capture_restart, 0 },
  { "delete",  rtems_capture_delete,  0 },
  { "begin",   rtems_capture_begin,   1 },
  { "exitted", rtems_capture_exitted, 1 }
};

typedef enum rtems_capture_cli_trig_state
{
  trig_type,
  trig_to,
  trig_from_from,
  trig_from
} rtems_capture_cli_trig_state;

#define RTEMS_CAPTURE_CLI_TRIGGERS_NUM \
  (sizeof (rtems_capture_cli_trigger) / sizeof (rtems_capture_cli_triggers))

static void
rtems_capture_cli_trigger_worker (int set, int argc, char** argv)
{
  rtems_status_code          sc;
  int                        arg;
  int                        trigger = 0; /* switch */
  rtems_capture_trigger_mode trigger_mode = rtems_capture_from_any;
  bool                       trigger_set = false;
  bool                       is_from = false;
  bool                       is_to = false;
  rtems_name                 name = 0;
  rtems_id                   id = 0;
  bool                       valid_name = false;
  bool                       valid_id = false;
  rtems_name                 from_name = 0;
  rtems_id                   from_id = 0;
  bool                       from_valid_name = false;
  bool                       from_valid_id = false;
  rtems_name                 to_name = 0;
  rtems_id                   to_id = 0;
  bool                       to_valid_name = false;
  bool                       to_valid_id = false;

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
          if (strcmp (argv[arg], rtems_capture_cli_trigger[t].name) == 0)
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

      if (strcmp (argv[arg], "from") == 0)
      {
        if (from_valid_name || from_valid_id)
          fprintf (stdout, "warning: extra 'from' ignored\n");

        is_from = true;
        continue;
      }

      if (strcmp (argv[arg], "to") == 0)
      {
        if (to_valid_name || from_valid_id)
          fprintf (stdout, "warning: extra 'to' ignored\n");

        is_to = true;
        continue;
      }

      if (!rtems_capture_cli_get_name_id (argv[arg], &valid_name, &valid_id,
                                          &name, &id))
        return;

      if (valid_name)
      {
        if (!is_from && !is_to)
          is_to = true;

        if (is_from)
        {
          if (!from_valid_name && !from_valid_id)
          {
            from_valid_name = true;
            from_name       = name;
          }
          else
            fprintf (stdout, "warning: extra name arguments ignored\n");
        }
        else if (!to_valid_name && !to_valid_id)
        {
          to_valid_name = true;
          to_name       = name;
        }
        else
          fprintf (stdout, "warning: extra name arguments ignored\n");
      }

      if (valid_id)
      {
        if (!is_from && !is_to)
          is_to = true;

        if (is_from)
        {
          if (!from_valid_name && !from_valid_id)
          {
            from_valid_id = true;
            from_id       = id;
          }
          else
            fprintf (stdout, "warning: extra id arguments ignored\n");
        }
        else if (!to_valid_name && !to_valid_id)
        {
          to_valid_id = true;
          to_id       = id;
        }
        else
          fprintf (stdout, "warning: extra id arguments ignored\n");
      }
    }
  }

  if (is_from && rtems_capture_cli_trigger[trigger].to_only)
  {
    fprintf (stdout, "error: a %s trigger can be a TO trigger\n",
             rtems_capture_cli_trigger[trigger].name);
    return;
  }

  if (!to_valid_name && !to_valid_id && !from_valid_name && !from_valid_id)
  {
    fprintf (stdout, trigger_set_usage, set ? "ctset" : "ctclear");
    return;
  }

  if (!is_from && !to_valid_name && !to_valid_id)
  {
    fprintf (stdout, "error: a %s trigger needs a TO name or id\n",
             rtems_capture_cli_trigger[trigger].name);
    return;
  }

  if (is_from && !from_valid_name && !from_valid_id)
  {
    fprintf (stdout, "error: a %s trigger needs a FROM name or id\n",
             rtems_capture_cli_trigger[trigger].name);
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
                                    rtems_capture_cli_trigger[trigger].type);
  else
    sc = rtems_capture_clear_trigger (from_name, from_id, to_name, to_id,
                                      trigger_mode,
                                      rtems_capture_cli_trigger[trigger].type);

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
 * This function is a monitor command that sets a trigger.
 */

static void
rtems_capture_cli_trigger_set (int                                argc,
                               char**                             argv,
                               const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                               bool                               verbose RC_UNUSED)
{
  rtems_capture_cli_trigger_worker (1, argc, argv);
}

/*
 * rtems_capture_cli_trigger_clear
 *
 * This function is a monitor command that clears a trigger.
 */

static void
rtems_capture_cli_trigger_clear (int                                argc,
                                 char**                             argv,
                                 const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                                 bool                               verbose RC_UNUSED)
{
  rtems_capture_cli_trigger_worker (0, argc, argv);
}

/*
 * rtems_capture_cli_trace_records
 *
 * This function is a monitor command that dumps trace records.
 */

static void
rtems_capture_cli_trace_records (int                                argc,
                                 char**                             argv,
                                 const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                                 bool                               verbose RC_UNUSED)
{
  bool                    csv = false;
  static int              dump_total = 22;
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

  rtems_capture_print_trace_records( dump_total, csv );
}

/*
 * rtems_capture_cli_flush
 *
 * This function is a monitor command that flushes and primes the capture
 * engine.
 */

static void
rtems_capture_cli_flush (int                                argc,
                         char**                             argv,
                         const rtems_monitor_command_arg_t* command_arg RC_UNUSED,
                         bool                               verbose RC_UNUSED)
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
    "usage: copen [-i] size",
    0,
    rtems_capture_cli_open,
    { 0 },
    0
  },
  {
    "cclose",
    "usage: cclose",
    0,
    rtems_capture_cli_close,
    { 0 },
    0
  },
  {
    "cenable",
    "usage: cenable",
    0,
    rtems_capture_cli_enable,
    { 0 },
    0
  },
  {
    "cdisable",
    "usage: cdisable",
    0,
    rtems_capture_cli_disable,
    { 0 },
    0
  },
  {
    "ctlist",
    "usage: ctlist",
    0,
     rtems_capture_cli_task_list,
    { 0 },
    0
  },
  {
    "cwlist",
    "usage: cwlist",
    0,
    rtems_capture_cli_watch_list,
    { 0 },
    0
  },
  {
    "cwadd",
    "usage: cwadd [task name] [id]",
    0,
    rtems_capture_cli_watch_add,
    { 0 },
    0
  },
  {
    "cwdel",
    "usage: cwdel [task name] [id]",
    0,
    rtems_capture_cli_watch_del,
    { 0 },
    0
  },
  {
    "cwctl",
    "usage: cwctl [task name] [id] on/off",
    0,
    rtems_capture_cli_watch_control,
    { 0 },
    0
  },
  {
    "cwglob",
    "usage: cwglob on/off",
    0,
    rtems_capture_cli_watch_global,
    { 0 },
    0
  },
  {
    "cwceil",
    "usage: cwceil priority",
    0,
    rtems_capture_cli_watch_ceiling,
    { 0 },
    0
  },
  {
    "cwfloor",
    "usage: cwfloor priority",
    0,
    rtems_capture_cli_watch_floor,
    { 0 },
    0
  },
  {
    "ctrace",
    "usage: ctrace [-c] [-r records]",
    0,
    rtems_capture_cli_trace_records,
    { 0 },
    0
  },
  {
    "ctset",
    "usage: ctset -h",
    0,
    rtems_capture_cli_trigger_set,
    { 0 },
    0
  },
  {
    "ctclear",
    "usage: ctclear -?",
    0,
    rtems_capture_cli_trigger_clear,
    { 0 },
    0
  },
  {
    "cflush",
    "usage: cflush [-n]",
    0,
    rtems_capture_cli_flush,
    { 0 },
    0
  }
};

/*
 * rtems_capture_cli_init
 *
 * This function initialises the command line interface to the capture
 * engine.
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
