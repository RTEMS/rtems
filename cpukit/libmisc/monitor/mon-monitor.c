/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS monitor main body
 *
 *  TODO:
 *      add stuff to RTEMS api
 *            rtems_get_name(id)
 *            rtems_get_type(id)
 *            rtems_build_id(node, type, num)
 *      Add a command to dump out info about an arbitrary id when
 *         types are added to id's
 *         rtems> id idnum
 *                idnum: node n, object: whatever, id: whatever
 *      allow id's to be specified as n:t:id, where 'n:t' is optional
 *      should have a separate monitor FILE stream (ala the debugger)
 *      remote request/response stuff should be cleaned up
 *         maybe we can use real rpc??
 *      'info' command to print out:
 *           interrupt stack location, direction and size
 *           floating point config stuff
 *           interrupt config stuff
 */

/*
 * COPYRIGHT (c) 1989-2022. On-Line Applications Research Corporation (OAR).
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

#include <rtems.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <rtems/monitor.h>

/*
 * Various id's for the monitor
 * They need to be public variables for access by other agencies
 * such as debugger and remote servers'
 */

rtems_id  rtems_monitor_task_id;

uint32_t   rtems_monitor_node;		/* our node number */
uint32_t   rtems_monitor_default_node;  /* current default for commands */

/*
 * The rtems symbol table
 */

rtems_symbol_table_t *rtems_monitor_symbols;

/*
 * The top-level commands
 */

static const rtems_monitor_command_entry_t rtems_monitor_commands[] = {
    { "config",
      "Show the system configuration.",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_CONFIG },
      &rtems_monitor_commands[1],
    },
    { "itask",
      "List init tasks for the system",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_INIT_TASK },
      &rtems_monitor_commands[2],
    },
   { "mpci",
      "Show the MPCI system configuration, if configured.",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_MPCI },
      &rtems_monitor_commands[3],
    },
    { "pause",
      "Monitor goes to \"sleep\" for specified ticks (default is 1). "
      "Monitor will resume at end of period or if explicitly awakened\n"
      "  pause [ticks]",
      0,
      rtems_monitor_pause_cmd,
      { 0 },
      &rtems_monitor_commands[4],
    },
    { "continue",
      "Put the monitor to sleep waiting for an explicit wakeup from the "
      "program running.\n",
      0,
      rtems_monitor_continue_cmd,
      { 0 },
      &rtems_monitor_commands[5],
    },
    { "go",
      "Alias for 'continue'",
      0,
      rtems_monitor_continue_cmd,
      { 0 },
      &rtems_monitor_commands[6],
    },
    { "symbol",
      "Display value associated with specified symbol. "
      "Defaults to displaying all known symbols.\n"
      "  symbol [ symbolname [symbolname ... ] ]",
      0,
      rtems_monitor_symbol_cmd,
      { .symbol_table = &rtems_monitor_symbols },
      &rtems_monitor_commands[7],
    },
    { "extension",
      "Display information about specified extensions. "
      "Default is to display information about all extensions on this node.\n"
      "  extension [id [id ...] ]",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_EXTENSION },
      &rtems_monitor_commands[8],
    },
    { "task",
      "Display information about the specified tasks. "
      "Default is to display information about all tasks on this node.\n"
      "  task [id [id ...] ]",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_TASK },
      &rtems_monitor_commands[9],
    },
    { "queue",
      "Display information about the specified message queues. "
      "Default is to display information about all queues on this node.\n"
      "  queue [id [id ... ] ]",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_QUEUE },
      &rtems_monitor_commands[10],
    },
    { "sema",
      "sema [id [id ... ] ]\n"
      "  display information about the specified semaphores\n"
      "  Default is to display information about all semaphores on this node\n"
      ,
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_SEMAPHORE },
      &rtems_monitor_commands[11],
    },
    { "region",
      "region [id [id ... ] ]\n"
      "  display information about the specified regions\n"
      "  Default is to display information about all regions on this node\n"
      ,
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_REGION },
      &rtems_monitor_commands[12],
    },
    { "part",
      "part [id [id ... ] ]\n"
      "  display information about the specified partitions\n"
      "  Default is to display information about all partitions on this node\n"
      ,
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_PARTITION },
      &rtems_monitor_commands[13],
    },
    { "object",
      "Display information about specified RTEMS objects. "
      "Object id's must include 'type' information. "
      "(which may normally be defaulted)\n"
      "  object [id [id ...] ]",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_INVALID },
      &rtems_monitor_commands[14],
    },
    { "driver",
      "Display the RTEMS device driver table.\n"
      "  driver [ major [ major ... ] ]",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_DRIVER },
      &rtems_monitor_commands[15],
    },
    { "dname",
      "Displays information about named drivers.\n",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_DNAME },
      &rtems_monitor_commands[16],
    },
    { "fatal",
      "'exit' with fatal error; default error is RTEMS_TASK_EXITTED\n"
      "  fatal [status]",
      0,
      rtems_monitor_fatal_cmd,
      { .status_code = RTEMS_TASK_EXITTED },		/* exit value */
      &rtems_monitor_commands[17],
    },
    { "reset",
      "(SW)Resets the System.",
      0,
      rtems_monitor_reset_cmd,
      { 0 },
      &rtems_monitor_commands[18],
    },
