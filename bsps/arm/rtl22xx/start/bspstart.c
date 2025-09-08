/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief LPC22XX/LPC21xx Startup code
 */

/*
 * Copyright (c) 2007 Ray Xu <rayx.cn@gmail.com>
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
#include <lpc22xx.h>

/*
 * bsp_start_default - BSP initialization function
 *
 * This function is called before RTEMS is initialized and used
 * adjust the kernel's configuration.
 *
 * This function also configures the CPU's memory protection unit.
 *
 * RESTRICTIONS/LIMITATIONS:
 *   Since RTEMS is not configured, no RTEMS functions can be called.
 */
static void bsp_start_default( void )
{
  PINSEL2 =0x0f814914;
  BCFG0 = 0x1000ffef;
  BCFG1 = 0x1000ffef;

  MEMMAP = 0x2;  //debug and excute outside chip

  PLLCON = 1;
  #if (Fpclk / (Fcclk / 4)) == 1
    VPBDIV = 0;
  #endif
  #if (Fpclk / (Fcclk / 4)) == 2
    VPBDIV = 2;
  #endif
  #if (Fpclk / (Fcclk / 4)) == 4
    VPBDIV = 1;
  #endif

  #if (Fcco / Fcclk) == 2
    PLLCFG = ((Fcclk / Fosc) - 1) | (0 << 5);
  #endif
  #if (Fcco / Fcclk) == 4
    PLLCFG = ((Fcclk / Fosc) - 1) | (1 << 5);
  #endif
  #if (Fcco / Fcclk) == 8
    PLLCFG = ((Fcclk / Fosc) - 1) | (2 << 5);
  #endif
  #if (Fcco / Fcclk) == 16
    PLLCFG = ((Fcclk / Fosc) - 1) | (3 << 5);
  #endif
  PLLFEED = 0xaa;
  PLLFEED = 0x55;
  while((PLLSTAT & (1 << 10)) == 0);
  PLLCON = 3;
  PLLFEED = 0xaa;
  PLLFEED = 0x55;

  /* memory configure */
  /* it is not needed in my formatter board */
  //MAMCR = 0;
  // MAMTIM = 3;
  //MAMCR = 2;

  UART0_Ini();

  /*
   * Init rtems interrupt management
   */
  bsp_interrupt_initialize();
} /* bsp_start */

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */
void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
