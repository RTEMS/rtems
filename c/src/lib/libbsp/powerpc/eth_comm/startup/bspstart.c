/*  bsp_start()
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  The MPC860 specific stuff was written by Jay Monkman (jmonkman@frasca.com)
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

#include <bsp.h>
#include <mpc860.h>
#include <rtems/libio.h>
 
#include <libcsupport.h>
 
#include <string.h>
#include <info.h>
 
#ifdef STACK_CHECKER_ON
#include <stackchk.h>
#endif

boardinfo_t M860_binfo;


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
 
void
bsp_pretasking_hook(void)
{
  extern int       _end;
  rtems_unsigned32  heap_start;

  /* 
   * Let's check to see if the size of M860_binfo is what
   * it should be. It might not be if the info.h files
   * for RTEMS and the bootloader define boardinfo_t
   * differently.
   */

  /* Oops. printf() won't work yet, since the console is not initialized.
     I should probably find some way of doing this though.
  if (M860_binfo.size != sizeof(boardinfo_t)) {
      printf("The size of the Board Info Block appears to be incorrect.\n");
      printf(" This could occur if the 'info.h' files for RTEMS and the\n");
      printf(" bootloader differ in their definition of boardinfo_t\n");
  }
  */
  heap_start = (rtems_unsigned32) &_end;

  /* Align the heap on a natural boundary (4 bytes?) */
  if (heap_start & (CPU_ALIGNMENT-1)) {
    heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);
  }
  /* set up a 256K heap */
  bsp_libc_init((void *) heap_start, 256 * 1024, 0);
  
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


void bsp_start(void)
{
  extern int _end;
  rtems_unsigned32  heap_start;
  rtems_unsigned32  ws_start;
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

  heap_start = (rtems_unsigned32) &_end;
  if (heap_start & (CPU_ALIGNMENT-1))
    heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);


  ws_start = heap_start + (256 * 1024);
  if (ws_start & ((512 * 1024) - 1)) {  /* align to 512K boundary */
    ws_start = (ws_start + (512 * 1024)) & ~((512 * 1024) - 1);
  }

  BSP_Configuration.work_space_start = (void *)ws_start;
  BSP_Configuration.work_space_size = 512 * 1024; 

  /*
   *  initialize the CPU table for this BSP
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

  Cpu_table.clicks_per_usec = 1;  /* for 4MHz extclk */
  Cpu_table.serial_per_sec = 10000000;
  Cpu_table.serial_external_clock = 1;
  Cpu_table.serial_xon_xoff = 0;
  Cpu_table.serial_cts_rts = 1;
  Cpu_table.serial_rate = 9600;
  Cpu_table.timer_average_overhead = 0;
  Cpu_table.timer_least_valid = 0;
  Cpu_table.clock_speed = 40000000;

  /*
   * Since we are currently autodetecting whether to use SCC1 or
   * the FEC for ethernet, we set up a register in the ethernet
   * transciever that is used for 10/100 Mbps ethernet now, so that 
   * we can attempt to read it later in rtems_enet_driver_attach()
  */
  m860.fec.mii_speed = 0x0a;
  m860.fec.mii_data = 0x680a0000;


  m860.scc2.sccm=0;
  m860.scc2p.rbase=0;
  m860.scc2p.tbase=0;
  M860ExecuteRISC(M860_CR_OP_STOP_TX | M860_CR_CHAN_SCC2);

  mmu_init(); 
}


