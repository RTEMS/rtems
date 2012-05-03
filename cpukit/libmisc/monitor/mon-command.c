/**
 * @file
 *
 * @brief Command support routines for RTEMS monitor.
 */

/*
 * 2001-01-30 KJO (vac4050@cae597.rsc.raytheon.com):
 *  Fixed rtems_monitor_command_lookup() to accept partial
 *  commands to uniqeness.  Added support for setting
 *  the monitor prompt via an environment variable:
 *  RTEMS_MONITOR_PROMPT
 *
 * CCJ: 26-3-2000, adding command history and command line
 * editing. This code is donated from My Right Boot and not
 * covered by GPL, only the RTEMS license.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/monitor.h>

static void
rtems_monitor_show_help (
  const rtems_monitor_command_entry_t *help_cmd,
  int                           max_cmd_len
)
{
#define MAX_HELP_LINE_LENGTH (75 - max_cmd_len - 2)

  if (help_cmd && help_cmd->command)
  {
    const char *help = help_cmd->usage;
    int         help_len = strlen (help);
    int         spaces = max_cmd_len - strlen (help_cmd->command);
    int         show_this_line = 0;
    int         line_one = 1;
    int         c;

    fprintf(stdout,"%s", help_cmd->command);

    if (help_len == 0)
    {
      fprintf(stdout," - No help associated.\n");
      return;
    }

    while (help_len)
    {
      fprintf(stdout,"%*c", spaces, ' ');

      if (line_one)
        fprintf(stdout," - ");

      spaces   = max_cmd_len + 2;
      line_one = 0;

      /*
       * See if greater then the line length if so, work back
       * from the end for a space, tab or lf or cr.
       */

      if (help_len > MAX_HELP_LINE_LENGTH)
      {
        for (show_this_line = MAX_HELP_LINE_LENGTH - 1;
             show_this_line;
             show_this_line--)
          if ((help[show_this_line] == ' ') ||
              (help[show_this_line] == '\n') ||
              (help[show_this_line] == '\r'))
            break;

        /*
         * If show_this_line is 0, it is a very long word !!
         */

        if (show_this_line == 0)
          show_this_line = MAX_HELP_LINE_LENGTH - 1;
      }
      else
        show_this_line = help_len;

      for (c = 0; c < show_this_line; c++)
        if ((help[c] == '\r') || (help[c] == '\n'))
          show_this_line = c;
        else
          putchar (help[c]);

      fprintf(stdout,"\n");

      help     += show_this_line;
      help_len -= show_this_line;

      /*
       * Move past the line feeds or what ever else is being skipped.
       */

      while (help_len)
      {
        if ((*help != '\r') && (*help != '\n'))
          break;

        if (*help != ' ')
        {
          help++;
          help_len--;
          break;
        }
        help++;
        help_len--;
      }
    }
  }
}

void
rtems_monitor_command_usage(
  const rtems_monitor_command_entry_t *table,
  const char                          *command_name
)
{
  const rtems_monitor_command_entry_t *command = table;
  int                           max_cmd_len = 0;

  /* if first entry in table is a usage, then print it out */

  if (command_name && (*command_name != '\0'))
  {
    command = rtems_monitor_command_lookup (command_name);

    if (command)
      rtems_monitor_show_help (command, strlen (command_name));
    else
      fprintf(stdout,"Unrecognised command; try just 'help'\n");
    return;
  }

  /*
   * Find the largest command size.
   */

  while (command)
  {
    int len = command->command ? strlen (command->command) : 0 ;

    if (len > max_cmd_len)
      max_cmd_len = len;

    command = command->next;
  }

  max_cmd_len++;

  command = table;

  /*
   * Now some nice formatting for the help.
   */

  while (command)
  {
    rtems_monitor_show_help (command, max_cmd_len);
    command = command->next;
  }
}


void rtems_monitor_help_cmd(
  int                                argc,
  char                             **argv,
  const rtems_monitor_command_arg_t *command_arg,
  bool                               verbose __attribute__((unused))
)
{
  int arg;
  const rtems_monitor_command_entry_t *command =
    command_arg->monitor_command_entry;

  if (argc == 1)
    rtems_monitor_command_usage(command, 0);
  else
  {
    for (arg = 1; argv[arg]; arg++)
      rtems_monitor_command_usage(command, argv[arg]);
  }
}

typedef struct {
  const char *name;
  size_t length;
  const rtems_monitor_command_entry_t *match;
} rtems_monitor_command_lookup_entry;

static bool rtems_monitor_command_lookup_routine(
  const rtems_monitor_command_entry_t *e,
  void *arg
)
{
  rtems_monitor_command_lookup_entry *le =
    (rtems_monitor_command_lookup_entry *) arg;

  /* Check name */
  if (strncmp(e->command, le->name, le->length) == 0) {
    /* Check for ambiguity */
    if (le->match == NULL) {
      le->match = e;
    } else {
      return false;
    }
  }

  return true;
}

/**
 * @brief Looks for a command with the name @a name in the list of registered
 * commands.
 *
 * The parameter @a name must not be NULL.
 *
 * Returns the corresponding command entry or NULL if no command is found.
 */
const rtems_monitor_command_entry_t *rtems_monitor_command_lookup(
  const char *name
)
{
  rtems_monitor_command_lookup_entry e = {
    .name = name,
    .length = strlen( name),
    .match = NULL
  };

  rtems_monitor_command_iterate(rtems_monitor_command_lookup_routine, &e);

  return e.match;
}
