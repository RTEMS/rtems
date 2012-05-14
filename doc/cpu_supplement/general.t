@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@ifinfo
@end ifinfo
@chapter Port Specific Information

This chaper provides a general description of the type of
architecture specific information which is in each of
the architecture specific chapters that follow.  The outline
of this chapter is identical to that of the architecture
specific chapters.

In each of the architecture specific chapters, this
introductory section will provide an overview of the
architecture

@subheading Architecture Documents

In each of the architecture specific chapters, this
section will provide pointers on where to obtain
documentation.

@c
@c
@c
@section CPU Model Dependent Features

Microprocessors are generally classified into families with a variety of
CPU models or implementations within that family.  Within a processor
family, there is a high level of binary compatibility.  This family
may be based on either an architectural specification or on maintaining
compatibility with a popular processor.  Recent microprocessor families
such as the SPARC or PowerPC are based on an architectural specification
which is independent or any particular CPU model or implementation.
Older families such as the Motorola 68000 and the Intel x86 evolved as the
manufacturer strived to produce higher performance processor models which
maintained binary compatibility with older models.

RTEMS takes advantage of the similarity of the various models within a
CPU family.  Although the models do vary in significant ways, the high
level of compatibility makes it possible to share the bulk of the CPU
dependent executive code across the entire family.  Each processor family
supported by RTEMS has a list of features which vary between CPU models
within a family.  For example, the most common model dependent feature
regardless of CPU family is the presence or absence of a floating point
unit or coprocessor.  When defining the list of features present on a
particular CPU model, one simply notes that floating point hardware
is or is not present and defines a single constant appropriately.
Conditional compilation is utilized to include the appropriate source
code for this CPU model's feature set.  It is important to note that
this means that RTEMS is thus compiled using the appropriate feature set
and compilation flags optimal for this CPU model used.  The alternative
would be to generate a binary which would execute on all family members
using only the features which were always present.

The set of CPU model feature macros are defined in the file
@code{cpukit/score/cpu/CPU/rtems/score/cpu.h} based upon the GNU tools
multilib variant that is appropriate for the particular CPU model defined
on the compilation command line.

In each of the architecture specific chapters, this section presents
the set of features which vary across various implementations of the
architecture that may be of importance to RTEMS application developers.

The subsections will vary amongst the target architecture chapters as
the specific features may vary.  However, each port will include a few
common features such as the CPU Model Name and presence of a hardware
Floating Point Unit.  The common features are described here.

@subsection CPU Model Name

The macro @code{CPU_MODEL_NAME} is a string which designates
the name of this CPU model.  For example, for the MC68020
processor model from the m68k architecture, this macro
is set to the string "mc68020".

@subsection Floating Point Unit

In most architectures, the presence of a floating point unit is an option.
It does not matter whether the hardware floating point support is
incorporated on-chip or is an external coprocessor as long as it
appears an FPU per the ISA.  However, if a hardware FPU is not present,
it is possible that the floating point emulation library for this
CPU is not reentrant and thus context switched by RTEMS.

RTEMS provides two feature macros to indicate the FPU configuration:

@itemize @bullet

@item CPU_HARDWARE_FP
is set to TRUE to indicate that a hardware FPU is present.

@item CPU_SOFTWARE_FP
is set to TRUE to indicate that a hardware FPU is not present and that
the FP software emulation will be context switched.

@end itemize

@c
@c
@c
@section Calling Conventions

Each high-level language compiler generates subroutine entry and exit
code based upon a set of rules known as the compiler's calling convention.
These rules address the following issues:

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

@subsection Calling Mechanism

In each of the architecture specific chapters, this subsection will
describe the instruction(s) used to perform a @i{normal} subroutine
invocation.  All RTEMS directives are invoked as @i{normal} C language
functions so it is important to the user application to understand the
call and return mechanism.

@subsection Register Usage

In each of the architecture specific chapters, this subsection will
detail the set of registers which are @b{NOT} preserved across subroutine
invocations.  The registers which are not preserved are assumed to be
available for use as scratch registers.  Therefore, the contents of these
registers should not be assumed upon return from any RTEMS directive.

In some architectures, there may be a set of registers made available
automatically as a side-effect of the subroutine invocation 
mechanism.

@subsection Parameter Passing

In each of the architecture specific chapters, this subsection will
describe the mechanism by which the parameters or arguments are passed
by the caller to a subroutine.  In some architectures, all parameters
are passed on the stack while in others some are passed in registers.

@subsection User-Provided Routines

All user-provided routines invoked by RTEMS, such as
user extensions, device drivers, and MPCI routines, must also
adhere to these calling conventions.

@c
@c
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

Most RTEMS target processors can be initialized to support a flat address
space.  Although the size of addresses varies between architectures, on
most RTEMS targets, an address is 32-bits wide which defines addresses
ranging from 0x00000000 to 0xFFFFFFFF (4 gigabytes).  Each address is
represented by a 32-bit value and is byte addressable.  The address may be
used to reference a single byte, word (2-bytes), or long word (4 bytes).
Memory accesses within this address space may be performed in little or
big endian fashion.

On smaller CPU architectures supported by RTEMS, the address space 
may only be 20 or 24 bits wide.  

If the CPU model has support for virtual memory or segmentation, it is
the responsibility of the Board Support Package (BSP) to initialize the
MMU hardware to perform address translations which correspond to flat
memory model.

In each of the architecture specific chapters, this subsection will
describe any architecture characteristics that differ from this general
description.

@c
@c
@c
@section Interrupt Processing

