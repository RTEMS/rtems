@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $
@c

@chapter Memory Commands

@section Introduction

The RTEMS shell has the following memory commands:

@itemize @bullet


@item @code{mdump} - Display contents of memory

@item @code{wdump} - Display contents of memory (word)

@item @code{medit} - Modify contents of memory

@item @code{mfill} - File memory with pattern

@item @code{mmove} - Move contents of memory

@item @code{malloc} - Obtain information on C Program Heap

@end itemize

@section Commands

@c
@c
@c
@page
@subsection mdump - display contents of memory

@pgindex mdump

@subheading SYNOPSYS:

mdump [addr [size]]

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use mdump:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MDUMP
@findex CONFIGURE_SHELL_COMMAND_MDUMP

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_MDUMP} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_MDUMP} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mdump

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mdump(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MDUMP_Command;
@end example

@c
@c
@c
@page
@subsection wdump - display contents of memory (word)

@pgindex wdump

@subheading SYNOPSYS:

wdump [addr [size]]

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use wdump:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_WDUMP
@findex CONFIGURE_SHELL_COMMAND_WDUMP

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_WDUMP} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_WDUMP} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_wdump

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_wdump(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_WDUMP_Command;
@end example

@c
@c
@c
@page
@subsection medit - modify contents of memory

@pgindex medit

@subheading SYNOPSYS:

medit addr value [value ...]

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use medit:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MEDIT
@findex CONFIGURE_SHELL_COMMAND_MEDIT

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_MEDIT} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_MEDIT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_medit

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_medit(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MEDIT_Command;
@end example

@c
@c
@c
@page
@subsection mfill - file memory with pattern

@pgindex mfill

@subheading SYNOPSYS:

mfill addr size value

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use mfill:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MFILL
@findex CONFIGURE_SHELL_COMMAND_MFILL

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_MFILL} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_MFILL} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mfill

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mfill(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MFILL_Command;
@end example

@c
@c
@c
@page
@subsection mmove - move contents of memory

@pgindex mmove

@subheading SYNOPSYS:

mmove dst src size

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use mmove:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MMOVE
@findex CONFIGURE_SHELL_COMMAND_MMOVE

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_MMOVE} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_MMOVE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mmove

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mmove(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MMOVE_Command;
@end example

@c
@c
@c
@page
@subsection malloc - obtain information on c program heap

@pgindex malloc

@subheading SYNOPSYS:

malloc [info|stats]

@subheading DESCRIPTION:

This command XXX

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use malloc:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MALLOC
@findex CONFIGURE_SHELL_COMMAND_MALLOC

This command is included as part of the all commands the shell command
set.  Define @code{CONFIGURE_SHELL_COMMAND_MALLOC} when building
a custom command set.

This command can be excluded from the shell command set with the
define @code{CONFIGURE_SHELL_NO_COMMAND_MALLOC} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_malloc

The @code{COMMAND} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_malloc(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{COMMAND} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MALLOC_Command;
@end example

