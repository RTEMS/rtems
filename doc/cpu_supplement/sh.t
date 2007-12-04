@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@end ifinfo
@chapter SuperH Specific Information

The Real Time Executive for Multiprocessor Systems (RTEMS)
is designed to be portable across multiple processor
architectures.  However, the nature of real-time systems makes
it essential that the application designer understand certain
processor dependent implementation details.  These processor
dependencies include calling convention, board support package
issues, interrupt processing, exact RTEMS memory requirements,
performance data, header files, and the assembly language
interface to the executive.

This document discusses the VENDOR XXX
architecture dependencies in this port of RTEMS.  The XXX
family has a wide variety of CPU models within it.  The part 
numbers ...

XXX fill in some things here

It is highly recommended that the XXX
RTEMS application developer obtain and become familiar with the
documentation for the processor being used as well as the
documentation for the family as a whole.

@subheading Architecture Documents

For information on the XXX architecture,
refer to the following documents available from VENDOR
(@file{http//www.XXX.com/}):

@itemize @bullet
@item @cite{XXX Family Reference, VENDOR, PART NUMBER}.
@end itemize

@subheading MODEL SPECIFIC DOCUMENTS

For information on specific processor models and
their associated coprocessors, refer to the following documents:

@itemize  @bullet
@item @cite{XXX MODEL Manual, VENDOR, PART NUMBER}.
@item @cite{XXX MODEL Manual, VENDOR, PART NUMBER}.
@end itemize

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section CPU Model Dependent Features


Microprocessors are generally classified into
families with a variety of CPU models or implementations within
that family.  Within a processor family, there is a high level
of binary compatibility.  This family may be based on either an
architectural specification or on maintaining compatibility with
a popular processor.  Recent microprocessor families such as the
SPARC or PowerPC are based on an architectural specification
which is independent or any particular CPU model or
implementation.  Older families such as the M68xxx and the iX86
evolved as the manufacturer strived to produce higher
performance processor models which maintained binary
compatibility with older models.

RTEMS takes advantage of the similarity of the
various models within a CPU family.  Although the models do vary
in significant ways, the high level of compatibility makes it
possible to share the bulk of the CPU dependent executive code
across the entire family.  Each processor family supported by
RTEMS has a list of features which vary between CPU models
within a family.  For example, the most common model dependent
feature regardless of CPU family is the presence or absence of a
floating point unit or coprocessor.  When defining the list of
features present on a particular CPU model, one simply notes
that floating point hardware is or is not present and defines a
single constant appropriately.  Conditional compilation is
utilized to include the appropriate source code for this CPU
model's feature set.  It is important to note that this means
that RTEMS is thus compiled using the appropriate feature set
and compilation flags optimal for this CPU model used.  The
alternative would be to generate a binary which would execute on
all family members using only the features which were always
present.

This chapter presents the set of features which vary
across SPARC implementations and are of importance to RTEMS.
The set of CPU model feature macros are defined in the file
cpukit/score/cpu/XXX/XXX.h based upon the particular CPU
model defined on the compilation command line.

@subsection CPU Model Name

The macro CPU_MODEL_NAME is a string which designates
the name of this CPU model.  For example, for the MODEL
processor, this macro is set to the string "XXX".

@subsection Floating Point Unit

The macro XXX_HAS_FPU is set to 1 to indicate that
this CPU model has a hardware floating point unit and 0
otherwise.  It does not matter whether the hardware floating
point support is incorporated on-chip or is an external
coprocessor.

@subsection Another Optional Feature

The macro XXX 
@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Calling Conventions


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

The Hitachi SH architecture supports a simple yet
effective call and return mechanism.  A subroutine is invoked
via the branch to subroutine (XXX) or the jump to subroutine
(XXX) instructions.  These instructions push the return address
on the current stack.  The return from subroutine (rts)
instruction pops the return address off the current stack and
transfers control to that instruction.  It is is important to
note that the MC68xxx call and return mechanism does not
automatically save or restore any registers.  It is the
responsibility of the high-level language compiler to define the
register preservation and usage convention.

@subsection Calling Mechanism

All RTEMS directives are invoked using either a bsr
or jsr instruction and return to the user application via the
rts instruction.

@subsection Register Usage

As discussed above, the bsr and jsr instructions do
not automatically save any registers.  RTEMS uses the registers
D0, D1, A0, and A1 as scratch registers.  These registers are
not preserved by RTEMS directives therefore, the contents of
these registers should not be assumed upon return from any RTEMS
directive.


> > The SH1 has 16 general registers (r0..r15)
> > r0..r3 used as general volatile registers
> > r4..r7 used to pass up to 4 arguments to functions, arguments above 4 are
> > passed via the stack)
> > r8..13 caller saved registers (i.e. push them to the stack if you need them
> > inside of a function)
> > r14 frame pointer
> > r15 stack pointer
>


@subsection Parameter Passing

RTEMS assumes that arguments are placed on the
current stack before the directive is invoked via the bsr or jsr
instruction.  The first argument is assumed to be closest to the
return address on the stack.  This means that the first argument
of the C calling sequence is pushed last.  The following
pseudo-code illustrates the typical sequence used to call a
RTEMS directive with three (3) arguments:

@example
@group
push third argument
push second argument
push first argument
invoke directive
remove arguments from the stack
@end group
@end example

The arguments to RTEMS are typically pushed onto the
stack using a move instruction with a pre-decremented stack
pointer as the destination.  These arguments must be removed
from the stack after control is returned to the caller.  This
removal is typically accomplished by adding the size of the
argument list in bytes to the current stack pointer.

@subsection User-Provided Routines

All user-provided routines invoked by RTEMS, such as
user extensions, device drivers, and MPCI routines, must also
adhere to these calling conventions.

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Memory Model


A processor may support any combination of memory
models ranging from pure physical addressing to complex demand
paged virtual memory systems.  RTEMS supports a flat memory
model which ranges contiguously over the processor's allowable
address space.  RTEMS does not support segmentation or virtual
memory of any kind.  The appropriate memory model for RTEMS
provided by the targeted processor and related characteristics
of that model are described in this chapter.

@subsection Flat Memory Model

The XXX family supports a flat 32-bit address
space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes).  Each address is represented by a 32-bit value and
is byte addressable.  The address may be used to reference a
single byte, word (2-bytes), or long word (4 bytes).  Memory
accesses within this address space are performed in big endian
fashion by the processors in this family.

