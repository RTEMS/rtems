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

RTEMS supports applications in which the application
and the executive execute within a single thirty-two bit address
space.  Thus RTEMS and the application share a common four
gigabyte address space within a single space.  The PA-RISC
automatically converts every address from a logical to a
physical address each time it is used.  The PA-RISC uses
information provided in the page table to perform this
translation.  The following protection levels are assumed:

@itemize @bullet
@item a single code segment at protection level (0) which
contains all application and executive code.

@item a single data segment at protection level zero (0) which
contains all application and executive data.
@end itemize

The PA-RISC space registers and associated stack --
including the stack pointer r27 -- must be initialized when the
initialize_executive directive is invoked.  RTEMS treats the
space registers as system resources shared by all tasks and does
not modify or context switch them.

This memory model supports a flat 32-bit address
space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes).  Each address is represented by a 32-bit value and
memory is addressable.  The address may be used to reference a
single byte, half-word (2-bytes), or word (4 bytes).

RTEMS does not require that logical addresses map
directly to physical addresses, although it is desirable in many
applications to do so.  RTEMS does not need any additional
information when physical addresses do not map directly to
physical addresses.  By not requiring that logical addresses map
directly to physical addresses, the memory space of an RTEMS
space can be separated from that of a ROM monitor.  For example,
a ROM monitor may load application programs into a separate
logical address space from itself.

RTEMS assumes that the space registers contain the
selector for the single data segment when a directive is
invoked.   This assumption is especially important when
developing interrupt service routines.

