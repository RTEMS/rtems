/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  Called by RTEMS::RTEMS constructor in startup-ctor.cc
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <bsp.h>
#include <rtems/libcsupport.h>
#include <rtems/libio.h>

uint32_t                     Heap_size;
int                          rtems_argc;
char                       **rtems_argv;

/*
 * May be overridden by RTEMS_WORKSPACE_SIZE and RTEMS_HEAPSPACE_SIZE
 * environment variables; see below.
 */

#define DEFAULT_WORKSPACE_SIZE (WORKSPACE_MB * (1024 * 1024))
#define DEFAULT_HEAPSPACE_SIZE (HEAPSPACE_MB * (1024 * 1024))

/*
 * Amount to increment itimer by each pass
 * It is a variable instead of a #define to allow the 'looptest'
 * script to bump it without recompiling rtems
 */

uint32_t         CPU_CLICKS_PER_TICK;

/*
 *  Use the shared implementations of the following routines
 */

void bsp_postdriver_hook(void);
void bsp_libc_init( void *, uint32_t, int );

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
 */

void bsp_pretasking_hook(void)
{
    void *heap_start;

    if (getenv("RTEMS_HEAPSPACE_SIZE"))
        Heap_size = strtol(getenv("RTEMS_HEAPSPACE_SIZE"), 0, 0);
    else
        Heap_size = DEFAULT_HEAPSPACE_SIZE;

    heap_start = 0;

    bsp_libc_init((void *)heap_start, Heap_size, 1024 * 1024);

#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif

    /*
     * Dump malloc stats on exit...
     */
#if defined(RTEMS_DEBUG)
  atexit(malloc_dump);
#endif
}

/*
 *  DO NOT Use the shared bsp_postdriver_hook() implementation
 */

void bsp_postdriver_hook(void)
{
  return;
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start(void)
{
    uintptr_t   workspace_ptr;

    /*
     *  If the node number is -1 then the application better provide
     *  it through environment variables RTEMS_NODE.
     *  Ditto for RTEMS_MAXIMUM_NODES
     */

    if (Configuration.User_multiprocessing_table) {
        char *p;

        if (Configuration.User_multiprocessing_table->node == -1) {
            p = getenv("RTEMS_NODE");
            Configuration.User_multiprocessing_table->node = p ? atoi(p) : 1;
        }

        /* If needed provide maximum_nodes also */
        if (Configuration.User_multiprocessing_table->maximum_nodes == -1) {
            p = getenv("RTEMS_MAXIMUM_NODES");
            Configuration.User_multiprocessing_table->maximum_nodes = p ? atoi(p) : 1;
        }
    }

    /*
     * Set cpu_number to accurately reflect our cpu number
     */

    if (Configuration.User_multiprocessing_table->User_multiprocessing_table)
        cpu_number = Configuration.User_multiprocessing_table->node - 1;
    else
        cpu_number = 0;

    if (getenv("RTEMS_WORKSPACE_SIZE"))
        rtems_configuration_get_work_space_size() =
           strtol(getenv("RTEMS_WORKSPACE_SIZE"), 0, 0);
    else
        rtems_configuration_get_work_space_size() = DEFAULT_WORKSPACE_SIZE;

    /*
     * Allocate workspace memory, ensuring it is properly aligned
     */

    workspace_ptr =
      (uintptr_t) sbrk(rtems_configuration_get_work_space_size() + CPU_ALIGNMENT);
    workspace_ptr += CPU_ALIGNMENT - 1;
    workspace_ptr &= ~(CPU_ALIGNMENT - 1);

    Configuration.work_space_start = (void *) workspace_ptr;

    CPU_CLICKS_PER_TICK = 1;

}
