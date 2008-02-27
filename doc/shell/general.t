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
@item @code{date} - Print current date and time
@item @code{id} - show uid gid euid and egid
@item @code{tty} - show ttyname
@item @code{whoami} - show current user
@item @code{logoff} - logoff from the system
@item @code{exit} - alias for logoff command

@end itemize

@section Commands

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
@subsection date - print current date and time

@pgindex date

@subheading SYNOPSYS:

@example
date
@end example

@subheading DESCRIPTION:

This command prints the current date.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

This comm

@subheading EXAMPLES:

The following is an example of how to use @code{date}:

@example
SHLL [/] $ date
Fri Jan  1 00:00:06 1988
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

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{id}:

@example
EXAMPLE_TBD
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

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{tty}:

@example
EXAMPLE_TBD
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
@subsection whoami - show current user

@pgindex whoami

@subheading SYNOPSYS:

@example
whoami
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{whoami}:

@example
EXAMPLE_TBD
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

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{logoff}:

@example
EXAMPLE_TBD
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
@subsection exit - alias for logoff command

@pgindex exit

@subheading SYNOPSYS:

@example
exit
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{exit}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_EXIT
@findex CONFIGURE_SHELL_COMMAND_EXIT

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_EXIT} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_EXIT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_exit

The @code{exit} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_exit(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{exit} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_EXIT_Command;
@end example

