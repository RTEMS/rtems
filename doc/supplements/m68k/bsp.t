@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Board Support Packages

@section Introduction

An RTEMS Board Support Package (BSP) must be designed
to support a particular processor and target board combination.
This chapter presents a discussion of MC68020 specific BSP
issues.   For more information on developing a BSP, refer to the
chapter titled Board Support Packages in the RTEMS
Applications User's Guide.

@section System Reset

An RTEMS based application is initiated or
re-initiated when the MC68020 processor is reset.  When the
MC68020 is reset, the processor performs the following actions:

@itemize @bullet
@item The tracing bits of the status register are cleared to
disable tracing.

@item The supervisor interrupt state is entered by setting the
supervisor (S) bit and clearing the master/interrupt (M) bit of
the status register.

@item The interrupt mask of the status register is set to
level 7 to effectively disable all maskable interrupts.

@item The vector base register (VBR) is set to zero.

@item The cache control register (CACR) is set to zero to
disable and freeze the processor cache.

@item The interrupt stack pointer (ISP) is set to the value
stored at vector 0 (bytes 0-3) of the exception vector table
(EVT).

@item The program counter (PC) is set to the value stored at
vector 1 (bytes 4-7) of the EVT.

@item The processor begins execution at the address stored in
the PC.
@end itemize

@section Processor Initialization

The address of the application's initialization code
should be stored in the first vector of the EVT which will allow
the immediate vectoring to the application code.  If the
application requires that the VBR be some value besides zero,
then it should be set to the required value at this point.  All
tasks share the same MC68020's VBR value.  Because interrupts
are enabled automatically by RTEMS as part of the initialize
executive directive, the VBR MUST be set before this directive
is invoked to insure correct interrupt vectoring.  If processor
caching is to be utilized, then it should be enabled during the
reset application initialization code.

In addition to the requirements described in the
Board Support Packages chapter of the Applications User's
Manual for the reset code which is executed before the call to
initialize executive, the MC68020 version has the following
specific requirements:

@itemize @bullet
@item Must leave the S bit of the status register set so that
the MC68020 remains in the supervisor state.

@item Must set the M bit of the status register to remove the
MC68020 from the interrupt state.

@item Must set the master stack pointer (MSP) such that a
minimum stack size of MINIMUM_STACK_SIZE bytes is provided for
the initialize executive directive.

@item Must initialize the MC68020's vector table.
@end itemize

Note that the BSP is not responsible for allocating
or installing the interrupt stack.  RTEMS does this
automatically as part of initialization.  If the BSP does not
install an interrupt stack and -- for whatever reason -- an
interrupt occurs before initialize_executive is invoked, then
the results are unpredictable.

