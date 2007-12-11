/*
 *  This routine is an implementation of the bsp_pretasking_hook
 *  that can be used by all m68k BSPs following linkcmds conventions
 *  regarding heap, stack, and workspace allocation.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

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

extern void bsp_libc_init( void *, unsigned long, int );

extern char         _RamBase[];
extern char         _WorkspaceBase[];
extern char         _HeapSize[];

unsigned long  _M68k_Ramsize;

void bsp_pretasking_hook(void)
{
    void         *heapStart;
    unsigned long heapSize = (unsigned long)_HeapSize;
    unsigned long ramSpace;

    heapStart =  (void *)
       ((unsigned long)_WorkspaceBase + rtems_configuration_get_work_space_size());
    ramSpace = (unsigned long)_RamBase + _M68k_Ramsize - (unsigned long)heapStart;

    /*
     * Can't use 'if(heapSize==0)' because the compiler "knows" that nothing
     * can have an address of 0 and proceeds to optimize-away the test.
     */
    if (heapSize < 10)
        heapSize = ramSpace;
    else if (heapSize > ramSpace)
        rtems_fatal_error_occurred (('H'<<24) | ('E'<<16) | ('A'<<8) | 'P');

    bsp_libc_init(heapStart, heapSize, 0);

#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif

}
