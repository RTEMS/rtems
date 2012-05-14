@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Configuration and Initialization

@section Introduction

This chapter provides information on how the application
configures and intializes the RTEMS shell.

@c
@c
@c
@section Configuration

The command set available to the application is user configurable.
It is configured using a mechanism similar to the @code{confdefs.h}
mechanism used to specify application configuration. 

In the simplest case, if the user wishes to configure a command
set with all commands available that are neither filesystem
management (e.g. mounting, formating, etc.) or network related,
then the following is all that is required:

@smallexample
#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL

#include <rtems/shellconfig.h>
@end smallexample

In a slightly more complex example, if the user wishes to include
all networking commands as well as support for mounting MS-DOS and
NFS filesystems, then the following is all that is required:

@smallexample
#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL
#define CONFIGURE_SHELL_MOUNT_MSDOS
#define CONFIGURE_SHELL_MOUNT_NFS

#include <rtems/shellconfig.h>
@end smallexample

@subsection Customizing the Command Set

The user can configure specific command sets by either building
up the set from individual commands or starting with a complete
set and disabling individual commands.  Each command has two
configuration macros associated with it.

@table @b

@item @code{CONFIGURE_SHELL_COMMAND_XXX}
Each command has a constant of this form which is defined when
building a command set by individually enabling specific
commands.  

@item @code{CONFIGURE_SHELL_NO_COMMAND_XXX}
In contrast, each command has a similar command which is
defined when the application is configuring a command set
by disabling specific commands in the set.

@end table

@subsection Adding Custom Commands

One of the design goals of the RTEMS Shell was to make it
easy for a user to add custom commands specific to their
application.  We believe this design goal was accomplished.
In order to add a custom command, the user is required to 
do the following:

@itemize @bullet

@item Provide a @i{main-style} function which implements
the command.  If that command function uses a @code{getopt}
related function to parse arguments, it @b{MUST} use the 
reentrant form.

@item Provide a command definition structure of type
@code{rtems_shell_cmd_t}.

@item Configure that command using the
@code{CONFIGURE_SHELL_USER_COMMANDS} macro.
@end itemize

Custom aliases are configured similarly but the user
only provides an alias definition structure of type
@code{rtems_shell_alias_t} and configures the alias
via the @code{CONFIGURE_SHELL_USER_ALIASES} macro.

In the following example, we have implemented a custom
command named @code{usercmd} which simply prints the 
arguments it was passed. We have also provided an
alias for @code{usercmd} named @code{userecho}.

@smallexample
#include <rtems/shell.h>

int main_usercmd(int argc, char **argv)
@{
  int i;
  printf( "UserCommand: argc=%d\n", argc );
  for (i=0 ; i<argc ; i++ )
    printf( "argv[%d]= %s\n", i, argv[i] );
  return 0;
@}

rtems_shell_cmd_t Shell_USERCMD_Command = @{
  "usercmd",                  /* name */
  "usercmd n1 [n2 [n3...]]",  /* usage */
  "user",                     /* topic */
  main_usercmd,               /* command */
  NULL,                       /* alias */
  NULL                        /* next */
@};

rtems_shell_alias_t Shell_USERECHO_Alias = @{
  "usercmd",                  /* command */
  "userecho"                  /* alias */
@};
  
#define CONFIGURE_SHELL_USER_COMMANDS &Shell_USERCMD_Command
#define CONFIGURE_SHELL_USER_ALIASES &Shell_USERECHO_Alias
#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL
#define CONFIGURE_SHELL_MOUNT_MSDOS

#include <rtems/shellconfig.h>
@end smallexample

Notice in the above example, that the user wrote the
@i{main} for their command (e.g. @code{main_usercmd})
which looks much like any other @code{main()}.  They
then defined a @code{rtems_shell_cmd_t} structure
named @code{Shell_USERCMD_Command} which describes that
command.  This command definition structure is registered
into the static command set by defining
@code{CONFIGURE_SHELL_USER_COMMANDS} to
@code{&Shell_USERCMD_Command}.

Similarly, to add the @code{userecho} alias, the user
provides the alias definition structure named
@code{Shell_USERECHO_Alias} and defines
@code{CONFIGURE_SHELL_USER_ALIASES} to configure
the alias.

The user can configure any number of commands
and aliases in this manner.

@c
@c
@c
@section Initialization

The shell may be easily attached to a serial port or
to the @code{telnetd} server.  This section describes
how that is accomplished. 

@c
@c
@c
@subsection Attached to a Serial Port

Starting the shell attached to the console or a serial 
port is very simple. The user invokes @code{rtems_shell_init}
with parameters to indicate the characteristics of the task
that will be executing the shell including name, stack size,
and priority.  The user also specifies the device that the
shell is to be attached to.

This example is taken from the @code{fileio} sample test.
This shell portion of this test can be run on any target which
provides a console with input and output capabilities.  It does
not include any commands which cannot be supported on all BSPs.
The source code for this test is in @code{testsuites/samples/fileio}
with the shell configuration in the @code{init.c} file.

@smallexample
#include <rtems/shell.h>

void start_shell(void)
@{
  printf(" =========================\n");
  printf(" starting shell\n");
  printf(" =========================\n");
  rtems_shell_init(
    "SHLL",                       /* task name */
    RTEMS_MINIMUM_STACK_SIZE * 4, /* task stack size */
    100,                          /* task priority */
    "/dev/console",               /* device name */
    false,                        /* run forever */
    true,                         /* wait for shell to terminate */
    rtems_shell_login_check       /* login check function,
                                     use NULL to disable a login check */
  );
@}
@end smallexample

In the above example, the call to @code{rtems_shell_init}
spawns a task to run the RTEMS Shell attached to @code{/dev/console}
and executing at priority 100.  The caller suspends itself and
lets the shell take over the console device.  When the shell
is exited by the user, then control returns to the caller.

@c
@c
@c
@subsection Attached to a Socket

TBD

@section Functions

This section describes the Shell related C functions which are 
publicly available related to initialization and configuration.

@page
@subsection rtems_shell_init - initialize the shell

@cindex initialization

@subheading CALLING SEQUENCE:

@findex rtems_shell_init
@example
rtems_status_code rtems_shell_init(
  const char          *task_name,
  size_t               task_stacksize,
  rtems_task_priority  task_priority,
  const char          *devname,
  bool                 forever,
  bool                 wait,
  rtems_login_check    login_check
);
@end example

@subheading DIRECTIVE STATUS CODES:
@code{RTEMS_SUCCESSFUL} - Shell task spawned successfully@*
others - to indicate a failure condition

@subheading DESCRIPTION:
This service creates a task with the specified characteristics to
run the RTEMS Shell attached to the specified @code{devname}.

@subheading NOTES:

This method invokes the @code{rtems_task_create} and @code{rtems_task_start}
directives and as such may return any status code that those directives
may return.

