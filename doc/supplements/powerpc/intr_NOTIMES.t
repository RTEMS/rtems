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
unique architecture. Discussed in this chapter are the PowerPC's
interrupt response and control mechanisms as they pertain to
RTEMS.

RTEMS and associated documentation uses the terms
interrupt and vector.  In the PowerPC architecture, these terms
correspond to exception and exception handler, respectively.  The terms will
be used interchangeably in this manual.

@section Synchronous Versus Asynchronous Exceptions

In the PowerPC architecture exceptions can be either precise or 
imprecise and either synchronous or asynchronous.  Asynchronous 
exceptions occur when an external event interrupts the processor.
Synchronous exceptions are caused by the actions of an 
instruction. During an exception SRR0 is used to calculate where 
instruction processing should resume.  All instructions prior to
the resume instruction will have completed execution.  SRR1 is used to 
store the machine status.

There are two asynchronous nonmaskable, highest-priority exceptions
system reset and machine check.  There are two asynchrononous maskable
low-priority exceptions external interrupt and decrementer.  Nonmaskable
execptions are never delayed, therefore if two nonmaskable, asynchronous 
exceptions occur in immediate succession, the state information saved by
the first exception may be overwritten when the subsequent exception occurs. 

The PowerPC arcitecure defines one imprecise exception, the imprecise 
floating point enabled exception.  All other synchronous exceptions are
precise.  The synchronization occuring during asynchronous precise 
exceptions conforms to the requirements for context synchronization.

@section Vectoring of Interrupt Handler

Upon determining that an exception can be taken the PowerPC automatically
performs the following actions:

@itemize @bullet
@item an instruction address is loaded into SRR0

@item bits 33-36 and 42-47 of SRR1 are loaded with information 
specific to the exception.

@item bits 0-32, 37-41, and 48-63 of SRR1 are loaded with corresponding
bits from the MSR.

@item the MSR is set based upon the exception type.

@item instruction fetch and execution resumes, using the new MSR value, at a location specific to the execption type. 

@end itemize

If the interrupt handler was installed as an RTEMS
interrupt handler, then upon receipt of the interrupt, the
processor passes control to the RTEMS interrupt handler which
performs the following actions:

@itemize @bullet
@item saves the state of the interrupted task on it's stack,

@item saves all registers which are not normally preserved
by the calling sequence so the user's interrupt service
routine can be written in a high-level language.

@item if this is the outermost (i.e. non-nested) interrupt,
then the RTEMS interrupt handler switches from the current stack
to the interrupt stack,

@item enables exceptions,

@item invokes the vectors to a user interrupt service routine (ISR).
@end itemize

Asynchronous interrupts are ignored while exceptions are
disabled.  Synchronous interrupts which occur while are
disabled result in the CPU being forced into an error mode.

A nested interrupt is processed similarly with the
exception that the current stack need not be switched to the
interrupt stack.

@section Interrupt Levels

The PowerPC architecture supports only a single external
asynchronous interrupt source.  This interrupt source
may be enabled and disabled via the External Interrupt Enable (EE)
bit in the Machine State Register (MSR).  Thus only two level (enabled
and disabled) of external device interrupt priorities are 
directly supported by the PowerPC architecture.  

Some PowerPC implementations include a Critical Interrupt capability
which is often used to receive interrupts from high priority external
devices.

The RTEMS interrupt level mapping scheme for the PowerPC is not 
a numeric level as on most RTEMS ports.  It is a bit mapping in
which the least three significiant bits of the interrupt level
are mapped directly to the enabling of specific interrupt 
sources as follows:

@table @b

@item Critical Interrupt
Setting bit 0 (the least significant bit) of the interrupt level
enables the Critical Interrupt source, if it is available on this
CPU model.

@item Machine Check
Setting bit 1 of the interrupt level enables Machine Check execptions.

@item External Interrupt
Setting bit 2 of the interrupt level enables External Interrupt execptions.

@end table

All other bits in the RTEMS task interrupt level are ignored.

@section Disabling of Interrupts by RTEMS

During the execution of directive calls, critical
sections of code may be executed.  When these sections are
encountered, RTEMS disables Critical Interrupts, External Interrupts
and Machine Checks before the execution of this section and restores
them to the previous level upon completion of the section.  RTEMS has been
optimized to insure that interrupts are disabled for less than
RTEMS_MAXIMUM_DISABLE_PERIOD microseconds on a 
RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ Mhz PowerPC 603e with zero
wait states.  These numbers will vary based the number of wait 
states and processor speed present on the target board.
[NOTE:  The maximum period with interrupts disabled is hand calculated.  This
calculation was last performed for Release 
RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

If a PowerPC implementation provides non-maskable interrupts (NMI)
which cannot be disabled, ISRs which process these interrupts
MUST NEVER issue RTEMS system calls.  If a directive is invoked,
unpredictable results may occur due to the inability of RTEMS
to protect its critical sections.  However, ISRs that make no
system calls may safely execute as non-maskable interrupts.

@section Interrupt Stack

The PowerPC architecture does not provide for a
dedicated interrupt stack.  Thus by default, exception handlers would
execute on the stack of the RTEMS task which they interrupted.
This artificially inflates the stack requirements for each task
since EVERY task stack would have to include enough space to
account for the worst case interrupt stack requirements in
addition to it's own worst case usage.  RTEMS addresses this
problem on the PowerPC by providing a dedicated interrupt stack
managed by software.

During system initialization, RTEMS allocates the
interrupt stack from the Workspace Area.  The amount of memory
allocated for the interrupt stack is determined by the
interrupt_stack_size field in the CPU Configuration Table.  As
part of processing a non-nested interrupt, RTEMS will switch to
the interrupt stack before invoking the installed handler.