Different types of processors respond to the occurrence of an interrupt
in its own unique fashion. In addition, each processor type provides
a control mechanism to allow for the proper handling of an interrupt.
The processor dependent response to the interrupt modifies the current
execution state and results in a change in the execution stream.  Most
processors require that an interrupt handler utilize some special control
mechanisms to return to the normal processing stream.  Although RTEMS
hides many of the processor dependent details of interrupt processing,
it is important to understand how the RTEMS interrupt manager is mapped
onto the processor's unique architecture.

RTEMS supports a dedicated interrupt stack for all architectures.
On architectures with hardware support for a dedicated interrupt stack,
it will be initialized such that when an interrupt occurs, the processor
automatically switches to this dedicated stack.  On architectures without
hardware support for a dedicated interrupt stack which is separate from
those of the tasks, RTEMS will support switching to a dedicated stack
for interrupt processing.

Without a dedicated interrupt stack, every task in
the system MUST have enough stack space to accommodate the worst
case stack usage of that particular task and the interrupt
service routines COMBINED.  By supporting a dedicated interrupt
stack, RTEMS significantly lowers the stack requirements for
each task.

A nested interrupt is processed similarly with the exception that since
the CPU is already executing on the interrupt stack, there is no need
to switch to the interrupt stack.

In some configurations, RTEMS allocates the interrupt stack from the
Workspace Area.  The amount of memory allocated for the interrupt stack
is user configured and based upon the @code{confdefs.h} parameter
@code{CONFIGURE_INTERRUPT_STACK_SIZE}.  This parameter is described
in detail in the Configuring a System chapter of the User's Guide.
On configurations in which RTEMS allocates the interrupt stack, during
the initialization process, RTEMS will also install its interrupt stack.
In other configurations, the interrupt stack is allocated and installed
by the Board Support Package (BSP).

In each of the architecture specific chapters, this section discesses
the interrupt response and control mechanisms of the architecture as
they pertain to RTEMS.

@subsection Vectoring of an Interrupt Handler

In each of the architecture specific chapters, this subsection will
describe the architecture specific details of the interrupt vectoring
process.  In particular, it should include a description of the 
Interrupt Stack Frame (ISF).

@subsection Interrupt Levels

In each of the architecture specific chapters, this subsection will
describe how the interrupt levels available on this particular architecture
are mapped onto the 255 reserved in the task mode.  The interrupt level
value of zero (0) should always mean that interrupts are enabled.

Any use of an  interrupt level that is is not undefined on a particular
architecture may result in behavior that is unpredictable.

@subsection Disabling of Interrupts by RTEMS

During the execution of directive calls, critical sections of code may
be executed.  When these sections are encountered, RTEMS disables all
external interrupts before the execution of this section and restores
them to the previous level upon completion of the section.  RTEMS has
been optimized to ensure that interrupts are disabled for the shortest
number of instructions possible.  Since the precise number of instructions
and their execution time varies based upon target CPU family, CPU model,
board memory speed, compiler version, and optimization level, it is 
not practical to provide the precise number for all possible RTEMS 
configurations. 

Historically, the measurements were made by hand analyzing and counting
the execution time of instruction sequences during interrupt disable 
critical sections.  For reference purposes, on a 16 Mhz Motorola
MC68020, the maximum interrupt disable period was typically approximately
ten (10) to thirteen (13) microseconds.  This architecture was memory bound
and had a slow bit scan instruction.  In contrast, during the same 
period a 14 Mhz SPARC would have a worst case disable time of approximately
two (2) to three (3) microseconds because it had a single cycle bit scan
instruction and used fewer cycles for memory accesses.

If you are interested in knowing the worst case execution time for
a particular version of RTEMS, please contact OAR Corporation and
we will be happy to product the results as a consulting service.

Non-maskable interrupts (NMI) cannot be disabled, and
ISRs which execute at this level MUST NEVER issue RTEMS system
calls.  If a directive is invoked, unpredictable results may
occur due to the inability of RTEMS to protect its critical
sections.  However, ISRs that make no system calls may safely
execute as non-maskable interrupts.


@c
@c
@c
@section Default Fatal Error Processing

Upon detection of a fatal error by either the application or RTEMS during
initialization the @code{rtems_fatal_error_occurred} directive supplied
by the Fatal Error Manager is invoked.  The Fatal Error Manager will
invoke the user-supplied fatal error handlers.  If no user-supplied
handlers are configured or all of them return without taking action to
shutdown the processor or reset, a default fatal error handler is invoked.

Most of the action performed as part of processing the fatal error are
described in detail in the Fatal Error Manager chapter in the User's
Guide.  However, the if no user provided extension or BSP specific fatal
error handler takes action, the final default action is to invoke a
CPU architecture specific function.  Typically this function disables
interrupts and halts the processor.

In each of the architecture specific chapters, this describes the precise
operations of the default CPU specific fatal error handler.

@c
@c
@c

@section Board Support Packages

An RTEMS Board Support Package (BSP) must be designed to support a
particular processor model and target board combination.

In each of the architecture specific chapters, this section will present
a discussion of architecture specific BSP issues.   For more information
on developing a BSP, refer to BSP and Device Driver Development Guide
and the chapter titled Board Support Packages in the RTEMS
Applications User's Guide.

@subsection System Reset

An RTEMS based application is initiated or re-initiated when the processor
is reset or transfer is passed to it from a boot monitor or ROM monitor.

In each of the architecture specific chapters, this subsection describes
the actions that the BSP must tak assuming the application gets control
when the microprocessor is reset.
