/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 * 
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
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
#include <zilog/z8036.h>
 
#include <string.h>
 
/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table  Configuration;
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
    rtems_unsigned32        heap_start;

#if 0
    extern int end;
    heap_start = (rtems_unsigned32) &end;
#else
    void * sbrk( int );
    heap_start = (rtems_unsigned32) sbrk( 64 * 1024 + CPU_ALIGNMENT );
#endif
    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    bsp_libc_init((void *) heap_start, 64 * 1024, 0);

 
#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}
 
/*
 *  main/bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

/* This is the original command line passed from DOS */
char **	Go32_Argv;

int main(
  int argc,
  char **argv,
  char **environp
)
{
  extern void * sbrk( int );
  extern volatile void _exit( int );

  /* Set up arguments that we can access later */
  Go32_Argv = argv;

  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS";

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.predriver_hook = NULL;
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.idle_task = NULL;  /* do not override system IDLE task */
  Cpu_table.do_zero_of_workspace = TRUE;
  Cpu_table.interrupt_table_segment = 0;/* get_ds(); */
  Cpu_table.interrupt_table_offset = (void *)0;
  Cpu_table.interrupt_stack_size = 4096;
  Cpu_table.extra_mpci_receive_server_stack = 0;

  /*
   *  Copy the table
   */
  BSP_Configuration = Configuration;

  BSP_Configuration.work_space_start = sbrk( Configuration.work_space_size );
  if ( BSP_Configuration.work_space_start == 0 )  {
    /* Big trouble */
    int write( int, void *, int );
    char msg[] = "bsp_start() couldn't sbrk() RTEMS work space\n";
    write( 2, msg, sizeof msg - 1 );
    _exit( 1 );
  }

  /*
   * Tell libio how many fd's we want and allow it to tweak config
   */

  rtems_libio_config(&BSP_Configuration, BSP_LIBIO_MAX_FDS);

  rtems_initialize_executive( &BSP_Configuration, &Cpu_table );
  /* does not return */

  /* We only return here if the executive has finished.  This happens	*/
  /* when the task has called exit().					*/
  /* At this point we call _exit() which resides in djgcc.		*/
    
  for (;;)
	  _exit( 0 );

  /* no cleanup necessary for GO32 */

  return 0;
}
