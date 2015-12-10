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
@item @code{@value{DIRPREFIX}initialize_executive} - Initialize RTEMS
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
initialization and the @i{initialization context} is not
a task in any sense and the user should be very careful
during initialization.

The BSP must ensure that the there is enough stack
space reserved for the initialization context to
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

The Initialization Manager @code{@value{DIRPREFIX}initialize_executive}
directives is called by the @code{boot_card} routine.  The @code{boot_card}
routine is invoked by the Board Support Package once a basic C run-time
environment is set up.  This consists of

@itemize @bullet
@item a valid and accessible text section, read-only data, read-write data and
zero-initialized data,
@item an initialization stack large enough to initialize the rest of the Board
Support Package, RTEMS and the device drivers,
@item all registers and components mandated by Application Binary Interface, and
@item disabled interrupts.
@end itemize

The @code{@value{DIRPREFIX}initialize_executive} directive uses a system
initialization linker set to initialize only those parts of the overall RTEMS
feature set that is necessary for a particular application.  @xref{Linker
Sets}.  Each RTEMS feature used the application may optionally register an
initialization handler.  The system initialization API is available via
@code{#included <rtems/sysinit.h>}.

A list of all initialization steps follows.  Some steps are optional depending
on the requested feature set of the application.  The initialization steps are
execute in the order presented here.

@table @dfn

@item RTEMS_SYSINIT_BSP_WORK_AREAS
The work areas consisting of C Program Heap and the RTEMS Workspace are
initialized by the Board Support Package.  This step is mandatory.

@item RTEMS_SYSINIT_BSP_START
Basic initialization step provided by the Board Support Package.  This step is
mandatory.

@item RTEMS_SYSINIT_INITIAL_EXTENSIONS
Initializes the initial extensions if at least one is configured by the Board
Support Package or application.

@item RTEMS_SYSINIT_DATA_STRUCTURES
This directive is called when the Board Support Package has completed its basic
initialization and allows RTEMS to initialize the application environment based
upon the information in the Configuration Table, User Initialization Tasks
Table, Device Driver Table, Multiprocessor Configuration Table, and the
Multiprocessor Communications Interface (MPCI) Table.

@item RTEMS_SYSINIT_USER_EXTENSIONS
Initializes the @ref{User Extensions Manager} in case it is used by the
application.

@item RTEMS_SYSINIT_CPU_SET
Initializes the global CPU set on SMP configurations in case the application
uses this information.

@item RTEMS_SYSINIT_BSP_LIBC
Depending on the application configuration the IO library and root filesystem
is initialized.  This step is mandatory.

@item RTEMS_SYSINIT_BEFORE_DRIVERS
This directive performs initialization that must occur between basis RTEMS data
structure initialization and device driver initialization.  In particular, in a
multiprocessor configuration, this directive will create the MPCI Server Task.

@item RTEMS_SYSINIT_BSP_PRE_DRIVERS
Initialization step performed right before device drivers are initialized
provided by the Board Support Package.  This step is mandatory.

@item RTEMS_SYSINIT_DEVICE_DRIVERS
This step initializes all statically configured device drivers and performs all
RTEMS initialization which requires device drivers to be initialized.  This
step is mandatory.

In a multiprocessor configuration, this service will initialize the
Multiprocessor Communications Interface (MPCI) and synchronize with the other
nodes in the system. 

@item RTEMS_SYSINIT_BSP_POST_DRIVERS
Initialization step performed right after device drivers are initialized
provided by the Board Support Package.  This step is mandatory.

@end table

The final action of the @code{@value{DIRPREFIX}initialize_executive} directive
is to start multitasking.  RTEMS does not return to the initialization context
and the initialization stack may be re-used for interrupt processing.

Many of RTEMS actions during initialization are based upon
the contents of the Configuration Table.  For more information
regarding the format and contents of this table, please refer
to the chapter @ref{Configuring a System}.

The final action in the initialization sequence is the
initiation of multitasking.  When the scheduler and dispatcher
are enabled, the highest priority, ready task will be dispatched
to run.  Control will not be returned to the Board Support
Package after multitasking is enabled.  The initialization stack may be re-used
for interrupt processing.

@subsection Shutting Down RTEMS

The @code{@value{DIRPREFIX}shutdown_executive} directive is invoked by the
application to end multitasking and terminate the system.

@section Directives

This section details the Initialization Manager's
directives.  A subsection is dedicated to each of this manager's
directives and describes the calling sequence, related
constants, usage, and status codes.

@page
@subsection INITIALIZE_EXECUTIVE - Initialize RTEMS

@cindex initialize RTEMS
@cindex start multitasking

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_initialize_executive
@example
void rtems_initialize_executive(void);
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

Iterates through the system initialization linker set and invokes the
registered handlers.  The final step is to start multitasking.

@subheading NOTES:

This directive should be called by @code{boot_card} only.

This directive @b{does not return} to the caller.  Errors in the initialization
sequence are usually fatal and lead to a system termination.

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

This directive is called when the application wishes to shutdown RTEMS.  The
system is terminated with a fatal source of @code{RTEMS_FATAL_SOURCE_EXIT} and
the specified @code{result} code.

@subheading NOTES:

This directive @b{must} be the last RTEMS directive
invoked by an application and it @b{does not return} to the caller.

This directive may be called any time.
