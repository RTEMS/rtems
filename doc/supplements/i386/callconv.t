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

The i386 architecture supports a simple yet effective
call and return mechanism.  A subroutine is invoked via the call
(call) instruction.  This instruction pushes the return address
on the stack.  The return from subroutine (ret) instruction pops
the return address off the current stack and transfers control
to that instruction.  It is is important to note that the i386
call and return mechanism does not automatically save or restore
any registers.  It is the responsibility of the high-level
language compiler to define the register preservation and usage
convention.

@section Calling Mechanism

All RTEMS directives are invoked using a call
instruction and return to the user application via the ret
instruction.

@section Register Usage

As discussed above, the call instruction does not
automatically save any registers.  RTEMS uses the registers EAX,
ECX, and EDX as scratch registers.  These registers are not
preserved by RTEMS directives therefore, the contents of these
registers should not be assumed upon return from any RTEMS
directive.

@section Parameter Passing

RTEMS assumes that arguments are placed on the
current stack before the directive is invoked via the call
instruction.  The first argument is assumed to be closest to the
return address on the stack.  This means that the first argument
of the C calling sequence is pushed last.  The following
pseudo-code illustrates the typical sequence used to call a
RTEMS directive with three (3) arguments:

@example
push third argument
push second argument
push first argument
invoke directive
remove arguments from the stack
@end example

The arguments to RTEMS are typically pushed onto the
stack using a push instruction.  These arguments must be removed
from the stack after control is returned to the caller.  This
removal is typically accomplished by adding the size of the
argument list in bytes to the stack pointer.

@section User-Provided Routines

All user-provided routines invoked by RTEMS, such as
user extensions, device drivers, and MPCI routines, must also
adhere to these calling conventions.

