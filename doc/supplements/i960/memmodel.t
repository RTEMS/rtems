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

The i960CA supports a flat 32-bit address space with
addresses ranging from 0x00000000 to 0xFFFFFFFF (4 gigabytes).
Although the i960CA reserves portions of this address space,
application code and data may be placed in any non-reserved
areas.  Each address is represented by a 32-bit value and is
byte addressable.  The address may be used to reference a single
byte, half-word (2-bytes), word (4 bytes), double-word (8
bytes), triple-word (12 bytes) or quad-word (16 bytes).  The
i960CA does not support virtual memory or segmentation.

The i960CA allows the memory space to be partitioned
into sixteen regions which may be configured individually as big
or little endian.  RTEMS assumes that the memory regions in
which its code, data, and the RTEMS Workspace reside are
configured as little endian.


