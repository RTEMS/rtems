@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Initialization Manager

@section Introduction

The Initialization Manager is responsible for
initiating and shutting down RTEMS.  Initiating RTEMS involves
creating and starting all configured initialization tasks, and
for invoking the initialization routine for each user-supplied
device driver.  In a multiprocessor configuration, this manager
also initializes the interprocessor communications layer.  The
directives provided by the Initialization Manager are:

@itemize @bullet
@item @code{@value{DIRPREFIX}initialize_data_structures} - Initialize RTEMS Data Structures
@item @code{@value{DIRPREFIX}initialize_before_drivers} - Perform Initialization Before Device Drivers
@item @code{@value{DIRPREFIX}initialize_device_drivers} - Initialize Device Drivers
@item @code{@value{DIRPREFIX}initialize_start_multitasking} - Complete Initialization and Start Multitasking
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
they must be configured at a priority and mode which will ensure
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

@subsection System Initialization

System Initialization begins with board reset and continues
through RTEMS initialization, initialization of all device
drivers, and eventually a context switch to the first user
task.  Remember, that interrupts are disabled during
initialization and the @i{initialization thread} is not
a task in any sense and the user should be very careful
during initialzation.

The BSP must ensure that the there is enough stack
space reserved for the initialization "thread" to
successfully execute the initialization routines for
all device drivers and, in multiprocessor configurations, the
Multiprocessor Communications Interface Layer initialization
routine.

@subsection The Idle Task

The Idle Task is the lowest priority task in a system
and executes only when no other task is ready to execute.  This
default implementation of this task consists of an infinite
loop. RTEMS allows the Idle Task body to be replaced by a CPU
specific implementation, a BSP specific implementation or an
application specific implementation.

The Idle Task is preemptible and @b{WILL} be preempted when
any other task is made ready to execute.  This characteristic is
critical to the overall behavior of any application.

@subsection Initialization Manager Failure

The @code{@value{DIRPREFIX}fatal_error_occurred} directive will
be invoked from @code{@value{DIRPREFIX}initialize_executive}
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
 
A discussion of RTEMS actions when a fatal error occurs 
may be found @ref{Fatal Error Manager Announcing a Fatal Error}.


@section Operations

@subsection Initializing RTEMS

The Initialization Manager directives are called by the
Board Support Package framework as part of its initialization
sequence.  RTEMS assumes that the Board Support Package
successfully completed its initialization activities.  These
directives initialize RTEMS by performing the following actions:

@itemize @bullet
@item Initializing internal RTEMS variables;
@item Allocating system resources;
@item Creating and starting the Idle Task;
@item Initialize all device drivers;
@item Creating and starting the user initialization task(s); and
@item Initiating multitasking.
@end itemize

The initialization directives MUST be called in the proper
sequence before any blocking directives may be used.  The services
in this manager should be invoked just once per application
and in precisely the following order:

@itemize @bullet
@item @code{@value{DIRPREFIX}initialize_data_structures}
@item @code{@value{DIRPREFIX}initialize_before_drivers}
@item @code{@value{DIRPREFIX}initialize_device_drivers}
@item @code{@value{DIRPREFIX}initialize_start_multitasking}
@end itemize

It is recommended that the Board Support Package use the
provided framework which will invoke these services as 
part of the executing the function @code{boot_card} in the
file @code{c/src/lib/libbsp/shared/bootcard.c}.  This
framework will also assist in allocating memory to the
RTEMS Workspace and C Program Heap and initializing the
C Library.

The effect of calling any blocking RTEMS directives before
@code{@value{DIRPREFIX}initialize_start_multitasking}
is unpredictable but guaranteed to be bad.  After the
directive @code{@value{DIRPREFIX}initialize_data_structures}
is invoked, it is permissible to allocate RTEMS objects and
perform non-blocking operations.  But the user should be
distinctly aware that multitasking is not available yet
and they are @b{NOT} executing in a task context.

Many of RTEMS actions during initialization are based upon
the contents of the Configuration Table.  For more information
regarding the format and contents of this table, please refer
to the chapter @ref{Configuring a System}.

The final step in the initialization sequence is the
initiation of multitasking.  When the scheduler and dispatcher
are enabled, the highest priority, ready task will be dispatched
to run.  Control will not be returned to the Board Support
Package after multitasking is enabled until the
@code{@value{DIRPREFIX}shutdown_executive} directive is called.
This directive is called as a side-effect of POSIX calls
including @code{exit}.

@subsection Shutting Down RTEMS

