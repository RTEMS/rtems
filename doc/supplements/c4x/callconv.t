@c
@c  COPYRIGHT (c) 1988-1999.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Calling Conventions

@section Introduction

Each high-level language compiler generates
subroutine entry and exit code based upon a set of rules known
as the compiler's calling convention.   These rules address the
following issues:

@itemize @bullet
@item register preservation and usage
@item parameter passing
@item call and return mechanism
@end itemize

A compiler's calling convention is of importance when
interfacing to subroutines written in another language either
assembly or high-level.  Even when the high-level language and
target processor are the same, different compilers may use
different calling conventions.  As a result, calling conventions
are both processor and compiler dependent.

The GNU Compiler Suite follows the same calling conventions
as the Texas Instruments toolset.

@section Processor Background

The TI C3x and C4x processors support a simple yet
effective call and return mechanism.  A subroutine is invoked
via the branch to subroutine (@code{XXX}) or the jump to subroutine
(@code{XXX}) instructions.  These instructions push the return address
on the current stack.  The return from subroutine (@code{XXX})
instruction pops the return address off the current stack and
transfers control to that instruction.  It is important to
note that the call and return mechanism for the C3x/C4x does not
automatically save or restore any registers.  It is the
responsibility of the high-level language compiler to define the
register preservation and usage convention.

XXX other supplements may have "is is".

@section Calling Mechanism

All subroutines are invoked using either a @code{XXX}
or @code{XXX} instruction and return to the user application via the
@code{XXX} instruction.

@section Register Usage

XXX

As discussed above, the @code{XXX} and @code{XXX} instructions do
not automatically save any registers.  Subroutines use the registers
@b{D0}, @b{D1}, @b{A0}, and @b{A1} as scratch registers.  These registers are
not preserved by subroutines therefore, the contents of
these registers should not be assumed upon return from any subroutine
call including but not limited to an RTEMS directive.

The GNU and Texas Instruments compilers follow the same conventions
for register usage.

@section Parameter Passing

Both the GNU and Texas Instruments compilers support two conventions
for passing parameters to subroutines.  Arguments may be passed in
memory on the stack or in registers.

@subsection Parameters Passed in Memory

When passing parameters on the stack, the calling convention assumes
that arguments are placed on the current stack before the subroutine
is invoked via the @code{XXX} instruction.  The first argument is
assumed to be closest to the return address on the stack.  This means
that the first argument of the C calling sequence is pushed last.  The
following pseudo-code illustrates the typical sequence used to call a
subroutine with three (3) arguments:

@example
@group
push third argument
push second argument
push first argument
invoke subroutine
remove arguments from the stack
@end group
@end example

The arguments to RTEMS are typically pushed onto the
stack using a @code{sti} instruction with a pre-incremented stack
pointer as the destination.  These arguments must be removed
from the stack after control is returned to the caller.  This
removal is typically accomplished by subtracting the size of the
argument list in words from the current stack pointer.

@c XXX XXX instruction .. XXX should be code format.

With the GNU Compiler Suite, parameter passing via the 
stack is selected by invoking the compiler with the
@code{-mmemparm XXX} argument.  This argument must be
included when linking the application in order to
ensure that support libraries also compiled assuming
parameter passing via the stack are used.  The default
parameter passing mechanism is XXX.

When this parameter passing mecahanism is selected, the @code{XXX}
symbol is predefined by the C and C++ compilers
and the @code{XXX} symbol is predefined by the assembler.
This behavior is the same for the GNU and Texas Instruments
toolsets.  RTEMS uses these predefines to determine how
parameters are passed in to those C3x/C4x specific routines
that were written in assembly language.

@subsection Parameters Passed in Registers

When passing parameters via registers, the calling convention assumes
that the arguments are placed in particular registers based upon
their position and data type before the subroutine is invoked via
the @code{XXX} instruction.  

The following pseudo-code illustrates
the typical sequence used to call a subroutine with three (3) arguments:

@example
@group
move third argument to XXX
move second argument to XXX
move first argument to XXX
invoke subroutine
@end group
@end example

With the GNU Compiler Suite, parameter passing via 
registers is selected by invoking the compiler with the
@code{-mregparm XXX} argument.  This argument must be
included when linking the application in order to
ensure that support libraries also compiled assuming
parameter passing via the stack are used.  The default
parameter passing mechanism is XXX.

When this parameter passing mecahanism is selected, the @code{XXX}
symbol is predefined by the C and C++ compilers
and the @code{XXX} symbol is predefined by the assembler.
This behavior is the same for the GNU and Texas Instruments
toolsets.  RTEMS uses these predefines to determine how
parameters are passed in to those C3x/C4x specific routines
that were written in assembly language.

@section User-Provided Routines

All user-provided routines invoked by RTEMS, such as
user extensions, device drivers, and MPCI routines, must also
adhere to these calling conventions.

