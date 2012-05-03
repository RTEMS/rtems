@c
@c  COPYRIGHT (c) 1988-2006.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@ifinfo
@end ifinfo
@chapter Blackfin Specific Information

This chapter discusses the Blackfin architecture dependencies in this
port of RTEMS.

@subheading Architecture Documents

For information on the Blackfin architecture, refer to the following
documents available from Analog Devices.

TBD

@itemize @bullet
@item @cite{"ADSP-BF533 Blackfin Processor Hardware Reference."}
@file{http://www.analog.com/UploadedFiles/Associated_Docs/892485982bf533_hwr.pdf} 

@end itemize


@section CPU Model Dependent Features

CPUs of the Blackfin 53X only differ in the peripherals and thus in the
device drivers. This port does not yet support the 56X dual core variants.

@subsection Count Leading Zeroes Instruction

The Blackfin CPU has the BITTST instruction which could be used to speed
up the find first bit operation.  The use of this instruction should
significantly speed up the scheduling associated with a thread blocking.

@section Calling Conventions

This section is heavily based on content taken from the Blackfin uCLinux
documentation wiki which is edited by Analog Devices and Arcturus
Networks.  @file{http://docs.blackfin.uclinux.org/}

@subsection Processor Background

The Blackfin architecture supports a simple call and return mechanism.
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

RTEMS assumes that the Blackfin GCC calling convention is followed.
The first three parameters are stored in registers R0, R1, and R2.
All other parameters are put pushed on the stack.  The result is returned
through register R0.

@section Memory Model

The Blackfin family architecutre support a single unified 4 GB byte
address space using 32-bit addresses. It maps all resources like internal
and external memory and IO registers into separate sections of this
common address space.

The Blackfin architcture supports some form of memory
protection via its Memory Management Unit. Since the
Blackfin port runs in supervisior mode this memory
protection mechanisms are not used.

@section Interrupt Processing

Discussed in this chapter are the Blackfin's interrupt response and
control mechanisms as they pertain to RTEMS. The Blackfin architecture
support 16 kinds of interrupts broken down into Core and general-purpose
interrupts.

@subsection Vectoring of an Interrupt Handler

RTEMS maps levels 0 -15 directly to Blackfins event vectors EVT0 -
EVT15. Since EVT0 - EVT6 are core events and it is suggested to use
EVT15 and EVT15 for Software interrupts, 7 Interrupts (EVT7-EVT13)
are left for periferical use.

When installing an RTEMS interrupt handler RTEMS installs a generic
Interrupt Handler which saves some context and enables nested interrupt
servicing and then vectors to the users interrupt handler.

@subsection Disabling of Interrupts by RTEMS

During interrupt disable critical sections, RTEMS disables interrupts to
level four (4) before the execution of this section and restores them
to the previous level upon completion of the section. RTEMS uses the
instructions CLI and STI to enable and disable Interrupts. Emulation,
Reset, NMI and Exception Interrupts are never disabled.

@subsection Interrupt Stack

The Blackfin Architecture works with two different kind of stacks,
User and Supervisor Stack. Since RTEMS and its Application run
in supervisor mode, all interrupts will use the interrupted
tasks stack for execution.

@section Default Fatal Error Processing

The default fatal error handler for the Blackfin performs the following
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
