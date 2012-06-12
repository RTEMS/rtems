@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@ifinfo
@end ifinfo
@chapter V850 Specific Information

This chapter discusses the
@uref{http://en.wikipedia.org/wiki/V850,V850 architecture}
dependencies in this port of RTEMS.  The V850 was originally manufactured
by NEC but is now part of the Renesas Electronics product line.

@subheading Architecture Documents

For information on the V850 architecture refer to the
@uref{http://am.renesas.com/products/mpumcu/v850/index.jsp,Renesas v850 product page}.

@section CPU Model Dependent Features

This section presents the set of features which vary across V850 implementations and are of importance to RTEMS.  The set of CPU model feature macros are defined in the file @file{cpukit/score/cpu/v850/rtems/score/v850.h} based upon the particular CPU
model flags specified on the compilation command line.

@subsection CPU Model Name

The macro @code{CPU_MODEL_NAME} is a string which designates
the architectural level of this CPU model.  See in
@file{cpukit/score/cpu/v850/rtems/score/v850.h} for the values.

@subsection Count Leading Zeroes Instruction

The V850v5 and later has the count leading zeroes @code{clz} instruction which
could be used to speed up the find first bit operation.  The use of this
instruction should significantly speed up the scheduling associated with a
thread blocking.  This is currently not used.

@subsection Floating Point Unit

A floating point unit is currently not supported.

@section Calling Conventions

Please refer to the
@uref{http://www.filibeto.org/unix/tru64/lib/ossc/doc/cygnus_doc-99r1/html/6_embed/embV850.html,
Procedure Call Standard for the V850 Architecture} or the GCC source
code for detailed information on the calling conventions.

@subsection Register Usage

Fixed registers are never available for register allocation in the
compiler. By default the following registers are fixed in GCC:

@itemize @bullet
@item r0 (zero)
@item r3 (sp)
@item r4 (gp)
@item r30 (ep)
@end itemize

@c r1 is mentioned as special purpose but I do not see a purpose

Caller saved registers can be used by the compiler to hold values that
do not live across function calls. The caller saved registers are r2,
r5 through r19, and r31.

Callee saved registers retain their value across function calls. The
callee saved registers are r20 through r29.

r6 through r9 are parameter registers while r10 and r11 are function return registers. r31 is the return pointer.

r29 is used as the frame pointer in some functions.

@section Memory Model

A flat 32-bit memory model is supported.  

@section Interrupt Processing

The V850 architecture has ...

@subsection Interrupt Levels

The RTEMS interrupt level mapping scheme for the V850 is very simple. If
the requested interrupt level is 1, then interrupts are disabled in the
PSW register using the @code{di} instruction. If the requested interrupt
level is 0, then interrupts are enabled in the PSW register using the
@code{ei} instruction or restoring the previous value of the PSW register.
 
@subsection Interrupt Stack

The board support package must initialize the interrupt stack. The memory for
the stacks is usually reserved in the linker script. 

@section Default Fatal Error Processing

The default fatal error handler for this architecture performs the
following actions:

@itemize @bullet
@item disables operating system supported interrupts (IRQ),
@item places the error code in @code{r10}, and
@item executes a halt processor instruction.
@end itemize
