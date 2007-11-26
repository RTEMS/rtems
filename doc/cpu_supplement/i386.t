@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@end ifinfo
@chapter Intel/AMD x86 Specific Information

The Real Time Executive for Multiprocessor Systems
(RTEMS) is designed to be portable across multiple processor
architectures.  However, the nature of real-time systems makes
it essential that the application designer understand certain
processor dependent implementation details.  These processor
dependencies include calling convention, board support package
issues, interrupt processing, exact RTEMS memory requirements,
performance data, header files, and the assembly language
interface to the executive.

For information on the i386 processor, refer to the
following documents:

@itemize @bullet
@item @cite{386 Programmer's Reference Manual, Intel, Order No.  230985-002}.

@item @cite{386 Microprocessor Hardware Reference Manual, Intel,
Order No. 231732-003}.

@item @cite{80386 System Software Writer's Guide, Intel, Order No.  231499-001}.

@item @cite{80387 Programmer's Reference Manual, Intel, Order No.  231917-001}.
@end itemize

It is highly recommended that the i386 RTEMS
application developer obtain and become familiar with Intel's
386 Programmer's Reference Manual.

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
across i386 implementations and are of importance to RTEMS.
The set of CPU model feature macros are defined in the file
cpukit/score/cpu/i386/i386.h based upon the particular CPU
model defined on the compilation command line.

@subsection CPU Model Name

The macro CPU_MODEL_NAME is a string which designates
the name of this CPU model.  For example, for the Intel i386 without an
i387 coprocessor, this macro is set to the string "i386 with i387".

@subsection bswap Instruction

The macro I386_HAS_BSWAP is set to 1 to indicate that
this CPU model has the @code{bswap} instruction which
endian swaps a thirty-two bit quantity.  This instruction
appears to be present in all CPU models
i486's and above.

@subsection Floating Point Unit

The macro I386_HAS_FPU is set to 1 to indicate that
this CPU model has a hardware floating point unit and 0
otherwise.  The hardware floating point may be on-chip (as in the
case of an i486DX or Pentium) or as a coprocessor (as in the case of
an i386/i387 combination).
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

@subsection Processor Background

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

@subsection Calling Mechanism

All RTEMS directives are invoked using a call
instruction and return to the user application via the ret
instruction.

@subsection Register Usage

As discussed above, the call instruction does not
automatically save any registers.  RTEMS uses the registers EAX,
ECX, and EDX as scratch registers.  These registers are not
preserved by RTEMS directives therefore, the contents of these
registers should not be assumed upon return from any RTEMS
directive.

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

RTEMS does not require that logical addresses map
directly to physical addresses, although it is desirable in many
applications to do so.  If logical and physical addresses are
not the same, then an additional selector will be required so
RTEMS can access the Interrupt Descriptor Table to install
interrupt service routines.  The selector number of this segment
is provided to RTEMS in the CPU Dependent Information Table.

By not requiring that logical addresses map directly
to physical addresses, the memory space of an RTEMS application
can be separated from that of a ROM monitor.  For example, on
the Force Computers CPU386, the ROM monitor loads application
programs into a logical address space where logical address
0x00000000 corresponds to physical address 0x0002000.  On this
board, RTEMS and the application use virtual addresses which do
not map to physical addresses.

RTEMS assumes that the DS and ES registers contain
the selector for the single data segment when a directive is
invoked.   This assumption is especially important when
developing interrupt service routines.

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Interrupt Processing


Different types of processors respond to the
occurrence of an interrupt in their own unique fashion. In
addition, each processor type provides a control mechanism to
allow the proper handling of an interrupt.  The processor
dependent response to the interrupt modifies the execution state
and results in the modification of the execution stream. This
modification usually requires that an interrupt handler utilize
the provided control mechanisms to return to the normal
processing stream.  Although RTEMS hides many of the processor
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

@subsection Disabling of Interrupts by RTEMS

During the execution of directive calls, critical
sections of code may be executed.  When these sections are
encountered, RTEMS disables interrupts before the execution of
this section and restores them to the previous level upon
completion of the section.  RTEMS has been optimized to insure
that interrupts are disabled for less than RTEMS_MAXIMUM_DISABLE_PERIOD
microseconds on a RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ Mhz i386 with zero
wait states.  These numbers will vary based the number of wait states
and processor speed present on the target board.   [NOTE:  The maximum
period with interrupts disabled within RTEMS was last calculated for
Release RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

Non-maskable interrupts (NMI) cannot be disabled, and
ISRs which execute at this level MUST NEVER issue RTEMS system
calls.  If a directive is invoked, unpredictable results may
occur due to the inability of RTEMS to protect its critical
sections.  However, ISRs that make no system calls may safely
execute as non-maskable interrupts.

@subsection Interrupt Stack

The i386 family does not support a dedicated hardware
interrupt stack.  On this processor, RTEMS allocates and manages
a dedicated interrupt stack.  As part of vectoring a non-nested
interrupt service routine, RTEMS switches from the stack of the
interrupted task to a dedicated interrupt stack.  When a
non-nested interrupt returns, RTEMS switches back to the stack
of the interrupted stack.  The current stack pointer is not
altered by RTEMS on nested interrupt.

Without a dedicated interrupt stack, every task in
the system MUST have enough stack space to accommodate the worst
case stack usage of that particular task and the interrupt
service routines COMBINED.  By supporting a dedicated interrupt
stack, RTEMS significantly lowers the stack requirements for
each task.

RTEMS allocates the dedicated interrupt stack from
the Workspace Area.  The amount of memory allocated for the
interrupt stack is determined by the interrupt_stack_size field
in the CPU Configuration Table.

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
the fatal_error_occurred directive when there is no user handler
configured or the user handler returns control to RTEMS.  The
default fatal error handler disables processor interrupts,
places the error code in EAX, and executes a HLT instruction to
halt the processor.

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Board Support Packages


An RTEMS Board Support Package (BSP) must be designed to support a
particular processor and target board combination.  This chapter presents a
discussion of i386 specific BSP issues.  For more information on developing
a BSP, refer to the chapter titled Board Support Packages in the RTEMS
Applications User's Guide. 

@subsection System Reset

An RTEMS based application is initiated when the i386
processor is reset.  When the i386 is reset,

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

For more information regarding the i386s data structures and their
contents, refer to Intel's 386 Programmer's Reference Manual.

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Processor Dependent Information Table


Any highly processor dependent information required
to describe a processor to RTEMS is provided in the CPU
Dependent Information Table.  This table is not required for all
processors supported by RTEMS.  This chapter describes the
contents, if any, for a particular processor type.

@subsection CPU Dependent Information Table

The i386 version of the RTEMS CPU Dependent
Information Table contains the information required to interface
a Board Support Package and RTEMS on the i386.  This information
is provided to allow RTEMS to interoperate effectively with the
BSP.  The C structure definition is given here:

@example
@group
typedef struct @{
  void       (*pretasking_hook)( void );
  void       (*predriver_hook)( void );
  void       (*idle_task)( void );
  boolean      do_zero_of_workspace;
  uint32_t     idle_task_stack_size;
  uint32_t     interrupt_stack_size;
  uint32_t     extra_mpci_receive_server_stack;
  void *     (*stack_allocate_hook)( unsigned32 );
  void       (*stack_free_hook)( void* );
  /* end of fields required on all CPUs */
 
@} rtems_cpu_table;
@end group
@end example

@table @code
@item pretasking_hook
is the address of the user provided routine which is invoked
once RTEMS APIs are initialized.  This routine will be invoked
before any system tasks are created.  Interrupts are disabled.
This field may be NULL to indicate that the hook is not utilized.

@item predriver_hook
is the address of the user provided
routine that is invoked immediately before the
the device drivers and MPCI are initialized. RTEMS
initialization is complete but interrupts and tasking are disabled.
This field may be NULL to indicate that the hook is not utilized.

@item postdriver_hook
is the address of the user provided
routine that is invoked immediately after the
the device drivers and MPCI are initialized. RTEMS
initialization is complete but interrupts and tasking are disabled.
This field may be NULL to indicate that the hook is not utilized.

@item idle_task
is the address of the optional user
provided routine which is used as the system's IDLE task.  If
this field is not NULL, then the RTEMS default IDLE task is not
used.  This field may be NULL to indicate that the default IDLE
is to be used.

@item do_zero_of_workspace
indicates whether RTEMS should
zero the Workspace as part of its initialization.  If set to
TRUE, the Workspace is zeroed.  Otherwise, it is not.

@item idle_task_stack_size
is the size of the RTEMS idle task stack in bytes.  
If this number is less than MINIMUM_STACK_SIZE, then the 
idle task's stack will be MINIMUM_STACK_SIZE in byte.

@item interrupt_stack_size
is the size of the RTEMS
allocated interrupt stack in bytes.  This value must be at least
as large as MINIMUM_STACK_SIZE.

@item extra_mpci_receive_server_stack
is the extra stack space allocated for the RTEMS MPCI receive server task
in bytes.  The MPCI receive server may invoke nearly all directives and 
may require extra stack space on some targets.

@item stack_allocate_hook
is the address of the optional user provided routine which allocates 
memory for task stacks.  If this hook is not NULL, then a stack_free_hook
must be provided as well.

@item stack_free_hook
is the address of the optional user provided routine which frees 
memory for task stacks.  If this hook is not NULL, then a stack_allocate_hook
must be provided as well.

@end table

The contents of the i386 Interrupt Descriptor Table
are discussed in  Intel's i386 User's Manual.  Structure
definitions for the i386 IDT is provided by including the file
rtems.h.

