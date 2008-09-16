/*
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
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

#include <string.h>
#include <fcntl.h>
#include <bsp.h>
#include <bsp/bootcard.h>

/*
 * system init stack
 */
#define INIT_STACK_SIZE 0x1000

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
  *work_area_start       = &end;
  *work_area_size       = (void *)&RAM_END - (void *)&end;
  *heap_start = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;
}
