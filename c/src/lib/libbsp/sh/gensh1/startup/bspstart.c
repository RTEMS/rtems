/*  bsp_start()
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  INPUT:  NONE
 *
 *  OUTPUT: NONE
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
 
#include <libcsupport.h>
 
#include <string.h>
#include <fcntl.h>
 
#ifdef STACK_CHECKER_ON
#include <stackchk.h>
#endif

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table Configuration;

rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;

/*      Initialize whatever libc we are using
 *      called from postdriver hook
 */


void bsp_libc_init()
{
    /*
     *  The last parameter to RTEMS_Malloc_Initialize is the "chunk"
     *  size which a multiple of will be requested on each sbrk()
     *  call by malloc().  A value of 0 indicates that sbrk() should
     *  not be called to extend the heap.
     */

    RTEMS_Malloc_Initialize(&HeapStart, sizeof(unsigned32) * (&HeapEnd - &HeapStart), 0);

    /*
     *  Init the RTEMS libio facility to provide UNIX-like system
     *  calls for use by newlib (ie: provide __rtems_open, __rtems_close, etc)
     *  Uses malloc() to get area for the iops, so must be after malloc init
     */

    rtems_libio_init();

    /*
     * Set up for the libc handling.
     */

    if (BSP_Configuration.ticks_per_timeslice > 0)
        libc_init(1);                /* reentrant if possible */
    else
        libc_init(0);                /* non-reentrant */
}

/*
 *  Function:   bsp_pretasking_hook
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
 * After drivers are setup, register some "filenames"
 * and open stdin, stdout, stderr files
 *
 * Newlib will automatically associate the files with these
 * (it hardcodes the numbers)
 */
 
void
bsp_postdriver_hook(void)
{
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
}

int bsp_start(
  int argc,
  char **argv,
  char **environp
)
{
  /*
     For real boards you need to setup the hardware 
     and need to copy the vector table from rom to ram.

     Depending on the board this can ether be done from inside the rom 
     startup code, rtems startup code or here.
   */
   
  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS";

  /*
   *  Allocate the memory for the RTEMS Work Space.  This can come from
   *  a variety of places: hard coded address, malloc'ed from outside
   *  RTEMS world (e.g. simulator or primitive memory manager), or (as
   *  typically done by stock BSPs) by subtracting the required amount
   *  of work space from the last physical address on the CPU board.
   */

  /*
   *  Copy the Configuration Table .. so we can change it
   */

  BSP_Configuration = Configuration;

  /*
   * Add 1 region for the RTEMS Malloc
   */

  BSP_Configuration.RTEMS_api_configuration->maximum_regions++;

  /*
   * Add 1 extension for newlib libc
   */

#ifdef RTEMS_NEWLIB
    BSP_Configuration.maximum_extensions++;
#endif

  /*
   * Add 1 extension for newlib libc
   */

#ifdef RTEMS_NEWLIB
    BSP_Configuration.maximum_extensions++;
#endif

#ifdef STACK_CHECKER_ON
    /*
     * Add 1 extension for stack checker
     */
 
    BSP_Configuration.maximum_extensions++;
#endif

  /*
   *  Need to "allocate" the memory for the RTEMS Workspace and
   *  tell the RTEMS configuration where it is.  This memory is
   *  not malloc'ed.  It is just "pulled from the air".
   */

  BSP_Configuration.work_space_start = (void *) &WorkSpaceStart ;
  BSP_Configuration.work_space_size  = 
    (unsigned32) &WorkSpaceEnd - 
    (unsigned32) &WorkSpaceStart ;
  
  /*
   *  initialize the CPU table for this BSP
   */

#if ( CPU_ALLOCATE_INTERRUPT_STACK == FALSE )
  _CPU_Interrupt_stack_low = &CPU_Interrupt_stack_low ;
  _CPU_Interrupt_stack_high = &CPU_Interrupt_stack_high ;

  /* This isn't used anywhere */
  Cpu_table.interrupt_stack_size = 
    (unsigned32) (&CPU_Interrupt_stack_high) -
    (unsigned32) (&CPU_Interrupt_stack_low) ;
#endif


  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */

  Cpu_table.predriver_hook = NULL;

  Cpu_table.postdriver_hook = bsp_postdriver_hook;

  Cpu_table.idle_task = NULL;  /* do not override system IDLE task */

  Cpu_table.do_zero_of_workspace = TRUE;

#if ( CPU_ALLOCATE_INTERRUPT_STACK == TRUE )
  Cpu_table.interrupt_stack_size = 4096;
#endif

  Cpu_table.extra_mpci_receive_server_stack = 0;

  /*
   *  Don't forget the other CPU Table entries.
   */

  /*
   * Tell libio how many fd's we want and allow it to tweak config
   */

  rtems_libio_config(&BSP_Configuration, BSP_LIBIO_MAX_FDS);

  /*
   *  Start RTEMS
   */

  rtems_initialize_executive( &BSP_Configuration, &Cpu_table );

  bsp_cleanup();

  return 0;
}
