/*
 *  This routine does the bulk of the system initialization.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  SS555 port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from c/src/lib/libbsp/powerpc/mbx8xx/startup/bspstart.c:
 *
 *  Modifications for MBX860:
 *  Copyright (c) 1999, National Research Council of Canada
 */

#include <rtems/bspIo.h>
#include <rtems/counter.h>
#include <bsp/bootcard.h>
#include <rtems/powerpc/powerpc.h>

#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>

#include <bsp/irq.h>
#include <bsp.h>

SPR_RW(SPRG1)

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clicks_per_usec;
uint32_t   bsp_clock_speed;	       /* Serial clocks per second */

uint32_t _CPU_Counter_frequency(void)
{
  return BSP_CRYSTAL_HZ / 4;
}

/*
 *  bsp_start()
 *
 *  Board-specific initialization code. Called from the generic boot_card()
 *  function defined in rtems/c/src/lib/libbsp/shared/main.c. That function
 *  does some of the board independent initialization. It is called from the
 *  SS555 entry point _start() defined in
 *  rtems/c/src/lib/libbsp/powerpc/ss555/start/start.S
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
  char* intrStack;

  /*
   * Get CPU identification dynamically.  Note that the get_ppc_cpu_type()
   * function stores the result in global variables so that it can be used
   * later.
   */
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  /*
   * Initialize some SPRG registers related to irq handling
   */
  intrStack = (char *)_ISR_Stack_area_end -
     PPC_MINIMUM_STACK_FRAME_SIZE;
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

  /*
   * Initalize RTEMS IRQ system
   */
  BSP_rtems_irq_mng_init(0);
}
