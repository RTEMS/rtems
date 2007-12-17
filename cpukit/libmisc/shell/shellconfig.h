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
extern rtems_shell_cmd_t rtems_Shell_HELP_Command;
extern rtems_shell_cmd_t rtems_Shell_ALIAS_Command;
extern rtems_shell_cmd_t rtems_Shell_LOGOFF_Command;

extern rtems_shell_cmd_t rtems_Shell_MDUMP_Command;
extern rtems_shell_cmd_t rtems_Shell_WDUMP_Command;
extern rtems_shell_cmd_t rtems_Shell_MEDIT_Command;
extern rtems_shell_cmd_t rtems_Shell_MFILL_Command;
extern rtems_shell_cmd_t rtems_Shell_MMOVE_Command;

extern rtems_shell_cmd_t rtems_Shell_DATE_Command;
extern rtems_shell_cmd_t rtems_Shell_ID_Command;
extern rtems_shell_cmd_t rtems_Shell_TTY_Command;
extern rtems_shell_cmd_t rtems_Shell_WHOAMI_Command;

extern rtems_shell_cmd_t rtems_Shell_PWD_Command;
extern rtems_shell_cmd_t rtems_Shell_LS_Command;
extern rtems_shell_cmd_t rtems_Shell_CHDIR_Command;
extern rtems_shell_cmd_t rtems_Shell_MKDIR_Command;
extern rtems_shell_cmd_t rtems_Shell_RMDIR_Command;
extern rtems_shell_cmd_t rtems_Shell_CHROOT_Command;
extern rtems_shell_cmd_t rtems_Shell_CHMOD_Command;
extern rtems_shell_cmd_t rtems_Shell_CAT_Command;
extern rtems_shell_cmd_t rtems_Shell_MSDOSFMT_Command;
extern rtems_shell_cmd_t rtems_Shell_RM_Command;
extern rtems_shell_cmd_t rtems_Shell_UMASK_Command;
extern rtems_shell_cmd_t rtems_Shell_MOUNT_Command;
extern rtems_shell_cmd_t rtems_Shell_UNMOUNT_Command;
extern rtems_shell_cmd_t rtems_Shell_BLKSYNC_Command;

extern rtems_shell_cmd_t rtems_Shell_CPUUSE_Command;
extern rtems_shell_cmd_t rtems_Shell_STACKUSE_Command;
extern rtems_shell_cmd_t rtems_Shell_MALLOC_DUMP_Command;

extern rtems_shell_cmd_t *rtems_Shell_Initial_commands[];

/*
 *  Extern for alias commands
 */
extern rtems_shell_alias_t rtems_Shell_DIR_Alias;
extern rtems_shell_alias_t rtems_Shell_CD_Alias;
extern rtems_shell_alias_t rtems_Shell_EXIT_Alias;

extern rtems_shell_alias_t *rtems_Shell_Initial_aliases[];

/*
 *  Extern for alias commands
 */
extern rtems_shell_filesystems_t rtems_Shell_Mount_MSDOS;
extern rtems_shell_filesystems_t rtems_Shell_Mount_TFTP;
extern rtems_shell_filesystems_t rtems_Shell_Mount_FTP;
extern rtems_shell_filesystems_t rtems_Shell_Mount_NFS;

extern rtems_shell_filesystems_t *rtems_Shell_Mount_filesystems[];

/*
 *  If we are configured to alias a command, then make sure the underlying
 *  command is configured.
 */

#if !defined(CONFIGURE_SHELL_COMMANDS_ALL)
  #if defined(CONFIGURE_SHELL_COMMANDS_DIR) && \
      !defined(CONFIGURE_SHELL_COMMANDS_LS)
    #define CONFIGURE_SHELL_COMMAND_LS
  #endif

  #if defined(CONFIGURE_SHELL_COMMANDS_CD) && \
      !defined(CONFIGURE_SHELL_COMMANDS_CHDIR)
    #define CONFIGURE_SHELL_COMMAND_CHDIR
  #endif

  #if defined(CONFIGURE_SHELL_COMMANDS_EXIT) && \
      !defined(CONFIGURE_SHELL_COMMANDS_LOGOFF)
    #define CONFIGURE_SHELL_COMMAND_LOGOFF
  #endif
#endif

