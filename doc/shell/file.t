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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_UMASK}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_UMASK}.


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

cp [-R [-H | -L | -P]] [-f | -i] [-pv] src target # copy

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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_CP}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_CP}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_PWD}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_PWD}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_LS}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_LS}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_CHDIR}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_CHDIR}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_MKDIR}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_MKDIR}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_RMDIR}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_RMDIR}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_CHROOT}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_CHROOT}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_CHMOD}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_CHMOD}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_CAT}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_CAT}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_RM}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_RM}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_MOUNT}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_MOUNT}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_UNMOUNT}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_UNMOUNT}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_BLKSYNC}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_BLKSYNC}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_DIR}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_DIR}.


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

In order to disable this command out if you are configuring all commands,
define @code{CONFIGURE_SHELL_NO_COMMAND_CD}.

In order to enable this command if you are building a custom command set
define @code{CONFIGURE_SHELL_COMMAND_CD}.


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

