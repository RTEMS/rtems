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
This chapter presents a discussion of PowerPC specific BSP issues.
For more information on developing a BSP, refer to the chapter
titled Board Support Packages in the RTEMS
Applications User's Guide.

@section System Reset

An RTEMS based application is initiated or
re-initiated when the PowerPC processor is reset.  The PowerPC 
architecture defines a Reset Exception, but leaves the
details of the CPU state as implementation specific.  Please
refer to the User's Manual for the CPU model in question.

In general, at power-up the PowerPC begin execution at address
0xFFF00100 in supervisor mode with all exceptions disabled.  For
soft resets, the CPU will vector to either 0xFFF00100 or 0x00000100
depending upon the setting of the Exception Prefix bit in the MSR.
If during a soft reset, a Machine Check Exception occurs, then the
CPU may execute a hard reset.

@section Processor Initialization

It is the responsibility of the application's
initialization code to initialize the CPU and board
to a quiescent state before invoking the @code{rtems_initialize_executive}
directive.  It is recommended that the BSP utilize the @code{predriver_hook}
to install default handlers for all exceptions.  These default handlers
may be overwritten as various device drivers and subsystems install
their own exception handlers.  Upon completion of RTEMS executive
initialization, all interrupts are enabled.

If this PowerPC implementation supports on-chip caching
and this is to be utilized, then it should be enabled during the
reset application initialization code.  On-chip caching has been
observed to prevent some emulators from working properly, so it
may be necessary to run with caching disabled to use these emulators.

In addition to the requirements described in the
@b{Board Support Packages} chapter of the @b{@value{LANGUAGE}
Applications User's Manual} for the reset code
which is executed before the call to @code{rtems_initialize_executive},
the PowrePC version has the following specific requirements:

@itemize @bullet
@item Must leave the PR bit of the Machine State Register (MSR) set 
to 0 so the PowerPC remains in the supervisor state.

@item Must set stack pointer (sp or r1) such that a minimum stack
size of MINIMUM_STACK_SIZE bytes is provided for the
@code{rtems_initialize_executive} directive.

@item Must disable all external interrupts (i.e. clear the EI (EE)
bit of the machine state register).

@item Must enable traps so window overflow and underflow
conditions can be properly handled.

@item Must initialize the PowerPC's initial Exception Table with default
handlers.

@end itemize

