@c
@c  Interrupt Stack Frame Picture
@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Interrupt Processing

@section Introduction

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

@section Vectoring of an Interrupt Handler

Depending on whether or not the particular CPU
supports a separate interrupt stack, the MC68xxx family has two
different interrupt handling models.

@subsection Models Without Separate Interrupt Stacks

Upon receipt of an interrupt the MC68xxx family
members without separate interrupt stacks automatically perform
the following actions:

@itemize @bullet
@item To Be Written
@end itemize

@subsection Models With Separate Interrupt Stacks

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

@section CPU Models Without VBR and RAM at 0

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


@section Interrupt Levels

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

@section Disabling of Interrupts by RTEMS

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

@section Interrupt Stack

RTEMS allocates the interrupt stack from the
Workspace Area.  The amount of memory allocated for the
interrupt stack is determined by the interrupt_stack_size field
in the CPU Configuration Table.  During the initialization
process, RTEMS will install its interrupt stack.

The MC68xxx port of RTEMS supports a software managed
dedicated interrupt stack on those CPU models which do not
support a separate interrupt stack in hardware.


