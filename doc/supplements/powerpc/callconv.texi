@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@node Calling Conventions, Calling Conventions Introduction, CPU Model Dependent Features CPU Model Implementation Notes, Top
@end ifinfo
@chapter Calling Conventions
@ifinfo
@menu
* Calling Conventions Introduction::
* Calling Conventions Programming Model::
* Calling Conventions Register Windows::
* Calling Conventions Call and Return Mechanism::
* Calling Conventions Calling Mechanism::
* Calling Conventions Register Usage::
* Calling Conventions Parameter Passing::
* Calling Conventions User-Provided Routines::
@end menu
@end ifinfo

@ifinfo
@node Calling Conventions Introduction, Calling Conventions Programming Model, Calling Conventions, Calling Conventions
@end ifinfo
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

RTEMS supports the Embedded Application Binary Interface (EABI)
calling convention.  Documentation for EABI is available by sending
a message with a subject line of "EABI" to eabi@@goth.sis.mot.com.

@ifinfo
@node Calling Conventions Programming Model, Non-Floating Point Registers, Calling Conventions Introduction, Calling Conventions
@end ifinfo
@section Programming Model
@ifinfo
@menu
* Non-Floating Point Registers::
* Floating Point Registers::
* Special Registers::
@end menu
@end ifinfo

This section discusses the programming model for the
PowerPC architecture.

@ifinfo
@node Non-Floating Point Registers, Floating Point Registers, Calling Conventions Programming Model, Calling Conventions Programming Model
@end ifinfo
@subsection Non-Floating Point Registers

The PowerPC architecture defines thirty-two non-floating point registers
directly visible to the programmer.  In thirty-two bit implementations, each
register is thirty-two bits wide.  In sixty-four bit implementations, each
register is sixty-four bits wide.

These registers are referred to as @code{gpr0} to @code{gpr31}.

Some of the registers serve defined roles in the EABI programming model.  
The following table describes the role of each of these registers:

@ifset use-ascii
@example
@group
     +---------------+----------------+----------------------+
     | Register Name | Alternate Name |      Description     |
     +---------------+----------------+----------------------+
     |     g0        |      na        |    reads return 0    |
     |               |                |  writes are ignored  |
     +---------------+----------------+----------------------+
     |     o6        |      sp        |     stack pointer    |
     +---------------+----------------+----------------------+
     |     i6        |      fp        |     frame pointer    |
     +---------------+----------------+----------------------+
     |     i7        |      na        |    return address    |
     +---------------+----------------+----------------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 1.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.75in{\enskip\hfil#\hfil}&
\vrule#\cr
\noalign{\hrule}
&\bf Register Name &&\bf Alternate Names&&\bf Description&\cr\noalign{\hrule}
&g0&&NA&&reads return 0; &\cr
&&&&&writes are ignored&\cr\noalign{\hrule}
&o6&&sp&&stack pointer&\cr\noalign{\hrule}
&i6&&fp&&frame pointer&\cr\noalign{\hrule}
&i7&&NA&&return address&\cr\noalign{\hrule}
}}\hfil}
@end tex
@end ifset
 
@ifset use-html
@html
<CENTER>
  <TABLE COLS=3 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Register Name</STRONG></TD>
    <TD ALIGN=center><STRONG>Alternate Name</STRONG></TD>
    <TD ALIGN=center><STRONG>Description</STRONG></TD></TR>
<TR><TD ALIGN=center>g0</TD>
    <TD ALIGN=center>NA</TD>
    <TD ALIGN=center>reads return 0 ; writes are ignored</TD></TR>
<TR><TD ALIGN=center>o6</TD>
    <TD ALIGN=center>sp</TD>
    <TD ALIGN=center>stack pointer</TD></TR>
<TR><TD ALIGN=center>i6</TD>
    <TD ALIGN=center>fp</TD>
    <TD ALIGN=center>frame pointer</TD></TR>
<TR><TD ALIGN=center>i7</TD>
    <TD ALIGN=center>NA</TD>
    <TD ALIGN=center>return address</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset


