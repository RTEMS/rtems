@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Initialization Manager

@section Introduction

The initialization manager is responsible for
initiating and shutting down RTEMS.  Initiating RTEMS involves
creating and starting all configured initialization tasks, and
for invoking the initialization routine for each user-supplied
device driver.  In a multiprocessor configuration, this manager
also initializes the interprocessor communications layer.  The
directives provided by the initialization manager are:

@itemize @bullet
@item @code{@value{DIRPREFIX}initialize_executive} - Initialize RTEMS
@item @code{@value{DIRPREFIX}initialize_executive_early} - Initialize RTEMS and do NOT Start Multitasking
@item @code{@value{DIRPREFIX}initialize_executive_late} - Complete Initialization and Start Multitasking
@item @code{@value{DIRPREFIX}shutdown_executive} - Shutdown RTEMS
@end itemize

@section Background

@subsection Initialization Tasks

@cindex initialization tasks

Initialization task(s) are the mechanism by which
RTEMS transfers initial control to the user's application.
Initialization tasks differ from other application tasks in that
they are defined in the User Initialization Tasks Table and
automatically created and started by RTEMS as part of its
initialization sequence.  Since the initialization tasks are
scheduled using the same algorithm as all other RTEMS tasks,
they must be configured at a priority and mode which will insure
that they will complete execution before other application tasks
execute.  Although there is no upper limit on the number of
initialization tasks, an application is required to define at
least one.

A typical initialization task will create and start
the static set of application tasks.  It may also create any
other objects used by the application.  Initialization tasks
which only perform initialization should delete themselves upon
completion to free resources for other tasks.  Initialization
tasks may transform themselves into a "normal" application task.
This transformation typically involves changing priority and
execution mode.  RTEMS does not automatically delete the
initialization tasks.

@subsection The System Initialization Task

The System Initialization Task is responsible for
initializing all device drivers.  As a result, this task has a
higher priority than all other tasks to insure that no
application tasks executes until all device drivers are
initialized.  After device initialization in a single processor
system, this task will delete itself.

The System Initialization Task must have enough stack
space to successfully execute the initialization routines for
all device drivers and, in multiprocessor configurations, the
Multiprocessor Communications Interface Layer initialization
routine.  The CPU Configuration Table contains a field which
allows the application or BSP to increase the default amount of
stack space allocated for this task.

In multiprocessor configurations, the System
Initialization Task does not delete itself after initializing
the device drivers.  Instead it transforms itself into the
Multiprocessing Server which initializes the Multiprocessor
Communications Interface Layer, verifies multiprocessor system
consistency, and processes all requests from remote nodes.

@subsection The Idle Task

The Idle Task is the lowest priority task in a system
and executes only when no other task is ready to execute.  This
task consists of an infinite loop and will be preempted when any
other task is made ready to execute.

@subsection Initialization Manager Failure

The @code{@value{DIRPREFIX}ifatal_error_occurred} directive will be called
from @code{@value{DIRPREFIX}initialize_executive}
for any of the following reasons:

@itemize @bullet
@item If either the Configuration Table or the CPU Dependent
Information Table is not provided.

@item If the starting address of the RTEMS RAM Workspace,
supplied by the application in the Configuration Table, is NULL
or is not aligned on a four-byte boundary.

@item If the size of the RTEMS RAM Workspace is not large
enough to initialize and configure the system.

@item If the interrupt stack size specified is too small.

@item If multiprocessing is configured and the node entry in
the Multiprocessor Configuration Table is not between one and
the maximum_nodes entry.

@item If a multiprocessor system is being configured and no
Multiprocessor Communications Interface is specified.

@item If no user initialization tasks are configured.  At
least one initialization task must be configured to allow RTEMS
to pass control to the application at the end of the executive
initialization sequence.

@item If any of the user initialization tasks cannot be
created or started successfully.
@end itemize

@section Operations

@subsection Initializing RTEMS

The @code{@value{DIRPREFIX}initialize_executive}
directive is called by the
board support package at the completion of its initialization
sequence.  RTEMS assumes that the board support package
successfully completed its initialization activities.  The
@code{@value{DIRPREFIX}initialize_executive}
directive completes the initialization
sequence by performing the following actions:

@itemize @bullet
@item Initializing internal RTEMS variables;
@item Allocating system resources;
@item Creating and starting the System Initialization Task;
@item Creating and starting the Idle Task;
@item Creating and starting the user initialization task(s); and
@item Initiating multitasking.
@end itemize

This directive MUST be called before any other RTEMS
directives.  The effect of calling any RTEMS directives before
@code{@value{DIRPREFIX}initialize_executive}
is unpredictable.  Many of RTEMS actions
during initialization are based upon the contents of the
Configuration Table and CPU Dependent Information Table.  For
more information regarding the format and contents of these
tables, please refer to the chapter Configuring a System.

The final step in the initialization sequence is the
initiation of multitasking.  When the scheduler and dispatcher
are enabled, the highest priority, ready task will be dispatched
to run.  Control will not be returned to the board support
package after multitasking is enabled until
@code{@value{DIRPREFIX}shutdown_executive}
the directive is called.

The @code{@value{DIRPREFIX}initialize_executive}
directive provides a
conceptually simple way to initialize RTEMS.  However, in
certain cases, this mechanism cannot be used.  The
@code{@value{DIRPREFIX}initialize_executive_early}
and @code{@value{DIRPREFIX}initialize_executive_late}
directives are provided as an alternative mechanism for
initializing RTEMS.  The
@code{@value{DIRPREFIX}initialize_executive_early} directive
returns to the caller BEFORE initiating multitasking.  The
@code{@value{DIRPREFIX}initialize_executive_late}
directive is invoked to start
multitasking.  It is critical that only one of the RTEMS
initialization sequences be used in an application.

