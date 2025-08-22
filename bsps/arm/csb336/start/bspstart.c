/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Cogent CSB336 - MC9328MXL SBC startup code
 */

/*
 * Copyright (C) 2004 Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
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
#include <rtems/bspIo.h>
#include <mc9328mxl.h>
#include <libcpu/mmu.h>

/*
 * bsp_start_default - BSP initialization function
 *
 *   This function is called before RTEMS is initialized and used
 *   adjust the kernel's configuration.
 *
 *   This function also configures the CPU's memory protection unit.
 *
 * RESTRICTIONS/LIMITATIONS:
 *   Since RTEMS is not configured, no RTEMS functions can be called.
 *
 */
static void bsp_start_default( void )
{
  int i;

  /* Set the MCU prescaler to divide by 1 */
  MC9328MXL_PLL_CSCR &= ~MC9328MXL_PLL_CSCR_PRESC;

  /* Enable the MCU PLL */
  MC9328MXL_PLL_CSCR |= MC9328MXL_PLL_CSCR_MPEN;

  /* Delay to allow time for PLL to get going */
  for (i = 0; i < 100; i++) {
    __asm__ volatile ("nop\n");
  }

  /* Set the CPU to asynchrous clock mode, so it uses its fastest clock */
  mmu_set_cpu_async_mode();

  /* disable interrupts */
  MC9328MXL_AITC_INTENABLEL = 0;
  MC9328MXL_AITC_INTENABLEH = 0;

  /* Set interrupt priority to -1 (allow all priorities) */
  MC9328MXL_AITC_NIMASK = 0x1f;

  /*
   * Init rtems interrupt management
   */
  bsp_interrupt_initialize();
} /* bsp_start */

/* Calcuate the frequency for perclk1 */
int get_perclk1_freq(void)
{
  unsigned int fin;
  unsigned int fpll;
  unsigned int pd;
  unsigned int mfd;
  unsigned int mfi;
  unsigned int mfn;
  uint32_t reg;
  int perclk1;

  if (MC9328MXL_PLL_CSCR & MC9328MXL_PLL_CSCR_SYSSEL) {
    /* Use external oscillator */
    fin = BSP_OSC_FREQ;
  } else {
    /* Use scaled xtal freq */
    fin = BSP_XTAL_FREQ * 512;
  }

  /* calculate the output of the system PLL */
  reg = MC9328MXL_PLL_SPCTL0;
  pd = ((reg & MC9328MXL_PLL_SPCTL_PD_MASK) >>
        MC9328MXL_PLL_SPCTL_PD_SHIFT);
  mfd = ((reg & MC9328MXL_PLL_SPCTL_MFD_MASK) >>
         MC9328MXL_PLL_SPCTL_MFD_SHIFT);
  mfi = ((reg & MC9328MXL_PLL_SPCTL_MFI_MASK) >>
         MC9328MXL_PLL_SPCTL_MFI_SHIFT);
  mfn = ((reg & MC9328MXL_PLL_SPCTL_MFN_MASK) >>
         MC9328MXL_PLL_SPCTL_MFN_SHIFT);

#if 0
  printk("fin = %d\n", fin);
  printk("pd = %d\n", pd);
  printk("mfd = %d\n", mfd);
  printk("mfi = %d\n", mfi);
  printk("mfn = %d\n", mfn);
  printk("rounded (fin * mfi) / (pd + 1) = %d\n", (fin * mfi) / (pd + 1));
  printk("rounded (fin * mfn) / ((pd + 1) * (mfd + 1)) = %d\n",
         ((long long)fin * mfn) / ((pd + 1) * (mfd + 1)));
#endif

  fpll = 2 * ( ((fin * mfi  + (pd + 1) / 2) / (pd + 1)) +
               (((long long)fin * mfn + ((pd + 1) * (mfd + 1)) / 2) /
               ((pd + 1) * (mfd + 1))) );

  /* calculate the output of the PERCLK1 divider */
  reg = MC9328MXL_PLL_PCDR;
  perclk1 = fpll / (1 + ((reg & MC9328MXL_PLL_PCDR_PCLK1_MASK) >>
                         MC9328MXL_PLL_PCDR_PCLK1_SHIFT));

  return perclk1;
}

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */
void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));

