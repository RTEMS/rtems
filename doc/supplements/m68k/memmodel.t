@c
@c  COPYRIGHT (c) 1988-1996.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c

@ifinfo
@node Memory Model, Memory Model Introduction, Calling Conventions User-Provided Routines, Top
@end ifinfo
@chapter Memory Model
@ifinfo
@menu
* Memory Model Introduction::
* Memory Model Flat Memory Model::
@end menu
@end ifinfo

@ifinfo
@node Memory Model Introduction, Memory Model Flat Memory Model, Memory Model, Memory Model
@end ifinfo
@section Introduction

A processor may support any combination of memory
models ranging from pure physical addressing to complex demand
paged virtual memory systems.  RTEMS supports a flat memory
model which ranges contiguously over the processor's allowable
address space.  RTEMS does not support segmentation or virtual
memory of any kind.  The appropriate memory model for RTEMS
provided by the targeted processor and related characteristics
of that model are described in this chapter.

@ifinfo
@node Memory Model Flat Memory Model, Interrupt Processing, Memory Model Introduction, Memory Model
@end ifinfo
@section Flat Memory Model

The MC68xxx family supports a flat 32-bit address
space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes).  Each address is represented by a 32-bit value and
is byte addressable.  The address may be used to reference a
single byte, word (2-bytes), or long word (4 bytes).  Memory
accesses within this address space are performed in big endian
fashion by the processors in this family.

Some of the MC68xxx family members such as the
MC68020, MC68030, and MC68040 support virtual memory and
segmentation.  The MC68020 requires external hardware support
such as the MC68851 Paged Memory Management Unit coprocessor
which is typically used to perform address translations for
these systems.  RTEMS does not support virtual memory or
segmentation on any of the MC68xxx family members.

