@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Interrupt Processing

@section Introduction

Different types of processors respond to the
occurrence of an interrupt in its own unique fashion. In
addition, each processor type provides a control mechanism to
allow for the proper handling of an interrupt.  The processor
dependent response to the interrupt modifies the current
execution state and results in a change in the execution stream.
Most processors require that an interrupt handler utilize some
special control mechanisms to return to the normal processing
stream.  Although RTEMS hides many of the processor dependent
details of interrupt processing, it is important to understand
how the RTEMS interrupt manager is mapped onto the processor's
unique architecture. Discussed in this chapter are the SPARC's
interrupt response and control mechanisms as they pertain to
RTEMS.

RTEMS and associated documentation uses the terms
interrupt and vector.  In the SPARC architecture, these terms
correspond to traps and trap type, respectively.  The terms will
be used interchangeably in this manual.

@section Synchronous Versus Asynchronous Traps

The SPARC architecture includes two classes of traps:
synchronous and asynchronous.  Asynchronous traps occur when an
external event interrupts the processor.  These traps are not
associated with any instruction executed by the processor and
logically occur between instructions.  The instruction currently
in the execute stage of the processor is allowed to complete
although subsequent instructions are annulled.  The return
address reported by the processor for asynchronous traps is the
pair of instructions following the current instruction.

Synchronous traps are caused by the actions of an
instruction.  The trap stimulus in this case either occurs
internally to the processor or is from an external signal that
was provoked by the instruction.  These traps are taken
immediately and the instruction that caused the trap is aborted
before any state changes occur in the processor itself.   The
return address reported by the processor for synchronous traps
is the instruction which caused the trap and the following
instruction.

@section Vectoring of Interrupt Handler

Upon receipt of an interrupt the SPARC automatically
performs the following actions:

@itemize @bullet
@item disables traps (sets the ET bit of the psr to 0),

@item the S bit of the psr is copied into the Previous
Supervisor Mode (PS) bit of the psr,

@item the cwp is decremented by one (modulo the number of
register windows) to activate a trap window,

@item the PC and nPC are loaded into local register 1 and 2
(l0 and l1),

@item the trap type (tt) field of the Trap Base Register (TBR)
is set to the appropriate value, and

@item if the trap is not a reset, then the PC is written with
the contents of the TBR and the nPC is written with TBR + 4.  If
the trap is a reset, then the PC is set to zero and the nPC is
set to 4.
@end itemize

Trap processing on the SPARC has two features which
are noticeably different than interrupt processing on other
architectures.  First, the value of psr register in effect
immediately before the trap occurred is not explicitly saved.
Instead only reversible alterations are made to it.  Second, the
Processor Interrupt Level (pil) is not set to correspond to that
of the interrupt being processed.  When a trap occurs, ALL
subsequent traps are disabled.  In order to safely invoke a
subroutine during trap handling, traps must be enabled to allow
for the possibility of register window overflow and underflow
traps.

If the interrupt handler was installed as an RTEMS
interrupt handler, then upon receipt of the interrupt, the
processor passes control to the RTEMS interrupt handler which
performs the following actions:

@itemize @bullet
@item saves the state of the interrupted task on it's stack,

@item insures that a register window is available for
subsequent traps,

@item if this is the outermost (i.e. non-nested) interrupt,
then the RTEMS interrupt handler switches from the current stack
to the interrupt stack,

@item enables traps,

@item invokes the vectors to a user interrupt service routine (ISR).
@end itemize

Asynchronous interrupts are ignored while traps are
disabled.  Synchronous traps which occur while traps are
disabled result in the CPU being forced into an error mode.

A nested interrupt is processed similarly with the
exception that the current stack need not be switched to the
interrupt stack.

@section Traps and Register Windows

One of the register windows must be reserved at all
times for trap processing.  This is critical to the proper
operation of the trap mechanism in the SPARC architecture.  It
is the responsibility of the trap handler to insure that there
is a register window available for a subsequent trap before
re-enabling traps.  It is likely that any high level language
routines invoked by the trap handler (such as a user-provided
RTEMS interrupt handler) will allocate a new register window.
The save operation could result in a window overflow trap.  This
trap cannot be correctly processed unless (1) traps are enabled
and (2) a register window is reserved for traps.  Thus, the
RTEMS interrupt handler insures that a register window is
available for subsequent traps before enabling traps and
invoking the user's interrupt handler.

@section Interrupt Levels

Sixteen levels (0-15) of interrupt priorities are
supported by the SPARC architecture with level fifteen (15)
being the highest priority.  Level zero (0) indicates that
interrupts are fully enabled.  Interrupt requests for interrupts
with priorities less than or equal to the current interrupt mask
level are ignored.

Although RTEMS supports 256 interrupt levels, the
SPARC only supports sixteen.  RTEMS interrupt levels 0 through
15 directly correspond to SPARC processor interrupt levels.  All
other RTEMS interrupt levels are undefined and their behavior is
unpredictable.

@section Disabling of Interrupts by RTEMS

During the execution of directive calls, critical
sections of code may be executed.  When these sections are
encountered, RTEMS disables interrupts to level seven (15)
before the execution of this section and restores them to the
previous level upon completion of the section.  RTEMS has been
optimized to insure that interrupts are disabled for less than
RTEMS_MAXIMUM_DISABLE_PERIOD microseconds on a RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ 
Mhz ERC32 with zero wait states.
These numbers will vary based the number of wait states and
processor speed present on the target board.
[NOTE:  The maximum period with interrupts disabled is hand calculated.  This
calculation was last performed for Release 
RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

[NOTE: It is thought that the length of time at which
the processor interrupt level is elevated to fifteen by RTEMS is
not anywhere near as long as the length of time ALL traps are
disabled as part of the "flush all register windows" operation.]

Non-maskable interrupts (NMI) cannot be disabled, and
ISRs which execute at this level MUST NEVER issue RTEMS system
calls.  If a directive is invoked, unpredictable results may
occur due to the inability of RTEMS to protect its critical
sections.  However, ISRs that make no system calls may safely
execute as non-maskable interrupts.

@section Interrupt Stack

The SPARC architecture does not provide for a
dedicated interrupt stack.  Thus by default, trap handlers would
execute on the stack of the RTEMS task which they interrupted.
This artificially inflates the stack requirements for each task
since EVERY task stack would have to include enough space to
account for the worst case interrupt stack requirements in
addition to it's own worst case usage.  RTEMS addresses this
problem on the SPARC by providing a dedicated interrupt stack
managed by software.

During system initialization, RTEMS allocates the
interrupt stack from the Workspace Area.  The amount of memory
allocated for the interrupt stack is determined by the
interrupt_stack_size field in the CPU Configuration Table.  As
part of processing a non-nested interrupt, RTEMS will switch to
the interrupt stack before invoking the installed handler.

