@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Interrupt Manager

@section Introduction

Any real-time executive must provide a mechanism for
quick response to externally generated interrupts to satisfy the
critical time constraints of the application.  The interrupt
manager provides this mechanism for RTEMS.  This manager permits
quick interrupt response times by providing the critical ability
to alter task execution which allows a task to be preempted upon
exit from an ISR.  The interrupt manager includes the following
directive:

@itemize @bullet
@item @code{@value{DIRPREFIX}interrupt_catch} - Establish an ISR
@item @code{@value{DIRPREFIX}interrupt_disable} - Disable Interrupts
@item @code{@value{DIRPREFIX}interrupt_enable} - Enable Interrupts
@item @code{@value{DIRPREFIX}interrupt_flash} - Flash Interrupt
@item @code{@value{DIRPREFIX}interrupt_is_in_progress} - Is an ISR in Progress
@end itemize

@section Background

@subsection Processing an Interrupt

@cindex interrupt processing

The interrupt manager allows the application to
connect a function to a hardware interrupt vector.  When an
interrupt occurs, the processor will automatically vector to
RTEMS.  RTEMS saves and restores all registers which are not
preserved by the normal @value{LANGUAGE} calling convention
for the target
processor and invokes the user's ISR.  The user's ISR is
responsible for processing the interrupt, clearing the interrupt
if necessary, and device specific manipulation.

@findex rtems_vector_number

The @code{@value{DIRPREFIX}interrupt_catch}
directive connects a procedure to
an interrupt vector.  The vector number is managed using
the @code{@value{DIRPREFIX}vector_number} data type.

The interrupt service routine is assumed
to abide by these conventions and have a prototype similar to
the following:

@ifset is-C
@findex rtems_isr

@example
rtems_isr user_isr(
  rtems_vector_number vector
);
@end example
@end ifset

@ifset is-Ada
@example
NOT SUPPORTED FROM Ada BINDING
@end example
@end ifset

The vector number argument is provided by RTEMS to
allow the application to identify the interrupt source.  This
could be used to allow a single routine to service interrupts
from multiple instances of the same device.  For example, a
single routine could service interrupts from multiple serial
ports and use the vector number to identify which port requires
servicing.

To minimize the masking of lower or equal priority
level interrupts, the ISR should perform the minimum actions
required to service the interrupt.  Other non-essential actions
should be handled by application tasks.  Once the user's ISR has
completed, it returns control to the RTEMS interrupt manager
which will perform task dispatching and restore the registers
saved before the ISR was invoked.

The RTEMS interrupt manager guarantees that proper
task scheduling and dispatching are performed at the conclusion
of an ISR.  A system call made by the ISR may have readied a
task of higher priority than the interrupted task.  Therefore,
when the ISR completes, the postponed dispatch processing must
be performed.  No dispatch processing is performed as part of
directives which have been invoked by an ISR.

Applications must adhere to the following rule if
proper task scheduling and dispatching is to be performed:

@itemize @b{ }

@item @b{The interrupt manager must be used for all ISRs which
may be interrupted by the highest priority ISR which invokes an
RTEMS directive.}

@end itemize

Consider a processor which allows a numerically low
interrupt level to interrupt a numerically greater interrupt
level.  In this example, if an RTEMS directive is used in a
level 4 ISR, then all ISRs which execute at levels 0 through 4
must use the interrupt manager.

Interrupts are nested whenever an interrupt occurs
during the execution of another ISR.  RTEMS supports efficient
interrupt nesting by allowing the nested ISRs to terminate
without performing any dispatch processing.  Only when the
outermost ISR terminates will the postponed dispatching occur.

@subsection RTEMS Interrupt Levels

@cindex interrupt levels

Many processors support multiple interrupt levels or
priorities.  The exact number of interrupt levels is processor
dependent.  RTEMS internally supports 256 interrupt levels which
are mapped to the processor's interrupt levels.  For specific
information on the mapping between RTEMS and the target
processor's interrupt levels, refer to the Interrupt Processing
chapter of the Applications Supplement document for a specific
target processor.

@subsection Disabling of Interrupts by RTEMS

