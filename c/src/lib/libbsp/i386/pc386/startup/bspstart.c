/*-------------------------------------------------------------------------+
| bspstart.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
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
| *  COPYRIGHT (c) 1989-1998.
| *  On-Line Applications Research Corporation (OAR).
| *  Copyright assigned to U.S. Government, 1994. 
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.OARcorp.com/rtems/license.html.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/


#include <bsp.h>
#include <libcsupport.h>
#include <rtems/libio.h>
 
/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
#ifdef RTEMS_SMALL_MEMORY
extern rtems_unsigned32 _end;           /* End of BSS. Defined in 'linkcmds'. */

rtems_unsigned32 rtemsFreeMemStart = (rtems_unsigned32)&_end;
                         /* Address of start of free memory - should be updated
                            after creating new partitions or regions.         */
#else
rtems_unsigned32 rtemsFreeMemStart = RAM_START;
                                             /* RAM_START defined in 'bsp.h'. */
#endif /* RTEMS_SMALL_MEMORY */

/* The original BSP configuration table from the application and our copy of it
   with some changes. */

extern rtems_configuration_table  Configuration;
       rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;                     /* CPU configuration table.    */
char            *rtems_progname;               /* Program name - from main(). */


/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern void _exit(int);  /* define in exit.c */
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
  if (rtemsFreeMemStart & (CPU_ALIGNMENT - 1))  /* not aligned => align it */
    rtemsFreeMemStart = (rtemsFreeMemStart+CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  bsp_libc_init((void *)rtemsFreeMemStart, HEAP_SIZE << 10, 0);
  rtemsFreeMemStart += HEAP_SIZE << 10;           /* HEAP_SIZE is in KBytes */


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
void bsp_start( void )
{
  /* If we don't have command line arguments set default program name. */

  Cpu_table.pretasking_hook         = bsp_pretasking_hook; /* init libc, etc. */
  Cpu_table.predriver_hook          = NULL;                /* use system's    */
  Cpu_table.postdriver_hook         = bsp_postdriver_hook;
  Cpu_table.idle_task               = NULL;
                                          /* do not override system IDLE task */
  Cpu_table.do_zero_of_workspace    = TRUE;
  Cpu_table.interrupt_table_segment = get_ds();
  Cpu_table.interrupt_table_offset  = (void *)Interrupt_descriptor_table;
  Cpu_table.interrupt_stack_size    = 4096;
  Cpu_table.extra_mpci_receive_server_stack = 0;

  /* Copy user's table and make necessary adjustments.              */

  BSP_Configuration = Configuration;

  /* Place RTEMS workspace at top of physical RAM (RAM_END defined in 'bsp.h' */

  BSP_Configuration.work_space_start =
                          (void *)(RAM_END - BSP_Configuration.work_space_size);

  /* Tell libio how many fd's we want and allow it to tweak config. */

  rtems_libio_config(&BSP_Configuration, BSP_LIBIO_MAX_FDS);

#if 0
  rtems_initialize_executive(&BSP_Configuration, &Cpu_table);
  /* does not return */

  /*-------------------------------------------------------------------------+
  | We only return here if the executive has finished. This happens when the
  | task has called exit(). We will then call _exit() which is part of the bsp.
  +--------------------------------------------------------------------------*/

  for (;;)
    _exit(0);

  /* no cleanup necessary for PC386 */

  return 0;
#endif
} /* bsp_start */
