/*  bsp_start()
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 * 
 *  INPUT:  NONE
 * 
 *  OUTPUT: NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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

unsigned char *duart_base;
extern struct duart_regs duart_info;

#define DELAY 5000

void led_putnum();

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */
 
extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;

rtems_cpu_table Cpu_table;
 
char *rtems_progname;

/*      Initialize whatever libc we are using
 *      called from postdriver hook
 */
 
void bsp_libc_init()
{
    extern int end;
    rtems_unsigned32        heap_start;
 
    heap_start = (rtems_unsigned32) &end;
    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);
 
	/* Create 64 KByte memory region for RTEMS executive */
    RTEMS_Malloc_Initialize((void *) heap_start, 64 * 1024, 0);

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
 
void
bsp_pretasking_hook(void)
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
 

/*
 * After drivers are setup, register some "filenames"
 * and open stdin, stdout, stderr files
 *
 * Newlib will automatically associate the files with these
 * (it hardcodes the numbers)
 */
 
void
bsp_postdriver_hook(void)
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

int main(
  int argc,
  char **argv,
  char **environp
)
{
  m68k_isr_entry *monitors_vector_table;
  int             index;

  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS";

  duart_base = (unsigned char *)DUART_ADDR;

  /* 
   *  Set the VBR here to the monitor's default.
   */

  monitors_vector_table = (m68k_isr_entry *)0;
   /* This is where you set vector base register = 0 */
  m68k_set_vbr( monitors_vector_table );

  /* The vector interrupt table for the 680x0 is in appendix B-2 
	 of the M68000 Family Programmer's reference table */
  for ( index=2 ; index<=255 ; index++ )
    M68Kvec[ index ] = monitors_vector_table[ 32 ];


  M68Kvec[  2 ] = monitors_vector_table[  2 ];   /* bus error vector */
  M68Kvec[  4 ] = monitors_vector_table[  4 ];   /* breakpoints vector */
  M68Kvec[  9 ] = monitors_vector_table[  9 ];   /* trace vector */

  /* 
   *  Set the VBR here if you do not want to use the monitor's vector table.
   */

  m68k_set_vbr( &M68Kvec );

  m68k_enable_caching();

  /*
   *  we only use a hook to get the C library initialized.
   */
 
  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */

  Cpu_table.predriver_hook = NULL;

  Cpu_table.postdriver_hook = bsp_postdriver_hook;

  Cpu_table.idle_task = NULL;  /* do not override system IDLE task */

  Cpu_table.do_zero_of_workspace = TRUE;
 
  Cpu_table.interrupt_vector_table = (m68k_isr_entry *) &M68Kvec;
 
  Cpu_table.interrupt_stack_size = 4096;

  Cpu_table.extra_mpci_receive_server_stack = 0;

  /*
   *  Copy the table
   */
 
  BSP_Configuration = Configuration;
 
  BSP_Configuration.work_space_start = (void *)
     (RAM_END - BSP_Configuration.work_space_size);
 
  /*
   * Add 1 region for the RTEMS Malloc
   */
 
  BSP_Configuration.RTEMS_api_configuration->maximum_regions++;
 
  /*
   * Add 1 extension for newlib libc
   */
 
#ifdef RTEMS_NEWLIB
    BSP_Configuration.maximum_extensions++;
#endif
 
  /*
   * Add another extension if using the stack checker
   */
 
#ifdef STACK_CHECKER_ON
    BSP_Configuration.maximum_extensions++;
#endif
 
  /*
   * Tell libio how many fd's we want and allow it to tweak config
   */

  rtems_libio_config(&BSP_Configuration, BSP_LIBIO_MAX_FDS);

/*  led_putnum('e'); * for debugging purposes only */
  rtems_initialize_executive( &BSP_Configuration, &Cpu_table );/* does not return */
 
  /* Clock_exit is done as an atexit() function */

  return 0;
}
