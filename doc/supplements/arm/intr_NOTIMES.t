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
unique architecture. Discussed in this chapter are the ARM's
interrupt response and control mechanisms as they pertain to
RTEMS.

The ARM has 7 exception types:
@itemize @bullet

@item Reset
@item Undefined instruction
@item Software interrupt (SWI)
@item Prefetch Abort
@item Data Abort
@item Interrupt (IRQ)
@item Fast Interrupt (FIQ)

@end itemize

Of these types, only IRQ and FIQ are handled through RTEMS's interrupt
vectoring.

@section Vectoring of an Interrupt Handler


Unlike many other architectures, the ARM has seperate stacks for each
interrupt. When the CPU receives an interrupt, it:

@itemize @bullet
@item switches to the exception mode corresponding to the interrupt,

@item saves the Current Processor Status Register (CPSR) to the
exception mode's Saved Processor Status Register (SPSR),

@item masks off the IRQ and if the interrupt source was FIQ, the FIQ
is masked off as well,

@item saves the Program Counter (PC) to the exception mode's Link
Register (LR - same as R14),
 
@item and sets the PC to the exception's vector address.

@end itemize

The vectors for both IRQ and FIQ point to the _ISR_Handler function. 
_ISR_Handler() calls the BSP specific handler, ExecuteITHandler(). Before
calling ExecuteITHandler(), registers R0-R3, R12, and R14(LR) are saved so 
that it is safe to call C functions. Even ExecuteITHandler() can be written
in C.

@section Interrupt Levels

The ARM architecture supports two external interrupts - IRQ and FIQ. FIQ 
has a higher priority than IRQ, and has its own version of register R8 - R14,
however RTEMS does not take advantage of them. Both interrupts are enabled
through the CPSR.

The RTEMS interrupt level mapping scheme for the AEM is not a numeric level
as on most RTEMS ports. It is a bit mapping that corresponds the enable 
bits's postions in the CPSR:

@table @b
@item FIQ
Setting bit 6 (0 is least significant bit) disables the FIQ.

@item IRQ
Setting bit 7 (0 is least significant bit) disables the IRQ.
 
@end table
 

@section Disabling of Interrupts by RTEMS

During the execution of directive calls, critical
sections of code may be executed.  When these sections are
encountered, RTEMS disables interrupts to level seven (7) before
the execution of this section and restores them to the previous
level upon completion of the section.  RTEMS has been optimized
to insure that interrupts are disabled for less than 
RTEMS_MAXIMUM_DISABLE_PERIOD microseconds on a 
RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ Mhz processor with 
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

RTEMS expects the interrupt stacks to be set up in bsp_start(). The memory
for the stacks is reserved in the linker script. 