@cindex disabling interrupts

During the execution of directive calls, critical
sections of code may be executed.  When these sections are
encountered, RTEMS disables all maskable interrupts before the
execution of the section and restores them to the previous level
upon completion of the section.  RTEMS has been optimized to
ensure that interrupts are disabled for a minimum length of
time.  The maximum length of time interrupts are disabled by
RTEMS is processor dependent and is detailed in the Timing
Specification chapter of the Applications Supplement document
for a specific target processor.

Non-maskable interrupts (NMI) cannot be disabled, and
ISRs which execute at this level MUST NEVER issue RTEMS system
calls.  If a directive is invoked, unpredictable results may
occur due to the inability of RTEMS to protect its critical
sections.  However, ISRs that make no system calls may safely
execute as non-maskable interrupts.

@section Operations

@subsection Establishing an ISR

The @code{@value{DIRPREFIX}interrupt_catch}
directive establishes an ISR for
the system.  The address of the ISR and its associated CPU
vector number are specified to this directive.  This directive
installs the RTEMS interrupt wrapper in the processor's
Interrupt Vector Table and the address of the user's ISR in the
RTEMS' Vector Table.  This directive returns the previous
contents of the specified vector in the RTEMS' Vector Table.

@subsection Directives Allowed from an ISR

Using the interrupt manager ensures that RTEMS knows
when a directive is being called from an ISR.  The ISR may then
use system calls to synchronize itself with an application task.
The synchronization may involve messages, events or signals
being passed by the ISR to the desired task.  Directives invoked
by an ISR must operate only on objects which reside on the local
node.  The following is a list of RTEMS system calls that may be
made from an ISR:

@itemize @bullet
@item Task Management

Although it is acceptable to operate on the RTEMS_SELF task (e.g.
the currently executing task), while in an ISR, this will refer
to the interrupted task.  Most of the time, it is an application
implementation error to use RTEMS_SELF from an ISR.

@itemize -
@item rtems_task_get_note
@item rtems_task_set_note
@item rtems_task_suspend
@item rtems_task_resume
@end itemize

@item Interrupt Management

@itemize -
@item rtems_interrupt_enable
@item rtems_interrupt_disable
@item rtems_interrupt_flash
@item rtems_interrupt_is_in_progress
@item rtems_interrupt_catch
@end itemize

@item Clock Management

@itemize
@item rtems_clock_set
@item rtems_clock_get
@item rtems_clock_get_tod
@item rtems_clock_get_tod_timeval
@item rtems_clock_get_seconds_since_epoch
@item rtems_clock_get_ticks_per_second
@item rtems_clock_get_ticks_since_boot
@item rtems_clock_get_uptime
@item rtems_clock_set_nanoseconds_extension
@item rtems_clock_tick
@end itemize

@item Message, Event, and Signal Management

@itemize -
@item rtems_message_queue_send
@item rtems_message_queue_urgent
@item rtems_event_send
@item rtems_signal_send
@end itemize

@item Semaphore Management

@itemize -
@item rtems_semaphore_release
@end itemize

@item Dual-Ported Memory Management

@itemize -
@item rtems_port_external_to_internal
@item rtems_port_internal_to_external
@end itemize

@item IO Management

The following services are safe to call from an ISR if and only if
the device driver service invoked is also safe.  The IO Manager itself
is safe but the invoked driver entry point may or may not be.
@itemize -
@item rtems_io_initialize
@item rtems_io_open
@item rtems_io_close
@item rtems_io_read
@item rtems_io_write
@item rtems_io_control
@end itemize

@item Fatal Error Management

@itemize -
@item rtems_fatal_error_occurred
@end itemize

@item Multiprocessing

@itemize -
@item rtems_multiprocessing_announce
@end itemize
@end itemize

@section Directives

This section details the interrupt manager's
directives.  A subsection is dedicated to each of this manager's
directives and describes the calling sequence, related
constants, usage, and status codes.

@c
@c
@c
@page
@subsection INTERRUPT_CATCH - Establish an ISR

