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
 *
 */
 
void bsp_pretasking_hook(void)
{
    extern int heap_bottom;
    rtems_unsigned32 heap_start;
    rtems_unsigned32 heap_size;

    heap_start = (rtems_unsigned32) &heap_bottom;
    if (heap_start & (CPU_ALIGNMENT-1))
      heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    heap_size = BSP_Configuration.work_space_start -(void *) heap_start ;
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

  /*
   *  we do not use the pretasking_hook.
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.interrupt_table_segment = get_ds();
  Cpu_table.interrupt_table_offset = (void *)Interrupt_descriptor_table;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

  BSP_Configuration.work_space_start = (void *)
     RAM_END - BSP_Configuration.work_space_size;

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
     BSP_Configuration.work_space_start );
  printk( "work_space_size = 0x%x\n",
     BSP_Configuration.work_space_size );
  printk( "maximum_extensions = 0x%x\n",
     BSP_Configuration.maximum_extensions );
  printk( "microseconds_per_tick = 0x%x\n",
     BSP_Configuration.microseconds_per_tick );
  printk( "ticks_per_timeslice = 0x%x\n",
     BSP_Configuration.ticks_per_timeslice );
  printk( "maximum_devices = 0x%x\n",
     BSP_Configuration.maximum_devices );
  printk( "number_of_device_drivers = 0x%x\n",
     BSP_Configuration.number_of_device_drivers );
  printk( "Device_driver_table = 0x%x\n",
     BSP_Configuration.Device_driver_table );

  /*  printk( "_heap_size = 0x%x\n", _heap_size );
      printk( "_stack_size = 0x%x\n", _stack_size ); */
#endif
}

void bsp_clean_up(void) {
  printk("bsp_cleanup called\n");
}

