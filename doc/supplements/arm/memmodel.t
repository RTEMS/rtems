@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Memory Model

@section Introduction

A processor may support any combination of memory
models ranging from pure physical addressing to complex demand
paged virtual memory systems.  RTEMS supports a flat memory
model which ranges contiguously over the processor's allowable
address space.  RTEMS does not support segmentation or virtual
memory of any kind.  The appropriate memory model for RTEMS
provided by the targeted processor and related characteristics
of that model are described in this chapter.

@section Flat Memory Model

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

