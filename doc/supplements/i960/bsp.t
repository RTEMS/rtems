@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Board Support Packages

@section Introduction

An RTEMS Board Support Package (BSP) must be designed
to support a particular processor and target board combination.
This chapter presents a discussion of i960CA specific BSP
issues.   For more information on developing a BSP, refer to the
chapter titled Board Support Packages in the RTEMS
Applications User's Guide.

@section System Reset

An RTEMS based application is initiated when the
i960CA processor is reset.  When the i960CA is reset, the
processor reads an Initial Memory Image (IMI) to establish its
state.  The IMI consists of the Initialization Boot Record (IBR)
and the Process Control Block (PRCB) from an Initial Memory
Image (IMI) at location 0xFFFFFF00.  The IBR contains the
initial bus configuration data, the address of the first
instruction to execute after reset, the address of the PRCB, and
the checksum used by the processor's self-test.

@section Processor Initialization

The PRCB contains the base addresses for system data
structures, and initial configuration information for the core
and integrated peripherals.  In particular, the PRCB contains
the initial contents of the Arithmetic Control (AC) Register as
well as the base addresses of the Interrupt Vector Table, System
Procedure Entry Table, Fault Entry Table, and the Control Table.
In addition, the PRCB is used to configure the depth of the
instruction and register caches and the actions when certain
types of faults are encountered.

The Process Controls (PC) Register is initialized to
0xC01F2002 which sets the i960CA's interrupt level to 0x1F  (31
decimal).  In addition, the Interrupt Mask (IMSK) Register
(alternately referred to as Special Function Register 1 or sf1)
is set to 0x00000000 to mask all external and DMA interrupt
sources.  Thus, all interrupts are disabled when the first
instruction is executed.

For more information regarding the i960CA's data
structures and their contents, refer to Intel's i960CA User's
Manual.
