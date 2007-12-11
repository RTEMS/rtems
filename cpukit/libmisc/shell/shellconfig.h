/*
 *  RTEMS Shell Command Set Configuration
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SHELL_CONFIG_h
#define _RTEMS_SHELL_CONFIG_h

#include <rtems/shell.h>

/*
 *  Externs for all command definition structures
 */
extern shell_cmd_t Shell_HELP_Command;
extern shell_cmd_t Shell_ALIAS_Command;
extern shell_cmd_t Shell_LOGOFF_Command;

extern shell_cmd_t Shell_MDUMP_Command;
extern shell_cmd_t Shell_WDUMP_Command;
extern shell_cmd_t Shell_MEDIT_Command;
extern shell_cmd_t Shell_MFILL_Command;
extern shell_cmd_t Shell_MMOVE_Command;

extern shell_cmd_t Shell_DATE_Command;
extern shell_cmd_t Shell_ID_Command;
extern shell_cmd_t Shell_TTY_Command;
extern shell_cmd_t Shell_WHOAMI_Command;

extern shell_cmd_t Shell_PWD_Command;
extern shell_cmd_t Shell_LS_Command;
extern shell_cmd_t Shell_CHDIR_Command;
extern shell_cmd_t Shell_MKDIR_Command;
extern shell_cmd_t Shell_RMDIR_Command;
extern shell_cmd_t Shell_CHROOT_Command;
extern shell_cmd_t Shell_CHMOD_Command;
extern shell_cmd_t Shell_CAT_Command;
extern shell_cmd_t Shell_RM_Command;
extern shell_cmd_t Shell_UMASK_Command;

extern shell_cmd_t Shell_MALLOC_DUMP_Command;

extern shell_cmd_t *Shell_Initial_commands[];

/*
 *  Extern for alias commands
 */
extern shell_alias_t Shell_DIR_Alias;
extern shell_alias_t Shell_CD_Alias;
extern shell_alias_t Shell_EXIT_Alias;

extern shell_alias_t *Shell_Initial_aliases[];

/*
 *  If we are configured to alias a command, then make sure the underlying command
 *  is configured.
 */

#if !defined(CONFIGURE_SHELL_COMMANDS_ALL)
  #if defined(CONFIGURE_SHELL_COMMANDS_DIR) && \
      !defined(CONFIGURE_SHELL_COMMANDS_LS)
    #define CONFIGURE_SHELL_COMMANDS_LS
  #endif

  #if defined(CONFIGURE_SHELL_COMMANDS_CD) && \
      !defined(CONFIGURE_SHELL_COMMANDS_CHDIR)
    #define CONFIGURE_SHELL_COMMANDS_CHDIR
  #endif

  #if defined(CONFIGURE_SHELL_COMMANDS_EXIT) && \
      !defined(CONFIGURE_SHELL_COMMANDS_LOGOFF)
    #define CONFIGURE_SHELL_COMMANDS_LOGOFF
  #endif
#endif

#if defined(CONFIGURE_SHELL_COMMANDS_INIT)
  shell_alias_t *Shell_Initial_aliases[] = {
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMANDS_DIR)
      &Shell_DIR_Alias,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMANDS_CD)
      &Shell_CD_Alias,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMANDS_EXIT)
      &Shell_EXIT_Alias,
    #endif

    /*
     *  User defined shell aliases
     */
    #if defined(CONFIGURE_SHELL_USER_ALIASES)
      CONFIGURE_SHELL_USER_ALIASES,
    #endif
    NULL
  };

  shell_cmd_t *Shell_Initial_commands[] = {
    /*
     *  General comamnds that should be present
     */
    &Shell_HELP_Command,
    &Shell_ALIAS_Command,

    /*
     *  Common commands that can be optional
     */
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_DATE)
      &Shell_DATE_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_ID)
      &Shell_ID_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_TTY)
      &Shell_TTY_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_WHOAMI)
      &Shell_WHOAMI_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_LOGOFF)
      &Shell_LOGOFF_Command,
    #endif

    /*
     *  Memory printing/modification family commands
     */
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_MDUMP)
      &Shell_MDUMP_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_WDUMP)
      &Shell_WDUMP_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_MEDIT)
      &Shell_MEDIT_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_MFILL)
      &Shell_MFILL_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_MMOVE)
      &Shell_MMOVE_Command,
    #endif

    /*
     *  File and directory commands
     */
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_PWD)
      &Shell_PWD_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_LS)
      &Shell_LS_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_CHDIR)
      &Shell_CHDIR_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_MKDIR)
      &Shell_MKDIR_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_RMDIR)
      &Shell_RMDIR_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_CHROOT)
      &Shell_CHROOT_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_CHMOD)
      &Shell_CHMOD_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_CAT)
      &Shell_CAT_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_RM)
      &Shell_RM_Command,
    #endif
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_UMASK)
      &Shell_UMASK_Command,
    #endif

    /*
     *  Malloc family commands
     */
    #if defined(CONFIGURE_SHELL_COMMANDS_ALL) || \
        defined(CONFIGURE_SHELL_COMMAND_MALLOC_DUMP)
      &Shell_MALLOC_DUMP_Command,
    #endif

    /*
     *  User defined shell commands
     */
    #if defined(CONFIGURE_SHELL_USER_COMMANDS)
      CONFIGURE_SHELL_USER_COMMANDS,
    #endif
    NULL
  };
#endif

#endif
