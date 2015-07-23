@c
@c  Copyright (c) 2015 University of York.
@c  Hesham ALMatary <hmka501@york.ac.uk>

@ifinfo
@end ifinfo
@chapter Epiphany Specific Information

This chapter discusses the
@uref{http://adapteva.com/docs/epiphany_sdk_ref.pdf, Epiphany Architecture}
dependencies in this port of RTEMS. Epiphany is a chip that can come with 16 and
64 cores, each of which can run RTEMS separately or they can work together to 
run a SMP RTEMS application.

@subheading Architecture Documents

For information on the Epiphany architecture refer to the
@uref{http://adapteva.com/docs/epiphany_arch_ref.pdf,Epiphany Architecture Reference}.

@section Calling Conventions

Please refer to the
@uref{http://adapteva.com/docs/epiphany_sdk_ref.pdf, Epiphany SDK}
Appendix A: Application Binary Interface

@subsection Floating Point Unit

A floating point unit is currently not supported.

@section Memory Model

A flat 32-bit memory model is supported, no caches. Each core has its own 32 KiB
strictly ordered local memory along with an access to a shared 32 MiB external
DRAM.

@section Interrupt Processing

Every Epiphany core has 10 exception types:

@itemize @bullet

@item Reset
@item Software Exception
@item Data Page Fault
@item Timer 0
@item Timer 1
@item Message Interrupt
@item DMA0 Interrupt
@item DMA1 Interrupt
@item WANT Interrupt
@item User Interrupt

@end itemize

@subsection Interrupt Levels

There are only two levels: interrupts enabled and interrupts disabled.

@subsection Interrupt Stack

The Epiphany RTEMS port uses a dedicated software interrupt stack.
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
