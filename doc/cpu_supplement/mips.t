@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@ifinfo
@end ifinfo
@chapter MIPS Specific Information

This chapter discusses the MIPS architecture dependencies
in this port of RTEMS.  The MIPS family has a wide variety
of implementations by a wide range of vendors.  Consequently,
there are many, many CPU models within it.

@subheading Architecture Documents

IDT docs are online at http://www.idt.com/products/risc/Welcome.html

For information on the XXX architecture, refer to the following documents
available from VENDOR (@file{http//www.XXX.com/}):

@itemize @bullet
@item @cite{XXX Family Reference, VENDOR, PART NUMBER}.
@end itemize

@c
@c
@c

@section CPU Model Dependent Features

This section presents the set of features which vary
across MIPS implementations and are of importance to RTEMS.
The set of CPU model feature macros are defined in the file
@code{cpukit/score/cpu/mips/mips.h} based upon the particular CPU
model specified on the compilation command line.

@subsection Another Optional Feature

The macro XXX 

@c
@c
@c

@section Calling Conventions

@subsection Processor Background

TBD

@subsection Calling Mechanism

TBD

@subsection Register Usage

TBD

@subsection Parameter Passing

TBD

@c
@c
@c
@section Memory Model

@subsection Flat Memory Model

The MIPS family supports a flat 32-bit address
space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes).  Each address is represented by a 32-bit value and
is byte addressable.  The address may be used to reference a
single byte, word (2-bytes), or long word (4 bytes).  Memory
accesses within this address space are performed in big endian
fashion by the processors in this family.

Some of the MIPS family members such as the support virtual memory and
segmentation.  RTEMS does not support virtual memory or
segmentation on any of these family members.

@c
@c
@c

@section Interrupt Processing

Although RTEMS hides many of the processor dependent
details of interrupt processing, it is important to understand
how the RTEMS interrupt manager is mapped onto the processor's
unique architecture. Discussed in this chapter are the MIPS's
interrupt response and control mechanisms as they pertain to
RTEMS.

@subsection Vectoring of an Interrupt Handler

Upon receipt of an interrupt the XXX family
members with separate interrupt stacks automatically perform the
following actions:

@itemize @bullet
@item TBD

@end itemize

A nested interrupt is processed similarly by these
CPU models with the exception that only a single ISF is placed
on the interrupt stack and the current stack need not be
switched.

@subsection Interrupt Levels

TBD

@c
@c
@c
@section Default Fatal Error Processing

The default fatal error handler for this target architecture disables
processor interrupts, places the error code in @b{XXX}, and executes a
@code{XXX} instruction to simulate a halt processor instruction.

@c
@c
@c
@section Board Support Packages

@subsection System Reset

An RTEMS based application is initiated or
re-initiated when the processor is reset.  When the
processor is reset, it performs the following actions:

@itemize @bullet
@item TBD

@end itemize

@subsection Processor Initialization

TBD
