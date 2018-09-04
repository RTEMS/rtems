/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

#include <libcpu/powerpc-utility.h>

#include <rtems/sysinit.h>

LINKER_SYMBOL(__rtems_end)

void bsp_work_area_initialize(void)
{
  /*
   * Cannot do work area initialization before bsp_start(), since BSP_mem_size
   * and MMU is not set up.
   */
}

static void bsp_work_area_initialize_later(void)
{
  uintptr_t work_size;
  uintptr_t work_area;

  work_area = (uintptr_t)__rtems_end;
  work_size = (uintptr_t)BSP_mem_size - work_area;

  bsp_work_area_initialize_default((void *) work_area, work_size);
}

RTEMS_SYSINIT_ITEM(
  bsp_work_area_initialize_later,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST
);
