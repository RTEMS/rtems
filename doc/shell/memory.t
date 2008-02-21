@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
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

@example
mdump [addr [size]]
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{mdump}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MDUMP
@findex CONFIGURE_SHELL_COMMAND_MDUMP

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MDUMP} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MDUMP} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mdump

The @code{mdump} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mdump(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{mdump} has the
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

@example
wdump [addr [size]]
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{wdump}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_WDUMP
@findex CONFIGURE_SHELL_COMMAND_WDUMP

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_WDUMP} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_WDUMP} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_wdump

The @code{wdump} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_wdump(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{wdump} has the
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

@example
medit addr value [value ...]
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{medit}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MEDIT
@findex CONFIGURE_SHELL_COMMAND_MEDIT

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MEDIT} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MEDIT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_medit

The @code{medit} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_medit(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{medit} has the
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

@example
mfill addr size value
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{mfill}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MFILL
@findex CONFIGURE_SHELL_COMMAND_MFILL

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MFILL} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MFILL} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mfill

The @code{mfill} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mfill(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{mfill} has the
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

@example
mmove dst src size
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{mmove}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MMOVE
@findex CONFIGURE_SHELL_COMMAND_MMOVE

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MMOVE} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MMOVE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mmove

The @code{mmove} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mmove(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{mmove} has the
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

@example
malloc [info|stats]
@end example

@subheading DESCRIPTION:

This command XXX

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{malloc}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MALLOC
@findex CONFIGURE_SHELL_COMMAND_MALLOC

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MALLOC} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MALLOC} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_malloc

The @code{malloc} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_malloc(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{malloc} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MALLOC_Command;
@end example

