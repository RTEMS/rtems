/*
 *  This routine is an implementation of the bsp_get_work_area()
 *  that can be used by all BSPs following linkcmds conventions
 *  regarding heap, stack, and workspace allocation.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#ifdef BSP_GET_WORK_AREA_DEBUG
  #include <rtems/bspIo.h>
#endif

/*
 *  These are provided by the linkcmds for ALL of the BSPs which use this file.
 */
extern char WorkAreaBase[];
extern char HeapSize[];
extern char HeapBase[];


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
  void      **work_area_start,
  uintptr_t  *work_area_size,
  void      **heap_start,
  uintptr_t  *heap_size
)
{
  uintptr_t ram_end;

  #ifdef HAS_UBOOT
    ram_end = (uintptr_t) bsp_uboot_board_info.bi_memstart +
                          bsp_uboot_board_info.bi_memsize;
  #else
    ram_end = (uintptr_t)RamBase + (uintptr_t)RamSize;
  #endif

  *work_area_start = WorkAreaBase;
  *work_area_size  = (uintptr_t) HeapSize;
  *heap_start      = (void*) HeapBase;
  *heap_size       = (uintptr_t) HeapSize;

  /*
   *  The following may be helpful in debugging what goes wrong when
   *  you are allocating the Work Area in a new BSP.
   */
  #ifdef BSP_GET_WORK_AREA_DEBUG
    {
      void *sp = __builtin_frame_address(0);
      void *end = *work_area_start + *work_area_size;
      printk(
		"bsp_get_work_area\n\r"
        "work_area_start = 0x%p\n\r"
        "work_area_size = %d 0x%08x\n\r"
        "end = 0x%p\n\r"
        "heap_start = 0x%p\n\r"
        "heap_size = %d\n\r"
        "current stack pointer = 0x%p%s\n\r",
        *work_area_start,
        *work_area_size,  /* decimal */
        *work_area_size,  /* hexadecimal */
        end,
        *heap_start,
        *heap_size,
        sp,
        ((sp >= *work_area_start && sp <= end) ? " OVERLAPS!" : "")
     );
  }
 #endif
}
