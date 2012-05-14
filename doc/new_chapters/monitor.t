@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 

@chapter Monitor Task

@section Introduction

The monitor task is a simple interactive shell that allows the user to
make inquries about he state of various system objects.  The routines
provided by the monitor task manager are:

@itemize @bullet
@item @code{rtems_monitor_init} - Initialize the Monitor Task
@item @code{rtems_monitor_wakeup} - Wakeup the Monitor Task
@end itemize

@section Background

There is no background information.

@section Operations

@subsection Initializing the Monitor

The monitor is initialized by calling @code{rtems_monitor_init}.  When
initialized, the monitor is created as an independent task.  An example
of initializing the monitor is shown below:

@example
@group
#include <rtems/monitor.h>
        ...
rtems_monitor_init(0);
@end group
@end example

The "0" parameter to the @code{rtems_monitor_init} routine
causes the monitor to immediately enter command mode.
This parameter is a bitfield.  If the monitor is to suspend
itself on startup, then the @code{RTEMS_MONITOR_SUSPEND} bit
should be set.

@section Routines

This section details the monitor task manager's routines.
A subsection is dedicated to each of this manager's routines
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection rtems_monitor_init - Initialize the Monitor Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void rtems_monitor_init(
  unsigned32 monitor_flags
);
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES: NONE

@subheading DESCRIPTION:

This routine initializes the RTEMS monitor task.  The
@code{monitor_flags} parameter indicates how the server
task is to start.  This parameter is a bitfield and
has the following constants associated with it:

@itemize @bullet
@item @b{RTEMS_MONITOR_SUSPEND} - suspend monitor on startup
@item @b{RTEMS_MONITOR_GLOBAL} - monitor should be global
@end itemize

If the @code{RTEMS_MONITOR_SUSPEND} bit is set, then the
monitor task will suspend itself after it is initialized.
A subsequent call to @code{rtems_monitor_wakeup} will be required
to activate it.

@subheading NOTES:

The monitor task is created with priority 1.  If there are 
application tasks at priority 1, then there may be times
when the monitor task is not executing.

@page
@subsection rtems_monitor_wakeup - Wakeup the Monitor Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void rtems_monitor_wakeup( void );
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES: NONE

@subheading DESCRIPTION:

This routine is used to activate the monitor task if it is suspended.

@subheading NOTES:

NONE

@page
@section Monitor Interactive Commands

The following commands are supported by the monitor task:

@itemize @bullet
@item @code{help} - Obtain Help
@item @code{pause} - Pause Monitor for a Specified Number of Ticks
@item @code{exit} - Invoke a Fatal RTEMS Error
@item @code{symbol} - Show Entries from Symbol Table
@item @code{continue} - Put Monitor to Sleep Waiting for Explicit Wakeup
@item @code{config} - Show System Configuration
@item @code{itask} - List Init Tasks
@item @code{mpci} - List MPCI Config
@item @code{task} - Show Task Information
@item @code{queue} - Show Message Queue Information
@item @code{extension} - User Extensions
@item @code{driver} - Show Information About Named Drivers
@item @code{dname} - Show Information About Named Drivers
@item @code{object} - Generic Object Information
@item @code{node} - Specify Default Node for Commands That Take IDs
@end itemize


@subsection help - Obtain Help

The @code{help} command prints out the list of commands.  If invoked
with a command name as the first argument, detailed help information
on that command is printed.

@subsection pause - Pause Monitor for a Specified Number of Ticks

The @code{pause} command cause the monitor task to suspend itself
for the specified number of ticks.  If this command is invoked with
no arguments, then the task is suspended for 1 clock tick.

@subsection exit - Invoke a Fatal RTEMS Error

The @code{exit} command invokes @code{rtems_error_occurred} directive
with the specified error code.  If this command is invoked with 
no arguments, then the @code{rtems_error_occurred} directive is 
invoked with an arbitrary error code.

@subsection symbol - Show Entries from Symbol Table

The @code{symbol} command lists the specified entries in the symbol table.
If this command is invoked with no arguments, then all the 
symbols in the symbol table are printed.

@subsection continue - Put Monitor to Sleep Waiting for Explicit Wakeup

The @code{continue} command suspends the monitor task with no timeout.

@subsection config - Show System Configuration

The @code{config} command prints the system configuration.

@subsection itask - List Init Tasks

The @code{itask} command lists the tasks in the initialization tasks table.

@subsection mpci - List MPCI Config

The @code{mpci} command shows the MPCI configuration information

@subsection task - Show Task Information

The @code{task} command prints out information about one or more tasks in
the system.  If invoked with no arguments, then
information on all the tasks in the system is printed.

@subsection queue - Show Message Queue Information

The @code{queue} command prints out information about one or more 
message queues in the system.  If invoked with no arguments, then
information on all the message queues in the system is printed.

@subsection extension - User Extensions

The @code{extension} command prints out information about the user
extensions.

@subsection driver - Show Information About Named Drivers

The @code{driver} command prints information about the device driver table.

@subsection dname - Show Information About Named Drivers

The @code{dname} command prints information about the named device drivers.

@subsection object - Generic Object Information

The @code{object} command prints information about RTEMS objects.

@subsection node - Specify Default Node for Commands That Take IDs

The @code{node} command sets the default node for commands that look
at object ID ranges.

