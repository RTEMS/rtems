@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Board Support Packages

@section Introduction

An RTEMS Board Support Package (BSP) must be designed to support a
particular processor and target board combination.  This chapter presents a
discussion of i386 specific BSP issues.  For more information on developing
a BSP, refer to the chapter titled Board Support Packages in the RTEMS
Applications User's Guide. 

@section System Reset

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

@section Processor Initialization

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

