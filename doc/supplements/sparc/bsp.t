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
This chapter presents a discussion of SPARC specific BSP issues.
For more information on developing a BSP, refer to the chapter
titled Board Support Packages in the RTEMS
Applications User's Guide.

@section System Reset

An RTEMS based application is initiated or
re-initiated when the SPARC processor is reset.  When the SPARC
is reset, the processor performs the following actions:

@itemize @bullet
@item the enable trap (ET) of the psr is set to 0 to disable
traps,

@item the supervisor bit (S) of the psr is set to 1 to enter
supervisor mode, and

@item the PC is set 0 and the nPC is set to 4.
@end itemize

The processor then begins to execute the code at
location 0.  It is important to note that all fields in the psr
are not explicitly set by the above steps and all other
registers retain their value from the previous execution mode.
This is true even of the Trap Base Register (TBR) whose contents
reflect the last trap which occurred before the reset.

@section Processor Initialization

It is the responsibility of the application's
initialization code to initialize the TBR and install trap
handlers for at least the register window overflow and register
window underflow conditions.  Traps should be enabled before
invoking any subroutines to allow for register window
management.  However, interrupts should be disabled by setting
the Processor Interrupt Level (pil) field of the psr to 15.
RTEMS installs it's own Trap Table as part of initialization
which is initialized with the contents of the Trap Table in
place when the @code{rtems_initialize_executive} directive was invoked.
Upon completion of executive initialization, interrupts are
enabled.

If this SPARC implementation supports on-chip caching
and this is to be utilized, then it should be enabled during the
reset application initialization code.

In addition to the requirements described in the
Board Support Packages chapter of the @value{LANGUAGE}
Applications User's Manual for the reset code
which is executed before the call to
@code{rtems_initialize_executive}, the SPARC version has the following
specific requirements:

@itemize @bullet
@item Must leave the S bit of the status register set so that
the SPARC remains in the supervisor state.

@item Must set stack pointer (sp) such that a minimum stack
size of MINIMUM_STACK_SIZE bytes is provided for the
@code{rtems_initialize_executive} directive.

@item Must disable all external interrupts (i.e. set the pil
to 15).

@item Must enable traps so window overflow and underflow
conditions can be properly handled.

@item Must initialize the SPARC's initial trap table with at
least trap handlers for register window overflow and register
window underflow.
@end itemize