@cindex establish an ISR
@cindex install an ISR

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_interrupt_catch
@example
rtems_status_code rtems_interrupt_catch(
  rtems_isr_entry      new_isr_handler,
  rtems_vector_number  vector,
  rtems_isr_entry     *old_isr_handler
);
@end example
@end ifset

@ifset is-Ada
@example
NOT SUPPORTED FROM Ada BINDING
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - ISR established successfully@*
@code{@value{RPREFIX}INVALID_NUMBER} - illegal vector number@*
@code{@value{RPREFIX}INVALID_ADDRESS} - illegal ISR entry point or invalid @code{old_isr_handler}

@subheading DESCRIPTION:

This directive establishes an interrupt service
routine (ISR) for the specified interrupt vector number.  The
@code{new_isr_handler} parameter specifies the entry point of the ISR.
The entry point of the previous ISR for the specified vector is
returned in @code{old_isr_handler}.

To release an interrupt vector, pass the old handler's address obtained
when the vector was first capture.

@subheading NOTES:

This directive will not cause the calling task to be preempted.

@c
@c
@c
@page
@subsection INTERRUPT_DISABLE - Disable Interrupts

@cindex disable interrupts

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_interrupt_disable
@example
void rtems_interrupt_disable(
  rtems_interrupt_level  level
);

/* this is implemented as a macro and sets level as a side-effect */
@end example
@end ifset

@ifset is-Ada
@example
function Interrupt_Disable return RTEMS.ISR_Level;
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive disables all maskable interrupts and returns
the previous @code{level}.  A later invocation of the
@code{@value{DIRPREFIX}interrupt_enable} directive should be used to
restore the interrupt level.

@subheading NOTES:

This directive will not cause the calling task to be preempted.

@ifset is-C
@b{This directive is implemented as a macro which modifies the @code{level}
parameter.}
@end ifset

@c
@c
@c
@page
@subsection INTERRUPT_ENABLE - Enable Interrupts

@cindex enable interrupts

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_interrupt_enable
@example
void rtems_interrupt_enable(
  rtems_interrupt_level  level
);
@end example
@end ifset

@ifset is-Ada
@example 
procedure Interrupt_Enable (
   Level : in     RTEMS.ISR_Level
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive enables maskable interrupts to the @code{level}
which was returned by a previous call to
@code{@value{DIRPREFIX}interrupt_disable}.
Immediately prior to invoking this directive, maskable interrupts should
be disabled by a call to @code{@value{DIRPREFIX}interrupt_disable}
and will be enabled when this directive returns to the caller.

@subheading NOTES:

This directive will not cause the calling task to be preempted.


@c
@c
@c
@page
@subsection INTERRUPT_FLASH - Flash Interrupts

@cindex flash interrupts

@subheading CALLING SEQUENCE:

@ifset is-C 
@findex rtems_interrupt_flash
@example
void rtems_interrupt_flash(
  rtems_interrupt_level level
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Interrupt_Flash (
   Level : in     RTEMS.ISR_Level
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive temporarily enables maskable interrupts to the @code{level}
which was returned by a previous call to
@code{@value{DIRPREFIX}interrupt_disable}.  
Immediately prior to invoking this directive, maskable interrupts should
be disabled by a call to @code{@value{DIRPREFIX}interrupt_disable}
and will be redisabled when this directive returns to the caller.

@subheading NOTES:

This directive will not cause the calling task to be preempted.

@c
@c
@c
@page
@subsection INTERRUPT_IS_IN_PROGRESS - Is an ISR in Progress

@cindex is interrupt in progress

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_interrupt_is_in_progress
@example
bool rtems_interrupt_is_in_progress( void );
@end example
@end ifset

@ifset is-Ada
@example
function Interrupt_Is_In_Progress return RTEMS.Boolean;
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:

NONE

@subheading DESCRIPTION:

This directive returns @code{TRUE} if the processor is currently
servicing an interrupt and @code{FALSE} otherwise.  A return value
of @code{TRUE} indicates that the caller is an interrupt service
routine, @b{NOT} a task.  The directives available to an interrupt
service routine are restricted.

@subheading NOTES:

This directive will not cause the calling task to be preempted.