@ifinfo
@node Floating Point Registers, Special Registers, Non-Floating Point Registers, Calling Conventions Programming Model
@end ifinfo
@subsection Floating Point Registers

The PowerPC architecture includes thirty-two,
sixty-four bit registers.  All PowwerPC floating point instructions 
interprete these registers as 32 double precision floating point registers,
regardless of whether the processor has 64-bit or 32-bit implementation.

The floating point status and control register (fpscr) records exceptions
and the type of result generated by floating-point operations.
Additionally, it controls the rounding mode of operations and allows the
reporting of floating exceptions to be enabled or disabled.

XXXXXX
A queue of the floating point instructions which have
started execution but not yet completed is maintained.  This
queue is needed to support the multiple cycle nature of floating
point operations and to aid floating point exception trap
handlers.  Once a floating point exception has been encountered,
the queue is frozen until it is emptied by the trap handler.
The floating point queue is loaded by launching instructions.
It is emptied normally when the floating point completes all
outstanding instructions and by floating point exception
handlers with the store double floating point queue (stdfq)
instruction.
XXX

@ifinfo
@node Special Registers, Calling Conventions Register Windows, Floating Point Registers, Calling Conventions Programming Model
@end ifinfo
@subsection Special Registers

The PowerPC architecture includes XXX special registers
which are critical to the programming model: the Machine State
Register (msr) and XXX the Window Invalid Mask (wim) XXX.  The msr
contains the processor mode, power management mode, endian mode, exception
information, privlige level, floating point available and floating point 
excepiton mode, address translation information and the exception prefix.

XXX
condition codes, processor interrupt level, trap
enable bit, supervisor mode and previous supervisor mode bits,
version information, floating point unit and coprocessor enable
bits, and the current window pointer (cwp).  The cwp field of
the psr and wim register are used to manage the register windows
in the SPARC architecture.  The register windows are discussed
in more detail below.
XXX

@ifinfo
@node Calling Conventions Register Windows, Calling Conventions Call and Return Mechanism, Special Registers, Calling Conventions
@end ifinfo
@section Register Windows

The SPARC architecture includes the concept of
register windows.  An overly simplistic way to think of these
windows is to imagine them as being an infinite supply of
"fresh" register sets available for each subroutine to use.  In
reality, they are much more complicated.

The save instruction is used to obtain a new register
window.  This instruction decrements the current window pointer,
thus providing a new set of registers for use.  This register
set includes eight fresh local registers for use exclusively by
this subroutine.  When done with a register set, the restore
instruction increments the current window pointer and the
previous register set is once again available.

The two primary issues complicating the use of
register windows are that (1) the set of register windows is
finite, and (2) some registers are shared between adjacent
registers windows.

Because the set of register windows is finite, it is
possible to execute enough save instructions without
corresponding restore's to consume all of the register windows.
This is easily accomplished in a high level language because
each subroutine typically performs a save instruction upon
entry.  Thus having a subroutine call depth greater than the
number of register windows will result in a window overflow
condition.  The window overflow condition generates a trap which
must be handled in software.  The window overflow trap handler
is responsible for saving the contents of the oldest register
window on the program stack.

Similarly, the subroutines will eventually complete
and begin to perform restore's.  If the restore results in the
need for a register window which has previously been written to
memory as part of an overflow, then a window underflow condition
results.  Just like the window overflow, the window underflow
condition must be handled in software by a trap handler.  The
window underflow trap handler is responsible for reloading the
contents of the register window requested by the restore
instruction from the program stack.

The Window Invalid Mask (wim) and the Current Window
Pointer (cwp) field in the psr are used in conjunction to manage
the finite set of register windows and detect the window
overflow and underflow conditions.  The cwp contains the index
of the register window currently in use.  The save instruction
decrements the cwp modulo the number of register windows.
Similarly, the restore instruction increments the cwp modulo the
number of register windows.  Each bit in the  wim represents
represents whether a register window contains valid information.
The value of 0 indicates the register window is valid and 1
indicates it is invalid.  When a save instruction causes the cwp
to point to a register window which is marked as invalid, a
window overflow condition results.  Conversely, the restore
instruction may result in a window underflow condition.

