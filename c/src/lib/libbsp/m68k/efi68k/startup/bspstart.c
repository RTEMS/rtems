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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <libcsupport.h>

#include <string.h>
 
/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;
 
rtems_unsigned32 Timer_interrupts;

extern void set_debug_traps(void);
extern void breakpoint(void);

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

  bsp_libc_init((void *) heap_start, (RAM_END - heap_start), 0);
 
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
  void           *vbr;
  extern 	unsigned int _WorkspaceBase;

/*   set_debug_traps();    */
/*   breakpoint(); */

  /*
   *  we only use a hook to get the C library initialized.
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;
  Cpu_table.postdriver_hook = bsp_postdriver_hook;

  m68k_get_vbr( vbr );
  Cpu_table.interrupt_vector_table = vbr;

  BSP_Configuration.work_space_start = (void *)
    (((unsigned int)_WorkspaceBase + STACK_SIZE + 0x100) & 0xffffff00);

#if 0
    (((unsigned int)_end + STACK_SIZE + 0x100) & 0xffffff00);
#endif

  /* Clock_exit is done as an atexit() function */
}

