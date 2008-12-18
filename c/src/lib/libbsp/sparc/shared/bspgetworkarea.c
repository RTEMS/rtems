/*
 *  This set of routines are the BSP specific initialization
 *  support routines.
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

/* Tells us where to put the workspace in case remote debugger is present.  */
extern uint32_t rdb_start;

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
  /* must be identical to STACK_SIZE in start.S */
  #define STACK_SIZE (16 * 1024)

  *work_area_start       = &end;
  *work_area_size       = (void *)rdb_start - (void *)&end - STACK_SIZE;
  *heap_start = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;
}
