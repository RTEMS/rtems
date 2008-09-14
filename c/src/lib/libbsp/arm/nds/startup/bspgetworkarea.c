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
 *  These are from the linker script.
 */
extern void * _end;
extern void *__ewram_end;

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
  *work_area_start = &_end;
  *work_area_size  = (void *)&__ewram_end - (void *)&_end;
  *heap_start      = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size       = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;
}

