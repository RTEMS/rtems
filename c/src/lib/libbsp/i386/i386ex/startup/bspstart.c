/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Ported to the i386ex and submitted by:
 *
 *    Erik Ivanenko 
 *    University of Toronto
 *    erik.ivanenko@utoronto.ca
 *  
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
 
#include <libcsupport.h>

#ifdef STACK_CHECKER_ON
#include <stackchk.h>
#endif

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table  Configuration;
rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

/*
 *  Tells us where to put the workspace in case remote debugger is present.
 */

extern rtems_unsigned32  rdb_start;

/*
 *  bsp_libc_init
 *
 *  Initialize whatever libc we are using called from bsp_postdriver_hook.
 */

 
void bsp_libc_init()
{
    extern int heap_bottom;
    rtems_unsigned32 heap_start;
    rtems_unsigned32 heap_size;

    heap_start = (rtems_unsigned32) &heap_bottom;
    if (heap_start & (CPU_ALIGNMENT-1))
      heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    heap_size = BSP_Configuration.work_space_start -(void *) heap_start ;
    heap_size &= 0xfffffff0;  /* keep it as a multiple of 16 bytes */

    heap_size &= 0xfffffff0;  /* keep it as a multiple of 16 bytes */
    RTEMS_Malloc_Initialize((void *) heap_start, heap_size, 0);

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
 *  Use the shared bsp_postdriver_hook() implementation 
 */
 
void bsp_postdriver_hook(void);

void bsp_start( void )
{
  /*
   *  we do not use the pretasking_hook.
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
 
  Cpu_table.predriver_hook = NULL;
 
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
 
  Cpu_table.idle_task = NULL;  /* do not override system IDLE task */
 
  Cpu_table.do_zero_of_workspace = TRUE;
 
  Cpu_table.interrupt_table_segment = get_ds();
 
  Cpu_table.interrupt_table_offset = (void *)Interrupt_descriptor_table;
 
  Cpu_table.interrupt_stack_size = 4096;  /* STACK_MINIMUM_SIZE */
 
  Cpu_table.extra_mpci_receive_server_stack = 0;

  /*
   *  Copy the table
   */

  BSP_Configuration = Configuration;

#if defined(RTEMS_POSIX_API)
  BSP_Configuration.work_space_size *= 3;
#endif

  BSP_Configuration.work_space_start = (void *)
     RAM_END - BSP_Configuration.work_space_size;

  /*
   *  Account for the console's resources
   */

  /*   console_reserve_resources( &BSP_Configuration ); */


  /*
   * Add another extension if using the stack checker
   */

#ifdef STACK_CHECKER_ON
    BSP_Configuration.maximum_extensions++;
#endif

}
