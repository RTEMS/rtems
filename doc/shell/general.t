@c
@c  COPYRIGHT (c) 1988-2010.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter General Commands

@section Introduction

The RTEMS shell has the following general commands:

@itemize @bullet

@item @code{alias} - Add alias for an existing command
@item @code{date} - Print or set current date and time
@item @code{echo} - Produce message in a shell script
@item @code{sleep} - Delay for a specified amount of time
@item @code{id} - show uid gid euid and egid
@item @code{tty} - show ttyname
@item @code{whoami} - print effective user id
@item @code{getenv} - print environment variable
@item @code{setenv} - set environment variable
@item @code{unsetenv} - unset environment variable
@item @code{time} - time command execution
@item @code{logoff} - logoff from the system
@item @code{rtc} - RTC driver configuration
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
@subsection echo - produce message in a shell script

@pgindex echo

@subheading SYNOPSYS:

@example
echo [-n | -e] args ...
@end example

@subheading DESCRIPTION:

echo prints its arguments on the standard output, separated by spaces.
Unless the @b{-n} option is present, a newline is output following the
arguments.  The @b{-e} option causes echo to treat the escape sequences 
specially, as described in the following paragraph.  The @b{-e} option is the
default, and is provided solely for compatibility with other systems.
Only one of the options @b{-n} and @b{-e} may be given.

If any of the following sequences of characters is encountered during
output, the sequence is not output.  Instead, the specified action is
performed:

@table @b
@item \b
A backspace character is output.

@item \c
Subsequent output is suppressed.  This is normally used at the
end of the last argument to suppress the trailing newline that
echo would otherwise output.

@item \f
Output a form feed.

@item \n
Output a newline character.

@item \r
Output a carriage return.

@item \t
Output a (horizontal) tab character.

@item \v
Output a vertical tab.

@item \0digits
Output the character whose value is given by zero to three digits.  
If there are zero digits, a nul character is output.

@item \\
Output a backslash.

@end table

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

The octal character escape mechanism (\0digits) differs from the C lan-
guage mechanism.

There is no way to force @code{echo} to treat its arguments literally, rather
than interpreting them as options and escape sequences.


@subheading EXAMPLES:

The following is an example of how to use @code{echo}:

@example
SHLL [/] $ echo a b c
a b c
SHLL [/] $ echo

@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_ECHO
@findex CONFIGURE_SHELL_COMMAND_ECHO

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_ECHO} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_ECHO} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_echo

The @code{echo} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_echo(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{echo} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_ECHO_Command;
@end example

@subheading ORIGIN:

The implementation and portions of the documentation for this
command are from NetBSD 4.0.

@c
@c
@c
@page
@subsection sleep - delay for a specified amount of time

@pgindex sleep

@subheading SYNOPSYS:

@example
sleep seconds
sleep seconds nanoseconds
@end example

@subheading DESCRIPTION:

This command causes the task executing the shell to block
for the specified number of @code{seconds} and @code{nanoseconds}.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

This command is implemented using the @code{nanosleep()} method.

The command line interface is similar to the @code{sleep} command
found on POSIX systems but the addition of the @code{nanoseconds}
parameter allows fine grained delays in shell scripts without
adding another command such as @code{usleep}.

@subheading EXAMPLES:

The following is an example of how to use @code{sleep}:

@example
SHLL [/] $ sleep 10
SHLL [/] $ sleep 0 5000000
@end example

It is not clear from the above but there is a ten second
pause after executing the first command before the prompt
is printed.  The second command completes very quickly
from a human perspective and there is no noticeable 
delay in the prompt being printed.
@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_SLEEP
@findex CONFIGURE_SHELL_COMMAND_SLEEP

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_SLEEP} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_SLEEP} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_sleep

The @code{sleep} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_sleep(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{sleep} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_SLEEP_Command;
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
@subsection getenv - print environment variable

@pgindex getenv

@subheading SYNOPSYS:

@example
getenv variable
@end example

@subheading DESCRIPTION:

This command is used to display the value of a @code{variable} in the set
of environment variables.

@subheading EXIT STATUS:

This command will return 1 and print a diagnostic message if
a failure occurs.

@subheading NOTES:

The entire RTEMS application shares a single set of environment variables.

@subheading EXAMPLES:

The following is an example of how to use @code{getenv}:

@example
SHLL [/] $ getenv BASEPATH
/mnt/hda1
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_GETENV
@findex CONFIGURE_SHELL_COMMAND_GETENV

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_GETENV} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_GETENV} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_getenv

The @code{getenv} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_getenv(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{getenv} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_GETENV_Command;
@end example

@c
@c
@c
@page
@subsection setenv - set environment variable

@pgindex setenv

@subheading SYNOPSYS:

@example
setenv variable [value]
@end example

@subheading DESCRIPTION:

This command is used to add a new @code{variable} to the set of environment
variables or to modify the variable of an already existing @code{variable}.
If the @code{value} is not provided, the @code{variable} will be set to the
empty string.

@subheading EXIT STATUS:

This command will return 1 and print a diagnostic message if
a failure occurs.

@subheading NOTES:

The entire RTEMS application shares a single set of environment variables.

@subheading EXAMPLES:

The following is an example of how to use @code{setenv}:

@example
SHLL [/] $ setenv BASEPATH /mnt/hda1
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_SETENV
@findex CONFIGURE_SHELL_COMMAND_SETENV

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_SETENV} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_SETENV} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_setenv

The @code{setenv} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_setenv(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{setenv} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_SETENV_Command;
@end example

@c
@c
@c
@page
@subsection unsetenv - unset environment variable

@pgindex unsetenv

@subheading SYNOPSYS:

@example
unsetenv variable
@end example

@subheading DESCRIPTION:

This command is remove to a @code{variable} from the set of environment
variables.

@subheading EXIT STATUS:

This command will return 1 and print a diagnostic message if
a failure occurs.

@subheading NOTES:

The entire RTEMS application shares a single set of environment variables.

@subheading EXAMPLES:

The following is an example of how to use @code{unsetenv}:

@example
SHLL [/] $ unsetenv BASEPATH
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_UNSETENV
@findex CONFIGURE_SHELL_COMMAND_UNSETENV

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_UNSETENV} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_UNSETENV} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_unsetenv

The @code{unsetenv} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_unsetenv(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{unsetenv} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_UNSETENV_Command;
@end example

@c
@c
@c
@page
@subsection time - time command execution

@pgindex time

@subheading SYNOPSYS:

@example
time command [argument ...]
@end example

@subheading DESCRIPTION:

The time command executes and times a command.  After the command
finishes, time writes the total time elapsed.  Times are reported in
seconds.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

None.

@subheading EXAMPLES:

The following is an example of how to use @code{time}:

@example
SHLL [/] $ time cp -r /nfs/directory /c
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_TIME
@findex CONFIGURE_SHELL_COMMAND_TIME

This command is included in the default shell command set.  When
building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_TIME} to have this command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_TIME} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_time

The @code{time} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_time(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{time} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_TIME_Command;
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
@subsection rtc - RTC driver configuration

@pgindex rtc

@subheading SYNOPSYS:

@example
rtc
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_RTC
@findex CONFIGURE_SHELL_COMMAND_RTC

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_RTC} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_RTC} when all
shell commands have been configured.

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
