/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This source file contains the implementation of bsp_start() and
 *   definitions of BSP-specific objects.
 */

/*
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
 *
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
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

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <bsp/leon3.h>
#include <bsp/bootcard.h>
#include <rtems/sysinit.h>

#if defined(RTEMS_SMP) || defined(RTEMS_MULTIPROCESSING)
/* Irq used by shared memory driver and for inter-processor interrupts.
 * Can be overridden by being defined in the application.
 */
const unsigned char LEON3_mp_irq __attribute__((weak)) = 14;
#endif

/*
 * Tells us if data cache snooping is available
 */
int CPU_SPARC_HAS_SNOOPING;

/* Index of CPU, in an AMP system CPU-index may be non-zero */
uint32_t LEON3_Cpu_Index = 0;

/*
 * set_snooping
 *
 * Read the cache control register to determine if
 * bus snooping is available and enabled. This is needed for some
 * drivers so that they can select the most efficient copy routines.
 *
 */

static inline int set_snooping(void)
{
  return (leon3_get_cache_control_register() >> 23) & 1;
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  CPU_SPARC_HAS_SNOOPING = set_snooping();

#ifndef RTEMS_DRVMGR_STARTUP
  bsp_interrupt_initialize();
#endif
}

static void leon3_cpu_index_init(void)
{
  /* Get the LEON3 CPU index, normally 0, but for MP systems we do
   * _not_ assume that this is CPU0. One may run another OS on CPU0
   * and RTEMS on this CPU, and AMP system with mixed operating
   * systems
   */
  LEON3_Cpu_Index = _LEON3_Get_current_processor();
}

RTEMS_SYSINIT_ITEM(
  leon3_cpu_index_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_FIRST
);

/*
 * Initialize shared interrupt handling, must be done after IRQ controller has
 * been found and initialized.
 */
#ifdef RTEMS_DRVMGR_STARTUP
RTEMS_SYSINIT_ITEM(
  bsp_interrupt_initialize,
  RTEMS_SYSINIT_DRVMGR_LEVEL_1,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
#endif
