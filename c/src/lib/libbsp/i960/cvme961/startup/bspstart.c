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

#include <rtems.h>
#include <bsp.h>
#include "libcsupport.h"


#include "stackchk.h"

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table Configuration;

rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

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

    RTEMS_Malloc_Initialize((void *) heap_start, 64 * 1024, 0);

    /*
     * Set up for the libc handling.
     */

    if (BSP_Configuration.ticks_per_timeslice > 0)
        libc_init(1);                /* reentrant if possible */
    else
        libc_init(0);                /* non-reentrant */
    /*
     *  Initialize the stack bounds checker
     */

#ifdef STACK_CHECKER_ON
    Stack_check_Initialize();
#endif
}

int bsp_start(
  int argc,
  char **argv,
  char **environp
)
{
  /* set node number in SQSIO4 CTL REG */

  *((rtems_unsigned32 *)0xc00000b0) =
       (Configuration.User_multiprocessing_table) ?
          Configuration.User_multiprocessing_table->node : 0;

  Prcb    = get_prcb();
  Ctl_tbl = Prcb->control_tbl;

  /* following configures the data breakpoint (which must be set
   *   before this is executed) to break on writes only.
   */

  Ctl_tbl->bpcon &= ~0x00cc0000;
  i960_reload_ctl_group( 6 );

  /*  bit 31 of the Register Cache Control can be set to
   *  enable an alternative caching algorithm.  It does
   *  not appear to help RTEMS.
   */

  /* Configure Number of Register Caches */

  Prcb->reg_cache_cfg = 8;
  i960_soft_reset( Prcb );

  /*
   *  we do not use the pretasking_hook.
   */

  Cpu_table.pretasking_hook = NULL;

  Cpu_table.predriver_hook = bsp_libc_init;  /* RTEMS resources available */

  Cpu_table.postdriver_hook = NULL;   /* Call our main() for constructors */

  Cpu_table.idle_task = NULL;  /* do not override system IDLE task */

  Cpu_table.do_zero_of_workspace = TRUE;

  Cpu_table.interrupt_stack_size = 4096;

  Cpu_table.extra_system_initialization_stack = 0;

  Cpu_table.Prcb = Prcb;

  /*
   *  Copy the table
   */

  BSP_Configuration = Configuration;

  /*
   * Add 1 region for the RTEMS Malloc
   */

  BSP_Configuration.maximum_regions++;

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

  BSP_Configuration.work_space_start = (void *)
     (RAM_END - BSP_Configuration.work_space_size);

  rtems_initialize_executive( &BSP_Configuration, &Cpu_table );
  /* does not return */

  bsp_cleanup();

  return 0;

}
