@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@ifinfo
@end ifinfo
@chapter Intel/AMD x86 Specific Information

This chapter discusses the Intel x86 architecture dependencies
in this port of RTEMS.  This family has multiple implementations
from multiple vendors and suffers more from having evolved rather
than being designed for growth.

For information on the i386 processor, refer to the
following documents:

@itemize @bullet
@item @cite{386 Programmer's Reference Manual, Intel, Order No.  230985-002}.

@item @cite{386 Microprocessor Hardware Reference Manual, Intel,
Order No. 231732-003}.

@item @cite{80386 System Software Writer's Guide, Intel, Order No.  231499-001}.

@item @cite{80387 Programmer's Reference Manual, Intel, Order No.  231917-001}.
@end itemize

@c
@c
@c
@section CPU Model Dependent Features

This section presents the set of features which vary
across i386 implementations and are of importance to RTEMS.
The set of CPU model feature macros are defined in the file
@code{cpukit/score/cpu/i386/i386.h} based upon the particular CPU
model specified on the compilation command line.

@subsection bswap Instruction

The macro @code{I386_HAS_BSWAP} is set to 1 to indicate that
this CPU model has the @code{bswap} instruction which
endian swaps a thirty-two bit quantity.  This instruction
appears to be present in all CPU models
i486's and above.

@c
@c
@c
@section Calling Conventions

@subsection Processor Background

The i386 architecture supports a simple yet effective
call and return mechanism.  A subroutine is invoked via the call
(@code{call}) instruction.  This instruction pushes the return address
on the stack.  The return from subroutine (@code{ret}) instruction pops
the return address off the current stack and transfers control
to that instruction.  It is is important to note that the i386
call and return mechanism does not automatically save or restore
any registers.  It is the responsibility of the high-level
language compiler to define the register preservation and usage
convention.

@subsection Calling Mechanism

All RTEMS directives are invoked using a call instruction and return to
the user application via the ret instruction.

@subsection Register Usage

As discussed above, the call instruction does not automatically save
any registers.  RTEMS uses the registers EAX, ECX, and EDX as scratch
registers.  These registers are not preserved by RTEMS directives
therefore, the contents of these registers should not be assumed upon
return from any RTEMS directive.

@subsection Parameter Passing

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

@c
@c
@c

@section Memory Model

@subsection Flat Memory Model

RTEMS supports the i386 protected mode, flat memory
model with paging disabled.  In this mode, the i386
automatically converts every address from a logical to a
physical address each time it is used.  The i386 uses
information provided in the segment registers and the Global
Descriptor Table to convert these addresses.  RTEMS assumes the
existence of the following segments:

@itemize @bullet
@item a single code segment at protection level (0) which
contains all application and executive code.

@item a single data segment at protection level zero (0) which
contains all application and executive data.
@end itemize

The i386 segment registers and associated selectors
must be initialized when the initialize_executive directive is
invoked.  RTEMS treats the segment registers as system registers
and does not modify or context switch them.

This i386 memory model supports a flat 32-bit address
space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes).  Each address is represented by a 32-bit value and
is byte addressable.  The address may be used to reference a
single byte, half-word (2-bytes), or word (4 bytes).

@c
@c
@c

@section Interrupt Processing

Although RTEMS hides many of the processor
dependent details of interrupt processing, it is important to
understand how the RTEMS interrupt manager is mapped onto the
processor's unique architecture. Discussed in this chapter are
the the processor's response and control mechanisms as they
pertain to RTEMS.

@subsection Vectoring of Interrupt Handler

Although the i386 supports multiple privilege levels,
RTEMS and all user software executes at privilege level 0.  This
decision was made by the RTEMS designers to enhance
compatibility with processors which do not provide sophisticated
protection facilities like those of the i386.  This decision
greatly simplifies the discussion of i386 processing, as one
need only consider interrupts without privilege transitions.

Upon receipt of an interrupt  the i386 automatically
performs the following actions:

@itemize @bullet
@item pushes the EFLAGS register

@item pushes the far address of the interrupted instruction

@item vectors to the interrupt service routine (ISR).
@end itemize

A nested interrupt is processed similarly by the
i386.

@subsection Interrupt Stack Frame

The structure of the Interrupt Stack Frame for the
i386 which is placed on the interrupt stack by the processor in
response to an interrupt is as follows:

@ifset use-ascii
@example
@group
               +----------------------+
               | Old EFLAGS Register  | ESP+8
               +----------+-----------+
               |   UNUSED |  Old CS   | ESP+4
               +----------+-----------+
               |       Old EIP        | ESP
               +----------------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\strut\vrule#&
\hbox to 1.00in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.00in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}
\cr
\multispan{4}\hrulefill\cr
& \multispan{3} Old EFLAGS Register\quad&&ESP+8\cr
\multispan{4}\hrulefill\cr
&UNUSED &&Old CS &&ESP+4\cr
\multispan{4}\hrulefill\cr
& \multispan{3} Old EIP && ESP\cr
\multispan{4}\hrulefill\cr
}}\hfil}
@end tex
@end ifset
 
@ifset use-html
@html
<CENTER>
  <TABLE COLS=3 WIDTH="40%" BORDER=2>
<TR><TD ALIGN=center COLSPAN=2><STRONG>Old EFLAGS Register</STRONG></TD>
    <TD ALIGN=center>0x0</TD></TR>
