/*
 *  This routine is an implementation of the bsp_get_work_area()
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
#include <bsp/bootcard.h>

/*
 *  These are provided by the linkcmds for ALL of the BSPs which use this file.
 */
extern char WorkAreaBase[];
extern char HeapSize[];

/*
 *  We may get the size information from U-Boot or the linker scripts.
 */
#ifdef HAS_UBOOT
  extern bd_t bsp_uboot_board_info;
#else
  extern char RamBase[];
  extern char RamSize[];
#endif /* HAS_UBOOT */

/*
 *  This method returns the base address and size of the area which
 *  is to be allocated between the RTEMS Workspace and the C Program
 *  Heap.
 */
void bsp_get_work_area(
  void   **work_area_start,
  size_t  *work_area_size,
  void   **heap_start,
  size_t  *heap_size
)
{
  uintptr_t ram_end;

  #ifdef HAS_UBOOT
    ram_end = (uintptr_t) bsp_uboot_board_info.bi_memstart +
                          bsp_uboot_board_info.bi_memsize;
  #else
    ram_end = RamBase + (uintptr_t)RamSize;
  #endif

  *work_area_start = WorkAreaBase;
  *work_area_size  = ram_end = (uintptr_t) WorkAreaBase;
  *heap_start      = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size       = (size_t) HeapSize;
}
