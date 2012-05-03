@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@ifinfo
@end ifinfo
@chapter M68xxx and Coldfire Specific Information

This chapter discusses the Freescale (formerly Motorola) MC68xxx
and Coldfire architectural dependencies.  The MC68xxx family has a
wide variety of CPU models within it based upon different CPU core
implementations.  Ignoring the Coldfire parts, the part numbers for
these models are generally divided into MC680xx and MC683xx.  The MC680xx
models are more general purpose processors with no integrated peripherals.
The MC683xx models, on the other hand, are more specialized and have a
variety of peripherals on chip including sophisticated timers and serial
communications controllers.

@subheading Architecture Documents

For information on the MC68xxx and Coldfire architecture, refer to the following documents available from Freescale website (@file{http//www.freescale.com/}):

@itemize @bullet
@item @cite{M68000 Family Reference, Motorola, FR68K/D}.
@item @cite{MC68020 User's Manual, Motorola, MC68020UM/AD}.
@item @cite{MC68881/MC68882 Floating-Point Coprocessor User's Manual,
Motorola, MC68881UM/AD}.
@end itemize

@c
@c
@c
@section CPU Model Dependent Features


This section presents the set of features which vary
across m68k/Coldfire implementations that are of importance to RTEMS.
The set of CPU model feature macros are defined in the file
@code{cpukit/score/cpu/m68k/m68k.h} based upon the particular CPU
model selected on the compilation command line.

@subsection BFFFO Instruction

The macro @code{M68K_HAS_BFFFO} is set to 1 to indicate that
this CPU model has the bfffo instruction.

@subsection Vector Base Register

The macro @code{M68K_HAS_VBR} is set to 1 to indicate that
this CPU model has a vector base register (vbr).

@subsection Separate Stacks

The macro @code{M68K_HAS_SEPARATE_STACKS} is set to 1 to
indicate that this CPU model has separate interrupt, user, and
supervisor mode stacks.

@subsection Pre-Indexing Address Mode

The macro @code{M68K_HAS_PREINDEXING} is set to 1 to indicate that
this CPU model has the pre-indexing address mode.

@subsection Extend Byte to Long Instruction
 
The macro @code{M68K_HAS_EXTB_L} is set to 1 to indicate that this CPU model 
has the extb.l instruction.  This instruction is supposed to be available
in all models based on the cpu32 core as well as mc68020 and up models.

@c
@c
@c
@section Calling Conventions

The MC68xxx architecture supports a simple yet effective call and
return mechanism.  A subroutine is invoked via the branch to subroutine
(@code{bsr}) or the jump to subroutine (@code{jsr}) instructions.
These instructions push the return address on the current stack.
The return from subroutine (@code{rts}) instruction pops the return
address off the current stack and transfers control to that instruction.
It is is important to note that the MC68xxx call and return mechanism does
not automatically save or restore any registers.  It is the responsibility
of the high-level language compiler to define the register preservation
and usage convention.

@subsection Calling Mechanism

All RTEMS directives are invoked using either a @code{bsr} or @code{jsr}
instruction and return to the user application via the rts instruction.

@subsection Register Usage

As discussed above, the @code{bsr} and @code{jsr} instructions do not
automatically save any registers.  RTEMS uses the registers D0, D1,
A0, and A1 as scratch registers.  These registers are not preserved by
RTEMS directives therefore, the contents of these registers should not
be assumed upon return from any RTEMS directive.

@subsection Parameter Passing

RTEMS assumes that arguments are placed on the current stack before
the directive is invoked via the bsr or jsr instruction.  The first
argument is assumed to be closest to the return address on the stack.
This means that the first argument of the C calling sequence is pushed
last.  The following pseudo-code illustrates the typical sequence used
to call a RTEMS directive with three (3) arguments:

@example
@group
push third argument
push second argument
push first argument
invoke directive
remove arguments from the stack
@end group
@end example

The arguments to RTEMS are typically pushed onto the stack using a move
instruction with a pre-decremented stack pointer as the destination.
These arguments must be removed from the stack after control is returned
to the caller.  This removal is typically accomplished by adding the
size of the argument list in bytes to the current stack pointer.

@c
@c
@c
@section Memory Model

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
@c
@c
@section Interrupt Processing

Discussed in this section are the MC68xxx's interrupt response and
control mechanisms as they pertain to RTEMS.

@subsection Vectoring of an Interrupt Handler

Depending on whether or not the particular CPU supports a separate
interrupt stack, the MC68xxx family has two different interrupt handling
models.

@subsubsection Models Without Separate Interrupt Stacks

Upon receipt of an interrupt the MC68xxx family members without separate
interrupt stacks automatically perform the following actions:

@itemize @bullet
@item To Be Written
@end itemize

@subsubsection Models With Separate Interrupt Stacks

Upon receipt of an interrupt the MC68xxx family members with separate
interrupt stacks automatically perform the following actions:

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

I think somebody on this list asked about the interupt vector handling
w/o VBR and RAM at 0.  The usual trick is to initialise the vector table
(except the first 2 two entries, of course) to point to the same location
BUT you also add the vector number times 0x1000000 to them. That is,
bits 31-24 contain the vector number and 23-0 the address of the common
handler.  Since the PC is 32 bit wide but the actual address bus is only
24, the top byte will be in the PC but will be ignored when jumping onto
your routine.

Then your common interrupt routine gets this info by loading the PC
into some register and based on that info, you can jump to a vector in
a vector table pointed by a virtual VBR:

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

@c
@c
@c
@section Default Fatal Error Processing

The default fatal error handler for this architecture disables processor
interrupts to level 7, places the error code in D0, and executes a
@code{stop} instruction to simulate a halt processor instruction.

@c
@c
@c

@section Board Support Packages

@subsection System Reset

An RTEMS based application is initiated or re-initiated when the MC68020
processor is reset.  When the MC68020 is reset, the processor performs
the following actions:

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

The address of the application's initialization code should be stored in
the first vector of the EVT which will allow the immediate vectoring to
the application code.  If the application requires that the VBR be some
value besides zero, then it should be set to the required value at this
point.  All tasks share the same MC68020's VBR value.  Because interrupts
are enabled automatically by RTEMS as part of the context switch to the
first task, the VBR MUST be set by either RTEMS of the BSP before this
occurs ensure correct interrupt vectoring.  If processor caching is
to be utilized, then it should be enabled during the reset application
initialization code.

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

