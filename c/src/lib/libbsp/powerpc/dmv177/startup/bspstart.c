/*  bspstart.c
 *
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1997.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
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
rtems_cpu_table                   Cpu_table;
rtems_unsigned32                  bsp_isr_level;

/*
 *  Use the shared implementations of the following routines
 */

void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

/*PAGE
 *
 *  bsp_pretasking_hook
 *
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */

void bsp_pretasking_hook(void)
{
  extern int end;
  rtems_unsigned32 heap_start;
  rtems_unsigned32 heap_size;

  heap_start = (rtems_unsigned32) &end;
  if (heap_start & (CPU_ALIGNMENT-1))
    heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  heap_size = BSP_Configuration.work_space_start - (void *)&end;
  heap_size &= 0xfffffff0;  /* keep it as a multiple of 16 bytes */

  bsp_libc_init((void *) heap_start, heap_size, 0);

#ifdef RTEMS_DEBUG
  rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}

/*  PAGE
 *
 *  bsp_predriver_hook
 *
 *  Initialization before drivers are setup.
 */ 

void bsp_predriver_hook(void)
{
  initialize_external_exception_vector();
}

/*PAGE
 *
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  unsigned char *work_space_start;
  unsigned int  msr_value = 0x2030;

  /*
   * Set BSP to initial value. Note: This value is a guess
   * check how the real board comes up. This is critical to
   * getting the source to work with the debugger.
   */

  _CPU_MSR_SET( msr_value );
   
  /*
   *  Need to "allocate" the memory for the RTEMS Workspace and
   *  tell the RTEMS configuration where it is.  This memory is
   *  not malloc'ed.  It is just "pulled from the air".
   */

  work_space_start = 
    (unsigned char *)&RAM_END - BSP_Configuration.work_space_size;

  if ( work_space_start <= (unsigned char *)&end ) {
    DEBUG_puts( "bspstart: Not enough RAM!!!\n" );
    bsp_cleanup();
  }

  BSP_Configuration.work_space_start = work_space_start;

  /*
   *  initialize the CPU table for this BSP
   */

  Cpu_table.exceptions_in_RAM = TRUE;
  Cpu_table.pretasking_hook   = bsp_pretasking_hook;    /* init libc, etc. */
  Cpu_table.predriver_hook    = bsp_predriver_hook;
  Cpu_table.postdriver_hook   = bsp_postdriver_hook;
  /* Cpu_table.clicks_per_usec   = 66666667 / 4000000; */
  Cpu_table.clicks_per_usec   = 66666667 / 4000000 / 2;

  Cpu_table.do_zero_of_workspace = TRUE;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

  /*
   *  Enable whatever caching is desired
   */

#if ( DMV177_USE_INSTRUCTION_CACHE )
  rtems_cache_enable_instruction();
#endif

#if ( PPC_USE_DATA_CACHE )
  rtems_cache_enable_data();
#endif
}
