/*  bspstart.c
 *
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  Called by RTEMS::RTEMS constructor in rtems-ctor.cc
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1997.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>

#include <libcsupport.h>

#include <string.h>
#include <fcntl.h>

#ifdef STACK_CHECKER_ON
#include <stackchk.h>
#endif

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */
 
extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;
rtems_cpu_table                   Cpu_table;
rtems_unsigned32                  bsp_isr_level;

/*  PAGE
 *
 *  bsp_libc_init
 *
 *  Initialize whatever libc we are using called from bsp_postdriver_hook.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values: NONE
 *  
 */

void bsp_libc_init(void)
{
  extern int end;
  rtems_unsigned32 heap_start;
  rtems_unsigned32 heap_size;

  heap_start = (rtems_unsigned32) &end;
  if (heap_start & (CPU_ALIGNMENT-1))
    heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  heap_size = BSP_Configuration.work_space_start - (void *)&end;
  heap_size &= 0xfffffff0;  /* keep it as a multiple of 16 bytes */

  RTEMS_Malloc_Initialize((void *) heap_start, heap_size, 0);

  /*
   *  Init the RTEMS libio facility to provide UNIX-like system
   *  calls for use by newlib (ie: provide __rtems_open, __rtems_close, etc)
   *  Uses malloc() to get area for the iops, so must be after malloc init
   */
 
  rtems_libio_init();
 
  /*
   * Set up for the libc handling.
   */
 
  if (BSP_Configuration.ticks_per_timeslice > 0)
    libc_init(1);                /* reentrant if possible */
  else
    libc_init(0);                /* non-reentrant */

}


/*  PAGE
 *
 *  bsp_pretasking_hook
 *
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values: NONE
 *  
 */

void bsp_pretasking_hook(void)
{
  bsp_libc_init();

#ifdef STACK_CHECKER_ON
  /*
   *  Initialize the stack bounds checker
   *  We can either turn it on here or from the app.
   */

  Stack_check_Initialize();
#endif

#ifdef RTEMS_DEBUG
  rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif

}

/*  PAGE
 *
 *  bsp_predriver_hook
 *
 *  Initialization before drivers are setup.
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values: NONE
 */ 

void bsp_predriver_hook(void)
{
  initialize_external_exception_vector();
}

/*  PAGE
 *
 *  bsp_postdriver_hook
 *
 *  After drivers are setup, register some "filenames"
 *  and open stdin, stdout, stderr files
 *
 *  Newlib will automatically associate the files with these
 *  (it hardcodes the numbers)
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values: NONE
 *  
 */
 
void bsp_postdriver_hook(void)
{
  int stdin_fd, stdout_fd, stderr_fd;
  int error_code;
 
  error_code = 'S' << 24 | 'T' << 16;
 
  if ((stdin_fd = __rtems_open("/dev/console", O_RDONLY, 0)) == -1)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '0' );
 
  if ((stdout_fd = __rtems_open("/dev/console", O_WRONLY, 0)) == -1)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '1' );
 
  if ((stderr_fd = __rtems_open("/dev/console", O_WRONLY, 0)) == -1)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '2' );
 
  if ((stdin_fd != 0) || (stdout_fd != 1) || (stderr_fd != 2))
    rtems_fatal_error_occurred( error_code | 'I' << 8 | 'O' );
}

/*  PAGE
 *
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values: NONE
 *  
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
   * Set up our hooks
   * Make sure libc_init is done before drivers initialized so that
   * they can use atexit()
   */

  Cpu_table.exceptions_in_RAM = TRUE;

  Cpu_table.pretasking_hook = bsp_pretasking_hook;    /* init libc, etc. */

  Cpu_table.predriver_hook = bsp_predriver_hook;

  Cpu_table.postdriver_hook = bsp_postdriver_hook;

  Cpu_table.idle_task = NULL;  /* do not override system IDLE task */

  Cpu_table.clicks_per_usec = 66 / 4; /* XXX get from linkcmds */


  /*
   *  SIS does zero out memory BUT only when IT begins execution.  Thus
   *  if we want to have a clean slate in the workspace each time we
   *  begin execution of OUR application, then we must zero the workspace.
   */

  Cpu_table.do_zero_of_workspace = TRUE;

  /*
   *  This should be enough interrupt stack.
   */

  Cpu_table.interrupt_stack_size = (12 * 1024);

  /*
   *  DMV170 does not support MP configurations so there is really no way
   *  to check this out.
   */

  Cpu_table.extra_mpci_receive_server_stack = 0;

  /*
   *  Copy the table and allocate memory for the RTEMS Workspace
   */

  BSP_Configuration = Configuration;

#if defined(RTEMS_POSIX_API)
  BSP_Configuration.work_space_size *= 3;
#endif

  work_space_start = 
    (unsigned char *)&RAM_END - BSP_Configuration.work_space_size;

  if ( work_space_start <= (unsigned char *)&end ) {
    DEBUG_puts( "bspstart: Not enough RAM!!!\n" );
    bsp_cleanup();
  }

  BSP_Configuration.work_space_start = work_space_start;

  /*
   * Add 1 region for RTEMS Malloc
   */

  BSP_Configuration.RTEMS_api_configuration->maximum_regions++;

  /*
   *  Account for the console's resources
   */

  console_reserve_resources( &BSP_Configuration );

#ifdef RTEMS_NEWLIB
  /*
   * Add 1 extension for newlib libc
   */

  BSP_Configuration.maximum_extensions++;
#endif

#ifdef STACK_CHECKER_ON
  /*
   * Add 1 extension for stack checker
   */

  BSP_Configuration.maximum_extensions++;
#endif

  /*
   * Add 1 extension for MPCI_fatal
   */

  if (BSP_Configuration.User_multiprocessing_table)
    BSP_Configuration.maximum_extensions++;

  /*
   *  Initialize RTEMS. main() will finish it up and start multitasking.
   */

  rtems_libio_config( &BSP_Configuration, BSP_LIBIO_MAX_FDS );
 
  bsp_isr_level = rtems_initialize_executive_early(
     &BSP_Configuration,
     &Cpu_table
  );
}




