@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Debugging Hints

The questions in this category are hints that can ease debugging.

@section How do I determine how much memory is left?

First there are two types of memory: RTEMS Workspace and Program Heap.
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
$3 = {back_flag = 1, front_flag = 68552, next = 0x1e260, previous = 0x1e25c}

In this case, I had 68552 bytes left in the workspace.

@subsection How much memory is left in the Heap?

The C heap is a region so this should work:

(gdb) p *((Region_Control *)_Region_Information->local_table[1])->Memory->first
$9 = {back_flag = 1, front_flag = 8058280, next = 0x7ea5b4,
  previous = 0x7ea5b0}

In this case, the first block on the C Heap has 8,058,280 bytes left.

