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
 *  COPYRIGHT (c) 1989-1998.
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
 
#ifdef STACK_CHECKER_ON
#include <stackchk.h>
#endif

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;

/*      Initialize whatever libc we are using
 *      called from postdriver hook
 */

void bsp_libc_init()
{
/*   extern int end; */
  rtems_unsigned32        heap_start;
  
  heap_start = (rtems_unsigned32) BSP_Configuration.work_space_start +
               (rtems_unsigned32) BSP_Configuration.work_space_size;
  if (heap_start & (CPU_ALIGNMENT-1))
    heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);
  
  if (heap_start > (rtems_unsigned32) RAM_END) {
    /* rtems_fatal_error_occurred can not be used before initalization */
    RAW_PUTS("\n\rRTEMS: Out of memory.\n\r");
    RAW_PUTS("RTEMS:    Check RAM_END and the size of the work space.\n\r");
  }

  RTEMS_Malloc_Initialize((void *) heap_start, 
			  (RAM_END - heap_start), 0);
    
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
  void           *vbr;

  /*
   *  we only use a hook to get the C library initialized.
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;

  Cpu_table.predriver_hook = NULL;

  Cpu_table.postdriver_hook = bsp_postdriver_hook;

  Cpu_table.idle_task = NULL;  /* do not override system IDLE task */

  Cpu_table.do_zero_of_workspace = TRUE;

  m68k_get_vbr( vbr );
  Cpu_table.interrupt_vector_table = vbr;

  Cpu_table.interrupt_stack_size = 0;

  Cpu_table.extra_mpci_receive_server_stack = 0;

  /*
   *  Copy the table
   */

  BSP_Configuration = Configuration;

  BSP_Configuration.work_space_start = (void *)
    (((unsigned int)_end + STACK_SIZE + 0x100) & 0xffffff00);

  /*
   * Tell libio how many fd's we want and allow it to tweak config
   */

  rtems_libio_config(&BSP_Configuration, BSP_LIBIO_MAX_FDS);

  /* Clock_exit is done as an atexit() function */
}