Some of the XXX family members such as the
XXX, XXX, and XXX support virtual memory and
segmentation.  The XXX requires external hardware support
such as the XXX Paged Memory Management Unit coprocessor
which is typically used to perform address translations for
these systems.  RTEMS does not support virtual memory or
segmentation on any of the XXX family members.

@c
@c  Interrupt Stack Frame Picture
@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Interrupt Processing


Different types of processors respond to the
occurrence of an interrupt in its own unique fashion. In
addition, each processor type provides a control mechanism to
allow for the proper handling of an interrupt.  The processor
dependent response to the interrupt modifies the current
execution state and results in a change in the execution stream.
Most processors require that an interrupt handler utilize some
special control mechanisms to return to the normal processing
stream.  Although RTEMS hides many of the processor dependent
details of interrupt processing, it is important to understand
how the RTEMS interrupt manager is mapped onto the processor's
unique architecture. Discussed in this chapter are the SH's
interrupt response and control mechanisms as they pertain to
RTEMS.

@subsection Vectoring of an Interrupt Handler

Depending on whether or not the particular CPU
supports a separate interrupt stack, the SH family has two
different interrupt handling models.

@subsubsection Models Without Separate Interrupt Stacks

Upon receipt of an interrupt the SH family
members without separate interrupt stacks automatically perform
the following actions:

@itemize @bullet
@item To Be Written
@end itemize

@subsubsection Models With Separate Interrupt Stacks

Upon receipt of an interrupt the SH family
members with separate interrupt stacks automatically perform the
following actions:

@itemize @bullet
@item saves the current status register (SR),

@item clears the master/interrupt (M) bit of the SR to
indicate the switch from master state to interrupt state,

@item sets the privilege mode to supervisor,

@item suppresses tracing,

@item sets the interrupt mask level equal to the level of the
interrupt being serviced,

@item pushes an interrupt stack frame (ISF), which includes
the program counter (PC), the status register (SR), and the
format/exception vector offset (FVO) word, onto the supervisor
and interrupt stacks,

@item switches the current stack to the interrupt stack and
vectors to an interrupt service routine (ISR).  If the ISR was
installed with the interrupt_catch directive, then the RTEMS
interrupt handler will begin execution.  The RTEMS interrupt
handler saves all registers which are not preserved according to
the calling conventions and invokes the application's ISR.
@end itemize

A nested interrupt is processed similarly by these
CPU models with the exception that only a single ISF is placed
on the interrupt stack and the current stack need not be
switched.

The FVO word in the Interrupt Stack Frame is examined
by RTEMS to determine when an outer most interrupt is being
exited. Since the FVO is used by RTEMS for this purpose, the
user application code MUST NOT modify this field.

The following shows the Interrupt Stack Frame for
XXX CPU models with separate interrupt stacks:

@ifset use-ascii
@example
@group
               +----------------------+
               |    Status Register   | 0x0
               +----------------------+    
               | Program Counter High | 0x2
               +----------------------+    
               | Program Counter Low  | 0x4
               +----------------------+    
               | Format/Vector Offset | 0x6
               +----------------------+    
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\strut\vrule#&
\hbox to 2.00in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.50in{\enskip\hfil#\hfil}
\cr
\multispan{3}\hrulefill\cr
& Status Register && 0x0\cr
\multispan{3}\hrulefill\cr
& Program Counter High && 0x2\cr
\multispan{3}\hrulefill\cr
& Program Counter Low && 0x4\cr
\multispan{3}\hrulefill\cr
& Format/Vector Offset && 0x6\cr
\multispan{3}\hrulefill\cr
}}\hfil}
@end tex
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="40%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Status Register</STRONG></TD>
    <TD ALIGN=center>0x0</TD></TR>
