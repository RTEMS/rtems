@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@end ifinfo
@chapter Motorola M68xxx and Coldfire Specific Information

The Real Time Executive for Multiprocessor Systems (RTEMS)
is designed to be portable across multiple processor
architectures.  However, the nature of real-time systems makes
it essential that the application designer understand certain
processor dependent implementation details.  These processor
dependencies include calling convention, board support package
issues, interrupt processing, exact RTEMS memory requirements,
performance data, header files, and the assembly language
interface to the executive.

This document discusses the Motorola MC68xxx
architecture dependencies in this port of RTEMS.  The MC68xxx
family has a wide variety of CPU models within it.  The part
numbers for these models are generally divided into MC680xx and
MC683xx.  The MC680xx models are more general purpose processors
with no integrated peripherals.  The MC683xx models, on the
other hand, are more specialized and have a variety of
peripherals on chip including sophisticated timers and serial
communications controllers.

It is highly recommended that the Motorola MC68xxx
RTEMS application developer obtain and become familiar with the
documentation for the processor being used as well as the
documentation for the family as a whole.

@subheading Architecture Documents

For information on the Motorola MC68xxx architecture,
refer to the following documents available from Motorola
(@file{http//www.moto.com/}):

@itemize @bullet
@item @cite{M68000 Family Reference, Motorola, FR68K/D}.
@end itemize

@subheading MODEL SPECIFIC DOCUMENTS

For information on specific processor models and
their associated coprocessors, refer to the following documents:

@itemize  @bullet
@item @cite{MC68020 User's Manual, Motorola, MC68020UM/AD}.

@item @cite{MC68881/MC68882 Floating-Point Coprocessor User's
Manual, Motorola, MC68881UM/AD}.
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
cpukit/score/cpu/m68k/m68k.h based upon the particular CPU
model defined on the compilation command line.

@subsection CPU Model Name

The macro CPU_MODEL_NAME is a string which designates
the name of this CPU model.  For example, for the MC68020
processor, this macro is set to the string "mc68020".

@subsection Floating Point Unit

The macro M68K_HAS_FPU is set to 1 to indicate that
this CPU model has a hardware floating point unit and 0
otherwise.  It does not matter whether the hardware floating
point support is incorporated on-chip or is an external
coprocessor.

@subsection BFFFO Instruction

The macro M68K_HAS_BFFFO is set to 1 to indicate that
this CPU model has the bfffo instruction.

@subsection Vector Base Register

The macro M68K_HAS_VBR is set to 1 to indicate that
this CPU model has a vector base register (vbr).

@subsection Separate Stacks

The macro M68K_HAS_SEPARATE_STACKS is set to 1 to
indicate that this CPU model has separate interrupt, user, and
supervisor mode stacks.

@subsection Pre-Indexing Address Mode

The macro M68K_HAS_PREINDEXING is set to 1 to indicate that
this CPU model has the pre-indexing address mode.

@subsection Extend Byte to Long Instruction
 
The macro M68K_HAS_EXTB_L is set to 1 to indicate that this CPU model 
has the extb.l instruction.  This instruction is supposed to be available
in all models based on the cpu32 core as well as mc68020 and up models.
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

The MC68xxx architecture supports a simple yet
effective call and return mechanism.  A subroutine is invoked
via the branch to subroutine (bsr) or the jump to subroutine
(jsr) instructions.  These instructions push the return address
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

The MC68xxx family supports a flat 32-bit address
space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes).  Each address is represented by a 32-bit value and
is byte addressable.  The address may be used to reference a
single byte, word (2-bytes), or long word (4 bytes).  Memory
accesses within this address space are performed in big endian
fashion by the processors in this family.

Some of the MC68xxx family members such as the
MC68020, MC68030, and MC68040 support virtual memory and
segmentation.  The MC68020 requires external hardware support
such as the MC68851 Paged Memory Management Unit coprocessor
which is typically used to perform address translations for
these systems.  RTEMS does not support virtual memory or
segmentation on any of the MC68xxx family members.

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
unique architecture. Discussed in this chapter are the MC68xxx's
interrupt response and control mechanisms as they pertain to
RTEMS.

@subsection Vectoring of an Interrupt Handler

Depending on whether or not the particular CPU
supports a separate interrupt stack, the MC68xxx family has two
different interrupt handling models.

@subsubsection Models Without Separate Interrupt Stacks

Upon receipt of an interrupt the MC68xxx family
members without separate interrupt stacks automatically perform
the following actions:

@itemize @bullet
@item To Be Written
@end itemize

@subsubsection Models With Separate Interrupt Stacks

Upon receipt of an interrupt the MC68xxx family
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
MC68xxx CPU models with separate interrupt stacks:

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

@subsection CPU Models Without VBR and RAM at 0

This is from a post by Zoltan Kocsi <zoltan@@bendor.com.au> and is
a nice trick in certain situations.  In his words:

I think somebody on this list asked about the interupt vector 
handling w/o VBR and RAM at 0.  The usual trick is 
to initialise the vector table (except the first 2 two entries, of 
course) to point to the same location BUT you also add the vector 
number times 0x1000000 to them. That is, bits 31-24 contain the vector 
number and 23-0 the address of the common handler.
Since the PC is 32 bit wide but the actual address bus is only 24,
the top byte will be in the PC but will be ignored when jumping
onto your routine.

Then your common interrupt routine gets this info by loading the PC 
into some register and based on that info, you can jump to a vector
in a vector table pointed by a virtual VBR:

@example
//
//  Real vector table at 0
//
 
    .long   initial_sp
    .long   initial_pc
    .long   myhandler+0x02000000
    .long   myhandler+0x03000000
    .long   myhandler+0x04000000
    ...
    .long   myhandler+0xff000000
   
   
//
// This handler will jump to the interrupt routine   of which
// the address is stored at VBR[ vector_no ]
// The registers and stackframe will be intact, the interrupt
// routine will see exactly what it would see if it was called
// directly from the HW vector table at 0.
//

    .comm    VBR,4,2        // This defines the 'virtual' VBR
                            // From C: extern void *VBR;

myhandler:                  // At entry, PC contains the full vector
    move.l  %d0,-(%sp)      // Save d0
    move.l  %a0,-(%sp)      // Save a0
    lea     0(%pc),%a0      // Get the value of the PC
    move.l  %a0,%d0         // Copy it to a data reg, d0 is VV??????
    swap    %d0             // Now d0 is ????VV??
    and.w   #0xff00,%d0     // Now d0 is ????VV00 (1)
    lsr.w   #6,%d0          // Now d0.w contains the VBR table offset
    move.l  VBR,%a0         // Get the address from VBR to a0
    move.l  (%a0,%d0.w),%a0 // Fetch the vector
    move.l  4(%sp),%d0      // Restore d0
    move.l  %a0,4(%sp)      // Place target address to the stack
    move.l  (%sp)+,%a0      // Restore a0, target address is on TOS 
    ret                     // This will jump to the handler and
                            // restore the stack

(1) If 'myhandler' is guaranteed to be in the first 64K, e.g. just
    after the vector table then that insn is not needed.

@end example

There are probably shorter ways to do this, but it I believe is enough 
to illustrate the trick. Optimisation is left as an exercise to the 
reader :-) 


@subsection Interrupt Levels

Eight levels (0-7) of interrupt priorities are
supported by MC68xxx family members with level seven (7) being
the highest priority.  Level zero (0) indicates that interrupts
are fully enabled.  Interrupt requests for interrupts with
priorities less than or equal to the current interrupt mask
level are ignored.

Although RTEMS supports 256 interrupt levels, the
MC68xxx family only supports eight.  RTEMS interrupt levels 0
through 7 directly correspond to MC68xxx interrupt levels.  All
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
RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ Mhz MC68020 with 
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

The MC68xxx port of RTEMS supports a software managed
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
the fatal_error_occurred directive when there is no user handler
configured or the user handler returns control to RTEMS.  The
default fatal error handler disables processor interrupts to
level 7, places the error code in D0, and executes a stop
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
This chapter presents a discussion of MC68020 specific BSP
issues.   For more information on developing a BSP, refer to the
chapter titled Board Support Packages in the RTEMS
Applications User's Guide.

@subsection System Reset

An RTEMS based application is initiated or
re-initiated when the MC68020 processor is reset.  When the
MC68020 is reset, the processor performs the following actions:

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
tasks share the same MC68020's VBR value.  Because interrupts
are enabled automatically by RTEMS as part of the initialize
executive directive, the VBR MUST be set before this directive
is invoked to insure correct interrupt vectoring.  If processor
caching is to be utilized, then it should be enabled during the
reset application initialization code.

In addition to the requirements described in the
Board Support Packages chapter of the Applications User's
Manual for the reset code which is executed before the call to
initialize executive, the MC68020 version has the following
specific requirements:

@itemize @bullet
@item Must leave the S bit of the status register set so that
the MC68020 remains in the supervisor state.

@item Must set the M bit of the status register to remove the
MC68020 from the interrupt state.

@item Must set the master stack pointer (MSP) such that a
minimum stack size of MINIMUM_STACK_SIZE bytes is provided for
the initialize executive directive.

@item Must initialize the MC68020's vector table.
@end itemize

Note that the BSP is not responsible for allocating
or installing the interrupt stack.  RTEMS does this
automatically as part of initialization.  If the BSP does not
install an interrupt stack and -- for whatever reason -- an
interrupt occurs before initialize_executive is invoked, then
the results are unpredictable.

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

The MC68xxx version of the RTEMS CPU Dependent
Information Table contains the information required to interface
a Board Support Package and RTEMS on the MC68xxx.  This
information is provided to allow RTEMS to interoperate
effectively with the BSP.  The C structure definition is given
here:

@example
@group
typedef struct @{
  void       (*pretasking_hook)( void );
  void       (*predriver_hook)( void );
  void       (*postdriver_hook)( void );
  void       (*idle_task)( void );
  boolean      do_zero_of_workspace;
  unsigned32   idle_task_stack_size;
  unsigned32   interrupt_stack_size;
  unsigned32   extra_mpci_receive_server_stack;
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

@item interrupt_vector_table
is the base address of the CPU's Exception Vector Table.

@end table
