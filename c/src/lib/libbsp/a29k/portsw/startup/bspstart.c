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
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
 
#ifndef lint
static char _sccsid[] = "@(#)bspstart.c 09/11/96     1.15\n";
#endif

int CPU_CLOCK_RATE_MHZ = 25;

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table Configuration;

rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;

/*      Initialize whatever libc we are using
 *      called from postdriver hook
 */

#define HEAP_BLOCK_SIZE (16 * 1024)

rtems_unsigned32        heap_size = 0;
rtems_unsigned32        heap_start;

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
   /* allocate a maximum of 2 megabytes for the heap */
    heap_size = 2 * 1024 * 1024;

    /* allocate all remaining memory to the heap */
    do {
       heap_size -= HEAP_BLOCK_SIZE;
/* JRS just to link 9/22/2000 */
#if 0
       heap_start = _sysalloc( heap_size );
#else
       heap_start = 0;
#endif
    } while ( !heap_start );

    if (!heap_start)
       rtems_fatal_error_occurred( heap_size );

    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);


    bsp_libc_init((void *) heap_start, heap_size, 0);


#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}
 
/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

int bsp_start(
  int argc,
  char **argv,
  char **environp
)
{
  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS";

  /* set the PIA0 register wait states */
  *(volatile unsigned32 *)0x80000020 = 0x04080000;

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

/* JRS just to link 9/22/2000 */
#if 0
  BSP_Configuration.work_space_start = _sysalloc( BSP_Configuration.work_space_size + 512 );
#else
  BSP_Configuration.work_space_start = 0;
#endif
  if (!BSP_Configuration.work_space_start)
    rtems_fatal_error_occurred( BSP_Configuration.work_space_size );
   
  BSP_Configuration.work_space_start = (void *) ((unsigned int)((char *)BSP_Configuration.work_space_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1));

  /*
   *  initialize the CPU table for this BSP
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

/* JRS just to link 9/22/2000 */
#if 0
  _settrap( 109,&a29k_enable_sup);
  _settrap( 110,&a29k_disable_sup);
  _settrap( 111,&a29k_enable_all_sup);
  _settrap( 112,&a29k_disable_all_sup);
  _settrap( 106,&a29k_context_switch_sup);
  _settrap( 107,&a29k_context_restore_sup);
  _settrap( 108,&a29k_context_save_sup);
  _settrap( 105,&a29k_sigdfl_sup);
#endif

  /*
   *  Start RTEMS
   */

  rtems_initialize_executive( &BSP_Configuration, &Cpu_table );

  bsp_cleanup();

  return 0;
}
