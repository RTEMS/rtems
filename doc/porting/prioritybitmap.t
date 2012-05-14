@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Priority Bitmap Manipulation

@section Introduction

The RTEMS chain of ready tasks is implemented as an array of FIFOs with
each priority having its own FIFO.  This makes it very efficient to
determine the first and last ready task at each priority.  In addition,
blocking a task only requires appending the task to the end of the FIFO
for its priority rather than a lengthy search down a single chain of all
ready tasks.  This works extremely well except for one problem.  When the
currently executing task blocks, there may be no easy way to determine
what is the next most important ready task.  If the blocking task was the
only ready task at its priority, then RTEMS must search all of the FIFOs
in the ready chain to determine the highest priority with a ready task.

RTEMS uses a bitmap array to efficiently solve this problem.  The state of
each bit in the priority map bit array indicates whether or not there is a
ready task at that priority.  The bit array can be efficiently searched to
determine the highest priority ready task.  This family of data type and
routines is used to maintain and search the bit map array.

When manipulating the bitmap array, RTEMS internally divides the 
8 bits of the task priority into "major" and "minor" components.
The most significant 4 bits are the major component, while the least
significant are the minor component.  The major component of a priority
value is used to determine which 16-bit wide entry in the
@code{_Priority_Bit_map} array is associated with this priority.
Each element in the @code{_Priority_Bit_map} array has a bit
in the @code{_Priority_Major_bit_map} associated with it.  
That bit is cleared when all of the bits in a particular
@code{_Priority_Bit_map} array entry are zero.

The minor component of a priority is used to determine
specifically which bit in @code{_Priority_Bit_map[major]}
indicates whether or not there is a ready to execute task
at the priority.

 
@section _Priority_bit_map_Control Type

The @code{_Priority_Bit_map_Control} type is the fundamental data type of the
priority bit map array used to determine which priorities have ready
tasks.  This type may be either 16 or 32 bits wide although only the 16
least significant bits will be used.  The data type is based upon what is
the most efficient type for this CPU to manipulate.  For example, some
CPUs have bit scan instructions that only operate on a particular size of
data.  In this case, this type will probably be defined to work with this
instruction.

@section Find First Bit Routine

The _CPU_Bitfield_Find_first_bit routine sets _output to the bit number of
the first bit set in @code{_value}.  @code{_value} is of CPU dependent type
@code{Priority_bit_map_Control}.  A stub version of this routine is as follows:

@example
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  @{ \
    (_output) = 0;   /* do something to prevent warnings */ \
  @}
@end example



There are a number of variables in using a "find first bit" type
instruction.

@enumerate

@item What happens when run on a value of zero?

@item Bits may be numbered from MSB to LSB or vice-versa.

@item The numbering may be zero or one based.

@item The "find first bit" instruction may search from MSB or LSB.

@end enumerate

RTEMS guarantees that (1) will never happen so it is not a concern.  
Cases (2),(3), (4) are handled by the macros _CPU_Priority_mask() and
_CPU_Priority_bits_index().  These three form a set of routines which must
logically operate together.  Bits in the @code{_value} are set and cleared based
on masks built by CPU_Priority_mask().  The basic major and minor values
calculated by _Priority_Major() and _Priority_Minor() are "massaged" by
_CPU_Priority_bits_index() to properly range between the values returned
by the "find first bit" instruction.  This makes it possible for
_Priority_Get_highest() to calculate the major and directly index into the
minor table.  This mapping is necessary to ensure that 0 (a high priority
major/minor) is the first bit found.

This entire "find first bit" and mapping process depends heavily on the
manner in which a priority is broken into a major and minor components
with the major being the 4 MSB of a priority and minor the 4 LSB.  Thus (0
<< 4) + 0 corresponds to priority 0 -- the highest priority.  And (15 <<
4) + 14 corresponds to priority 254 -- the next to the lowest priority.

If your CPU does not have a "find first bit" instruction, then there are
ways to make do without it.  Here are a handful of ways to implement this
in software:

@itemize @bullet

@item  a series of 16 bit test instructions

@item  a "binary search using if's"

@item  the following algorithm based upon a 16 entry lookup table.  In this pseudo-code, bit_set_table[16] has values which indicate the first bit set:

@example
_number = 0 if _value > 0x00ff
     _value >>=8
     _number = 8;
 if _value > 0x0000f
     _value >=8
     _number += 4
 
_number += bit_set_table[ _value ]
@end example

@end itemize

The following illustrates how the CPU_USE_GENERIC_BITFIELD_CODE macro may
be so the port can use the generic implementation of this bitfield code.  
This can be used temporarily during the porting process to avoid writing
these routines until the end.  This results in a functional although lower
performance port.  This is perfectly acceptable during development and
testing phases.

@example
#define CPU_USE_GENERIC_BITFIELD_CODE TRUE
#define CPU_USE_GENERIC_BITFIELD_DATA TRUE
@end example

Eventually, CPU specific implementations of these routines are usually
written since they dramatically impact the performance of blocking
operations.  However they may take advantage of instructions which are not
available on all models in the CPU family.  In this case, one might find
something like this stub example did:

@example
#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  @{ \
    (_output) = 0;   /* do something to prevent warnings */ \
  @}
#endif
@end example

@section Build Bit Field Mask

The _CPU_Priority_Mask routine builds the mask that corresponds to the bit
fields searched by _CPU_Bitfield_Find_first_bit().  See the discussion of
that routine for more details.

The following is a typical implementation when the
_CPU_Bitfield_Find_first_bit searches for the most significant bit set:

@example
#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)
#define _CPU_Priority_Mask( _bit_number ) \
  ( 1 << (_bit_number) )
#endif
@end example

@section Bit Scan Support

The @code{_CPU_Priority_bits_index} routine translates the bit numbers
returned by @code{_CPU_Bitfield_Find_first_bit()} into something
suitable for use as a major or minor component of a priority.  
The find first bit routine may number the bits in a
way that is difficult to map into the major and minor components
of the priority.  For example, when finding the first bit set in 
the value 0x8000, a CPU may indicate that bit 15 or 16 is set
based on whether the least significant bit is "zero" or "one".
Similarly, a CPU may only scan 32-bit values and consider the
most significant bit to be bit zero or one.  In this case, this
would be bit 16 or 17.

This routine allows that unwieldy form to be converted
into a normalized form that is easier to process and use
as an index.  

@example
#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)
#define _CPU_Priority_bits_index( _priority ) \
  (_priority)
#endif
@end example


