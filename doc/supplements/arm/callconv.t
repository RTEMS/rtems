@c
@c  COPYRIGHT (c) 1988-2002.
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

@section Processor Background

The ARM architecture supports a simple yet
effective call and return mechanism.  A subroutine is invoked
via the branch and link (@code{bl}) instruction.  This instruction
saves the return address in the @code{lr} register.  Returning
from a subroutine only requires that the return address be
moved into the program counter (@code{pc}), possibly with
an offset.  It is is important to
note that the @code{bl} instruction does not
automatically save or restore any registers.  It is the
responsibility of the high-level language compiler to define the
register preservation and usage convention.

@section Calling Mechanism

All RTEMS directives are invoked using the @code{bl}
instruction and return to the user application via the
mechanism described above.

@section Register Usage

As discussed above, the ARM's call and return mechanism dos
not automatically save any registers.  RTEMS uses the registers
@code{r0}, @code{r1}, @code{r2}, and @code{r3} as scratch registers and
per ARM calling convention, the @code{lr} register is altered
as well.  These registers are not preserved by RTEMS directives
therefore, the contents of these registers should not be assumed
upon return from any RTEMS directive. 

@section Parameter Passing

RTEMS assumes that ARM calling conventions are followed and that
the first four arguments are placed in registers @code{r0} through
@code{r3}.  If there are more arguments, than that, then they
are place on the stack.

@section User-Provided Routines

All user-provided routines invoked by RTEMS, such as
user extensions, device drivers, and MPCI routines, must also
adhere to these calling conventions.

