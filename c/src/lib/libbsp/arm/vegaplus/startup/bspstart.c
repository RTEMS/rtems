/*-------------------------------------------------------------------------+
| This file contains the ARM BSP startup package. It includes application,
| board, and monitor specific initialization and configuration. The generic CPU
| dependent initialization has been performed before this routine is invoked. 
+--------------------------------------------------------------------------+
|
| Copyright (c) 2000 Canon Research Centre France SA.
| Emmanuel Raguet, mailto:raguet@crf.canon.fr
|
|   The license and distribution terms for this file may be
|   found in found in the file LICENSE in this distribution or at
|   http://www.OARcorp.com/rtems/license.html.
|
+--------------------------------------------------------------------------*/


#include <bsp.h>
#include <rtems/libcsupport.h>
#include <rtems/libio.h>

#include <uart.h>

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
volatile unsigned long *Regs = (unsigned long*)0xF0000; 		/* Chip registers */

extern rtems_unsigned32 _end;         /* End of BSS. Defined in 'linkcmds'. */
/* 
 * Size of heap if it is 0 it will be dynamically defined by memory size, 
 * otherwise the value should be changed by binary patch 
 */
rtems_unsigned32 _heap_size = 0; 

/* Size of stack used during initialization. Defined in 'start.s'.  */
extern rtems_unsigned32 _stack_size;

rtems_unsigned32 rtemsFreeMemStart;
                         /* Address of start of free memory - should be updated
                            after creating new partitions or regions.         */

/* The original BSP configuration table from the application and our copy of it
   with some changes. */

extern rtems_configuration_table  Configuration;
       rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;                     /* CPU configuration table.    */
char            *rtems_progname;               /* Program name - from main(). */

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern void rtems_irq_mngt_init(void);
void bsp_libc_init( void *, unsigned32, int );
void bsp_postdriver_hook(void);

/*-------------------------------------------------------------------------+
|         Function: bsp_pretasking_hook
|      Description: BSP pretasking hook.  Called just before drivers are
|                   initialized. Used to setup libc and install any BSP
|                   extensions. NOTE: Must not use libc (to do io) from here,
|                   since drivers are not yet initialized.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void bsp_pretasking_hook(void)
{

  if(_heap_size == 0)
    {
      _heap_size = 0x420000 - rtemsFreeMemStart;
    }
        
  bsp_libc_init((void *)rtemsFreeMemStart, _heap_size, 0);
  
  rtemsFreeMemStart += _heap_size;           /* HEAP_SIZE  in KBytes */


#ifdef RTEMS_DEBUG

  rtems_debug_enable(RTEMS_DEBUG_ALL_MASK);

#endif /* RTEMS_DEBUG */

} /* bsp_pretasking_hook */
 

/*-------------------------------------------------------------------------+
|         Function: bsp_start
|      Description: Called before main is invoked.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void bsp_start_default( void )
{
  rtemsFreeMemStart = (rtems_unsigned32)(&_end);  /* &_end+_stack_size;*/
                                    /* set the value of start of free memory. */

  /* If we don't have command line arguments set default program name. */

  Cpu_table.pretasking_hook         = bsp_pretasking_hook; /* init libc, etc. */
  Cpu_table.predriver_hook          = NULL;                /* use system's    */
  Cpu_table.postdriver_hook         = bsp_postdriver_hook;
  Cpu_table.idle_task               = NULL;
                                          /* do not override system IDLE task */
  Cpu_table.do_zero_of_workspace    = TRUE;
  Cpu_table.interrupt_stack_size    = 4096;
  Cpu_table.extra_mpci_receive_server_stack = 0;

  /* Place RTEMS workspace at beginning of free memory. */
  BSP_Configuration.work_space_start = (void *)rtemsFreeMemStart;
 
   rtemsFreeMemStart += BSP_Configuration.work_space_size;

  /*
   * Init rtems exceptions management
   */
  rtems_exception_init_mngt();

  /*
   * Init rtems interrupt management
   */
  rtems_irq_mngt_init();

  /*
   *  The following information is very useful when debugging.
   */

#if 0
  printk( "work_space_size = 0x%x\n", BSP_Configuration.work_space_size );
  printk( "maximum_extensions = 0x%x\n", BSP_Configuration.maximum_extensions );
  printk( "microseconds_per_tick = 0x%x\n",
     BSP_Configuration.microseconds_per_tick );
  printk( "ticks_per_timeslice = 0x%x\n",
     BSP_Configuration.ticks_per_timeslice );
  printk( "maximum_devices = 0x%x\n", BSP_Configuration.maximum_devices );
  printk( "number_of_device_drivers = 0x%x\n",
     BSP_Configuration.number_of_device_drivers );
  printk( "Device_driver_table = 0x%x\n",
     BSP_Configuration.Device_driver_table );

  printk( "_heap_size = 0x%x\n", _heap_size );
  /*  printk( "_stack_size = 0x%x\n", _stack_size );*/
  printk( "rtemsFreeMemStart = 0x%x\n", rtemsFreeMemStart );
  printk( "work_space_start = 0x%x\n", BSP_Configuration.work_space_start );
  printk( "work_space_size = 0x%x\n", BSP_Configuration.work_space_size );
#endif

} /* bsp_start */

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */

void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