#if defined(RTEMS_MULTIPROCESSING)
    { "node",
      "Specify default node number for commands that take id's.\n"
      "  node [ node number ]",
      0,
      rtems_monitor_node_cmd,
      { 0 },
      &rtems_monitor_commands[19],
    },
  #define RTEMS_MONITOR_POSIX_NEXT 20
#else
  #define RTEMS_MONITOR_POSIX_NEXT 19
#endif
    { "pthread",
      "Display information about the specified pthreads. "
      "Default is to display information about all pthreads on this node.\n"
      "  pthread [id [id ...] ]",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_PTHREAD },
      &rtems_monitor_commands[RTEMS_MONITOR_POSIX_NEXT],
    },
  #define RTEMS_MONITOR_DEBUGGER_NEXT (RTEMS_MONITOR_POSIX_NEXT + 1)
#ifdef CPU_INVOKE_DEBUGGER
    { "debugger",
      "Enter the debugger, if possible. "
      "A continue from the debugger will return to the monitor.\n",
      0,
      rtems_monitor_debugger_cmd,
      { 0 },
      &rtems_monitor_commands[RTEMS_MONITOR_DEBUGGER_NEXT],
    },
#endif
    { "help",
      "Provide information about commands. "
      "Default is show basic command summary.\n"
      "help [ command [ command ] ]",
      0,
      rtems_monitor_help_cmd,
      { .monitor_command_entry = rtems_monitor_commands },
      NULL
    }
};

/*
 * All registered commands.
 */

static const rtems_monitor_command_entry_t *rtems_monitor_registered_commands =
  &rtems_monitor_commands [0];


rtems_status_code
rtems_monitor_suspend(rtems_interval timeout)
{
    rtems_event_set event_set;
    rtems_status_code status;

    status = rtems_event_receive(MONITOR_WAKEUP_EVENT,
                                 RTEMS_DEFAULT_OPTIONS,
                                 timeout,
                                 &event_set);
    return status;
}

void __attribute__((weak))
rtems_monitor_reset_cmd(
  int argc,
  char **argv,
  const rtems_monitor_command_arg_t* command_arg,
  bool verbose
)
{
    (void) argc;
    (void) argv;
    (void) command_arg;
    (void) verbose;
}

void
rtems_monitor_wakeup(void)
{
    rtems_event_send(rtems_monitor_task_id, MONITOR_WAKEUP_EVENT);
}

void rtems_monitor_debugger_cmd(
  int                                argc RTEMS_UNUSED,
  char                             **argv RTEMS_UNUSED,
  const rtems_monitor_command_arg_t *command_arg RTEMS_UNUSED,
  bool                               verbose RTEMS_UNUSED
)
{
#ifdef CPU_INVOKE_DEBUGGER
    CPU_INVOKE_DEBUGGER;
#endif
}

void rtems_monitor_pause_cmd(
  int                                argc,
  char                             **argv,
  const rtems_monitor_command_arg_t *command_arg RTEMS_UNUSED,
  bool                               verbose RTEMS_UNUSED
)
{
    if (argc == 1)
        rtems_monitor_suspend(1);
    else
        rtems_monitor_suspend(strtoul(argv[1], 0, 0));
}

void rtems_monitor_fatal_cmd(
  int                                argc,
  char                             **argv,
  const rtems_monitor_command_arg_t *command_arg,
  bool                               verbose RTEMS_UNUSED
)
{
    if (argc == 1)
        rtems_fatal_error_occurred(command_arg->status_code);
    else
        rtems_fatal_error_occurred(strtoul(argv[1], 0, 0));
}

