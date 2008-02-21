@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $
@c

@chapter File and Directory Commands

@section Introduction

The RTEMS shell has the following file and directory commands:

@itemize @bullet


@item @code{umask} - Set file mode creation mask

@item @code{cp} - copy files

@item @code{pwd} - print work directory

@item @code{ls} - list files in the directory

@item @code{chdir} - change the current directory

@item @code{mkdir} - create a directory

@item @code{rmdir} - remove directory

@item @code{chroot} - change the root directory

@item @code{chmod} - change permissions of a file

@item @code{cat} - display file contents

@item @code{msdosfmt} - format disk

@item @code{rm} - remove files

@item @code{mount} - mount disk

@item @code{unmount} - unmount disk

@item @code{blksync} - sync the block driver

@item @code{dir} - alias for ls

@item @code{cd} - alias for chdir

@end itemize

@section Commands

@c
@c
@c
@page
@subsection umask - set file mode creation mask

@pgindex umask

@subheading SYNOPSYS:

umask [new_umask]

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use umask:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_UMASK
@findex CONFIGURE_SHELL_COMMAND_UMASK

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_UMASK} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_UMASK} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_umask

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_umask(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_UMASK_Command;
@end example

@c
@c
@c
@page
@subsection cp - copy files

@pgindex cp

@subheading SYNOPSYS:

cp [-R [-H | -L | -P]] [-f | -i] [-pv] src target

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use cp:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CP
@findex CONFIGURE_SHELL_COMMAND_CP

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_CP} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_CP} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_cp

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_cp(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CP_Command;
@end example

@c
@c
@c
@page
@subsection pwd - print work directory

@pgindex pwd

@subheading SYNOPSYS:

pwd

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use pwd:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_PWD
@findex CONFIGURE_SHELL_COMMAND_PWD

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_PWD} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_PWD} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_pwd

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_pwd(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_PWD_Command;
@end example

@c
@c
@c
@page
@subsection ls - list files in the directory

@pgindex ls

@subheading SYNOPSYS:

ls [dir]

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use ls:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_LS
@findex CONFIGURE_SHELL_COMMAND_LS

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_LS} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_LS} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_ls

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_ls(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_LS_Command;
@end example

@c
@c
@c
@page
@subsection chdir - change the current directory

@pgindex chdir

@subheading SYNOPSYS:

chdir [dir]

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use chdir:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CHDIR
@findex CONFIGURE_SHELL_COMMAND_CHDIR

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_CHDIR} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_CHDIR} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_chdir

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_chdir(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CHDIR_Command;
@end example

@c
@c
@c
@page
@subsection mkdir - create a directory

@pgindex mkdir

@subheading SYNOPSYS:

mkdir  dir

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use mkdir:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MKDIR
@findex CONFIGURE_SHELL_COMMAND_MKDIR

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_MKDIR} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_MKDIR} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mkdir

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mkdir(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MKDIR_Command;
@end example

@c
@c
@c
@page
@subsection rmdir - remove directory

@pgindex rmdir

@subheading SYNOPSYS:

rmdir  dir

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use rmdir:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_RMDIR
@findex CONFIGURE_SHELL_COMMAND_RMDIR

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_RMDIR} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_RMDIR} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_rmdir

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_rmdir(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_RMDIR_Command;
@end example

@c
@c
@c
@page
@subsection chroot - change the root directory

@pgindex chroot

@subheading SYNOPSYS:

chroot [dir]

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use chroot:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CHROOT
@findex CONFIGURE_SHELL_COMMAND_CHROOT

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_CHROOT} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_CHROOT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_chroot

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_chroot(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CHROOT_Command;
@end example

@c
@c
@c
@page
@subsection chmod - change permissions of a file

@pgindex chmod

@subheading SYNOPSYS:

chmod 0777 n1 n2...

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use chmod:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CHMOD
@findex CONFIGURE_SHELL_COMMAND_CHMOD

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_CHMOD} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_CHMOD} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_chmod

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_chmod(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CHMOD_Command;
@end example

@c
@c
@c
@page
@subsection cat - display file contents

@pgindex cat

@subheading SYNOPSYS:

cat n1 [n2 [n3...]]

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use cat:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CAT
@findex CONFIGURE_SHELL_COMMAND_CAT

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_CAT} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_CAT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_cat

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_cat(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CAT_Command;
@end example

@c
@c
@c
@page
@subsection rm - remove files

@pgindex rm

@subheading SYNOPSYS:

rm n1 [n2 [n3...]]

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use rm:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_RM
@findex CONFIGURE_SHELL_COMMAND_RM

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_RM} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_RM} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_rm

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_rm(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_RM_Command;
@end example

@c
@c
@c
@page
@subsection mount - mount disk

@pgindex mount

@subheading SYNOPSYS:

mount [-t fstype] [-r] [-L] device path

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use mount:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MOUNT
@findex CONFIGURE_SHELL_COMMAND_MOUNT

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_MOUNT} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_MOUNT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mount

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mount(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MOUNT_Command;
@end example

@c
@c
@c
@page
@subsection unmount - unmount disk

@pgindex unmount

@subheading SYNOPSYS:

unmount path

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use unmount:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_UNMOUNT
@findex CONFIGURE_SHELL_COMMAND_UNMOUNT

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_UNMOUNT} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_UNMOUNT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_unmount

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_unmount(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_UNMOUNT_Command;
@end example

@c
@c
@c
@page
@subsection blksync - sync the block driver

@pgindex blksync

@subheading SYNOPSYS:

blksync driver

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use blksync:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_BLKSYNC
@findex CONFIGURE_SHELL_COMMAND_BLKSYNC

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_BLKSYNC} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_BLKSYNC} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_blksync

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_blksync(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_BLKSYNC_Command;
@end example

@c
@c
@c
@page
@subsection dir - alias for ls

@pgindex dir

@subheading SYNOPSYS:

display directory contents

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use dir:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_DIR
@findex CONFIGURE_SHELL_COMMAND_DIR

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_DIR} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_DIR} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_dir

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_dir(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_DIR_Command;
@end example

@c
@c
@c
@page
@subsection cd - alias for chdir

@pgindex cd

@subheading SYNOPSYS:

change the current directory

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use cd:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CD
@findex CONFIGURE_SHELL_COMMAND_CD

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_CD} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_CD} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_cd

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_cd(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CD_Command;
@end example