The @code{@value{DIRPREFIX}shutdown_executive} directive is invoked by the
application to end multitasking and return control to the board
support package.  The board support package resumes execution at
the code immediately following the invocation of the
@code{@value{DIRPREFIX}initialize_start_multitasking} directive.

@section Directives

This section details the Initialization Manager's
directives.  A subsection is dedicated to each of this manager's
directives and describes the calling sequence, related
constants, usage, and status codes.

@page
@subsection INITIALIZE_DATA_STRUCTURES - Initialize RTEMS Data Structures

@cindex initialize RTEMS data structures

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_initialize_data_structures
@example
void rtems_initialize_data_structures(void);
@end example
@end ifset

@ifset is-Ada
@example
NOT SUPPORTED FROM Ada BINDING
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive is called when the Board Support
Package has completed its basic initialization and
allows RTEMS to initialize the application environment based upon the
information in the Configuration Table, User Initialization
Tasks Table, Device Driver Table, User Extension Table,
Multiprocessor Configuration Table, and the Multiprocessor
Communications Interface (MPCI) Table.  This directive returns
to the caller after completing the basic RTEMS initialization.

@subheading NOTES:

The Initialization Manager directives must be used in the
proper sequence and invokved only once in the life of an application.

This directive must be invoked with interrupts disabled.
Interrupts should be disabled as early as possible in
the initialization sequence and remain disabled until
the first context switch.

@page
@subsection INITIALIZE_BEFORE_DRIVERS - Perform Initialization Before Device Drivers

@cindex initialize RTEMS before device drivers

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_initialize_before_drivers
@example
void rtems_initialize_before_drivers(void);
@end example
@end ifset

@ifset is-Ada
@example
NOT SUPPORTED FROM Ada BINDING
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive is called by the Board Support Package as the
second step in initializing RTEMS.  This directive performs
initialization that must occur between basis RTEMS data structure
initialization and device driver initialization.  In particular,
in a multiprocessor configuration, this directive will create the
MPCI Server Task.  This directive returns to the caller after
completing the basic RTEMS initialization.

@subheading NOTES:

The Initialization Manager directives must be used in the
proper sequence and invokved only once in the life of an application.

This directive must be invoked with interrupts disabled.
Interrupts should be disabled as early as possible in
the initialization sequence and remain disabled until
the first context switch.

@page
@subsection INITIALIZE_DEVICE_DRIVERS - Initialize Device Drivers

@cindex initialize device drivers

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_initialize_device_drivers
@example
void rtems_initialize_device_drivers(void);
@end example
@end ifset

@ifset is-Ada
@example
NOT SUPPORTED FROM Ada BINDING
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive is called by the Board Support Package as the
third step in initializing RTEMS.  This directive initializes
all statically configured device drivers and performs all RTEMS
initialization which requires device drivers to be initialized.

In a multiprocessor configuration, this service will initialize
the Multiprocessor Communications Interface (MPCI) and synchronize
with the other nodes in the system. 

After this directive is executed, control will be returned to
the Board Support Package framework.

@subheading NOTES:

The Initialization Manager directives must be used in the
proper sequence and invokved only once in the life of an application.

This directive must be invoked with interrupts disabled.
Interrupts should be disabled as early as possible in
the initialization sequence and remain disabled until
the first context switch.

@page
@subsection INITIALIZE_START_MULTITASKING - Complete Initialization and Start Multitasking

@cindex initialize RTEMS
@cindex start multitasking

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_initialize_start_multitasking
@example
uint32_t rtems_initialize_start_multitasking(void);
@end example
@end ifset

@ifset is-Ada
@example
NOT SUPPORTED FROM Ada BINDING
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

This directive returns the status code passed in to the 
@code{@value{DIRPREFIX}shutdown_executive}.

@subheading DESCRIPTION:

This directive is called after the other Initialization Manager
directives have successfully completed.  This directive
initiates multitasking and performs a context switch to
the first user application task and enables interrupts as
a side-effect of that context switch.

@subheading NOTES:

This directive @b{DOES NOT RETURN} to the caller until the
@code{@value{DIRPREFIX}shutdown_executive} is invoked.

This directive causes all nodes in the system to
verify that certain configuration parameters are the same as
those of the local node.  If an inconsistency is detected, then
a fatal error is generated.

@page
@subsection SHUTDOWN_EXECUTIVE - Shutdown RTEMS

@cindex shutdown RTEMS

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_shutdown_executive
@example
void rtems_shutdown_executive(
  uint32_t result
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Shutdown_Executive(
  Status : in     RTEMS.Unsigned32
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
