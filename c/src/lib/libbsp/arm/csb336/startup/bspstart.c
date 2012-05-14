/*
 * Cogent CSB336 - MC9328MXL SBC startup code
 *
 * Copyright (c) 2004 by Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <rtems/bspIo.h>
#include <mc9328mxl.h>

extern void rtems_exception_init_mngt(void);

extern void mmu_set_cpu_async_mode(void);

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
void bsp_start_default( void )
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
   * Init rtems exceptions management
   */
  rtems_exception_init_mngt();

  /*
   * Init rtems interrupt management
   */
  if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
    _CPU_Fatal_halt(0xe);
  }
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

