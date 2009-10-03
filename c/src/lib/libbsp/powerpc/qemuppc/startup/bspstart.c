/*
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <string.h>
#include <fcntl.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>
#include <rtems/powerpc/powerpc.h>
#include <libcpu/powerpc-utility.h>
#include <libcpu/raw_exception.h>
#include <bsp/irq-generic.h>
#include <bsp/ppc_exc_bspsupp.h>

#include <libcpu/cpuIdent.h>
#include <libcpu/bat.h>
#include <libcpu/spr.h>

/*
 * CPU Bus Frequency
 */
unsigned int BSP_bus_frequency;

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clicks_per_usec;

/*
 * Memory on this board.
 */
extern char RamSize[];
extern char bsp_interrupt_stack_start[];
extern char bsp_interrupt_stack_end[];
extern char bsp_interrupt_stack_size[];
uint32_t BSP_mem_size = (uint32_t)RamSize;

/*
 * Time base divisior (how many tick for 1 second).
 */
unsigned int BSP_time_base_divisor;

/* Default decrementer exception handler */
static int default_decrementer_exception_handler( BSP_Exception_frame *frame, unsigned number)
{
  ppc_set_decrementer_register(UINT32_MAX);

  return 0;
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  uint32_t intrStackStart;
  uint32_t intrStackSize;
  int rv = 0;
  rtems_status_code sc;

  /*
   * Note we can not get CPU identification dynamically, so
   * force current_ppc_cpu.
   */
  current_ppc_cpu = PPC_PSIM;

  /*
   *  initialize the device driver parameters
   */
  /* BSP_bus_frequency        = (unsigned int)&PSIM_INSTRUCTIONS_PER_MICROSECOND; */
  /* bsp_clicks_per_usec      = BSP_bus_frequency; */
  /* BSP_time_base_divisor    = 1; */

  /*
   * Initialize the interrupt related settings.
   */
  intrStackStart = (uint32_t) bsp_interrupt_stack_start;
  intrStackSize =  (uint32_t) bsp_interrupt_stack_size;

  BSP_mem_size = (uint32_t )RamSize;

  /*
   * Initialize default raw exception handlers.
   */
  ppc_exc_initialize(
    PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
    intrStackStart,
    intrStackSize
  );

  /* Install default handler for the decrementer exception */
  rv = ppc_exc_set_handler( ASM_DEC_VECTOR, default_decrementer_exception_handler);
  if (rv < 0) {
    BSP_panic( "Cannot install decrementer exception handler!\n");
  }

  /* Initalize interrupt support */
  sc = bsp_interrupt_initialize();
  if (sc != RTEMS_SUCCESSFUL) {
    BSP_panic( "Cannot intitialize interrupt support\n");
  }

#if 0
  /*
   * Setup BATs and enable MMU
   */
  /* Memory */
  setdbat(0, 0x0<<24, 0x0<<24, 2<<24, _PAGE_RW);
  setibat(0, 0x0<<24, 0x0<<24, 2<<24,        0);
  /* PCI    */
  setdbat(1, 0x8<<24, 0x8<<24, 1<<24,  IO_PAGE);
  setdbat(2, 0xc<<24, 0xc<<24, 1<<24,  IO_PAGE);

  _write_MSR(_read_MSR() | MSR_DR | MSR_IR);
  asm volatile("sync; isync");
#endif
}
