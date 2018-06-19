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

/* #define BSP_GET_WORK_AREA_DEBUG */
#include <bsp.h>
#include <bsp/bootcard.h>
#ifdef BSP_GET_WORK_AREA_DEBUG
  #include <rtems/bspIo.h>
#endif

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

  /*
   *  The following may be helpful in debugging what goes wrong when
   *  you are allocating the Work Area in a new BSP.
   */
  #ifdef BSP_GET_WORK_AREA_DEBUG
    {
      void *sp = __builtin_frame_address(0);
      void *end = *work_area_start + *work_area_size;
      printk(
        "work_area_start = 0x%p\n"
        "work_area_size = %d 0x%08x\n"
        "end = 0x%p\n"
        "current stack pointer = 0x%p%s\n",
        work_area_start,
        work_area_size,  /* decimal */
        work_area_size,  /* hexadecimal */
        end,
        sp,
        ((sp >= *work_area_start && sp <= end) ? " OVERLAPS!" : "")
     );
     printk( "rdb_start = 0x%08x\n", rdb_start );
   }
 #endif

  bsp_work_area_initialize_default(work_area_start, work_area_size);
}