<TR><TD ALIGN=center><STRONG>Program Counter High</STRONG></TD>
    <TD ALIGN=center>0x2</TD></TR>
<TR><TD ALIGN=center><STRONG>Program Counter Low</STRONG></TD>
    <TD ALIGN=center>0x4</TD></TR>
<TR><TD ALIGN=center><STRONG>Format/Vector Offset</STRONG></TD>
    <TD ALIGN=center>0x6</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@subsection Interrupt Levels

Eight levels (0-7) of interrupt priorities are
supported by XXX family members with level seven (7) being
the highest priority.  Level zero (0) indicates that interrupts
are fully enabled.  Interrupt requests for interrupts with
priorities less than or equal to the current interrupt mask
level are ignored.

Although RTEMS supports 256 interrupt levels, the
XXX family only supports eight.  RTEMS interrupt levels 0
through 7 directly correspond to XXX interrupt levels.  All
other RTEMS interrupt levels are undefined and their behavior is
unpredictable.

@subsection Disabling of Interrupts by RTEMS

During the execution of directive calls, critical
sections of code may be executed.  When these sections are
encountered, RTEMS disables interrupts to level seven (7) before
the execution of this section and restores them to the previous
level upon completion of the section.  RTEMS has been optimized
to insure that interrupts are disabled for less than 
RTEMS_MAXIMUM_DISABLE_PERIOD microseconds on a 
RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ Mhz XXX with 
zero wait states.  These numbers will vary based the 
number of wait states and processor speed present on the target board.
[NOTE:  The maximum period with interrupts disabled is hand calculated.  This
calculation was last performed for Release 
RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

Non-maskable interrupts (NMI) cannot be disabled, and
ISRs which execute at this level MUST NEVER issue RTEMS system
calls.  If a directive is invoked, unpredictable results may
occur due to the inability of RTEMS to protect its critical
sections.  However, ISRs that make no system calls may safely
execute as non-maskable interrupts.

@subsection Interrupt Stack

RTEMS allocates the interrupt stack from the
Workspace Area.  The amount of memory allocated for the
interrupt stack is determined by the interrupt_stack_size field
in the CPU Configuration Table.  During the initialization
process, RTEMS will install its interrupt stack.

The XXX port of RTEMS supports a software managed
dedicated interrupt stack on those CPU models which do not
support a separate interrupt stack in hardware.


@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Default Fatal Error Processing


Upon detection of a fatal error by either the
application or RTEMS the fatal error manager is invoked.  The
fatal error manager will invoke the user-supplied fatal error
handlers.  If no user-supplied handlers are configured,  the
RTEMS provided default fatal error handler is invoked.  If the
user-supplied fatal error handlers return to the executive the
default fatal error handler is then invoked.  This chapter
describes the precise operations of the default fatal error
handler.

@subsection Default Fatal Error Handler Operations

The default fatal error handler which is invoked by
the @code{rtems_fatal_error_occurred} directive when there is
no user handler configured or the user handler returns control to
RTEMS.  The default fatal error handler disables processor interrupts,
places the error code in @b{XXX}, and executes a @code{XXX}
instruction to simulate a halt processor instruction.

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Board Support Packages


An RTEMS Board Support Package (BSP) must be designed
to support a particular processor and target board combination.
This chapter presents a discussion of XXX specific BSP
issues.   For more information on developing a BSP, refer to the
chapter titled Board Support Packages in the RTEMS
Applications User's Guide.

@subsection System Reset

An RTEMS based application is initiated or
re-initiated when the XXX processor is reset.  When the
XXX is reset, the processor performs the following actions:

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

@subsection Processor Initialization

The address of the application's initialization code
should be stored in the first vector of the EVT which will allow
the immediate vectoring to the application code.  If the
application requires that the VBR be some value besides zero,
then it should be set to the required value at this point.  All
tasks share the same XXX's VBR value.  Because interrupts
are enabled automatically by RTEMS as part of the initialize
executive directive, the VBR MUST be set before this directive
is invoked to insure correct interrupt vectoring.  If processor
caching is to be utilized, then it should be enabled during the
reset application initialization code.

In addition to the requirements described in the
Board Support Packages chapter of the Applications User's
Manual for the reset code which is executed before the call to
initialize executive, the XXX version has the following
specific requirements:

@itemize @bullet
@item Must leave the S bit of the status register set so that
the XXX remains in the supervisor state.

@item Must set the M bit of the status register to remove the
XXX from the interrupt state.

@item Must set the master stack pointer (MSP) such that a
minimum stack size of MINIMUM_STACK_SIZE bytes is provided for
the initialize executive directive.

@item Must initialize the XXX's vector table.
@end itemize

Note that the BSP is not responsible for allocating
or installing the interrupt stack.  RTEMS does this
automatically as part of initialization.  If the BSP does not
install an interrupt stack and -- for whatever reason -- an
interrupt occurs before initialize_executive is invoked, then
the results are unpredictable.
