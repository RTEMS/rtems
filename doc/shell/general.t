@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $
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

alias oldCommand newCommand

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use alias:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_ALIAS
@findex CONFIGURE_SHELL_COMMAND_ALIAS

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_ALIAS} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_ALIAS} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_alias

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_alias(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
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

date

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use date:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_DATE
@findex CONFIGURE_SHELL_COMMAND_DATE

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_DATE} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_DATE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_date

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_date(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
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

id

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use id:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_ID
@findex CONFIGURE_SHELL_COMMAND_ID

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_ID} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_ID} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_id

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_id(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
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

tty

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use tty:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_TTY
@findex CONFIGURE_SHELL_COMMAND_TTY

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_TTY} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_TTY} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_tty

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_tty(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
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

whoami

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use whoami:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_WHOAMI
@findex CONFIGURE_SHELL_COMMAND_WHOAMI

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_WHOAMI} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_WHOAMI} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_whoami

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_whoami(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
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

logoff

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use logoff:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_LOGOFF
@findex CONFIGURE_SHELL_COMMAND_LOGOFF

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_LOGOFF} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_LOGOFF} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_logoff

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_logoff(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
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

exit

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use exit:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_EXIT
@findex CONFIGURE_SHELL_COMMAND_EXIT

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_EXIT} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_EXIT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_exit

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_exit(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_EXIT_Command;
@end example

