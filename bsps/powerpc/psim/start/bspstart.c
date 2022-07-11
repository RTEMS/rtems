/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <fcntl.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <psim.h>
#include <bsp/bootcard.h>
#include <bsp/linker-symbols.h>
#include <rtems/bspIo.h>
#include <rtems/counter.h>
#include <rtems/powerpc/powerpc.h>

#include <libcpu/cpuIdent.h>
#include <libcpu/bat.h>
#include <libcpu/spr.h>

SPR_RW(SPRG1)

/*  On psim, each click of the decrementer register corresponds
 *  to 1 instruction.  By setting this to 100, we are indicating
 *  that we are assuming it can execute 100 instructions per
 *  microsecond.  This corresponds to sustaining 1 instruction
 *  per cycle at 100 Mhz.  Whether this is a good guess or not
 *  is anyone's guess.
 */
extern int PSIM_INSTRUCTIONS_PER_MICROSECOND[];

/*
 * PCI Bus Frequency
 */
unsigned int BSP_bus_frequency;

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clicks_per_usec;

/*
 * Memory on this board.
 */
uint32_t BSP_mem_size = (uint32_t)RamSize;

/*
 * Time base divisior (how many tick for 1 second).
 */
unsigned int BSP_time_base_divisor;

extern unsigned long __rtems_end[];

uint32_t _CPU_Counter_frequency(void)
{
  return bsp_clicks_per_usec * 1000000;
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  /*
   * Note we can not get CPU identification dynamically.
   * PVR has to be set to PPC_PSIM (0xfffe) from the device
   * file.
   */

  get_ppc_cpu_type();

  /*
   *  initialize the device driver parameters
   */
  BSP_bus_frequency        = (unsigned int)PSIM_INSTRUCTIONS_PER_MICROSECOND;
  bsp_clicks_per_usec      = BSP_bus_frequency;
  BSP_time_base_divisor    = 1;

  ppc_exc_initialize_with_vector_base(
    (uintptr_t) _ISR_Stack_area_begin,
    (void *) 0xfff00000
  );

  /*
   * Initalize RTEMS IRQ system
   */
  BSP_rtems_irq_mng_init(0);

  /*
   * Setup BATs and enable MMU
   */
  /* Memory */
  setdbat(0, 0x0<<28, 0x0<<28, 1<<28, _PAGE_RW);
  setibat(0, 0x0<<28, 0x0<<28, 1<<28,        0);
  /* PCI    */
  setdbat(1, 0x08<<24, 0x08<<24, 1<<24,  IO_PAGE);
  setdbat(2, 0xfc<<24, 0xfc<<24, 1<<24,  IO_PAGE);

  _write_MSR(_read_MSR() | MSR_DR | MSR_IR);
  __asm__ volatile("sync; isync");

}
