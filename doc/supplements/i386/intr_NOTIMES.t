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

@section Vectoring of Interrupt Handler

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

@section Interrupt Stack Frame

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

@section Interrupt Levels

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

@section Disabling of Interrupts by RTEMS

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

@section Interrupt Stack

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

