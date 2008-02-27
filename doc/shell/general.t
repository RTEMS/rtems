@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter General Commands

@section Introduction

The RTEMS shell has the following general commands:

@itemize @bullet

@item @code{alias} - Add alias for an existing command
@item @code{date} - Print or set current date and time
@item @code{id} - show uid gid euid and egid
@item @code{tty} - show ttyname
@item @code{whoami} - print effective user id
@item @code{logoff} - logoff from the system
@item @code{exit} - alias for logoff command

@end itemize

@section Commands

This section details the General Commands available.  A
subsection is dedicated to each of the commands and
describes the behavior and configuration of that
command as well as providing an example usage.
@c
@c
@c
@page
@subsection alias - add alias for an existing command

@pgindex alias

@subheading SYNOPSYS:

@example
alias oldCommand newCommand
@end example

@subheading DESCRIPTION:

This command adds an alternate name for an existing command to
the command set.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{alias}:

@example
SHLL [/] $ me
shell:me command not found
SHLL [/] $ alias whoami me
SHLL [/] $ me
rtems
SHLL [/] $ whoami
rtems
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_ALIAS
@findex CONFIGURE_SHELL_COMMAND_ALIAS

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_ALIAS} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_ALIAS} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_alias

The @code{alias} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_alias(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{alias} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_ALIAS_Command;
@end example

@c
@c
@c
@page
@subsection date - print or set current date and time

@pgindex date

@subheading SYNOPSYS:

@example
date
date DATE TIME
@end example

@subheading DESCRIPTION:

This command operates one of two modes.  When invoked with no
arguments, it prints the current date and time.  When invoked
with both @code{date} and @code{time} arguments, it sets the
current time.  

The @code{date} is specified in @code{YYYY-MM-DD} format.
The @code{time} is specified in @code{HH:MM:SS} format.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

This comm

@subheading EXAMPLES:

The following is an example of how to use @code{date}:

@example
SHLL [/] $ date
Fri Jan  1 00:00:09 1988
SHLL [/] $ date 2008-02-29 06:45:32
SHLL [/] $ date
Fri Feb 29 06:45:35 2008
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_DATE
@findex CONFIGURE_SHELL_COMMAND_DATE

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_DATE} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_DATE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_date

The @code{date} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_date(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{date} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_DATE_Command;
@end example

@c
@c
@c
@page
@subsection id - show uid gid euid and egid

@pgindex id

@subheading SYNOPSYS:

@example
id
@end example

@subheading DESCRIPTION:

This command prints the user identity.  This includes the user id
(uid), group id (gid), effective user id (euid), and effective
group id (egid).

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

Remember there is only one POSIX process in a single processor RTEMS
application. Each thread may have its own user identity and that
identity is used by the filesystem to enforce permissions.

@subheading EXAMPLES:

The first example of the @code{id} command is from a session logged
in as the normal user @code{rtems}:

@example
SHLL [/] # id
uid=1(rtems),gid=1(rtems),euid=1(rtems),egid=1(rtems)
@end example

The second example of the @code{id} command is from a session logged
in as the @code{root} user:

@example
SHLL [/] # id
uid=0(root),gid=0(root),euid=0(root),egid=0(root)
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_ID
@findex CONFIGURE_SHELL_COMMAND_ID

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_ID} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_ID} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_id

The @code{id} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_id(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{id} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_ID_Command;
@end example

@c
@c
@c
@page
@subsection tty - show ttyname

@pgindex tty

@subheading SYNOPSYS:

@example
tty
@end example

@subheading DESCRIPTION:

This command prints the file name of the device connected
to standard input.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{tty}:

@example
SHLL [/] $ tty
/dev/console
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_TTY
@findex CONFIGURE_SHELL_COMMAND_TTY

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_TTY} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_TTY} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_tty

The @code{tty} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_tty(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{tty} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_TTY_Command;
@end example

@c
@c
@c
@page
@subsection whoami - print effective user id

@pgindex whoami

@subheading SYNOPSYS:

@example
whoami
@end example

@subheading DESCRIPTION:

This command displays the user name associated with the current
effective user id.

@subheading EXIT STATUS:

This command always succeeds.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{whoami}:

@example
SHLL [/] $ whoami
rtems
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_WHOAMI
@findex CONFIGURE_SHELL_COMMAND_WHOAMI

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_WHOAMI} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_WHOAMI} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_whoami

The @code{whoami} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_whoami(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{whoami} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_WHOAMI_Command;
@end example

@c
@c
@c
@page
@subsection logoff - logoff from the system

@pgindex logoff

@subheading SYNOPSYS:

@example
logoff
@end example

@subheading DESCRIPTION:

This command logs the user out of the shell.

@subheading EXIT STATUS:

This command does not return.

@subheading NOTES:

The system behavior when the shell is exited depends upon how the
shell was initiated.  The typical behavior is that a login prompt
will be displayed for the next login attempt or that the connection
will be dropped by the RTEMS system.

@subheading EXAMPLES:

The following is an example of how to use @code{logoff}:

@example
SHLL [/] $ logoff
logoff from the system...
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_LOGOFF
@findex CONFIGURE_SHELL_COMMAND_LOGOFF

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_LOGOFF} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_LOGOFF} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_logoff

The @code{logoff} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_logoff(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{logoff} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_LOGOFF_Command;
@end example

@c
@c
@c
@page
@subsection exit - exit the shell

@pgindex exit

@subheading SYNOPSYS:

@example
exit
@end example

@subheading DESCRIPTION:

This command causes the shell interpreter to @code{exit}.

@subheading EXIT STATUS:

This command does not return.

@subheading NOTES:

In contrast to @ref{General Commands logoff - logoff from the system, logoff},
this command is built into the shell interpreter loop.

@subheading EXAMPLES:

The following is an example of how to use @code{exit}:

@example
SHLL [/] $ exit
Shell exiting
@end example

@subheading CONFIGURATION:

This command is always present and cannot be disabled.

@subheading PROGRAMMING INFORMATION:

The @code{exit} is implemented directly in the shell interpreter.
There is no C routine associated with it.

