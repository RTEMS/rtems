@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
@c  Micro-Research Finland Oy
@c

@ifinfo
@end ifinfo
@chapter Lattice Mico32 Specific Information

This chaper discusses the Lattice Mico32 architecture dependencies in
this port of RTEMS. The Lattice Mico32 is a 32-bit Harvard, RISC
architecture "soft" microprocessor, available for free with an open IP
core licensing agreement. Although mainly targeted for Lattice FPGA
devices the microprocessor can be implemented on other vendors' FPGAs,
too.

@subheading Architecture Documents

For information on the Lattice Mico32 architecture, refer to the
following documents available from Lattice Semiconductor
@file{http://www.latticesemi.com/}.

@itemize @bullet
@item @cite{"LatticeMico32 Processor Reference Manual"}
@file{http://www.latticesemi.com/dynamic/view_document.cfm?document_id=20890}
@end itemize

@c
@c
@c
@section CPU Model Dependent Features

The Lattice Mico32 architecture allows for different configurations of
the processor. This port is based on the assumption that the following options are implemented:

@itemize @bullet
@item hardware multiplier
@item hardware divider
@item hardware barrel shifter
@item sign extension instructions
@item instruction cache
@item data cache
@item debug
@end itemize

@c
@c
@c
@section Register Architecture

This section gives a brief introduction to the register architecture
of the Lattice Mico32 processor.

The Lattice Mico32 is a RISC archictecture processor with a
32-register file of 32-bit registers.

@multitable {Register Name} {general pupose / global pointer}
@headitem Register Name @tab Function
@item r0     @tab holds value zero
@item r1-r25 @tab general purpose
@item r26/gp @tab general pupose / global pointer
@item r27/fp @tab general pupose / frame pointer
@item r28/sp @tab stack pointer
@item r29/ra @tab return address
@item r30/ea @tab exception address
@item r31/ba @tab breakpoint address
@end multitable

Note that on processor startup all register values are undefined
including r0, thus r0 has to be initialized to zero.

@c
@c
@c
@section Calling Conventions

@subsection Calling Mechanism

A call instruction places the return address to register r29 and a
return from subroutine (ret) is actually a branch to r29/ra.

@subsection Register Usage

A subroutine may freely use registers r1 to r10 which are @b{not}
preserved across subroutine invocations.

@subsection Parameter Passing

When calling a C function the first eight arguments are stored in
registers r1 to r8. Registers r1 and r2 hold the return value.

@c
@c
@c
@section Memory Model

The Lattice Mico32 processor supports a flat memory model with a 4
Gbyte address space with 32-bit addresses.

The following data types are supported:

@multitable {unsigned half-word} {Bits} {unsigned int / unsigned long}
@headitem Type           @tab Bits @tab C Compiler Type
@item unsigned byte      @tab 8    @tab unsigned char
@item signed byte        @tab 8    @tab char
@item unsigned half-word @tab 16   @tab unsigned short
@item signed half-word   @tab 16   @tab short
@item unsigned word      @tab 32   @tab unsigned int / unsigned long
@item signed word        @tab 32   @tab int / long
@end multitable

Data accesses need to be aligned, with unaligned accesses result are
undefined.

@c
@c
@c
@section Interrupt Processing

The Lattice Mico32 has 32 interrupt lines which are however served by
only one exception vector. When an interrupt occurs following happens:

@itemize @bullet
@item address of next instruction placed in r30/ea
@item IE field of IE CSR saved to EIE field and IE field cleared preventing further exceptions from occuring.
@item branch to interrupt exception address EBA CSR + 0xC0
@end itemize

The interrupt exception handler determines from the state of the
interrupt pending registers (IP CSR) and interrupt enable register (IE
CSR) which interrupt to serve and jumps to the interrupt routine
pointed to by the corresponding interrupt vector.

For now there is no dedicated interrupt stack so every task in
the system MUST have enough stack space to accommodate the worst
case stack usage of that particular task and the interrupt
service routines COMBINED.

Nested interrupts are not supported.

@c
@c
@c
@section Default Fatal Error Processing

Upon detection of a fatal error by either the application or RTEMS during
initialization the @code{rtems_fatal_error_occurred} directive supplied
by the Fatal Error Manager is invoked.  The Fatal Error Manager will
invoke the user-supplied fatal error handlers.  If no user-supplied
handlers are configured or all of them return without taking action to
shutdown the processor or reset, a default fatal error handler is invoked.

Most of the action performed as part of processing the fatal error are
described in detail in the Fatal Error Manager chapter in the User's
Guide.  However, the if no user provided extension or BSP specific fatal
error handler takes action, the final default action is to invoke a
CPU architecture specific function.  Typically this function disables
interrupts and halts the processor.

In each of the architecture specific chapters, this describes the precise
operations of the default CPU specific fatal error handler.

@c
@c
@c

@section Board Support Packages

An RTEMS Board Support Package (BSP) must be designed to support a
particular processor model and target board combination.

In each of the architecture specific chapters, this section will present
a discussion of architecture specific BSP issues.   For more information
on developing a BSP, refer to BSP and Device Driver Development Guide
and the chapter titled Board Support Packages in the RTEMS
Applications User's Guide.

@subsection System Reset

An RTEMS based application is initiated or re-initiated when the processor
is reset.
