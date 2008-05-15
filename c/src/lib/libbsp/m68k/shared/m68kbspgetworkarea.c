/*
 *  This routine is an implementation of the bsp_get_workarea()
 *  that can be used by all m68k BSPs following linkcmds conventions
 *  regarding heap, stack, and workspace allocation.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>

/*
 *  This method returns the base address and size of the area which
 *  is to be allocated between the RTEMS Workspace and the C Program
 *  Heap.
 */
void bsp_get_workarea(
  void   **workarea_base,
  size_t  *workarea_size,
  size_t  *requested_heap_size
)
{
  extern char         _RamBase[];
  extern char         _WorkspaceBase[];
  extern char         _HeapSize[];
  extern void         *_RamSize;

  *workarea_base       = _WorkspaceBase;
  *workarea_size       = (unsigned long)_RamBase + (unsigned long) _RamSize -
       (unsigned long)_WorkspaceBase;
  *requested_heap_size = (size_t) _HeapSize;
}