<TR><TD ALIGN=center><STRONG>UNUSED</STRONG></TD>
    <TD ALIGN=center><STRONG>Old CS</STRONG></TD>
    <TD ALIGN=center>0x2</TD></TR>
<TR><TD ALIGN=center COLSPAN=2><STRONG>Old EIP</STRONG></TD>
    <TD ALIGN=center>0x4</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@subsection Interrupt Levels

Although RTEMS supports 256 interrupt levels, the
i386 only supports two -- enabled and disabled.  Interrupts are
enabled when the interrupt-enable flag (IF) in the extended
flags (EFLAGS) is set.  Conversely, interrupt processing is
inhibited when the IF is cleared.  During a non-maskable
interrupt, all other interrupts, including other non-maskable
ones, are inhibited.

RTEMS interrupt levels 0 and 1 such that level zero
(0) indicates that interrupts are fully enabled and level one
that interrupts are disabled.  All other RTEMS interrupt levels
are undefined and their behavior is unpredictable.

@subsection Interrupt Stack

The i386 family does not support a dedicated hardware
interrupt stack.  On this processor, RTEMS allocates and manages
a dedicated interrupt stack.  As part of vectoring a non-nested
interrupt service routine, RTEMS switches from the stack of the
interrupted task to a dedicated interrupt stack.  When a
non-nested interrupt returns, RTEMS switches back to the stack
of the interrupted stack.  The current stack pointer is not
altered by RTEMS on nested interrupt.

@c
@c
@c

@section Default Fatal Error Processing

The default fatal error handler for this architecture disables processor
interrupts, places the error code in EAX, and executes a HLT instruction
to halt the processor.

@c
@c
@c

@section Board Support Packages

@subsection System Reset

An RTEMS based application is initiated when the i386 processor is reset.
When the i386 is reset,

@itemize @bullet

@item The EAX register is set to indicate the results of the processor's
power-up self test.  If the self-test was not executed, the contents of
this register are undefined.  Otherwise, a non-zero value indicates the
processor is faulty and a zero value indicates a successful self-test.

@item The DX register holds a component identifier and revision level.  DH
contains 3 to indicate an i386 component and DL contains a unique revision
level indicator. 

@item Control register zero (CR0) is set such that the processor is in real
mode with paging disabled.  Other portions of CR0 are used to indicate the
presence of a numeric coprocessor. 

@item All bits in the extended flags register (EFLAG) which are not
permanently set are cleared.  This inhibits all maskable interrupts. 

@item The Interrupt Descriptor Register (IDTR) is set to point at address
zero. 

@item All segment registers are set to zero. 

@item The instruction pointer is set to 0x0000FFF0.  The first instruction
executed after a reset is actually at 0xFFFFFFF0 because the i386 asserts
the upper twelve address until the first intersegment (FAR) JMP or CALL
instruction.  When a JMP or CALL is executed, the upper twelve address
lines are lowered and the processor begins executing in the first megabyte
of memory.

@end itemize

Typically, an intersegment JMP to the application's initialization code is
placed at address 0xFFFFFFF0. 

@subsection Processor Initialization

This initialization code is responsible for initializing all data
structures required by the i386 in protected mode and for actually entering
protected mode.  The i386 must be placed in protected mode and the segment
registers and associated selectors must be initialized before the
initialize_executive directive is invoked. 

The initialization code is responsible for initializing the Global
Descriptor Table such that the i386 is in the thirty-two bit flat memory
model with paging disabled.  In this mode, the i386 automatically converts
every address from a logical to a physical address each time it is used. 
For more information on the memory model used by RTEMS, please refer to the
Memory Model chapter in this document. 

Since the processor is in real mode upon reset, the processor must be
switched to protected mode before RTEMS can execute.  Before switching to
protected mode, at least one descriptor table and two descriptors must be
created.  Descriptors are needed for a code segment and a data segment. (
This will give you the flat memory model.)  The stack can be placed in a
normal read/write data segment, so no descriptor for the stack is needed.
Before the GDT can be used, the base address and limit must be loaded into
the GDTR register using an LGDT instruction. 

If the hardware allows an NMI to be generated, you need to create the IDT
and a gate for the NMI interrupt handler.  Before the IDT can be used, the
base address and limit for the idt must be loaded into the IDTR register
using an LIDT instruction. 

Protected mode is entered by setting thye PE bit in the CR0 register. 
Either a LMSW or MOV CR0 instruction may be used to set this bit. Because
the processor overlaps the interpretation of several instructions, it is
necessary to discard the instructions from the read-ahead cache. A JMP
instruction immediately after the LMSW changes the flow and empties the
processor if intructions which have been pre-fetched and/or decoded.  At
this point, the processor is in protected mode and begins to perform
protected mode application initialization.

If the application requires that the IDTR be some value besides zero, then
it should set it to the required value at this point.  All tasks share the
same i386 IDTR value.  Because interrupts are enabled automatically by
RTEMS as part of the initialize_executive directive, the IDTR MUST be set
properly before this directive is invoked to insure correct interrupt
vectoring.  If processor caching is to be utilized, then it should be
enabled during the reset application initialization code.  The reset code
which is executed before the call to initialize_executive has the following
requirements:

For more information regarding the i386 data structures and their
contents, refer to Intel's 386 Programmer's Reference Manual.

