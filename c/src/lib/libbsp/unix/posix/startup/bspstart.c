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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <bsp.h>
#include <libcsupport.h>

#include <rtems/libio.h>

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

    if (getenv("RTEMS_HEAPSPACE_SIZE"))
        Heap_size = strtol(getenv("RTEMS_HEAPSPACE_SIZE"), 0, 0);
    else
        Heap_size = DEFAULT_HEAPSPACE_SIZE;
 
    heap_start = 0;

    RTEMS_Malloc_Initialize((void *)heap_start, Heap_size, 1024 * 1024);

    /*
     *  Init the RTEMS libio facility to provide UNIX-like system
     *  calls for use by newlib (ie: provide __rtems_open, __rtems_close, etc)
     *  Uses malloc() to get area for the iops, so must be after malloc init
     */

    rtems_libio_init();

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

    /*
     * Dump malloc stats on exit...
     */
#if defined(RTEMS_DEBUG)
  atexit(malloc_dump);
#endif
}

/*
 * After drivers are setup, register some "filenames"
 * and open stdin, stdout, stderr files
 *
 * Newlib will automatically associate the files with these
 * (it hardcodes the numbers)
 */
 
void
bsp_postdriver_hook(void)
{
#if 0
  int stdin_fd, stdout_fd, stderr_fd;
  int error_code;
 
  error_code = 'S' << 24 | 'T' << 16;
 
  if ((stdin_fd = __rtems_open("/dev/console", O_RDONLY, 0)) == -1)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '0' );
 
  if ((stdout_fd = __rtems_open("/dev/console", O_WRONLY, 0)) == -1)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '1' );
 
  if ((stderr_fd = __rtems_open("/dev/console", O_WRONLY, 0)) == -1)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '2' );
 
  if ((stdin_fd != 0) || (stdout_fd != 1) || (stderr_fd != 2))
    rtems_fatal_error_occurred( error_code | 'I' << 8 | 'O' );
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
    unsigned32 workspace_ptr;

    /*
     *  Copy the table
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

    Cpu_table.interrupt_stack_size = (12 * 1024);

    Cpu_table.extra_mpci_receive_server_stack = 0;

    /*
     * Add 1 region for RTEMS Malloc
     */

    BSP_Configuration.RTEMS_api_configuration->maximum_regions++;

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
   * Tell libio how many fd's we want and allow it to tweak config
   */

  rtems_libio_config(&BSP_Configuration, BSP_LIBIO_MAX_FDS);

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
