@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Network Commands

@section Introduction

The RTEMS shell has the following network commands:

@itemize @bullet

@item @code{netstats} - obtain network statistics
@item @code{ifconfig} - configure a network interface
@item @code{route} - show or manipulate the IP routing table

@end itemize

@section Commands

@c
@c
@c
@page
@subsection netstats - obtain network statistics

@pgindex netstats

@subheading SYNOPSYS:

@example
netstats [-Aimfpcutv]
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{netstats}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_NETSTATS
@findex CONFIGURE_SHELL_COMMAND_NETSTATS

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_NETSTATS} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_NETSTATS} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_netstats

The @code{netstats} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_netstats(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{netstats} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_NETSTATS_Command;
@end example

@c
@c
@c
@page
@subsection ifconfig - configure a network interface

@pgindex ifconfig

@subheading SYNOPSYS:

@example
ifconfig
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{ifconfig}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_IFCONFIG
@findex CONFIGURE_SHELL_COMMAND_IFCONFIG

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_IFCONFIG} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_IFCONFIG} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_ifconfig

The @code{ifconfig} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_ifconfig(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{ifconfig} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_IFCONFIG_Command;
@end example

@c
@c
@c
@page
@subsection route - show or manipulate the ip routing table

@pgindex route

@subheading SYNOPSYS:

@example
route [subcommand]
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{route}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_ROUTE
@findex CONFIGURE_SHELL_COMMAND_ROUTE

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_ROUTE} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_ROUTE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_route

The @code{route} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_route(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{route} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_ROUTE_Command;
@end example

