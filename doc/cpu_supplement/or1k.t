@c
@c  COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
@c  All rights reserved.

@ifinfo
@end ifinfo
@chapter OpenRISC 1000 Specific Information

This chapter discusses the
@uref{http://opencores.org/or1k/Main_Page, OpenRISC 1000 architecture}
dependencies in this port of RTEMS. There are many implementations
for OpenRISC like or1200 and mor1kx. Currently RTEMS supports basic
features that all implementations should have.

@subheading Architecture Documents

For information on the OpenRISC 1000 architecture refer to the
@uref{http://openrisc.github.io/or1k.html,OpenRISC 1000 architecture manual}.

@section Calling Conventions

Please refer to the
@uref{http://openrisc.github.io/or1k.html#__RefHeading__504887_595890882,Function Calling Sequence}.

@subsection Floating Point Unit

A floating point unit is currently not supported.

@section Memory Model

A flat 32-bit memory model is supported.

@section Interrupt Processing

OpenRISC 1000 architecture has 13 exception types:

@itemize @bullet

@item Reset
@item Bus Error
@item Data Page Fault
@item Instruction Page Fault
@item Tick Timer
@item Alignment
@item Illegal Instruction
@item External Interrupt
@item D-TLB Miss
@item I-TLB Miss
@item Range
@item System Call
@item Floating Point
@item Trap

@end itemize

@subsection Interrupt Levels

There are only two levels: interrupts enabled and interrupts disabled.

@subsection Interrupt Stack

The OpenRISC RTEMS port uses a dedicated software interrupt stack.
The stack for interrupts is allocated during interrupt driver initialization.
When an  interrupt is entered, the _ISR_Handler routine is responsible for
switching from the interrupted task stack to RTEMS software interrupt stack.

@section Default Fatal Error Processing

The default fatal error handler for this architecture performs the
following actions:

@itemize @bullet
@item disables operating system supported interrupts (IRQ),
@item places the error code in @code{r0}, and
@item executes an infinite loop to simulate a halt processor instruction.
@end itemize

@section Symmetric Multiprocessing

SMP is not supported.
