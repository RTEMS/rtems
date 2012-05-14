@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@ifinfo
@end ifinfo
@chapter ARM Specific Information

This chapter discusses the
@uref{http://en.wikipedia.org/wiki/ARM_architecture,ARM architecture}
dependencies in this port of RTEMS.  The ARM family has a wide variety of
implementations by a wide range of vendors.  Consequently, there are many, many
CPU models within it.  Currently the ARMv5 (and compatible) architecture
version as defined in the @code{ARMv5 Architecture Reference Manual} is supported by RTEMS.

@subheading Architecture Documents

For information on the ARM architecture refer to the
@uref{http://infocenter.arm.com,ARM Infocenter}.

@section CPU Model Dependent Features

This section presents the set of features which vary
across ARM implementations and are of importance to RTEMS.  The set of CPU
model feature macros are defined in the file
@file{cpukit/score/cpu/arm/rtems/score/arm.h} based upon the particular CPU
model flags specified on the compilation command line.

@subsection CPU Model Name

The macro @code{CPU_MODEL_NAME} is a string which designates
the architectural level of this CPU model.  See in
@file{cpukit/score/cpu/arm/rtems/score/arm.h} for the values.

@subsection Count Leading Zeroes Instruction

The ARMv5 and later has the count leading zeroes @code{clz} instruction which
could be used to speed up the find first bit operation.  The use of this
instruction should significantly speed up the scheduling associated with a
thread blocking.  This is currently not used.

@subsection Floating Point Unit

A floating point unit is currently not supported.

@section Calling Conventions

Please refer to the
@uref{http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042c/IHI0042C_aapcs.pdf,Procedure Call Standard for the ARM Architecture}.

@section Memory Model

A flat 32-bit memory model is supported.  The board support package must take
care about the MMU if necessary.

@section Interrupt Processing

The ARMv5 (and compatible) architecture has seven exception types:

@itemize @bullet

@item Reset
@item Undefined
@item Software Interrupt (SWI)
@item Prefetch Abort
@item Data Abort
@item Interrupt (IRQ)
@item Fast Interrupt (FIQ)

@end itemize

Of these types only the IRQ has explicit operating system support.  It is
intentional that the FIQ is not supported by the operating system.  Without
operating system support for the FIQ it is not necessary to disable them during
critical sections of the system.

@subsection Interrupt Levels

The RTEMS interrupt level mapping scheme for the ARM is not a numeric level as
on most RTEMS ports.  It is a bit mapping that corresponds the enable bit
postions in the Current Program Status Register (CPSR).  There are only two
levels: IRQ enabled and IRQ disabled.
 
@subsection Interrupt Stack

The board support package must initialize the interrupt stack. The memory for
the stacks is usually reserved in the linker script. 

@section Default Fatal Error Processing

The default fatal error handler for this architecture performs the
following actions:

@itemize @bullet
@item disables operating system supported interrupts (IRQ),
@item places the error code in @code{r0}, and
@item executes an infinite loop to simulate a halt processor instruction.
@end itemize
