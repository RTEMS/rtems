/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic MPC5200 BSP
 *
 * This file contains the BSP initialization code
 *
 * This routine starts the application. It includes
 * application, board, and monitor specific
 * initialization and configuration. The generic CPU
 * dependent initialization has been performed before
 * this routine is invoked.
 *
 * The MPC860 specific stuff was written by Jay Monkman.
 * Modified for the MPC8260ADS board by Andy Dachs.
 *
 * A 40MHz system clock is assumed.
 * The PON. RST.CONF. Dip switches (DS1) are
 * 1 - Off
 * 2 - On
 * 3 - Off
 * 4 - On
 * 5 - Off
 * 6 - Off
 * 7 - Off
 * 8 - Off
 * Dip switches on DS2 and DS3 are all set to ON
 * The LEDs on the board are used to signal panic and fatal_error
 * conditions.
 * The mmu is unused at this time.
 */

/*
 * Copyright (C) 2005 embedded brains GmbH & Co. KG
 * Copyright (C) 2003 IPR Engineering
 * Copyright (C) 2001 Andy Dachs <a.dachs@sstl.co.uk>
 * Copyright (C) 1998 Jay Monkman (jmonkman@frasca.com)
 * Copyright (C) 1989, 2007 On-Line Applications Research Corporation (OAR)
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

#include <rtems.h>
#include <rtems/counter.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/mpc5200.h>

/* Configuration parameter for clock driver */
uint32_t bsp_time_base_frequency;

/* Legacy */
uint32_t bsp_clicks_per_usec;

uint32_t _CPU_Counter_frequency(void)
{
  return bsp_time_base_frequency;
}

void bsp_start(void)
{
  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function store the result in global variables so that it can be used
   * later...
   */
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  #if defined(HAS_UBOOT) && defined(SHOW_MORE_INIT_SETTINGS)
    {
      void dumpUBootBDInfo( bd_t * );
      dumpUBootBDInfo( &bsp_uboot_board_info );
    }
  #endif

  cpu_init();

  if(get_ppc_cpu_revision() >= 0x2014) {
    /* Special settings for MPC5200B (B variant) */
    uint32_t xlb_cfg = mpc5200.config;

    /* XXX: The Freescale documentation for BSDIS seems to be wrong */
    xlb_cfg |= XLB_CFG_BSDIS;

    xlb_cfg &= ~XLB_CFG_PLDIS;

    mpc5200.config = xlb_cfg;
  }

  bsp_time_base_frequency = XLB_CLOCK / 4;
  bsp_clicks_per_usec    = (XLB_CLOCK/4000000);

  /* Initialize exception handler */
  ppc_exc_cache_wb_check = 0;
  ppc_exc_initialize();
  ppc_exc_set_handler(ASM_ALIGN_VECTOR, ppc_exc_alignment_handler);

  /* Initialize interrupt support */
  bsp_interrupt_initialize();

  /*
   *  If the BSP was built with IRQ benchmarking enabled,
   *  then intialize it.
   */
  #if (BENCHMARK_IRQ_PROCESSING == 1)
    BSP_IRQ_Benchmarking_Reset();
  #endif

  #ifdef SHOW_MORE_INIT_SETTINGS
    printk("Exit from bspstart\n");
  #endif
}
