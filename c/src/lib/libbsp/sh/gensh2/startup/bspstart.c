/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
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
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
 
/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern void bsp_hw_init(void);

extern rtems_configuration_table Configuration;

rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;

/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

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
 
void bsp_pretasking_hook(void)
{
    bsp_libc_init(&HeapStart, sizeof(unsigned32) * (&HeapEnd - &HeapStart), 0);
 
#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start(void)
{
  /*
     For real boards you need to setup the hardware 
     and need to copy the vector table from rom to ram.

     Depending on the board this can ether be done from inside the rom 
     startup code, rtems startup code or here.
   */

#ifndef START_HW_INIT
  /* board hardware setup here, or from 'start.S' */
  bsp_hw_init();
#endif

  /*
   *  Allocate the memory for the RTEMS Work Space.  This can come from
   *  a variety of places: hard coded address, malloc'ed from outside
   *  RTEMS world (e.g. simulator or primitive memory manager), or (as
   *  typically done by stock BSPs) by subtracting the required amount
   *  of work space from the last physical address on the CPU board.
   */

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
  Cpu_table.postdriver_hook = bsp_postdriver_hook;

#if ( CPU_ALLOCATE_INTERRUPT_STACK == TRUE )
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;
#endif

  Cpu_table.clicks_per_second = CPU_CLOCK_RATE_HZ ;
}
