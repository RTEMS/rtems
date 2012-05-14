@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Board Support Packages

@cindex Board Support Packages
@cindex BSPs

@section Introduction

@cindex BSP, definition

A board support package (BSP) is a collection of
user-provided facilities which interface RTEMS and an
application with a specific hardware platform.  These facilities
may  include hardware initialization, device drivers, user
extensions, and a Multiprocessor Communications Interface
(MPCI).  However, a minimal BSP need only support processor
reset and initialization and, if needed, a clock tick.

@section Reset and Initialization

An RTEMS based application is initiated or
re-initiated when the processor is reset.  This initialization
code is responsible for preparing the target platform for the
RTEMS application.  Although the exact actions performed by the
initialization code are highly processor and target dependent,
the logical functionality of these actions are similar across a
variety of processors and target platforms.

Normally, the BSP and some of the application initialization is
intertwined in the RTEMS initialization sequence controlled by
the shared function @code{boot_card()}.

The reset application initialization code is executed
first when the processor is reset.  All of the hardware must be
initialized to a quiescent state by this software before
initializing RTEMS.  When in quiescent state, devices do not
generate any interrupts or require any servicing by the
application.  Some of the hardware components may be initialized
in this code as well as any application initialization that does
not involve calls to RTEMS directives.

The processor's Interrupt Vector Table which will be used by the
application may need to be set to the required value by the reset
application initialization code.  Because interrupts are enabled
automatically by RTEMS as part of the context switch to the first task,
the Interrupt Vector Table MUST be set before this directive is invoked
to ensure correct interrupt vectoring.  The processor's Interrupt Vector
Table must be accessible by RTEMS as it will be modified by the when
installing user Interrupt Service Routines (ISRs) On some CPUs, RTEMS
installs it's own Interrupt Vector Table as part of initialization and
thus these requirements are met automatically.  The reset code which is
executed before the call to any RTEMS initialization routines has the
following requirements:

@itemize @bullet
@item Must not make any blocking RTEMS directive calls.

@item If the processor supports multiple privilege levels, must leave
the processor in the most privileged, or supervisory, state.

@item Must allocate a stack of sufficient size to execute the initialization
and shutdown of the system.  This stack area will NOT be used by any task
once the system is initialized.  This stack is often reserved via the
linker script or in the assembly language start up file.

@item Must initialize the stack pointer for the initialization process to
that allocated.

@item Must initialize the processor's Interrupt Vector Table.

@item Must disable all maskable interrupts.

@item If the processor supports a separate interrupt stack, must allocate
the interrupt stack and initialize the interrupt stack pointer.

@end itemize

At the end of the initialization sequence, RTEMS does not return to the
BSP initialization code, but instead context switches to the highest
priority task to begin application execution.  This task is typically
a User Initialization Task which is responsible for performing both
local and global application initialization which is dependent on RTEMS
facilities.  It is also responsible for initializing any higher level
RTEMS services the application uses such as networking and blocking
device drivers.

@subsection Interrupt Stack Requirements

The worst-case stack usage by interrupt service
routines must be taken into account when designing an
application.  If the processor supports interrupt nesting, the
stack usage must include the deepest nest level.  The worst-case
stack usage must account for the following requirements:

@itemize @bullet
@item Processor's interrupt stack frame

@item Processor's subroutine call stack frame

@item RTEMS system calls

@item Registers saved on stack

@item Application subroutine calls
@end itemize

The size of the interrupt stack must be greater than or equal to the
confugured minimum stack size.

@subsection Processors with a Separate Interrupt Stack

Some processors support a separate stack for interrupts.  When an
interrupt is vectored and the interrupt is not nested, the processor
will automatically switch from the current stack to the interrupt stack.
The size of this stack is based solely on the worst-case stack usage by
interrupt service routines.

The dedicated interrupt stack for the entire application on some
architectures is supplied and initialized by the reset and initialization
code of the user's Board Support Package.  Whether allocated and
initialized by the BSP or RTEMS, since all ISRs use this stack, the
stack size must take into account the worst case stack usage by any
combination of nested ISRs.

@subsection Processors Without a Separate Interrupt Stack

Some processors do not support a separate stack for interrupts.  In this
case, without special assistance every task's stack must include
enough space to handle the task's worst-case stack usage as well as
the worst-case interrupt stack usage.  This is necessary because the
worst-case interrupt nesting could occur while any task is executing.

On many processors without dedicated hardware managed interrupt stacks,
RTEMS manages a dedicated interrupt stack in software.  If this capability
is supported on a CPU, then it is logically equivalent to the processor
supporting a separate interrupt stack in hardware.

@section Device Drivers

Device drivers consist of control software for
special peripheral devices and provide a logical interface for
the application developer.  The RTEMS I/O manager provides
directives which allow applications to access these device
drivers in a consistent fashion.  A Board Support Package may
include device drivers to access the hardware on the target
platform.  These devices typically include serial and parallel
ports, counter/timer peripherals, real-time clocks, disk
interfaces, and network controllers.

For more information on device drivers, refer to the
I/O Manager chapter.

