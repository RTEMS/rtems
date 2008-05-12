/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to the i386ex and submitted by:
 *
 *    Erik Ivanenko
 *    University of Toronto
 *    erik.ivanenko@utoronto.ca
 *
 *  $Id$
 */

void bsp_clean_up(void);

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

/*
 *  Tells us where to put the workspace in case remote debugger is present.
 */

extern uint32_t          rdb_start;

/*
 *  Use the shared implementations of the following routines
 */

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
 *
 */

void bsp_pretasking_hook(void)
{
    extern int heap_bottom;
    uint32_t         heap_start;
    uint32_t         heap_size;

    heap_start = (uint32_t) &heap_bottom;
    if (heap_start & (CPU_ALIGNMENT-1))
      heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    heap_size = Configuration.work_space_start -(void *) heap_start ;
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
  void rtems_irq_mngt_init();

  Configuration.work_space_start = (void *)
     RAM_END - rtems_configuration_get_work_space_size();

  /*
   * Init rtems_interrupt_management
   */
  rtems_irq_mngt_init();

  /*
   * Init rtems exceptions management
   */
  rtems_exception_init_mngt();

  /*
   *  The following information is very useful when debugging.
   */

#ifdef BSP_DEBUG
  printk( "RAM_START = 0x%x\nRAM_END = 0x%x\n", RAM_START, RAM_END );
  printk( "work_space_start = 0x%x\n",
     Configuration.work_space_start );
  printk( "work_space_size = 0x%x\n",
     rtems_configuration_get_work_space_size() );
  printk( "microseconds_per_tick = 0x%x\n",
     rtems_configuration_get_microseconds_per_tick() );
  printk( "ticks_per_timeslice = 0x%x\n",
     rtems_configuration_get_ticks_per_timeslice() );

  /*  printk( "_heap_size = 0x%x\n", _heap_size );
      printk( "_stack_size = 0x%x\n", _stack_size ); */
#endif
}

void bsp_clean_up(void) {
  printk("bsp_cleanup called\n");
}
