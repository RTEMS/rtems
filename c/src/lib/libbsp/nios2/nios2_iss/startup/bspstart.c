/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 2005-2006 Kolja Waschk rtemsdev/ixo.de
 *  Derived from no_cpu/no_bsp/startup/bspstart.c 1.23.
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

/*
 *  Use the shared implementations of the following routines
 */

extern void bsp_postdriver_hook(void);
extern void bsp_libc_init( void *, uint32_t, int );

#if 0
extern char         _RAMBase[];
extern char         _RAMSize[];
extern char         _WorkspaceBase[];
extern char         _HeapSize[];
#else
extern char __alt_heap_start[];
#endif

/*
 *  Function:   bsp_pretasking_hook
 *  Created:    95/03/10
 *
 *  Description:
 *      BSP pretasking hook.  Called just before drivers are initialized.
 *      Used to setup libc and install any BSP extensions.
 *
 *  NOTES:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 *
 */

void bsp_pretasking_hook(void)
{
    unsigned long heapStart;
#if 0
    unsigned long heapSize = (unsigned long)_HeapSize;
#endif
    unsigned long ramSpace;

    heapStart = (unsigned long)Configuration.work_space_start
              + rtems_configuration_get_work_space_size();

    if (heapStart & (CPU_ALIGNMENT-1))
        heapStart = (heapStart + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

#if 0
    ramSpace = (unsigned long)_RAMBase + (unsigned long)_RAMSize - heapStart;
#else
#if 0
    ramSpace = SRAM_0_BASE 
             + (SRAM_0_SRAM_MEMORY_SIZE * SRAM_0_SRAM_MEMORY_UNITS)
             - heapStart;
#else
    ramSpace = RAM_BASE + RAM_BYTES - heapStart;
#endif
#endif

    /* TODO */
    ramSpace -= 16384; /* Space for initial stack, not to be zeroed */

#if 0
    if (heapSize < 10)
        heapSize = ramSpace;
    else if (heapSize > ramSpace)
        rtems_fatal_error_occurred (('H'<<24) | ('E'<<16) | ('A'<<8) | 'P');

    bsp_libc_init((void *)heapStart, heapSize, 0);
#else
    bsp_libc_init((void *)heapStart, ramSpace, 0);
#endif

#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  /*
   *  Need to "allocate" the memory for the RTEMS Workspace and
   *  tell the RTEMS configuration where it is.  This memory is
   *  not malloc'ed.  It is just "pulled from the air".
   */

#if 0
  Configuration.work_space_start = (void *)_WorkspaceBase;
#else
  Configuration.work_space_start = (void *)__alt_heap_start;
#endif

}