#if defined(CONFIGURE_SHELL_COMMANDS_INIT)
  rtems_shell_alias_t *rtems_Shell_Initial_aliases[] = {
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_DIR)) || \
        defined(CONFIGURE_SHELL_COMMAND_DIR)
      &rtems_Shell_DIR_Alias,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CD)) || \
        defined(CONFIGURE_SHELL_COMMAND_CD)
      &rtems_Shell_CD_Alias,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_EXIT)) || \
        defined(CONFIGURE_SHELL_COMMAND_EXIT)
      &rtems_Shell_EXIT_Alias,
    #endif

    /*
     *  User defined shell aliases
     */
    #if defined(CONFIGURE_SHELL_USER_ALIASES)
      CONFIGURE_SHELL_USER_ALIASES,
    #endif
    NULL
  };

  rtems_shell_cmd_t *rtems_Shell_Initial_commands[] = {
    /*
     *  General comamnds that should be present
     */
    &rtems_Shell_HELP_Command,
    &rtems_Shell_ALIAS_Command,

    /*
     *  Common commands that can be optional
     */
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_DATE)) || \
        defined(CONFIGURE_SHELL_COMMAND_DATE)
      &rtems_Shell_DATE_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_ID)) || \
        defined(CONFIGURE_SHELL_COMMAND_ID)
      &rtems_Shell_ID_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_TTY)) || \
        defined(CONFIGURE_SHELL_COMMAND_TTY)
      &rtems_Shell_TTY_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_WHOAMI)) || \
        defined(CONFIGURE_SHELL_COMMAND_WHOAMI)
      &rtems_Shell_WHOAMI_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_LOGOFF)) || \
        defined(CONFIGURE_SHELL_COMMAND_LOGOFF)
      &rtems_Shell_LOGOFF_Command,
    #endif

    /*
     *  Memory printing/modification family commands
     */
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
        !defined(CONFIGURE_SHELL_NO_COMMAND_MDUMP)) || \
        defined(CONFIGURE_SHELL_COMMAND_MDUMP)
      &rtems_Shell_MDUMP_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_WDUMP)) || \
        defined(CONFIGURE_SHELL_COMMAND_WDUMP)
      &rtems_Shell_WDUMP_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MEDIT)) || \
        defined(CONFIGURE_SHELL_COMMAND_MEDIT)
      &rtems_Shell_MEDIT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MFILL)) || \
        defined(CONFIGURE_SHELL_COMMAND_MFILL)
      &rtems_Shell_MFILL_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MMOVE)) || \
        defined(CONFIGURE_SHELL_COMMAND_MMOVE)
      &rtems_Shell_MMOVE_Command,
    #endif

    /*
     *  File and directory commands
     */
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_PWD)) || \
        defined(CONFIGURE_SHELL_COMMAND_PWD)
      &rtems_Shell_PWD_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_LS)) || \
        defined(CONFIGURE_SHELL_COMMAND_LS)
      &rtems_Shell_LS_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CHDIR)) || \
        defined(CONFIGURE_SHELL_COMMAND_CHDIR)
      &rtems_Shell_CHDIR_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MKDIR)) || \
        defined(CONFIGURE_SHELL_COMMAND_MKDIR)
      &rtems_Shell_MKDIR_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_RMDIR)) || \
        defined(CONFIGURE_SHELL_COMMAND_RMDIR)
      &rtems_Shell_RMDIR_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CHROOT)) || \
        defined(CONFIGURE_SHELL_COMMAND_CHROOT)
      &rtems_Shell_CHROOT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CHMOD)) || \
        defined(CONFIGURE_SHELL_COMMAND_CHMOD)
      &rtems_Shell_CHMOD_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CAT)) || \
        defined(CONFIGURE_SHELL_COMMAND_CAT)
      &rtems_Shell_CAT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MSDOSFMT)) || \
        defined(CONFIGURE_SHELL_COMMAND_MSDOSFMT)
      &rtems_Shell_MSDOSFMT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_RM)) || \
        defined(CONFIGURE_SHELL_COMMAND_RM)
      &rtems_Shell_RM_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_UMASK)) || \
        defined(CONFIGURE_SHELL_COMMAND_UMASK)
      &rtems_Shell_UMASK_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MOUNT)) || \
        defined(CONFIGURE_SHELL_COMMAND_MOUNT)
      &rtems_Shell_MOUNT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_UNMOUNT)) || \
        defined(CONFIGURE_SHELL_COMMAND_UNMOUNT)
      &rtems_Shell_UNMOUNT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_BLKSYNC)) || \
        defined(CONFIGURE_SHELL_COMMAND_BLKSYNC)
      &rtems_Shell_BLKSYNC_Command,
    #endif

    /*
     *  RTEMS Related commands
     */
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CPUUSE)) || \
        defined(CONFIGURE_SHELL_COMMAND_CPUUSE)
      &rtems_Shell_CPUUSE_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_STACKUSE)) || \
        defined(CONFIGURE_SHELL_COMMAND_STACKUSE)
      &rtems_Shell_STACKUSE_Command,
    #endif

    /*
     *  Malloc family commands
     */
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_COMMAND_MALLOC_DUMP)) || \
        defined(CONFIGURE_SHELL_COMMAND_MALLOC_DUMP)
      &rtems_Shell_MALLOC_DUMP_Command,
    #endif

    /*
     *  User defined shell commands
     */
    #if defined(CONFIGURE_SHELL_USER_COMMANDS)
      CONFIGURE_SHELL_USER_COMMANDS,
    #endif
    NULL
  };

  /*
   * The mount command's support file system types.
   */
  #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
       !defined(CONFIGURE_SHELL_COMMAND_MALLOC_DUMP)) || \
       defined(CONFIGURE_SHELL_COMMAND_MALLOC_DUMP)
    rtems_shell_filesystems_t *rtems_Shell_Mount_filesystems[] = {
      #if defined(CONFIGURE_SHELL_MOUNT_MSDOS)
        &rtems_Shell_Mount_MSDOS,
      #endif
      #if RTEMS_NETWORKING
        #if defined(CONFIGURE_SHELL_MOUNT_TFTP)
          &rtems_Shell_Mount_TFTP,
        #endif
        #if defined(CONFIGURE_SHELL_MOUNT_FTP)
          &rtems_Shell_Mount_FTP,
        #endif
        #if defined(CONFIGURE_SHELL_MOUNT_NFS)
          &rtems_Shell_Mount_NFS,
        #endif
      #endif
      NULL
    };
  #endif

#endif

#endif
