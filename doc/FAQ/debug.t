@c
@c  COPYRIGHT (c) 1988-1999.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Debugging Hints

The questions in this category are hints that can ease debugging.

@section Malloc

@subsection Is malloc reentrant?

Yes.  The RTEMS Malloc implementation is reentrant.  It is
implemented as calls to the Region Manager in the Classic API.

@subsection When is malloc initialized?

During BSP initialization, the @code{bsp_libc_init} routine
is called.  This routine initializes the heap as well as
the RTEMS system call layer (open, read, write, etc.) and
the RTEMS reentrancy support for the Cygnus newlib Standard C  
Library.

The @code{bsp_libc_init} routine is passed the size and starting
address of the memory area to be used for the program heap as well
as the amount of memory to ask @code{sbrk} for when the heap is
exhausted.  For most BSPs, all memory available is placed in the
program heap thus it can not be extended dynamically by calls to
@code{sbrk}.

@section How do I determine how much memory is left?

First there are two types of memory: RTEMS Workspace and Program Heap.
The RTEMS Workspace is the memory used by RTEMS to allocate control
structures for system objects like tasks and semaphores, task 
stacks, and some system data structures like the ready chains.
The Program Heap is where "malloc'ed" memory comes from.

Both are essentially managed as heaps based on the Heap Manager
in the RTEMS SuperCore.  The RTEMS Workspace uses the Heap Manager
directly while the Program Heap is actually based on an RTEMS Region 
from the Classic API.  RTEMS Regions are in turn based on the Heap
Manager in the SuperCore.

@subsection How much memory is left in the RTEMS Workspace?

An executive workspace overage can be fairly easily spotted with a
debugger.  Look at _Workspace_Area.  If first == last, then there is only
one free block of memory in the workspace (very likely if no task
deletions).  Then do this:

(gdb) p *(Heap_Block *)_Workspace_Area->first
$3 = @{back_flag = 1, front_flag = 68552, next = 0x1e260, previous = 0x1e25c@}

In this case, I had 68552 bytes left in the workspace.

@subsection How much memory is left in the Heap?

The C heap is a region so this should work:

(gdb) p *((Region_Control *)_Region_Information->local_table[1])->Memory->first
$9 = @{back_flag = 1, front_flag = 8058280, next = 0x7ea5b4,
  previous = 0x7ea5b0@}

In this case, the first block on the C Heap has 8,058,280 bytes left.

@section How do I convert an executable to IEEE-695?

This section is based on an email from Andrew Bythell
<abythell@@nortelnetworks.com> in July 1999.

Using Objcopy to convert m68k-coff to IEEE did not work.  The new IEEE
object could not be read by tools like the XRay BDM Debugger.  

The exact nature of this problem is beyond me, but I did narrow it down to a
problem with objcopy in binutils 2-9.1.  To no surprise, others have
discovered this problem as well, as it has been fixed in later releases.

I compiled a snapshot of the development sources from 07/26/99 and
everything now works as it should.  The development sources are at
@uref{http://sourceware.cygnus.com/binutils} (thanks Ian!)

Additional notes on converting an m68k-coff object for use with XRay (and
others):

@enumerate


@item The m68k-coff object must be built with the -gstabs+ flag.  The -g flag
alone didn't work for me.  

@item Run Objcopy with the --debugging flag to copy debugging information. 

@end enumerate


