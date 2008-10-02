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

extern void *__rtems_end;
extern uintptr_t _bsp_sbrk_init(uintptr_t, uintptr_t*);

/*
 *  This method returns the base address and size of the area which
 *  is to be allocated between the RTEMS Workspace and the C Program
 *  Heap.
 */
void bsp_get_work_area(
  void    **work_area_start,
  ssize_t  *work_area_size,
  void    **heap_start,
  ssize_t  *heap_size
)
{
  uintptr_t work_size;
  uintptr_t spared;
  uintptr_t work_area;

  work_area = (uintptr_t)&__rtems_end +
              rtems_configuration_get_interrupt_stack_size();
  work_size = (uintptr_t)BSP_mem_size - work_area;

  spared = _bsp_sbrk_init( work_area, &work_size );

  *work_area_start = (void *)work_area,
  *work_area_size  = work_size;
  *heap_start      = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size       = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;

  /*
   *  The following may be helpful in debugging what goes wrong when
   *  you are allocating the Work Area in a new BSP.
   */
  #if 0
    printk( "Work Area Base %d %x\n", work_area, work_area );
    printk( "Work Area Size %d %x\n", work_size, work_size );
    printk( "Work Area End %d %x\n",
      work_area + work_size, work_area + work_size );
  #endif
}

