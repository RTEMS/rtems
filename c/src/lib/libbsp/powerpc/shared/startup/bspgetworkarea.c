/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

#include <libcpu/powerpc-utility.h>

LINKER_SYMBOL(__rtems_end)

void bsp_work_area_initialize(void)
{
  uintptr_t work_size;
  uintptr_t work_area;

  work_area = (uintptr_t)__rtems_end +
              rtems_configuration_get_interrupt_stack_size();
  work_size = (uintptr_t)BSP_mem_size - work_area;

  bsp_work_area_initialize_default((void *) work_area, work_size);
}
