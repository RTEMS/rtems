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
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/bspIo.h>
#include <rtems/powerpc/powerpc.h>

#include <libcpu/cpuIdent.h>
#include <libcpu/bat.h>
#include <libcpu/spr.h>

SPR_RW(SPRG0)
SPR_RW(SPRG1)

void  initialize_exceptions(void);

/*  On psim, each click of the decrementer register corresponds
 *  to 1 instruction.  By setting this to 100, we are indicating
 *  that we are assuming it can execute 100 instructions per
 *  microsecond.  This corresponds to sustaining 1 instruction
 *  per cycle at 100 Mhz.  Whether this is a good guess or not
 *  is anyone's guess.
 */
extern int PSIM_INSTRUCTIONS_PER_MICROSECOND;

/*
 * PCI Bus Frequency
 */
unsigned int BSP_bus_frequency;

/*
 * Time base divisior (how many tick for 1 second).
 */
unsigned int BSP_time_base_divisor;

/*
 * system init stack
 */
#define INIT_STACK_SIZE 0x1000

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
 *  This method returns the base address and size of the area which
 *  is to be allocated between the RTEMS Workspace and the C Program
 *  Heap.
 */
void bsp_get_workarea(
  void   **workarea_base,
  size_t  *workarea_size,
  size_t  *requested_heap_size
)
{
  *workarea_base       = &end;
  *workarea_size       = (void *)&RAM_END - (void *)&end;
  *requested_heap_size = 0;
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  extern unsigned long __rtems_end[];
  register uint32_t  intrStack;
  register uint32_t *intrStackPtr;

  /*
   * Note we can not get CPU identification dynamically, so
   * force current_ppc_cpu.
   */
  current_ppc_cpu = PPC_PSIM;

  /*
   *  initialize the device driver parameters
   */
  BSP_bus_frequency        = (unsigned int)&PSIM_INSTRUCTIONS_PER_MICROSECOND;
  BSP_time_base_divisor    = 1;

  /*
   *  The simulator likes the exception table to be at 0xfff00000.
   */

  bsp_exceptions_in_RAM = FALSE;

  /*
   * Initialize the interrupt related settings
   * SPRG1 = software managed IRQ stack
   *
   * This could be done latter (e.g in IRQ_INIT) but it helps to understand
   * some settings below...
   */
  intrStack = ((uint32_t) __rtems_end) + INIT_STACK_SIZE + 
    rtems_configuration_get_interrupt_stack_size() -
    PPC_MINIMUM_STACK_FRAME_SIZE;

  /* make sure it's properly aligned */
  intrStack &= ~(CPU_STACK_ALIGNMENT-1);

  /* tag the bottom (T. Straumann 6/36/2001 <strauman@slac.stanford.edu>) */
  intrStackPtr = (uint32_t*) intrStack;
  *intrStackPtr = 0;

  _write_SPRG1(intrStack);

  /* signal them that we have fixed PR288 - eventually, this should go away */
  _write_SPRG0(PPC_BSP_HAS_FIXED_PR288);

  /*
   * Initialize default raw exception handlers. See vectors/vectors_init.c
   */
  initialize_exceptions();

  /*
   * Initalize RTEMS IRQ system
   */
  BSP_rtems_irq_mng_init(0);

  /*
   * Setup BATs and enable MMU
   */
  /* Memory */
  setdbat(0, 0x0<<24, 0x0<<24, 1<<24, _PAGE_RW);
  setibat(0, 0x0<<24, 0x0<<24, 1<<24,        0);
  /* PCI    */
  setdbat(1, 0x8<<24, 0x8<<24, 1<<24,  IO_PAGE);
  setdbat(2, 0xc<<24, 0xc<<24, 1<<24,  IO_PAGE);

  _write_MSR(_read_MSR() | MSR_DR | MSR_IR);
  asm volatile("sync; isync");
}
