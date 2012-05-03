/*
 *  XXX -- Just monitor commands until those can be integrated better
 *
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/monitor.h>
#include <rtems/shell.h>
#include "internal.h"

/*-----------------------------------------------------------*
 * with this you can call at all the rtems monitor commands.
 * Not all work fine but you can show the rtems status and more.
 *-----------------------------------------------------------*/
int rtems_shell_main_monitor(int argc, char **argv) {
  const rtems_monitor_command_entry_t *command = NULL;

  if (argc < 1) {
    return 1;
  }

  command = rtems_monitor_command_lookup(argv [0]);

  if (command == NULL) {
    return 1;
  }

  command->command_function(argc, argv, &command->command_arg, 0);

  return 0;
}

static bool rtems_shell_register_command(const rtems_monitor_command_entry_t *e, void *arg __attribute__((unused)))
{
  /* Exclude EXIT (alias quit)*/
  if (strcmp("exit", e->command) != 0) {
    rtems_shell_cmd_t *shell_cmd =
      (rtems_shell_cmd_t *) malloc(sizeof(rtems_shell_cmd_t));

    if (shell_cmd != NULL) {
      shell_cmd->name    = e->command;
      shell_cmd->topic   = "monitor";
      shell_cmd->usage   = e->usage;
      shell_cmd->command = rtems_shell_main_monitor;
      shell_cmd->alias   = NULL;
      shell_cmd->next    = NULL;

      if (rtems_shell_add_cmd_struct(shell_cmd) == NULL) {
        free(shell_cmd);
      }
    }
  }

  return true;
}

void rtems_shell_register_monitor_commands(void)
{
  rtems_monitor_command_iterate(rtems_shell_register_command, NULL);
}
