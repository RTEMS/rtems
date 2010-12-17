/*
 *  BASED UPON SOURCE IN RTEMS, MODIFIED FOR SIMULATOR
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "shell.h"
#include <stdio.h>

extern rtems_shell_cmd_t rtems_shell_ECHO_Command;
extern rtems_shell_cmd_t rtems_shell_HELP_Command;

extern rtems_shell_cmd_t rtems_shell_RTEMS_INIT_Command;
extern rtems_shell_cmd_t rtems_shell_TASK_CREATE_Command;
extern rtems_shell_cmd_t rtems_shell_TASK_DELETE_Command;
extern rtems_shell_cmd_t rtems_shell_TASK_MODE_Command;
extern rtems_shell_cmd_t rtems_shell_TASK_PRIORITY_Command;
extern rtems_shell_cmd_t rtems_shell_TASK_SUSPEND_Command;
extern rtems_shell_cmd_t rtems_shell_TASK_RESUME_Command;
extern rtems_shell_cmd_t rtems_shell_TASK_WAKE_AFTER_Command;

extern rtems_shell_cmd_t rtems_shell_CLOCK_TICK_Command;


extern rtems_shell_cmd_t rtems_shell_SEMAPHORE_CREATE_Command;
extern rtems_shell_cmd_t rtems_shell_SEMAPHORE_DELETE_Command;
extern rtems_shell_cmd_t rtems_shell_SEMAPHORE_OBTAIN_Command;
extern rtems_shell_cmd_t rtems_shell_SEMAPHORE_RELEASE_Command;
extern rtems_shell_cmd_t rtems_shell_SEMAPHORE_FLUSH_Command;

extern rtems_shell_cmd_t rtems_shell_TASK_EXECUTING_Command;
extern rtems_shell_cmd_t rtems_shell_TASK_HEIR_Command;

rtems_shell_cmd_t *rtems_shell_Initial_commands[] = {
  /* Generic Commands */
  &rtems_shell_ECHO_Command,
  &rtems_shell_HELP_Command,

  /* RTEMS Classic API Type Commands */
  &rtems_shell_RTEMS_INIT_Command,
  &rtems_shell_TASK_CREATE_Command,
  &rtems_shell_TASK_DELETE_Command,
  &rtems_shell_TASK_MODE_Command,
  &rtems_shell_TASK_PRIORITY_Command,
  &rtems_shell_TASK_SUSPEND_Command,
  &rtems_shell_TASK_RESUME_Command,
  &rtems_shell_TASK_WAKE_AFTER_Command,

  &rtems_shell_CLOCK_TICK_Command,

  &rtems_shell_SEMAPHORE_CREATE_Command,
  &rtems_shell_SEMAPHORE_DELETE_Command,
  &rtems_shell_SEMAPHORE_OBTAIN_Command,
  &rtems_shell_SEMAPHORE_RELEASE_Command,
  &rtems_shell_SEMAPHORE_FLUSH_Command,

  /* RTEMS Helper Commands */
  &rtems_shell_TASK_EXECUTING_Command,
  &rtems_shell_TASK_HEIR_Command,
  NULL
};

rtems_shell_alias_t *rtems_shell_Initial_aliases[] = {
  NULL
};

