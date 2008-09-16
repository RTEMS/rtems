/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <stdint.h>

/*
 *  These are provided by the linkcmds for ALL of the BSPs which use this file.
 */
extern char RamBase[];
extern char WorkAreaBase[];
extern char HeapSize[];
extern char RamSize[];

#if defined(HAS_UBOOT)
  extern bd_t *uboot_bdinfo_ptr;
#endif

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
  #ifdef HAS_UBOOT
    char *ram_end = (char *) uboot_bdinfo_ptr->bi_memstart +
                                 uboot_bdinfo_ptr->bi_memsize;
  #else /* HAS_UBOOT */
    char *ram_end = (uintptr_t)RamBase + (uintptr_t)RamSize;
  #endif /* HAS_UBOOT */

  *work_area_start = bsp_work_area_start;
  *work_area_size = ram_end - bsp_work_area_start;
  *heap_start = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size = HeapSize;
}

