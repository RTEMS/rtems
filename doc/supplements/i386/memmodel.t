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

RTEMS supports the i386 protected mode, flat memory
model with paging disabled.  In this mode, the i386
automatically converts every address from a logical to a
physical address each time it is used.  The i386 uses
information provided in the segment registers and the Global
Descriptor Table to convert these addresses.  RTEMS assumes the
existence of the following segments:

@itemize @bullet
@item a single code segment at protection level (0) which
contains all application and executive code.

@item a single data segment at protection level zero (0) which
contains all application and executive data.
@end itemize

The i386 segment registers and associated selectors
must be initialized when the initialize_executive directive is
invoked.  RTEMS treats the segment registers as system registers
and does not modify or context switch them.

This i386 memory model supports a flat 32-bit address
space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes).  Each address is represented by a 32-bit value and
is byte addressable.  The address may be used to reference a
single byte, half-word (2-bytes), or word (4 bytes).

RTEMS does not require that logical addresses map
directly to physical addresses, although it is desirable in many
applications to do so.  If logical and physical addresses are
not the same, then an additional selector will be required so
RTEMS can access the Interrupt Descriptor Table to install
interrupt service routines.  The selector number of this segment
is provided to RTEMS in the CPU Dependent Information Table.

By not requiring that logical addresses map directly
to physical addresses, the memory space of an RTEMS application
can be separated from that of a ROM monitor.  For example, on
the Force Computers CPU386, the ROM monitor loads application
programs into a logical address space where logical address
0x00000000 corresponds to physical address 0x0002000.  On this
board, RTEMS and the application use virtual addresses which do
not map to physical addresses.

RTEMS assumes that the DS and ES registers contain
the selector for the single data segment when a directive is
invoked.   This assumption is especially important when
developing interrupt service routines.

