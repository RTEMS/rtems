@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@node Board Support Packages, Board Support Packages Introduction, Default Fatal Error Processing Default Fatal Error Handler Operations, Top
@end ifinfo
@chapter Board Support Packages
@ifinfo
@menu
* Board Support Packages Introduction::
* Board Support Packages System Reset::
* Board Support Packages Processor Initialization::
@end menu
@end ifinfo

@ifinfo
@node Board Support Packages Introduction, Board Support Packages System Reset, Board Support Packages, Board Support Packages
@end ifinfo
@section Introduction

An RTEMS Board Support Package (BSP) must be designed
to support a particular processor and target board combination.
This chapter presents a discussion of PowerPC specific BSP issues.
For more information on developing a BSP, refer to the chapter
titled Board Support Packages in the RTEMS
Applications User's Guide.

@ifinfo
@node Board Support Packages System Reset, Board Support Packages Processor Initialization, Board Support Packages Introduction, Board Support Packages
@end ifinfo
@section System Reset

An RTEMS based application is initiated or
re-initiated when the PowerPC processor is reset.  When the PowerPC
is reset, the processor performs the following actions:

@itemize @bullet
@item TBD

@item TBD

@item TBD
@end itemize

The processor then begins to execute the code at location 0x00100.  
By using the SRR1 bit corresponding to MSR[RI] the softwere may 
distinguish between power-on reset and other types of system resets.

It is important to note that all fields in the psr
are not explicitly set by the above steps and all other
registers retain their value from the previous execution mode.
This is true even of the Trap Base Register (TBR) whose contents
reflect the last trap which occurred before the reset.

@ifinfo
@node Board Support Packages Processor Initialization, Processor Dependent Information Table, Board Support Packages System Reset, Board Support Packages
@end ifinfo
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
place when the rtems_initialize_executive directive was invoked.
Upon completion of executive initialization, interrupts are
enabled.

If this PowerPC implementation supports on-chip caching
and this is to be utilized, then it should be enabled during the
reset application initialization code.

In addition to the requirements described in the
Board Support Packages chapter of the @value{LANGUAGE}
Applications User's Manual for the reset code
which is executed before the call to
rtems_initialize executive, the PowrePC version has the following
specific requirements:

@itemize @bullet
@item Must leave the PR bit of the machine state register set so that
the PowerPC remains in the supervisor state.

@item Must set stack pointer (sp) such that a minimum stack
size of MINIMUM_STACK_SIZE bytes is provided for the
rtems_initialize executive directive.

@item Must disable all external interrupts (i.e. clear the EI (EE)
bit of the machine state register).

@item Must enable traps so window overflow and underflow
conditions can be properly handled.

@item Must initialize the PowerPC's initial trap table with at
least trap handlers for register window overflow and register
window underflow.
@end itemize

