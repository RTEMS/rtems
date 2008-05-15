/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  bspstart.c,v 1.4.2.1 2003/09/04 18:44:49 joel Exp
 */

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

#define LIBC_HEAP_SIZE (64 * 1024)

extern int end; /* defined by linker */

/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_libc_init( void *, uint32_t, int );

void init_exc_vecs(void);

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
    uint32_t heap_start;

    heap_start = (uint32_t) &end;
    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    bsp_libc_init((void *) heap_start, LIBC_HEAP_SIZE, 0);
}
 
/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  extern int WorkspaceBase;
  extern void mips_install_isr_entries(void);

  Configuration.work_space_start =
       (void *)((uint64_t)((&end) + LIBC_HEAP_SIZE + 0x100) & ~0x7);

  mips_install_isr_entries();  /* Install generic MIPS exception handler */
}

