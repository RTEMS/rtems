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
extern rtems_shell_cmd_t rtems_shell_HELP_Command;
extern rtems_shell_cmd_t rtems_shell_ALIAS_Command;
extern rtems_shell_cmd_t rtems_shell_LOGOFF_Command;

extern rtems_shell_cmd_t rtems_shell_MDUMP_Command;
extern rtems_shell_cmd_t rtems_shell_WDUMP_Command;
extern rtems_shell_cmd_t rtems_shell_MEDIT_Command;
extern rtems_shell_cmd_t rtems_shell_MFILL_Command;
extern rtems_shell_cmd_t rtems_shell_MMOVE_Command;

extern rtems_shell_cmd_t rtems_shell_DATE_Command;
extern rtems_shell_cmd_t rtems_shell_ID_Command;
extern rtems_shell_cmd_t rtems_shell_TTY_Command;
extern rtems_shell_cmd_t rtems_shell_WHOAMI_Command;

extern rtems_shell_cmd_t rtems_shell_CP_Command;
extern rtems_shell_cmd_t rtems_shell_PWD_Command;
extern rtems_shell_cmd_t rtems_shell_LS_Command;
extern rtems_shell_cmd_t rtems_shell_CHDIR_Command;
extern rtems_shell_cmd_t rtems_shell_MKDIR_Command;
extern rtems_shell_cmd_t rtems_shell_RMDIR_Command;
extern rtems_shell_cmd_t rtems_shell_CHROOT_Command;
extern rtems_shell_cmd_t rtems_shell_CHMOD_Command;
extern rtems_shell_cmd_t rtems_shell_CAT_Command;
extern rtems_shell_cmd_t rtems_shell_MSDOSFMT_Command;
extern rtems_shell_cmd_t rtems_shell_RM_Command;
extern rtems_shell_cmd_t rtems_shell_UMASK_Command;
extern rtems_shell_cmd_t rtems_shell_MOUNT_Command;
extern rtems_shell_cmd_t rtems_shell_UNMOUNT_Command;
extern rtems_shell_cmd_t rtems_shell_BLKSYNC_Command;

extern rtems_shell_cmd_t rtems_shell_CPUUSE_Command;
extern rtems_shell_cmd_t rtems_shell_STACKUSE_Command;
extern rtems_shell_cmd_t rtems_shell_PERIODUSE_Command;
extern rtems_shell_cmd_t rtems_shell_WKSPACE_INFO_Command;
extern rtems_shell_cmd_t rtems_shell_MALLOC_INFO_Command;

extern rtems_shell_cmd_t *rtems_shell_Initial_commands[];

/*
 *  Extern for alias commands
 */
extern rtems_shell_alias_t rtems_shell_DIR_Alias;
extern rtems_shell_alias_t rtems_shell_CD_Alias;
extern rtems_shell_alias_t rtems_shell_EXIT_Alias;

extern rtems_shell_alias_t *rtems_shell_Initial_aliases[];

/*
 *  Extern for alias commands
 */
extern rtems_shell_filesystems_t rtems_shell_Mount_MSDOS;
extern rtems_shell_filesystems_t rtems_shell_Mount_TFTP;
extern rtems_shell_filesystems_t rtems_shell_Mount_FTP;
extern rtems_shell_filesystems_t rtems_shell_Mount_NFS;

