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
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
 
#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
 
#define HEAP_SIZE 1024*1024*2

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern int putnum(unsigned int);

extern rtems_configuration_table Configuration;
extern void bsp_postdriver_hook(void); /* defined in shared directory */

rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;
unsigned int top_of_used_memory;

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

    extern int end;
    rtems_unsigned32        heap_start;

*(unsigned char *)(0x120f) = 0xd;
/*     heap_start = (rtems_unsigned32) &end; */
    heap_start = (rtems_unsigned32) top_of_used_memory;
    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    bsp_libc_init((void *) heap_start, 64 * 1024, 0);


#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
*(unsigned char *)(0x120f) = 0xf;
}
 

/* we need to have the top of memory remembered later to start libc_init with 
   the correct values
*/
int rx_boot_card( int argc, char **argv, char **environp)
{
    extern int end;
    top_of_used_memory = (rtems_unsigned32) &end + 0x1000;
  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS/RP";

	boot_card(argc, argv);

}

bsp_start(void)
{

  *(unsigned int *)OIMR_ADDR = 0xff;	/* Mask all primary PCI Interrupts*/
  Prcb    = get_prcb();
  Ctl_tbl = Prcb->control_tbl;

*(unsigned char *)(0x120f) = 8;
  /*
   *  we do not use the pretasking_hook.
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.do_zero_of_workspace = TRUE;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;
  Cpu_table.extra_mpci_receive_server_stack = 0;

  /* just trying to get along */
  Cpu_table.stack_allocate_hook = 0;
  Cpu_table.stack_free_hook = 0;
  
  /*
   * Tell libio how many fd's we want and allow it to tweak config
   */

*(unsigned char *)(0x120f) = 0x09;

  BSP_Configuration.work_space_start = (void *) top_of_used_memory;
     top_of_used_memory +=  (BSP_Configuration.work_space_size + 0x1000);


*(unsigned char *)(0x120f) = 0x0a;

  return 0;
}

