@c
@c  COPYRIGHT (c) 1988-1999.
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

@section Byte Addressable versus Word Addressable

Processor in the Texas Instruments C3x/C4x family are 
word addressable.  This is in sharp contrast to CISC and
RISC processors that are typically byte addressable.  In a word
addressable architecture, each address points not to an
8-bit byte or octet but to 32 bits.

On first glance, byte versus word addressability does not
sound like a problem but in fact, this issue can result in
subtle problems in high-level language software that is ported
to a word addressable processor family.  The following is a 
list of the commonly encountered problems:

@table @b

@item String Optimizations
Although each character in a string occupies a single address just
as it does on a byte addressable CPU, each character occupies
32 rather than 8 bits.  The most significant 24 bytes are 
of each address are ignored.  This in and of itself does not
cause problems but it violates the assumption that two 
adjacent characters in a string have no intervening bits.
This assumption is often implicit in string and memory comparison
routines that are optimized to compare 4 adjacent characters
with a word oriented operation.  This optimization is 
invalid on word addressable processors.

@item Sizeof
The C operation @code{sizeof} returns very different results
on the C3x/C4x than on traditional RISC/CISC processors. 
The @code{sizeof(char)}, @code{sizeof(short)}, and @code{sizeof(int)}
are all 1 since each occupies a single addressable unit that is
thirty-two bits wide.  On most thirty-two bit processors,
@code{sizeof(char} is one, @code{sizeof(short)} is two,
and @code{sizeof(int)} is four.  Just as software makes assumptions
about the sizes of the primitive data types has problems
when ported to a sixty-four bit architecture, these same
assumptions cause problems on the C3x/C4x.

@item Alignment
Since each addressable unit is thirty-two bit wide, there
are no alignment restrictions.  The native integer type
need only be aligned on a "one unit" boundary not a "four
unit" boundary as on numerous other processors.

@end table

@section Flat Memory Model

XXX check actual bits on the various processor families.

The XXX family supports a flat 32-bit address
space with addresses ranging from 0x00000000 to 0xFFFFFFFF (4
gigabytes).  Each address is represented by a 32-bit value and
is byte addressable.  The address may be used to reference a
single byte, word (2-bytes), or long word (4 bytes).  Memory
accesses within this address space are performed in big endian
fashion by the processors in this family.

@section Compiler Memory Models

The Texas Instruments C3x/C4x processors include a Data Page
(@code{dp}) register that logically is a base address.  The
@code{dp} register allows the use of shorter offsets in
instructions.  Up to 64K words may be addressed using
offsets from the @code{dp} register.  In order to address
words not addressable based on the current value of
@code{dp}, the register must be loaded with a different
value.

The @code{dp} register is managed automatically by
the high-level language compilers.
The various compilers for this processor family support
two memory models that manage the @code{dp} register
in very different manners.  The large and small memory
models are discussed in the following sections.

NOTE: The C3x/C4x port of RTEMS has been written
so that it should support either memory model.
However, it has only been tested using the
large memory model.

@subsection Small Memory Model

The small memory model is the simplest and most
efficient.  However, it includes a limitation that
make it inappropriate for numerous applications.  The
small memory model assumes that the application needs
to access no more than 64K words. Thus the @code{dp} 
register can be loaded at application start time 
and never reloaded.  Thus the compiler will not
even generate instructions to load the @code{dp}.

This can significantly reduce the code space
required by an application but the application
is limited in the amount of data it can access.

With the GNU Compiler Suite, small memory model is 
selected by invoking the compiler with either the 
@code{-msmall} or @code{-msmallmemoryXXX} argument.
This argument must be included when linking the application
in order to ensure that support libraries also compiled
for the large memory model are used.  
The default memory model is XXX.

When this memory model is selected, the @code{XXX}
symbol is predefined by the C and C++ compilers
and the @code{XXX} symbol is predefined by the assembler.
This behavior is the same for the GNU and Texas Instruments
toolsets.  RTEMS uses these predefines to determine the proper handling
of the @code{dp} register in those C3x/C4x specific routines
that were written in assembly language.

@subsection Large Memory Model

The large memory model is more complex and less efficient
than the small memory model.  However, it removes the
64K uninitialized data restriction from applications.
The @code{dp} register is reloaded automatically
by the compiler each time data is accessed.  This leads
to an increase in the code space requirements for the
application but gives it access to much more data space.

With the GNU Compiler Suite, large memory model is 
selected by invoking the compiler with either the 
@code{-mlarge} or @code{-mlargememoryXXX} argument.
This argument must be included when linking the application
in order to ensure that support libraries also compiled
for the large memory model are used.
The default memory model is XXX.

When this memory model is selected, the @code{XXX}
symbol is predefined by the C and C++ compilers
and the @code{XXX} symbol is predefined by the assembler.
This behavior is the same for the GNU and Texas Instruments
toolsets.  RTEMS uses these predefines to determine the proper handling
of the @code{dp} register in those C3x/C4x specific routines
that were written in assembly language.
