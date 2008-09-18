/*
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
#include <rtems/libcsupport.h>
#include <rtems/libio.h>

void bsp_get_work_area(
  void   **work_area_start,
  size_t  *work_area_size,
  void   **heap_start,
  size_t  *heap_size
)
{
  uintptr_t   workspace_ptr;

  workspace_ptr = (uintptr_t)
     sbrk(rtems_configuration_get_work_space_size() + CPU_ALIGNMENT);
  workspace_ptr += CPU_ALIGNMENT - 1;
  workspace_ptr &= ~(CPU_ALIGNMENT - 1);

  /* start with requested workspace + 1 MB for heap */
  *work_area_start = (void *) workspace_ptr;
  *work_area_size  = rtems_configuration_get_work_space_size() +
      (1 * 1024 * 1024);
  *heap_start      = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size       = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;
}
