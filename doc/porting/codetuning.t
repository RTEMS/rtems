@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Code Tuning Parameters

@section Inline Thread_Enable_dispatch

Should the calls to _Thread_Enable_dispatch be inlined?

If TRUE, then they are inlined.

If FALSE, then a subroutine call is made.


Basically this is an example of the classic trade-off of size versus
speed.  Inlining the call (TRUE) typically increases the size of RTEMS
while speeding up the enabling of dispatching.

[NOTE: In general, the _Thread_Dispatch_disable_level will only be 0 or 1
unless you are in an interrupt handler and that interrupt handler invokes
the executive.] When not inlined something calls _Thread_Enable_dispatch
which in turns calls _Thread_Dispatch.  If the enable dispatch is inlined,
then one subroutine call is avoided entirely.]

@example
#define CPU_INLINE_ENABLE_DISPATCH       FALSE
@end example

@section Inline Thread_queue_Enqueue_priority

Should the body of the search loops in _Thread_queue_Enqueue_priority be
unrolled one time?  In unrolled each iteration of the loop examines two
"nodes" on the chain being searched.  Otherwise, only one node is examined
per iteration.

If TRUE, then the loops are unrolled.

If FALSE, then the loops are not unrolled.

The primary factor in making this decision is the cost of disabling and
enabling interrupts (_ISR_Flash) versus the cost of rest of the body of
the loop.  On some CPUs, the flash is more expensive than one iteration of
the loop body.  In this case, it might be desirable to unroll the loop.  
It is important to note that on some CPUs, this code is the longest
interrupt disable period in RTEMS.  So it is necessary to strike a balance
when setting this parameter.

@example
#define CPU_UNROLL_ENQUEUE_PRIORITY      TRUE
@end example


@section Structure Alignment Optimization

The following macro may be defined to the attribute setting used to force
alignment of critical RTEMS structures.  On some processors it may make
sense to have these aligned on tighter boundaries than the minimum
requirements of the compiler in order to have as much of the critical data
area as possible in a cache line.  This ensures that the first access of
an element in that structure fetches most, if not all, of the data
structure and places it in the data cache.  Modern CPUs often have cache
lines of at least 16 bytes and thus a single access implicitly fetches
some surrounding data and places that unreferenced data in the cache.  
Taking advantage of this allows RTEMS to essentially prefetch critical
data elements.

The placement of this macro in the declaration of the variables is based
on the syntactically requirements of the GNU C "__attribute__" extension.  
For another toolset, the placement of this macro could be incorrect.  For
example with GNU C, use the following definition of
CPU_STRUCTURE_ALIGNMENT to force a structures to a 32 byte boundary.

#define CPU_STRUCTURE_ALIGNMENT __attribute__ ((aligned (32)))

To benefit from using this, the data must be heavily used so it will stay
in the cache and used frequently enough in the executive to justify
turning this on.  NOTE:  Because of this, only the Priority Bit Map table
currently uses this feature.

The following illustrates how the CPU_STRUCTURE_ALIGNMENT is defined on
ports which require no special alignment for optimized access to data
structures:

@example
#define CPU_STRUCTURE_ALIGNMENT
@end example

@section Data Alignment Requirements

@subsection Data Element Alignment

The CPU_ALIGNMENT macro should be set to the CPU's worst alignment
requirement for data types on a byte boundary.  This is typically the
alignment requirement for a C double. This alignment does not take into
account the requirements for the stack.

The following sets the CPU_ALIGNMENT macro to 8 which indicates that there
is a basic C data type for this port which much be aligned to an 8 byte
boundary.

@example
#define CPU_ALIGNMENT              8
@end example

@subsection Heap Element Alignment

The CPU_HEAP_ALIGNMENT macro is set to indicate the byte alignment
requirement for data allocated by the RTEMS Code Heap Handler.  This
alignment requirement may be stricter than that for the data types
alignment specified by CPU_ALIGNMENT.  It is common for the heap to follow
the same alignment requirement as CPU_ALIGNMENT.  If the CPU_ALIGNMENT is
strict enough for the heap, then this should be set to CPU_ALIGNMENT. This
macro is necessary to ensure that allocated memory is properly aligned for
use by high level language routines.

The following example illustrates how the CPU_HEAP_ALIGNMENT macro is set
when the required alignment for elements from the heap is the same as the
basic CPU alignment requirements.

@example
#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT
@end example

NOTE:  This does not have to be a power of 2.  It does have to be greater
or equal to than CPU_ALIGNMENT.

@subsection Partition Element Alignment

The CPU_PARTITION_ALIGNMENT macro is set to indicate the byte alignment
requirement for memory buffers allocated by the RTEMS Partition Manager
that is part of the Classic API.  This alignment requirement may be
stricter than that for the data types alignment specified by
CPU_ALIGNMENT.  It is common for the partition to follow the same
alignment requirement as CPU_ALIGNMENT.  If the CPU_ALIGNMENT is strict
enough for the partition, then this should be set to CPU_ALIGNMENT.  This
macro is necessary to ensure that allocated memory is properly aligned for
use by high level language routines.

The following example illustrates how the CPU_PARTITION_ALIGNMENT macro is
set when the required alignment for elements from the RTEMS Partition
Manager is the same as the basic CPU alignment requirements.


@example
#define CPU_PARTITION_ALIGNMENT    CPU_ALIGNMENT
@end example

NOTE:  This does not have to be a power of 2.  It does have to be greater
or equal to than CPU_ALIGNMENT.

