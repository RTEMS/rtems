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

extern rtems_configuration_table  Configuration;

/*
 * A copy of the configuration table from the application
 * with some changes applied to it.
 */

rtems_configuration_table    BSP_Configuration;
rtems_multiprocessing_table  BSP_Multiprocessing;
rtems_cpu_table              Cpu_table;
rtems_unsigned32             bsp_isr_level;
rtems_unsigned32             Heap_size;
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

rtems_unsigned32 CPU_CLICKS_PER_TICK;

/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

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
    unsigned32 workspace_ptr;

    /*
     *  Copy the table (normally done in shared main).
     */

    BSP_Configuration = Configuration;
 
    /*
     *  If the node number is -1 then the application better provide
     *  it through environment variables RTEMS_NODE.
     *  Ditto for RTEMS_MAXIMUM_NODES
     */

    if (BSP_Configuration.User_multiprocessing_table) {
        char *p;
 
        /* make a copy for possible editing */
        BSP_Multiprocessing = *BSP_Configuration.User_multiprocessing_table;
        BSP_Configuration.User_multiprocessing_table = &BSP_Multiprocessing;
 
        if (BSP_Multiprocessing.node == -1)
        {
            p = getenv("RTEMS_NODE");
            BSP_Multiprocessing.node = p ? atoi(p) : 1;
        }
 
        /* If needed provide maximum_nodes also */
        if (BSP_Multiprocessing.maximum_nodes == -1)
        {
            p = getenv("RTEMS_MAXIMUM_NODES");
            BSP_Multiprocessing.maximum_nodes = p ? atoi(p) : 1;
        }
    }

    /*
     * Set cpu_number to accurately reflect our cpu number
     */

    if (BSP_Configuration.User_multiprocessing_table)
        cpu_number = BSP_Configuration.User_multiprocessing_table->node - 1;
    else
        cpu_number = 0;

    if (getenv("RTEMS_WORKSPACE_SIZE"))
        BSP_Configuration.work_space_size = 
           strtol(getenv("RTEMS_WORKSPACE_SIZE"), 0, 0);
    else
        BSP_Configuration.work_space_size = DEFAULT_WORKSPACE_SIZE;
 
    /*
     * Allocate workspace memory, ensuring it is properly aligned
     */
 
    workspace_ptr = 
      (unsigned32) sbrk(BSP_Configuration.work_space_size + CPU_ALIGNMENT);
    workspace_ptr += CPU_ALIGNMENT - 1;
    workspace_ptr &= ~(CPU_ALIGNMENT - 1);

    BSP_Configuration.work_space_start = (void *) workspace_ptr;
 
    /*
     * Set up our hooks
     * Make sure libc_init is done before drivers init'd so that
     * they can use atexit()
     */

    Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */

    Cpu_table.predriver_hook = NULL;

    Cpu_table.postdriver_hook = bsp_postdriver_hook;

    Cpu_table.idle_task = NULL;  /* do not override system IDLE task */

    /*
     *  Don't zero out the workspace since it is in the BSS under UNIX.
     */

    Cpu_table.do_zero_of_workspace = FALSE;

    /*
     * XXX; interrupt stack not currently used, so this doesn't matter
     */

    Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

    Cpu_table.extra_mpci_receive_server_stack = 0;

    /*
     * Add 1 extension for MPCI_fatal
     */

    if (BSP_Configuration.User_multiprocessing_table)
        BSP_Configuration.maximum_extensions++;

    CPU_CLICKS_PER_TICK = 1;

    /*
     *  Start most of RTEMS
     *  main() will start the rest
     */

    bsp_isr_level = rtems_initialize_executive_early(
      &BSP_Configuration,
      &Cpu_table
    );
}
