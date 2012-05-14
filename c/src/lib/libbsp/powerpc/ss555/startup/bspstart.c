/*  bspstart.c
 *
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  SS555 port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from c/src/lib/libbsp/powerpc/mbx8xx/startup/bspstart.c:
 *
 *  Modifications for MBX860:
 *  Copyright (c) 1999, National Research Council of Canada
 */

#warning The interrupt disable mask is now stored in SPRG0, please verify that this is compatible to this BSP (see also bootcard.c).

#include <rtems/bspIo.h>
#include <rtems/powerpc/powerpc.h>

#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>

#include <bsp/irq.h>
#include <bsp.h>

SPR_RW(SPRG1)

extern unsigned long intrStackPtr;

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clicks_per_usec;
uint32_t   bsp_clock_speed;	       /* Serial clocks per second */
uint32_t   bsp_timer_least_valid;
uint32_t   bsp_timer_average_overhead;

void BSP_panic(char *s)
{
  printk("%s PANIC %s\n",_RTEMS_version, s);
  __asm__ __volatile ("sc");
}

void _BSP_Fatal_error(unsigned int v)
{
  printk("%s PANIC ERROR %x\n",_RTEMS_version, v);
  __asm__ __volatile ("sc");
}

/*
 *  bsp_start()
 *
 *  Board-specific initialization code. Called from the generic boot_card()
 *  function defined in rtems/c/src/lib/libbsp/shared/main.c. That function
 *  does some of the board independent initialization. It is called from the
 *  SS555 entry point _start() defined in
 *  rtems/c/src/lib/libbsp/powerpc/ss555/startup/start.S
 *
 *  _start() has set up a stack, has zeroed the .bss section, has set up the
 *  .data section from contents stored in ROM, has turned off interrupts,
 *  and placed the processor in the supervisor mode.  boot_card() has left
 *  the processor in that state when bsp_start() was called.
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */
void bsp_start(void)
{
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;
  register unsigned char* intrStack;

  /*
   * Get CPU identification dynamically.  Note that the get_ppc_cpu_type()
   * function stores the result in global variables so that it can be used
   * later.
   */
  myCpu 	= get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  /*
   * Initialize some SPRG registers related to irq handling
   */
  intrStack = (((unsigned char*)&intrStackPtr) - PPC_MINIMUM_STACK_FRAME_SIZE);
  _write_SPRG1((unsigned int)intrStack);

  /*
   * Install our own set of exception vectors
   */
  initialize_exceptions();

  /*
   *  initialize the device driver parameters
   */
  bsp_clicks_per_usec = BSP_CRYSTAL_HZ / 4 / 1000000;
  bsp_clock_speed     = BSP_CLOCK_HZ;	/* for SCI baud rate generator */
  bsp_timer_least_valid      = 0;
  bsp_timer_average_overhead = 0;

  /*
   * Initalize RTEMS IRQ system
   */
  BSP_rtems_irq_mng_init(0);
}
