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
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include <rtems.h>
#include <rtems/monitor.h>
#include <rtems/shell.h>
#include "internal.h"

/*-----------------------------------------------------------*
 * with this you can call at all the rtems monitor commands.
 * Not all work fine but you can show the rtems status and more.
 *-----------------------------------------------------------*/
int rtems_shell_main_monitor(int argc,char * argv[]) {
  rtems_monitor_command_entry_t *command;

  if ((command=rtems_monitor_command_lookup(rtems_monitor_commands,argc,argv)))
    command->command_function(argc, argv, &command->command_arg, 0);

  return 0;
}

void rtems_shell_register_monitor_commands(void)
{
  rtems_monitor_command_entry_t *command;
  /* monitor topic */
  command = rtems_monitor_commands;
  while (command) {
   if (strcmp("exit",command->command)) /* Exclude EXIT (alias quit)*/
     rtems_shell_add_cmd(command->command,"monitor",
                         command->usage,rtems_shell_main_monitor);
     command = command->next;
  }
}
