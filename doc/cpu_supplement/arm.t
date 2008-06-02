@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@end ifinfo
@chapter ARM Specific Information

This chapter discusses the ARM architecture dependencies
in this port of RTEMS.  The ARM family has a wide variety
of implementations by a wide range of vendors.  Consequently,
there are many, many CPU models within it.

@subheading Architecture Documents

For information on the ARM architecture, refer to the following documents
available from Arm, Limited (@file{http//www.arm.com/}).  There does
not appear to be an electronic version of a manual on the architecture
in general on that site.  The following book is a good resource:

@itemize @bullet
@item @cite{David Seal. "ARM Architecture Reference Manual."
Addison-Wesley. @b{ISBN 0-201-73719-1}. 2001.}

@end itemize


@c
@c
@c

@section CPU Model Dependent Features

This section presents the set of features which vary
across ARM implementations and are of importance to RTEMS.
The set of CPU model feature macros are defined in the file
@code{cpukit/score/cpu/arm/rtems/score/arm.h} based upon the particular CPU
model flags specified on the compilation command line.

@subsection CPU Model Name

The macro @code{CPU_MODEL_NAME} is a string which designates
the architectural level of this CPU model.  The following is
a list of the settings for this string based upon @code{gcc}
CPU model predefines:

@example
__ARM_ARCH4__   "ARMv4"
__ARM_ARCH4T__  "ARMv4T"
__ARM_ARCH5__   "ARMv5"
__ARM_ARCH5T__  "ARMv5T"
__ARM_ARCH5E__  "ARMv5E"
__ARM_ARCH5TE__ "ARMv5TE"
@end example

@subsection Count Leading Zeroes Instruction

The ARMv5 and later has the count leading zeroes (@code{clz})
instruction which could be used to speed up the find first bit
operation.  The use of this instruction should significantly speed up
the scheduling associated with a thread blocking.

@subsection Floating Point Unit

The macro ARM_HAS_FPU is set to 1 to indicate that
this CPU model has a hardware floating point unit and 0
otherwise.  It does not matter whether the hardware floating
point support is incorporated on-chip or is an external
coprocessor.

@c
@c
@c
@section Calling Conventions

The ARM architecture supports a simple yet effective call and
return mechanism.  A subroutine is invoked via the branch and link
(@code{bl}) instruction.  This instruction saves the return address
in the @code{lr} register.  Returning from a subroutine only requires
that the return address be moved into the program counter (@code{pc}),
possibly with an offset.  It is is important to note that the @code{bl}
instruction does not automatically save or restore any registers.
It is the responsibility of the high-level language compiler to define
the register preservation and usage convention.

@subsection Calling Mechanism

All RTEMS directives are invoked using the @code{bl} instruction and
return to the user application via the mechanism described above.

@subsection Register Usage

As discussed above, the ARM's call and return mechanism dos
not automatically save any registers.  RTEMS uses the registers
@code{r0}, @code{r1}, @code{r2}, and @code{r3} as scratch registers and
per ARM calling convention, the @code{lr} register is altered
as well.  These registers are not preserved by RTEMS directives
therefore, the contents of these registers should not be assumed
upon return from any RTEMS directive. 

@subsection Parameter Passing

RTEMS assumes that ARM calling conventions are followed and that
the first four arguments are placed in registers @code{r0} through
@code{r3}.  If there are more arguments, than that, then they
are place on the stack.

@c
@c
@c

@section Memory Model

@subsection Flat Memory Model

Members of the ARM family newer than Version 3 support a flat 32-bit
address space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes).  Each address is represented by a 32-bit value and is byte
addressable.  The address may be used to reference a single byte, word
(2-bytes), or long word (4 bytes).  Memory accesses within this address
space are performed in the endian mode that the processor is configured
for.   In general, ARM processors are used in little endian mode.

Some of the ARM family members such as the 920 and 720 include an MMU
and thus support virtual memory and segmentation.  RTEMS does not support
virtual memory or segmentation on any of the ARM family members.

@c
@c
@c
@section Interrupt Processing

Although RTEMS hides many of the processor dependent
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

@subsection Vectoring of an Interrupt Handler

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

@subsection Interrupt Levels

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
 
@subsection Interrupt Stack

RTEMS expects the interrupt stacks to be set up in bsp_start(). The memory
for the stacks is reserved in the linker script. 

@c
@c
@c
@section Default Fatal Error Processing

The default fatal error handler for this architecture performs the
following actions:

@itemize @bullet
@item disables processor interrupts,
@item places the error code in @b{r0}, and
@item executes an infinite loop (@code{while(0);} to
simulate a halt processor instruction.
@end itemize

@c
@c
@c
@section Board Support Packages

@subsection System Reset

An RTEMS based application is initiated or re-initiated when the processor
is reset.  When the processor is reset, the processor performs the
following actions:

@itemize @bullet
@item TBD

@end itemize

@subsection Processor Initialization

TBD