void rtems_monitor_continue_cmd(
  int                                argc RTEMS_UNUSED,
  char                             **argv RTEMS_UNUSED,
  const rtems_monitor_command_arg_t *command_arg RTEMS_UNUSED,
  bool                               verbose RTEMS_UNUSED
)
{
    rtems_monitor_suspend(RTEMS_NO_TIMEOUT);
}

#if defined(RTEMS_MULTIPROCESSING)
void rtems_monitor_node_cmd(
  int                                argc,
  char                             **argv,
  const rtems_monitor_command_arg_t *command_arg RTEMS_UNUSED,
  bool                               verbose RTEMS_UNUSED
)
{
  uint32_t   new_node = rtems_monitor_default_node;

  switch (argc) {
    case 1: 		/* no node, just set back to ours */
      new_node = rtems_monitor_node;
      break;

    case 2:
      new_node = strtoul(argv[1], 0, 0);
      break;

    default:
      fprintf(stdout,"invalid syntax, try 'help node'\n");
      break;
  }

  if ((new_node >= 1) &&
    (new_node <= _MPCI_Configuration.maximum_nodes))
	rtems_monitor_default_node = new_node;
}
#endif


/*
 *  Function:   rtems_monitor_symbols_loadup
 *
 *  Description:
 *      Create and load the monitor's symbol table.
 *      We are reading the output format of 'gnm' which looks like this:
 *
 *              400a7068 ? _Rate_monotonic_Information
 *              400a708c ? _Thread_Dispatch_disable_level
 *              400a7090 ? _Configuration_Table
 *
 *      We ignore the type field.
 *
 *  Side Effects:
 *      Creates and fills in 'rtems_monitor_symbols' table
 *
 *  TODO
 *      there should be a BSP #define or something like that
 *         to do this;  Assuming stdio is crazy.
 *      Someday this should know BFD
 *              Maybe we could get objcopy to just copy the symbol areas
 *              and copy that down.
 *
 */

void
rtems_monitor_symbols_loadup(void)
{
    FILE *fp;
    char buffer[128];

    if (rtems_monitor_symbols)
        rtems_symbol_table_destroy(rtems_monitor_symbols);

    rtems_monitor_symbols = rtems_symbol_table_create();
    if (rtems_monitor_symbols == 0)
        return;

    fp = fopen("symbols", "r");

    if (fp == 0)
        return;

    while (fgets(buffer, sizeof(buffer) - 1, fp))
    {
        char *symbol;
        char *value;
        char *ignored_type;

        value = strtok(buffer, " \t\n");
        ignored_type = strtok(0, " \t\n");
        symbol = strtok(0, " \t\n");

        if (symbol && ignored_type && value)
        {
            rtems_symbol_t *sp;
            sp = rtems_symbol_create(rtems_monitor_symbols,
                                     symbol,
                                     (uint32_t) strtoul(value, 0, 16));
            if (sp == 0)
            {
                fprintf(stdout,"could not define symbol '%s'\n", symbol);
                goto done;
            }
        }
        else
        {
            fprintf(stdout,"parsing error on '%s'\n", buffer);
            goto done;
        }
    }

done:
    fclose(fp);
    return;
}

/*
 * User registered commands.
 */

int
rtems_monitor_insert_cmd (
  rtems_monitor_command_entry_t *command
)
{
  const rtems_monitor_command_entry_t *e = rtems_monitor_registered_commands;

  /* Reject empty commands */
  if (command->command == NULL) {
    return 0;
  }

  /* Reject command if already present */
  while (e->next != NULL) {
      if (e->command != NULL && strcmp(command->command, e->command) == 0) {
        return 0;
      }
      e = e->next;
  }

  /* Prepend new command */
  command->next = rtems_monitor_registered_commands;
  rtems_monitor_registered_commands = command;

  return 1;
}

/**
 * @brief Iterates through all registerd commands.
 *
 * For each command the interation routine @a routine is called with the
 * command entry and the user provided argument @a arg.  It is guaranteed that
 * the command name and function are not NULL.
 */
void rtems_monitor_command_iterate(
  rtems_monitor_per_command_routine routine,
  void *arg
)
{
  const rtems_monitor_command_entry_t *e = rtems_monitor_registered_commands;

  while (e != NULL) {
    if (e->command != NULL && e->command_function != NULL) {
      if (!routine(e, arg)) {
        break;
      }
    }
    e = e->next;
  }
}
