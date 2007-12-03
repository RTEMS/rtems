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

The Real Time Executive for Multiprocessor Systems (RTEMS)
is designed to be portable across multiple processor
architectures.  However, the nature of real-time systems makes
it essential that the application designer understand certain
processor dependent implementation details.  These processor
dependencies include calling convention, board support package
issues, interrupt processing, exact RTEMS memory requirements,
performance data, header files, and the assembly language
interface to the executive.

This document discusses the ARM architecture dependencies
in this port of RTEMS.  The ARM family has a wide variety
of implementations by a wide range of vendors.  Consequently,
there are 100's of CPU models within it.

It is highly recommended that the ARM
RTEMS application developer obtain and become familiar with the
documentation for the processor being used as well as the
documentation for the ARM architecture as a whole.

@subheading Architecture Documents

For information on the ARM architecture,
refer to the following documents available from Arm, Limited
(@file{http//www.arm.com/}).  There does not appear to
be an electronic version of a manual on the architecture
in general on that site.  The following book is a good 
resource:

@itemize @bullet
@item @cite{David Seal. "ARM Architecture Reference Manual."
Addison-Wesley. @b{ISBN 0-201-73719-1}. 2001.}

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
ARM, SPARC, and PowerPC are based on an architectural specification
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
across ARM implementations and are of importance to RTEMS.
The set of CPU model feature macros are defined in the file
cpukit/score/cpu/arm/rtems/score/arm.h based upon the particular CPU
model defined on the compilation command line.

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

The ARM architecture supports a simple yet
effective call and return mechanism.  A subroutine is invoked
via the branch and link (@code{bl}) instruction.  This instruction
saves the return address in the @code{lr} register.  Returning
from a subroutine only requires that the return address be
moved into the program counter (@code{pc}), possibly with
an offset.  It is is important to
note that the @code{bl} instruction does not
automatically save or restore any registers.  It is the
responsibility of the high-level language compiler to define the
register preservation and usage convention.

@subsection Calling Mechanism

All RTEMS directives are invoked using the @code{bl}
instruction and return to the user application via the
mechanism described above.

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

Members of the ARM family newer than Version 3 support a flat
32-bit address space with addresses ranging from 0x00000000 to
0xFFFFFFFF (4 gigabytes).  Each address is represented by a
32-bit value and is byte addressable.  
The address may be used to reference a
single byte, word (2-bytes), or long word (4 bytes).  Memory
accesses within this address space are performed in the endian
mode that the processor is configured for.   In general, ARM
processors are used in little endian mode.

Some of the ARM family members such as the 
920 and 720 include an MMU and thus support virtual memory and
segmentation.  RTEMS does not support virtual memory or
segmentation on any of the ARM family members.

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
 

@subsection Disabling of Interrupts by RTEMS

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

@subsection Interrupt Stack

RTEMS expects the interrupt stacks to be set up in bsp_start(). The memory
for the stacks is reserved in the linker script. 

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
RTEMS.  The default fatal error handler performs the
following actions:

@itemize @bullet
@item disables processor interrupts,
@item places the error code in @b{r0}, and
@item executes an infinite loop (@code{while(0);} to
simulate a halt processor instruction.
@end itemize

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

The XXX version of the RTEMS CPU Dependent
Information Table contains the information required to interface
a Board Support Package and RTEMS on the XXX.  This
information is provided to allow RTEMS to interoperate
effectively with the BSP.  The C structure definition is given
here:

@example
@group
typedef struct @{
  uint32_t     interrupt_stack_size;
  /* end of fields required on all CPUs */
@} rtems_cpu_table;
@end group
@end example

@table @code 
@item interrupt_stack_size
is the size of the RTEMS
allocated interrupt stack in bytes.  This value must be at least
as large as MINIMUM_STACK_SIZE.

@end table
