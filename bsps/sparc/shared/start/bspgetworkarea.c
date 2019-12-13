/*
 *  This set of routines are the BSP specific initialization
 *  support routines.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
void bsp_work_area_initialize(void)
{
  /* Early dynamic memory allocator is placed just above _end  */
  void *work_area_start = (void *)&end;
  uintptr_t work_area_size = (uintptr_t)rdb_start - (uintptr_t)work_area_start;

  bsp_work_area_initialize_default(work_area_start, work_area_size);
}
