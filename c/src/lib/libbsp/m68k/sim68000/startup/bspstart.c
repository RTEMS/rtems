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

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
 
/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table Configuration;

rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;

/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );
void bsp_pretasking_hook(void);               /* m68k version */

void bsp_predriver_hook(void)
{
  void bsp_spurious_initialize();
  bsp_spurious_initialize();
}
/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  extern void          * _WorkspaceBase;
  extern void          *_RamSize;
  extern unsigned long  _M68k_Ramsize;

  _M68k_Ramsize = (unsigned long)&_RamSize; /* RAM size set in linker script */

#if defined(mcpu32)
#warning "do something about vectors!!!"
#endif

  /*
   *  Clear interrupt sources.
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.predriver_hook  = bsp_predriver_hook;
  Cpu_table.postdriver_hook = bsp_postdriver_hook;

/* XXX address shutdown
  if ( BSP_Configuration.work_space_size >(128*1024) )
   _sys_exit( 1 );
*/

  BSP_Configuration.work_space_start = (void *) &_WorkspaceBase;
}

#include <rtems/bspIo.h>

void debug_putc( char c ) { write( 2, &c, 1 ); }
BSP_output_char_function_type BSP_output_char =  debug_putc;
BSP_polling_getchar_function_type BSP_poll_char = NULL;
