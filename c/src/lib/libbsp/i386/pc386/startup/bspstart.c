/*-------------------------------------------------------------------------+
| This file contains the PC386 BSP startup package. It includes application,
| board, and monitor specific initialization and configuration. The generic CPU
| dependent initialization has been performed before this routine is invoked. 
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   bspstart.c,v 1.8 1996/05/28 13:12:40 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1999.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.rtems.com/license/LICENSE.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/


#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <libcpu/cpuModel.h>

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
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
  rtems_unsigned32 topAddr, val;
  int i;
  
  
  if (rtemsFreeMemStart & (CPU_ALIGNMENT - 1))  /* not aligned => align it */
    rtemsFreeMemStart = (rtemsFreeMemStart+CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  if(_heap_size == 0)
    {
      /* 
       * We have to dynamically size memory. Memory size can be anything
       * between 2M and 2048M.
       * let us first write
       */
      for(i=2048; i>=2; i--)
	{
	  topAddr = i*1024*1024 - 4;
	  *(volatile rtems_unsigned32 *)topAddr = topAddr;
	}

      for(i=2; i<=2048; i++)
	{
	  topAddr = i*1024*1024 - 4;
	  val =  *(rtems_unsigned32 *)topAddr;
	  if(val != topAddr)
	    {
	      break;
	    }
	}
      
      topAddr = (i-1)*1024*1024 - 4;

      _heap_size = topAddr - rtemsFreeMemStart;
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
  void Calibrate_loop_1ms(void);
  
  /*
   * Calibrate variable for 1ms-loop (see timer.c)
   */
  Calibrate_loop_1ms();

  rtemsFreeMemStart = (rtems_unsigned32)&_end + _stack_size;
                                    /* set the value of start of free memory. */

  /* If we don't have command line arguments set default program name. */

  Cpu_table.pretasking_hook         = bsp_pretasking_hook; /* init libc, etc. */
  Cpu_table.predriver_hook          = NULL;                /* use system's    */
  Cpu_table.postdriver_hook         = bsp_postdriver_hook;
  Cpu_table.idle_task               = NULL;
                                          /* do not override system IDLE task */
  Cpu_table.do_zero_of_workspace    = TRUE;
  Cpu_table.interrupt_table_segment = get_ds();
  Cpu_table.interrupt_table_offset  = (void *)Interrupt_descriptor_table;
  Cpu_table.interrupt_stack_size    = CONFIGURE_INTERRUPT_STACK_MEMORY;
  Cpu_table.extra_mpci_receive_server_stack = 0;

  /* Place RTEMS workspace at beginning of free memory. */

  if (rtemsFreeMemStart & (CPU_ALIGNMENT - 1))  /* not aligned => align it */
    rtemsFreeMemStart = (rtemsFreeMemStart+CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  BSP_Configuration.work_space_start = (void *)rtemsFreeMemStart;
  rtemsFreeMemStart += BSP_Configuration.work_space_size;

  /*
   * Init rtems interrupt management
   */
  rtems_irq_mngt_init();
  /*
   * Init rtems exceptions management
   */
  rtems_exception_init_mngt();
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
  printk( "_stack_size = 0x%x\n", _stack_size );
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