@subsection Clock Tick Device Driver

Most RTEMS applications will include a clock tick
device driver which invokes the @code{@value{DIRPREFIX}clock_tick}
directive at regular intervals.  The clock tick is necessary if
the application is to utilize timeslicing, the clock manager, the
timer manager, the rate monotonic manager, or the timeout option on blocking
directives.

The clock tick is usually provided as an interrupt from a counter/timer
or a real-time clock device.  When a counter/timer is used to provide the
clock tick, the device is typically programmed to operate in continuous
mode.  This mode selection causes the device to automatically reload the
initial count and continue the countdown without programmer intervention.
This reduces the overhead required to manipulate the counter/timer in
the clock tick ISR and increases the accuracy of tick occurrences.
The initial count can be based on the microseconds_per_tick field
in the RTEMS Configuration Table.  An alternate approach is to set
the initial count for a fixed time period (such as one millisecond)
and have the ISR invoke @code{@value{DIRPREFIX}clock_tick} on the
configured @code{microseconds_per_tick} boundaries.  Obviously, this
can induce some error if the configured @code{microseconds_per_tick}
is not evenly divisible by the chosen clock interrupt quantum.

It is important to note that the interval between
clock ticks directly impacts the granularity of RTEMS timing
operations.  In addition, the frequency of clock ticks is an
important factor in the overall level of system overhead.  A
high clock tick frequency results in less processor time being
available for task execution due to the increased number of
clock tick ISRs.

@section User Extensions

RTEMS allows the application developer to augment
selected features by invoking user-supplied extension routines
when the following system events occur:

@itemize @bullet
@item Task creation
@item Task initiation
@item Task reinitiation
@item Task deletion
@item Task context switch
@item Post task context switch
@item Task begin
@item Task exits
@item Fatal error detection
@end itemize

User extensions can be used to implement a wide variety of
functions including execution profiling, non-standard
coprocessor support, debug support, and error detection and
recovery.  For example, the context of a non-standard numeric
coprocessor may be maintained via the user extensions.  In this
example, the task creation and deletion extensions are
responsible for allocating and deallocating the context area,
the task initiation and reinitiation extensions would be
responsible for priming the context area, and the task context
switch extension would save and restore the context of the
device.

For more information on user extensions, refer to the
@ref{User Extensions Manager} chapter.

@section Multiprocessor Communications Interface (MPCI)

RTEMS requires that an MPCI layer be provided when a
multiple node application is developed.  This MPCI layer must
provide an efficient and reliable communications mechanism
between the multiple nodes.  Tasks on different nodes
communicate and synchronize with one another via the MPCI.  Each
MPCI layer must be tailored to support the architecture of the
target platform.

For more information on the MPCI, refer to the
Multiprocessing Manager chapter.

@subsection Tightly-Coupled Systems

A tightly-coupled system is a multiprocessor
configuration in which the processors communicate solely via
shared global memory.  The MPCI can simply place the RTEMS
packets in the shared memory space.  The two primary
considerations when designing an MPCI for a tightly-coupled
system are data consistency and informing another node of a
packet.

The data consistency problem may be solved using
atomic "test and set" operations to provide a "lock" in the
shared memory.  It is important to minimize the length of time
any particular processor locks a shared data structure.

The problem of informing another node of a packet can
be addressed using one of two techniques.  The first technique
is to use an interprocessor interrupt capability to cause an
interrupt on the receiving node.  This technique requires that
special support hardware be provided by either the processor
itself or the target platform.  The second technique is to have
a node poll for arrival of packets.  The drawback to this
technique is the overhead associated with polling.

@subsection Loosely-Coupled Systems

A loosely-coupled system is a multiprocessor
configuration in which the processors communicate via some type
of communications link which is not shared global memory.  The
MPCI sends the RTEMS packets across the communications link to
the destination node.  The characteristics of the communications
link vary widely and have a significant impact on the MPCI
layer.  For example, the bandwidth of the communications link
has an obvious impact on the maximum MPCI throughput.

The characteristics of a shared network, such as
Ethernet, lend themselves to supporting an MPCI layer.  These
networks provide both the point-to-point and broadcast
capabilities which are expected by RTEMS.

@subsection Systems with Mixed Coupling

A mixed-coupling system is a multiprocessor
configuration in which the processors communicate via both
shared memory and communications links.  A unique characteristic
of mixed-coupling systems is that a node may not have access to
all communication methods.  There may be multiple shared memory
areas and communication links.  Therefore, one of the primary
functions of the MPCI layer is to efficiently route RTEMS
packets between nodes.  This routing may be based on numerous
algorithms. In addition, the router may provide alternate
communications paths in the event of an overload or a partial
failure.

@subsection Heterogeneous Systems

Designing an MPCI layer for a heterogeneous system
requires special considerations by the developer.  RTEMS is
designed to eliminate many of the problems associated with
sharing data in a heterogeneous environment.  The MPCI layer
need only address the representation of thirty-two (32) bit
unsigned quantities.

For more information on supporting a heterogeneous
system, refer the Supporting Heterogeneous Environments in the
Multiprocessing Manager chapter.
