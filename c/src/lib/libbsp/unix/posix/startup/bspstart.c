/*
 *      @(#)bspstart.c  1.7 - 95/04/07
 *
 */

/*  bsp_start()
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  Called by RTEMS::RTEMS constructor in startup-ctor.cc
 *
 *  INPUT:  NONE
 *
 *  OUTPUT: NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <bsp.h>
#include <libcsupport.h>

#ifdef STACK_CHECKER_ON
#include <stackchk.h>
#endif

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
char                       **rtems_envp;


#define WORKSPACE_SIZE (WORKSPACE_MB * (1024 * 1024))
#define HEAPSPACE_SIZE (HEAPSPACE_MB * (1024 * 1024))

rtems_unsigned8   MY_WORK_SPACE[ WORKSPACE_SIZE ] CPU_STRUCTURE_ALIGNMENT;

/*
 * Amount to increment itimer by each pass
 * It is a variable instead of a #define to allow the 'looptest'
 * script to bump it without recompiling rtems
 */

rtems_unsigned32 CPU_CLICKS_PER_TICK;

/*
 *  Function:   bsp_libc_init
 *  Created:    94/12/6
 *
 *  Description:
 *      Initialize whatever libc we are using
 *      called from bsp_postdriver_hook
 *
 *
 *  Parameters:
 *      none
 *
 *  Returns:
 *      none.
 *
 *  Side Effects:
 *
 *
 *  Notes:
 *
 *  Deficiencies/ToDo:
 *
 *
 */

void
bsp_libc_init(void)
{
    void *heap_start;

    Heap_size = HEAPSPACE_SIZE;
    heap_start = 0;

    RTEMS_Malloc_Initialize((void *)heap_start, Heap_size, 1024 * 1024);

    libc_init(1);
}


/*
 *  Function:   bsp_pretasking_hook
 *  Created:    95/03/10
 *
 *  Description:
 *      BSP pretasking hook.  Called just before drivers are initialized.
 *      Used to setup libc and install any BSP extensions.
 *
 *  Parameters:
 *      none
 *
 *  Returns:
 *      nada
 *
 *  Side Effects:
 *      installs a few extensions
 *
 *  Notes:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 *
 *  Deficiencies/ToDo:
 *
 *
 */

void
bsp_pretasking_hook(void)
{
    bsp_libc_init();

#ifdef STACK_CHECKER_ON
    /*
     *  Initialize the stack bounds checker
     *  We can either turn it on here or from the app.
     */

    Stack_check_Initialize();
#endif

#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}

/*
 *  Function:   bsp_start
 *  Created:    94/12/6
 *
 *  Description:
 *      called by crt0 as our "main" equivalent
 *
 *
 *
 *  Parameters:
 *
 *
 *  Returns:
 *
 *
 *  Side Effects:
 *
 *
 *  Notes:
 *
 *
 *  Deficiencies/ToDo:
 *
 *
 */

void
bsp_start(void)
{
    struct stat  stat_buf;
    char         buf[256];
    char         node[6];
    char        *home;
    int          fd;

    /*
     *  Copy the table
     */

    BSP_Configuration = Configuration;
 
    /*
     * If the node number is -1 then the application better provide
     * it through the file $HOME/rtems_node
     */

    BSP_Multiprocessing.node = -1;

    if (BSP_Configuration.User_multiprocessing_table) {
      if (BSP_Configuration.User_multiprocessing_table->node == -1) {
        home = getenv("HOME");
        sprintf(buf, "%s/%s", home, "rtems_node");
        if ((stat(buf, &stat_buf)) == 0) {
          fd = open(buf, O_RDONLY);
          read(fd, node, 5);
          close(fd);
          unlink(buf);
          BSP_Multiprocessing = *BSP_Configuration.User_multiprocessing_table;
          BSP_Multiprocessing.node = atoi(node);
          BSP_Configuration.User_multiprocessing_table = &BSP_Multiprocessing;
        }
        if (BSP_Configuration.User_multiprocessing_table->maximum_nodes == -1) {
          home = getenv("HOME");
          sprintf(buf, "%s/%s", home, "rtems_max_node");
          if ((stat(buf, &stat_buf)) == 0) {
            fd = open(buf, O_RDONLY);
            read(fd, node, 5);
            close(fd);
            BSP_Multiprocessing.maximum_nodes = atoi(node);
          }
        }
      }
    }

    /*
     * Set cpu_number to accurately reflect our cpu number
     */

    if (BSP_Configuration.User_multiprocessing_table)
        cpu_number = BSP_Configuration.User_multiprocessing_table->node - 1;
    else
        cpu_number = 0;

    BSP_Configuration.work_space_start = (void *)MY_WORK_SPACE;
    if (BSP_Configuration.work_space_size)
        BSP_Configuration.work_space_size = WORKSPACE_SIZE;

    /*
     * Set up our hooks
     * Make sure libc_init is done before drivers init'd so that
     * they can use atexit()
     */

    Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */

    Cpu_table.predriver_hook = NULL;

    Cpu_table.postdriver_hook = NULL;

    Cpu_table.idle_task = NULL;  /* do not override system IDLE task */

    /*
     *  Don't zero out the workspace since it is in the BSS under UNIX.
     */

    Cpu_table.do_zero_of_workspace = FALSE;

    /*
     * XXX; interrupt stack not currently used, so this doesn't matter
     */

    Cpu_table.interrupt_stack_size = (12 * 1024);

    Cpu_table.extra_system_initialization_stack = 0;

    /*
     * Add 1 region for RTEMS Malloc
     */

    BSP_Configuration.maximum_regions++;

#ifdef RTEMS_NEWLIB
    /*
     * Add 1 extension for newlib libc
     */

    BSP_Configuration.maximum_extensions++;
#endif

#ifdef STACK_CHECKER_ON
  /*
   * Add 1 extension for stack checker
   */

    BSP_Configuration.maximum_extensions++;
#endif

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
