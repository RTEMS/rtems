/*-------------------------------------------------------------------------+
| bspstart.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| This file contains the PC386 BSP startup package. It includes application,
| board, and monitor specific initialization and configuration. The generic CPU
| dependent initialization has been performed before this routine is invoked. 
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   bspstart.c,v 1.8 1996/05/28 13:12:40 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1997.
| *  On-Line Applications Research Corporation (OAR).
| *  Copyright assigned to U.S. Government, 1994. 
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.OARcorp.com/rtems/license.html.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/


#include <fcntl.h>

#include <bsp.h>
#include <libcsupport.h>
#include <rtems/libio.h>
 
#ifdef STACK_CHECKER_ON
#include <stackchk.h>
#endif

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
#ifdef RTEMS_SMALL_MEMORY
extern rtems_unsigned32 _end;           /* End of BSS. Defined in 'linkcmds'. */

rtems_unsigned32 rtemsFreeMemStart = (rtems_unsigned32)&_end;
                         /* Address of start of free memory - should be updated
                            after creating new partitions or regions.         */
#else
rtems_unsigned32 rtemsFreeMemStart = RAM_START;
                                             /* RAM_START defined in 'bsp.h'. */
#endif /* RTEMS_SMALL_MEMORY */

/* The original BSP configuration table from the application and our copy of it
   with some changes. */

extern rtems_configuration_table  Configuration;
       rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;                     /* CPU configuration table.    */
char            *rtems_progname;               /* Program name - from main(). */


/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern void _exit(int);  /* define in exit.c */

/*-------------------------------------------------------------------------+
|         Function: bsp_libc_init
|      Description: Initialize whatever libc we are using. Called from
|                   pretasking hook. 
| Global Variables: rtemsFreeMemStart.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static void
bsp_libc_init(void)
{
  if (rtemsFreeMemStart & (CPU_ALIGNMENT - 1))  /* not aligned => align it */
    rtemsFreeMemStart = (rtemsFreeMemStart+CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  RTEMS_Malloc_Initialize((void *)rtemsFreeMemStart, HEAP_SIZE << 10, 0);
  rtemsFreeMemStart += HEAP_SIZE << 10;           /* HEAP_SIZE is in KBytes */

  /* Init the RTEMS libio facility to provide UNIX-like system calls for use by
     newlib (ie: provide __rtems_open, __rtems_close, etc). Uses malloc()
     to get area for the iops, so must be after malloc initialization. */

  rtems_libio_init();

  /* Set up for the libc handling. */

  if (BSP_Configuration.ticks_per_timeslice > 0)
    libc_init(1); /* reentrant if possible */
  else
    libc_init(0); /* non-reentrant         */
} /* bsp_libc_init */

 
/*-------------------------------------------------------------------------+
|         Function: bsp_pretasking_hook
|      Description: BSP pretasking hook.  Called just before drivers are
|                   initialized. Used to setup libc and install any BSP
|                   extensions. NOTE: Must not use libc (to do io) from here,
|                   since drivers are not yet initialized.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
bsp_pretasking_hook(void)
{
  bsp_libc_init();

#ifdef STACK_CHECKER_ON
  /* Initialize the stack bounds checker. We can either turn it on here or from
     the app. */
   
  Stack_check_Initialize();

#endif /* STACK_CHECKER_ON */
 
#ifdef RTEMS_DEBUG

  rtems_debug_enable(RTEMS_DEBUG_ALL_MASK);

#endif /* RTEMS_DEBUG */
} /* bsp_pretasking_hook */
 

/*-------------------------------------------------------------------------+
|         Function: bsp_postdriver_hook
|      Description: After drivers are setup, register some "filenames" and open
|                   stdin, stdout, stderr files. Newlib will automatically
|                   associate the files with these (it hardcodes the numbers).
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
bsp_postdriver_hook(void)
{
  int stdin_fd, stdout_fd, stderr_fd;

  rtems_status_code error_code;
 
  error_code = 'S' << 24 | 'T' << 16;

  /* open standard devices: stdout, stderr and stdin */
 
  if ((stdin_fd =  __rtems_open("/dev/console", O_RDONLY, 0)) < 0)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '0' );
 
  if ((stdout_fd = __rtems_open("/dev/console", O_WRONLY, 0)) < 0)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '1' );
 
  if ((stderr_fd = __rtems_open("/dev/console", O_WRONLY, 0)) < 1)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '2' );
 
  if ((stdin_fd != 0) || (stdout_fd != 1) || (stderr_fd != 2))
    rtems_fatal_error_occurred( error_code | 'I' << 8 | 'O' );
} /* bsp_postdriver_hook */ 


/*-------------------------------------------------------------------------+
|         Function: main
|      Description: Called from bsp's startup code ('start.s').
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
int main(int argc, char **argv, char **environp)
{
  /* If we don't have command line arguments set default program name. */

  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS";

  Cpu_table.pretasking_hook         = bsp_pretasking_hook; /* init libc, etc. */
  Cpu_table.predriver_hook          = NULL;                /* use system's    */
  Cpu_table.postdriver_hook         = bsp_postdriver_hook;
  Cpu_table.idle_task               = NULL;
                                          /* do not override system IDLE task */
  Cpu_table.do_zero_of_workspace    = TRUE;
  Cpu_table.interrupt_table_segment = get_ds();
  Cpu_table.interrupt_table_offset  = (void *)Interrupt_descriptor_table;
  Cpu_table.interrupt_stack_size    = 4096;
  Cpu_table.extra_mpci_receive_server_stack = 0;

  /* Copy user's table and make necessary adjustments.              */

  BSP_Configuration = Configuration;

  /* Place RTEMS workspace at top of physical RAM (RAM_END defined in 'bsp.h' */

  BSP_Configuration.work_space_start =
                          (void *)(RAM_END - BSP_Configuration.work_space_size);

  /* Add 1 region for Malloc in libc_low.                           */

  BSP_Configuration.RTEMS_api_configuration->maximum_regions++;

  /* Add 1 extension for newlib libc.                               */

#ifdef RTEMS_NEWLIB
  BSP_Configuration.maximum_extensions++;
#endif

  /* Add another extension if using the stack checker.              */

#ifdef STACK_CHECKER_ON
  BSP_Configuration.maximum_extensions++;
#endif

  /* Tell libio how many fd's we want and allow it to tweak config. */

  rtems_libio_config(&BSP_Configuration, BSP_LIBIO_MAX_FDS);

  rtems_initialize_executive(&BSP_Configuration, &Cpu_table);
  /* does not return */

  /*-------------------------------------------------------------------------+
  | We only return here if the executive has finished. This happens when the
  | task has called exit(). We will then call _exit() which is part of the bsp.
  +--------------------------------------------------------------------------*/

  for (;;)
    _exit(0);

  /* no cleanup necessary for PC386 */

  return 0;
} /* main */