Other than the assumption that a register window is
always available for trap (i.e. interrupt) handlers, the SPARC
architecture places no limits on the number of register windows
simultaneously marked as invalid (i.e. number of bits set in the
wim).  However, RTEMS assumes that only one register window is
marked invalid at a time (i.e. only one bit set in the wim).
This makes the maximum possible number of register windows
available to the user while still meeting the requirement that
window overflow and underflow conditions can be detected.

The window overflow and window underflow trap
handlers are a critical part of the run-time environment for a
SPARC application.  The SPARC architectural specification allows
for the number of register windows to be any power of two less
than or equal to 32.  The most common choice for SPARC
implementations appears to be 8 register windows.  This results
in the cwp ranging in value from 0 to 7 on most implementations.


The second complicating factor is the sharing of
registers between adjacent register windows.  While each
register window has its own set of local registers, the input
and output registers are shared between adjacent windows.  The
output registers for register window N are the same as the input
registers for register window ((N - 1) modulo RW) where RW is
the number of register windows.  An alternative way to think of
this is to remember how parameters are passed to a subroutine on
the SPARC.  The caller loads values into what are its output
registers.  Then after the callee executes a save instruction,
those parameters are available in its input registers.  This is
a very efficient way to pass parameters as no data is actually
moved by the save or restore instructions.

@ifinfo
@node Calling Conventions Call and Return Mechanism, Calling Conventions Calling Mechanism, Calling Conventions Register Windows, Calling Conventions
@end ifinfo
@section Call and Return Mechanism

The SPARC architecture supports a simple yet
effective call and return mechanism.  A subroutine is invoked
via the call (call) instruction.  This instruction places the
return address in the caller's output register 7 (o7).  After
the callee executes a save instruction, this value is available
in input register 7 (i7) until the corresponding restore
instruction is executed.

The callee returns to the caller via a jmp to the
return address.  There is a delay slot following this
instruction which is commonly used to execute a restore
instruction -- if a register window was allocated by this
subroutine.

It is important to note that the SPARC subroutine
call and return mechanism does not automatically save and
restore any registers.  This is accomplished via the save and
restore instructions which manage the set of registers windows.

@ifinfo
@node Calling Conventions Calling Mechanism, Calling Conventions Register Usage, Calling Conventions Call and Return Mechanism, Calling Conventions
@end ifinfo
@section Calling Mechanism

All RTEMS directives are invoked using the regular
SPARC calling convention via the call instruction.

@ifinfo
@node Calling Conventions Register Usage, Calling Conventions Parameter Passing, Calling Conventions Calling Mechanism, Calling Conventions
@end ifinfo
@section Register Usage

As discussed above, the call instruction does not
automatically save any registers.  The save and restore
instructions are used to allocate and deallocate register
windows.  When a register window is allocated, the new set of
local registers are available for the exclusive use of the
subroutine which allocated this register set.

@ifinfo
@node Calling Conventions Parameter Passing, Calling Conventions User-Provided Routines, Calling Conventions Register Usage, Calling Conventions
@end ifinfo
@section Parameter Passing

RTEMS assumes that arguments are placed in the
caller's output registers with the first argument in output
register 0 (o0), the second argument in output register 1 (o1),
and so forth.  Until the callee executes a save instruction, the
parameters are still visible in the output registers.  After the
callee executes a save instruction, the parameters are visible
in the corresponding input registers.  The following pseudo-code
illustrates the typical sequence used to call a RTEMS directive
with three (3) arguments:

@example
load third argument into o2
load second argument into o1
load first argument into o0
invoke directive
@end example

@ifinfo
@node Calling Conventions User-Provided Routines, Memory Model, Calling Conventions Parameter Passing, Calling Conventions
@end ifinfo
@section User-Provided Routines

All user-provided routines invoked by RTEMS, such as
user extensions, device drivers, and MPCI routines, must also
adhere to these calling conventions.



