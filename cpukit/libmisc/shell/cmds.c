/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief Integrate monitor commands as a set
 */

/*
 * Copyright (C) 2001 Fernando Ruiz Casas <fruizcasas@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

static bool rtems_shell_register_command(const rtems_monitor_command_entry_t *e, void *arg RTEMS_UNUSED)
{
  /* Exclude EXIT (alias quit)*/
  if (strcmp("exit", e->command) != 0) {
    rtems_shell_cmd_t *shell_cmd =
      (rtems_shell_cmd_t *) calloc(1, sizeof(*shell_cmd));

    if (shell_cmd != NULL) {
      shell_cmd->name    = e->command;
      shell_cmd->topic   = "monitor";
      shell_cmd->usage   = e->usage;
      shell_cmd->command = rtems_shell_main_monitor;

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