extern rtems_shell_filesystems_t *rtems_shell_Mount_filesystems[];

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
  rtems_shell_alias_t *rtems_shell_Initial_aliases[] = {
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_DIR)) || \
        defined(CONFIGURE_SHELL_COMMAND_DIR)
      &rtems_shell_DIR_Alias,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CD)) || \
        defined(CONFIGURE_SHELL_COMMAND_CD)
      &rtems_shell_CD_Alias,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_EXIT)) || \
        defined(CONFIGURE_SHELL_COMMAND_EXIT)
      &rtems_shell_EXIT_Alias,
    #endif

    /*
     *  User defined shell aliases
     */
    #if defined(CONFIGURE_SHELL_USER_ALIASES)
      CONFIGURE_SHELL_USER_ALIASES,
    #endif
    NULL
  };

  rtems_shell_cmd_t *rtems_shell_Initial_commands[] = {
    /*
     *  General comamnds that should be present
     */
    &rtems_shell_HELP_Command,
    &rtems_shell_ALIAS_Command,

    /*
     *  Common commands that can be optional
     */
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_DATE)) || \
        defined(CONFIGURE_SHELL_COMMAND_DATE)
      &rtems_shell_DATE_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_ID)) || \
        defined(CONFIGURE_SHELL_COMMAND_ID)
      &rtems_shell_ID_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_TTY)) || \
        defined(CONFIGURE_SHELL_COMMAND_TTY)
      &rtems_shell_TTY_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_WHOAMI)) || \
        defined(CONFIGURE_SHELL_COMMAND_WHOAMI)
      &rtems_shell_WHOAMI_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_LOGOFF)) || \
        defined(CONFIGURE_SHELL_COMMAND_LOGOFF)
      &rtems_shell_LOGOFF_Command,
    #endif

    /*
     *  Memory printing/modification family commands
     */
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
        !defined(CONFIGURE_SHELL_NO_COMMAND_MDUMP)) || \
        defined(CONFIGURE_SHELL_COMMAND_MDUMP)
      &rtems_shell_MDUMP_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_WDUMP)) || \
        defined(CONFIGURE_SHELL_COMMAND_WDUMP)
      &rtems_shell_WDUMP_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MEDIT)) || \
        defined(CONFIGURE_SHELL_COMMAND_MEDIT)
      &rtems_shell_MEDIT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MFILL)) || \
        defined(CONFIGURE_SHELL_COMMAND_MFILL)
      &rtems_shell_MFILL_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MMOVE)) || \
        defined(CONFIGURE_SHELL_COMMAND_MMOVE)
      &rtems_shell_MMOVE_Command,
    #endif

    /*
     *  File and directory commands
     */
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CP)) || \
        defined(CONFIGURE_SHELL_COMMAND_CP)
      &rtems_shell_CP_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_PWD)) || \
        defined(CONFIGURE_SHELL_COMMAND_PWD)
      &rtems_shell_PWD_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_LS)) || \
        defined(CONFIGURE_SHELL_COMMAND_LS)
      &rtems_shell_LS_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CHDIR)) || \
        defined(CONFIGURE_SHELL_COMMAND_CHDIR)
      &rtems_shell_CHDIR_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MKDIR)) || \
        defined(CONFIGURE_SHELL_COMMAND_MKDIR)
      &rtems_shell_MKDIR_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_RMDIR)) || \
        defined(CONFIGURE_SHELL_COMMAND_RMDIR)
      &rtems_shell_RMDIR_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CHROOT)) || \
        defined(CONFIGURE_SHELL_COMMAND_CHROOT)
      &rtems_shell_CHROOT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CHMOD)) || \
        defined(CONFIGURE_SHELL_COMMAND_CHMOD)
      &rtems_shell_CHMOD_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CAT)) || \
        defined(CONFIGURE_SHELL_COMMAND_CAT)
      &rtems_shell_CAT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MSDOSFMT)) || \
        defined(CONFIGURE_SHELL_COMMAND_MSDOSFMT)
      &rtems_shell_MSDOSFMT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_RM)) || \
        defined(CONFIGURE_SHELL_COMMAND_RM)
      &rtems_shell_RM_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_UMASK)) || \
        defined(CONFIGURE_SHELL_COMMAND_UMASK)
      &rtems_shell_UMASK_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_MOUNT)) || \
        defined(CONFIGURE_SHELL_COMMAND_MOUNT)
      &rtems_shell_MOUNT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_UNMOUNT)) || \
        defined(CONFIGURE_SHELL_COMMAND_UNMOUNT)
      &rtems_shell_UNMOUNT_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_BLKSYNC)) || \
        defined(CONFIGURE_SHELL_COMMAND_BLKSYNC)
      &rtems_shell_BLKSYNC_Command,
    #endif

    /*
     *  RTEMS Related commands
     */
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_CPUUSE)) || \
        defined(CONFIGURE_SHELL_COMMAND_CPUUSE)
      &rtems_shell_CPUUSE_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_STACKUSE)) || \
        defined(CONFIGURE_SHELL_COMMAND_STACKUSE)
      &rtems_shell_STACKUSE_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_NO_COMMAND_PERIODUSE)) || \
        defined(CONFIGURE_SHELL_COMMAND_PERIODUSE)
      &rtems_shell_PERIODUSE_Command,
    #endif
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_COMMAND_WKSPACE_INFO)) || \
        defined(CONFIGURE_SHELL_COMMAND_WKSPACE_INFO)
      &rtems_shell_WKSPACE_INFO_Command,
    #endif


    /*
     *  Malloc family commands
     */
    #if (defined(CONFIGURE_SHELL_COMMANDS_ALL) && \
         !defined(CONFIGURE_SHELL_COMMAND_MALLOC_INFO)) || \
        defined(CONFIGURE_SHELL_COMMAND_MALLOC_INFO)
      &rtems_shell_MALLOC_INFO_Command,
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
       !defined(CONFIGURE_SHELL_COMMAND_MOUNT)) || \
       defined(CONFIGURE_SHELL_COMMAND_UNMOUNT)
    rtems_shell_filesystems_t *rtems_shell_Mount_filesystems[] = {
      #if defined(CONFIGURE_SHELL_MOUNT_MSDOS)
        &rtems_shell_Mount_MSDOS,
      #endif
      #if RTEMS_NETWORKING
        #if defined(CONFIGURE_SHELL_MOUNT_TFTP)
          &rtems_shell_Mount_TFTP,
        #endif
        #if defined(CONFIGURE_SHELL_MOUNT_FTP)
          &rtems_shell_Mount_FTP,
        #endif
        #if defined(CONFIGURE_SHELL_MOUNT_NFS)
          &rtems_shell_Mount_NFS,
        #endif
      #endif
      NULL
    };
  #endif

#endif

#endif
