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

#define C961_INIT

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
    extern int end;
    rtems_unsigned32        heap_start;

    heap_start = (rtems_unsigned32) &end;
    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    bsp_libc_init((void *) heap_start, 64 * 1024, 0);
 
#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}
 
/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
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

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

  BSP_Configuration.work_space_start = (void *)
     (RAM_END - BSP_Configuration.work_space_size);
}
