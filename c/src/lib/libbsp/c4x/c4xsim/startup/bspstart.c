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
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

/*
 *  Use the shared implementations of the following routines
 */

void bsp_libc_init( void *, uint32_t, int );
extern void bsp_spurious_initialize();

/*
 *  bsp_pretasking_hook
 *
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */

void bsp_pretasking_hook(void)
{
  extern void      *_HeapStart;
  extern uint32_t   _HeapSize;

  bsp_libc_init(&_HeapStart, (unsigned int) &_HeapSize, 0);

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
  extern void     *_WorkspaceBase;
  extern uint32_t _WorkspaceMax;

  Configuration.work_space_start = (void *)&_WorkspaceBase;
  /* XXX check to see if satisfying small memory model */

  if ( rtems_configuration_get_work_space_size() > (int) &_WorkspaceMax )
    rtems_fatal_error_occurred( 0x43218765 );

  BSP_output_char = C4X_BSP_output_char;
  BSP_poll_char = (BSP_polling_getchar_function_type) NULL;
}
