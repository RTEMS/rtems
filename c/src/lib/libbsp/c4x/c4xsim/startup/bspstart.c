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

#include <bsp.h>

/*
 *  Use the shared implementations of the following routines
 */

extern void bsp_spurious_initialize();

/*
 *  bsp_pretasking_hook
 *
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */

void bsp_pretasking_hook(void)
{
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
  BSP_output_char = C4X_BSP_output_char;
  BSP_poll_char = (BSP_polling_getchar_function_type) NULL;
}
