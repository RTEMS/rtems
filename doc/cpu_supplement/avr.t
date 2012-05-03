@c
@c  COPYRIGHT (c) 1988-2009.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@ifinfo
@end ifinfo
@chapter Atmel AVR Specific Information

This chapter discusses the AVR architecture dependencies in this
port of RTEMS.

@subheading Architecture Documents

For information on the AVR architecture, refer to the following
documents available from Atmel.

TBD

@itemize @bullet

@item See other CPUs for documentation reference formatting examples.

@end itemize


@section CPU Model Dependent Features

CPUs of the AVR 53X only differ in the peripherals and thus in the
device drivers. This port does not yet support the 56X dual core variants.

@subsection Count Leading Zeroes Instruction

The AVR CPU has the XXX instruction which could be used to speed
up the find first bit operation.  The use of this instruction should
significantly speed up the scheduling associated with a thread blocking.

@section Calling Conventions

@subsection Processor Background

The AVR architecture supports a simple call and return mechanism.
A subroutine is invoked via the call (@code{call}) instruction.
This instruction saves the return address in the @code{RETS} register
and transfers the execution to the given address.

It is the called funcions responsability to use the link instruction
to reserve space on the stack for the local variables.  Returning from
a subroutine is done by using the RTS (@code{RTS}) instruction which
loads the PC with the adress stored in RETS.

It is is important to note that the @code{call} instruction does not
automatically save or restore any registers.  It is the responsibility
of the high-level language compiler to define the register preservation
and usage convention.

@subsection Register Usage

A called function may clobber all registers, except RETS, R4-R7, P3-P5,
FP and SP.  It may also modify the first 12 bytes in the caller’s stack
frame which is used as an argument area for the first three arguments
(which are passed in R0...R3 but may be placed on the stack by the
called function).

@subsection Parameter Passing

RTEMS assumes that the AVR GCC calling convention is followed.
The first three parameters are stored in registers R0, R1, and R2.
All other parameters are put pushed on the stack.  The result is returned
through register R0.

@section Memory Model

The AVR family architecutre support a single unified 4 GB byte
address space using 32-bit addresses. It maps all resources like internal
and external memory and IO registers into separate sections of this
common address space.

The AVR architcture supports some form of memory
protection via its Memory Management Unit. Since the
AVR port runs in supervisior mode this memory
protection mechanisms are not used.

@section Interrupt Processing

Discussed in this chapter are the AVR's interrupt response and
control mechanisms as they pertain to RTEMS.

@subsection Vectoring of an Interrupt Handler

TBD

@subsection Disabling of Interrupts by RTEMS

During interrupt disable critical sections, RTEMS disables interrupts to
level N (N) before the execution of this section and restores them
to the previous level upon completion of the section. RTEMS uses the
instructions CLI and STI to enable and disable Interrupts. Emulation,
Reset, NMI and Exception Interrupts are never disabled.

@subsection Interrupt Stack

The AVR Architecture works with two different kind of stacks,
User and Supervisor Stack. Since RTEMS and its Application run
in supervisor mode, all interrupts will use the interrupted
tasks stack for execution.

@section Default Fatal Error Processing

The default fatal error handler for the AVR performs the following
actions:

@itemize @bullet
@item disables processor interrupts,
@item places the error code in @b{r0}, and
@item executes an infinite loop (@code{while(0);} to
simulate a halt processor instruction.
@end itemize

@section Board Support Packages


@subsection System Reset

TBD