@subsection Shutting Down RTEMS

The @code{@value{DIRPREFIX}shutdown_executive} directive is invoked by the
application to end multitasking and return control to the board
support package.  The board support package resumes execution at
the code immediately following the invocation of the
@code{@value{DIRPREFIX}initialize_executive} directive.

@section Directives

This section details the initialization manager's
directives.  A subsection is dedicated to each of this manager's
directives and describes the calling sequence, related
constants, usage, and status codes.

@page
@subsection INITIALIZE_EXECUTIVE - Initialize RTEMS

@cindex initialize RTEMS

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_initialize_executive
@example
void rtems_initialize_executive(
  rtems_configuration_table *configuration_table,
  rtems_cpu_table           *cpu_table
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Initialize_Executive (
  Configuration_Table : in     RTEMS.Configuration_Table_Pointer;
  CPU_Table           : in     RTEMS.CPU_Table_Pointer
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive is called when the board support
package has completed its initialization to allow RTEMS to
initialize the application environment based upon the
information in the Configuration Table, CPU Dependent
Information Table, User Initialization Tasks Table, Device
Driver Table, User Extension Table, Multiprocessor Configuration
Table, and the Multiprocessor Communications Interface (MPCI)
Table.  This directive starts multitasking and does not return
to the caller until the @code{@value{DIRPREFIX}shutdown_executive}
directive is invoked.

@subheading NOTES:

This directive MUST be the first RTEMS directive
called and it DOES NOT RETURN to the caller until the
@code{@value{DIRPREFIX}shutdown_executive}
is invoked.

This directive causes all nodes in the system to
verify that certain configuration parameters are the same as
those of the local node.  If an inconsistency is detected, then
a fatal error is generated.

The application must use only one of the two
initialization sequences: 
@code{@value{DIRPREFIX}initialize_executive} or
@code{@value{DIRPREFIX}initialize_executive_early} and 
@code{@value{DIRPREFIX}initialize_executive_late}.  The
@code{@value{DIRPREFIX}initialize_executive}
directive is logically equivalent to invoking 
@code{@value{DIRPREFIX}initialize_executive_early} and
@code{@value{DIRPREFIX}initialize_executive_late}
with no intervening actions.

@page
@subsection INITIALIZE_EXECUTIVE_EARLY - Initialize RTEMS and do NOT Start Multitasking

@cindex initialize RTEMS

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_initialize_executive_early
@example
rtems_interrupt_level rtems_initialize_executive_early(
  rtems_configuration_table *configuration_table,
  rtems_cpu_table           *cpu_table
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Initialize_Executive_Early(
  Configuration_Table : in     RTEMS.Configuration_Table_Pointer;
  CPU_Table           : in     RTEMS.Cpu_Table;
  Level               :    out RTEMS.ISR_Level
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive is called when the board support
package has completed its initialization to allow RTEMS to
initialize the application environment based upon the
information in the Configuration Table, CPU Dependent
Information Table, User Initialization Tasks Table, Device
Driver Table, User Extension Table, Multiprocessor Configuration
Table, and the Multiprocessor Communications Interface (MPCI)
Table.  This directive returns to the caller after completing
the basic RTEMS initialization but before multitasking is
initiated.  The interrupt level in place when the directive is
invoked is returned to the caller.  This interrupt level should
be the same one passed to
@code{@value{DIRPREFIX}initialize_executive_late}.

@subheading NOTES:

The application must use only one of the two
initialization sequences:
@code{@value{DIRPREFIX}initialize_executive} or
@code{@value{DIRPREFIX}initialize_executive_early} and 
@code{@value{DIRPREFIX}initialize_executive_late}.

@page
@subsection INITIALIZE_EXECUTIVE_LATE - Complete Initialization and Start Multitasking

@cindex initialize RTEMS
@cindex start multitasking

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_initialize_executive_late
@example
void rtems_initialize_executive_late(
  rtems_interrupt_level  bsp_level
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Initialize_Executive_Late(
  BSP_Level : in    RTEMS.ISR_Level
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive is called after the
@code{@value{DIRPREFIX}initialize_executive_early}
directive has been called to complete
the RTEMS initialization sequence and initiate multitasking.
The interrupt level returned by the
@code{@value{DIRPREFIX}initialize_executive_early}
directive should be in bsp_level and this value is restored as
part of this directive returning to the caller after the
@code{@value{DIRPREFIX}shutdown_executive}
directive is invoked.

@subheading NOTES:

This directive MUST be the second RTEMS directive
called and it DOES NOT RETURN to the caller until the
@code{@value{DIRPREFIX}shutdown_executive} is invoked.

This directive causes all nodes in the system to
verify that certain configuration parameters are the same as
those of the local node.  If an inconsistency is detected, then
a fatal error is generated.

The application must use only one of the two
initialization sequences:
@code{@value{DIRPREFIX}initialize_executive} or
@code{@value{DIRPREFIX}initialize_executive_early} and
@code{@value{DIRPREFIX}initialize_executive_late}.



@page
@subsection SHUTDOWN_EXECUTIVE - Shutdown RTEMS

@cindex shutdown RTEMS

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_shutdown_executive
@example
void rtems_shutdown_executive(
  rtems_unsigned32 result
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Shutdown_Executive(
  result : in     RTEMS.Unsigned32
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive is called when the application wishes
to shutdown RTEMS and return control to the board support
package.  The board support package resumes execution at the
code immediately following the invocation of the
@code{@value{DIRPREFIX}initialize_executive} directive.

@subheading NOTES:

This directive MUST be the last RTEMS directive
invoked by an application and it DOES NOT RETURN to the caller.

This directive should not be invoked until the
executive has successfully completed initialization.
