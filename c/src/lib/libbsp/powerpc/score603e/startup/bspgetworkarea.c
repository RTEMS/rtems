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

extern void *RAM_END;
extern void *__rtems_end;
extern uint32_t _bsp_sbrk_init(uint32_t, uint32_t*);

unsigned int BSP_mem_size = (unsigned int)&RAM_END;

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
  uintptr_t size;
  uintptr_t reserve;

  reserve  = (uintptr_t)BSP_INIT_STACK_SIZE;
  reserve += rtems_configuration_get_interrupt_stack_size();
  size     = (uintptr_t)&RAM_END - (uintptr_t)&__rtems_end + reserve;

  *work_area_start = (void *)(&__rtems_end + reserve);
  *work_area_size  = size;
  _bsp_sbrk_init( *work_area_start, work_area_size );
  *heap_start      = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size       = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;
}

