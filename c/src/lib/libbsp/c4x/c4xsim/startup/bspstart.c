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

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */
 
extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;

rtems_cpu_table   Cpu_table;

/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );
extern void bsp_spurious_initialize();

/*
 *  bsp_pretasking_hook
 *
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */

void bsp_pretasking_hook(void)
{
  extern void *_HeapStart;
  extern rtems_unsigned32 _HeapSize;

  bsp_libc_init(&_HeapStart, (unsigned int) &_HeapSize, 0);

#ifdef RTEMS_DEBUG
  rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif

  bsp_spurious_initialize();
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

#include <rtems/bspIo.h>

BSP_output_char_function_type 		BSP_output_char;
BSP_polling_getchar_function_type 	BSP_poll_char;
extern void C4X_BSP_output_char(char c);

void bsp_start( void )
{
  extern void *_WorkspaceBase;
  extern rtems_unsigned32 _WorkspaceMax;
  /*
   * Set up our hooks
   * Make sure libc_init is done before drivers initialized so that
   * they can use atexit()
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;    /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;

  /*
   *  SIS does zero out memory BUT only when IT begins execution.  Thus
   *  if we want to have a clean slate in the workspace each time we
   *  begin execution of OUR application, then we must zero the workspace.
   */
  Cpu_table.do_zero_of_workspace = FALSE;

  /*
   *  This should be enough interrupt stack.
   */

  Cpu_table.interrupt_stack_size = 0;

  BSP_Configuration.work_space_start = (void *)&_WorkspaceBase;
  /* XXX check to see if satisfying small memory model */

  if ( BSP_Configuration.work_space_size > (int) &_WorkspaceMax )
    rtems_fatal_error_occurred( 0x43218765 );
  
  BSP_output_char = C4X_BSP_output_char;
  BSP_poll_char = (BSP_polling_getchar_function_type) NULL;
}
