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
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 *
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/bspIo.h>

/*
 *  LEON2 Cache Snooping Support
 */
#ifdef LEON2
  /*
   * Tells us if data cache snooping is available
   */ 
  int CPU_SPARC_HAS_SNOOPING;

  /*
   * set_snooping
   * 
   * Read the data cache configuration register to determine if
   * bus snooping is available. This is needed for some drivers so
   * that they can select the most efficient copy routines.  
   */
  static inline int set_snooping(void) 
  {
    unsigned int tmp = *(unsigned int *)0x80000014; /* Cache control register */
    return ((tmp>>23) & 1); /* Data cache snooping enabled */
  }
#endif

/*
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */
void bsp_pretasking_hook(void)
{
  extern void bsp_spurious_initialize();

  bsp_spurious_initialize();
}

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
  /* Tells us where to put the workspace in case remote debugger is present.  */
  extern uint32_t rdb_start;
  /* must be identical to STACK_SIZE in start.S */
  #define STACK_SIZE (16 * 1024)

  *work_area_start       = &end;
  *work_area_size       = (void *)rdb_start - (void *)&end - STACK_SIZE;
  *heap_start = BSP_BOOTCARD_HEAP_USES_WORK_AREA;
  *heap_size = BSP_BOOTCARD_HEAP_SIZE_DEFAULT;
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  #ifdef LEON2
    CPU_SPARC_HAS_SNOOPING = set_snooping();
  #endif
}
