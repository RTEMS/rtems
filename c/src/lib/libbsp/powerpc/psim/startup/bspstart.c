/*
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <string.h>
#include <fcntl.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/bspIo.h>

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */
 
extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;

rtems_cpu_table   Cpu_table;
rtems_unsigned32  bsp_isr_level;

/*
 *  Tells us where to put the workspace in case remote debugger is present.
 */

#if 0
extern rtems_unsigned32  rdb_start;
#endif

/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

/*
 *  bsp_pretasking_hook
 *
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */

void bsp_pretasking_hook(void)
{
  extern int end;
  rtems_unsigned32 heap_start;
  rtems_unsigned32 heap_size;

  heap_start = (rtems_unsigned32) &end;
  if (heap_start & (CPU_ALIGNMENT-1))
    heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  heap_size = BSP_Configuration.work_space_start - (void *)&end;
  heap_size &= 0xfffffff0;  /* keep it as a multiple of 16 bytes */

  bsp_libc_init((void *) heap_start, heap_size, 0);

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
  unsigned char *work_space_start;

#if 0
  /* 
   * Set MSR to show vectors at 0 XXX
   */
  _CPU_MSR_Value( msr_value );
  msr_value &= ~PPC_MSR_EP;
  _CPU_MSR_SET( msr_value );
#endif

  /*
   * Set up our hooks
   * Make sure libc_init is done before drivers initialized so that
   * they can use atexit()
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;    /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;

  /*
   *  Is this true?
   *
   *  PSIM does zero out memory BUT only when IT begins execution.  Thus
   *  if we want to have a clean slate in the workspace each time we
   *  begin execution of OUR application, then we must zero the workspace.
   *
   *  It is true that it takes simulated time to clear the memory.
   */

  Cpu_table.do_zero_of_workspace = FALSE;

  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

  /*
   *  The monitor likes the exception table to be at 0x0.
   */

  Cpu_table.exceptions_in_RAM = TRUE;

  BSP_Configuration.work_space_size += 1024;

  work_space_start = 
    (unsigned char *)&RAM_END - BSP_Configuration.work_space_size;

  if ( work_space_start <= (unsigned char *)&end ) {
    printk( "bspstart: Not enough RAM!!!\n" );
    bsp_cleanup();
  }

  BSP_Configuration.work_space_start = work_space_start;

}
