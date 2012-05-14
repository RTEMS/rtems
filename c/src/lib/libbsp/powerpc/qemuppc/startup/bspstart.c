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
 */

#include <string.h>
#include <fcntl.h>

#include <libcpu/bat.h>
#include <libcpu/spr.h>
#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>

/*
 * CPU Bus Frequency
 */
unsigned int BSP_bus_frequency;

/* Configuration parameter for clock driver */
uint32_t bsp_time_base_frequency;

/* Legacy */
uint32_t bsp_clicks_per_usec;

/*
 * Memory on this board.
 */
extern char RamSize[];
extern char bsp_interrupt_stack_start[];
extern char bsp_interrupt_stack_end[];
extern char bsp_interrupt_stack_size[];
uint32_t BSP_mem_size = (uint32_t)RamSize;

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
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uintptr_t intrStackStart;
  uintptr_t intrStackSize;

  /*
   * Note we can not get CPU identification dynamically, so
   * force current_ppc_cpu.
   */
  current_ppc_cpu = PPC_PSIM;

  /*
   *  initialize the device driver parameters
   * assume we are running with 20MHz bus
   * this should speed up some tests :-)
   */
  BSP_bus_frequency        = 20;
  bsp_time_base_frequency  = 20000000;
  bsp_clicks_per_usec      = BSP_bus_frequency;

  /*
   * Initialize the interrupt related settings.
   */
  intrStackStart = (uintptr_t) bsp_interrupt_stack_start;
  intrStackSize =  (uintptr_t) bsp_interrupt_stack_size;

  BSP_mem_size = (uint32_t )RamSize;

  /*
   * Initialize default raw exception handlers.
   */
  sc = ppc_exc_initialize(
    PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
    intrStackStart,
    intrStackSize
  );
  if (sc != RTEMS_SUCCESSFUL) {
    BSP_panic("cannot initialize exceptions");
  }

  /* Install default handler for the decrementer exception */
  sc = ppc_exc_set_handler( ASM_DEC_VECTOR, default_decrementer_exception_handler);
  if (sc != RTEMS_SUCCESSFUL) {
    BSP_panic("cannot install decrementer exception handler");
  }

  /* Initalize interrupt support */
  sc = bsp_interrupt_initialize();
  if (sc != RTEMS_SUCCESSFUL) {
    BSP_panic("cannot intitialize interrupts");
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
  __asm__ volatile("sync; isync");
#endif
}
