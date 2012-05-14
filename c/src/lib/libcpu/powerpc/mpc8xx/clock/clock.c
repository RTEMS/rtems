/*  clock.c
 *
 *  This routine initializes the PIT on the MPC8xx.
 *  The tick frequency is specified by the bsp.
 *
 *  Author: Jay Monkman (jmonkman@frasca.com)
 *  Copyright (C) 1998 by Frasca International, Inc.
 *
 *  Derived from c/src/lib/libcpu/ppc/ppc403/clock/clock.c:
 *
 *  Author: Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libcpu/hppa1_1/clock/clock.c:
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/libio.h>
#include <rtems/powerpc/powerpc.h>

#include <stdlib.h>                     /* for atexit() */
#include <mpc8xx.h>

volatile uint32_t   Clock_driver_ticks;
extern volatile m8xx_t m8xx;
extern int BSP_get_clock_irq_level(void);
extern int BSP_connect_clock_handler(rtems_isr_entry);
extern int BSP_disconnect_clock_handler(void);
extern uint32_t bsp_clicks_per_usec;
extern uint32_t bsp_clock_speed;

void Clock_exit( void );

/*
 * These are set by clock driver during its init
 */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*
 *  ISR Handler
 */
rtems_isr Clock_isr(rtems_vector_number vector)
{
  m8xx.piscr |= M8xx_PISCR_PS;
  Clock_driver_ticks++;
  rtems_clock_tick();
}

void clockOn(void* unused)
{
  unsigned desiredLevel;
  uint32_t   pit_value;
  uint32_t   extclk;
  bool force_prescaler = false;
  uint32_t immr_val;

  if (bsp_clicks_per_usec == 0) {
    /*
     * oscclk is too low for PIT, compute extclk and derive PIT from there
     */
    /*
     * determine external input clock by examining the PLL settings
     * this must be done differently depending on type of PLL
     */
    _mfspr(immr_val,M8xx_IMMR);
    if (8 == ((immr_val & 0x0000FF00) >> 8)) {
      /*
       * for MPC866: complex PLL
       */
      uint32_t plprcr_val;
      uint32_t mfn_value;
      uint32_t mfd_value;
      uint32_t mfi_value;
      uint32_t pdf_value;
      uint32_t s_value;

      plprcr_val = m8xx.plprcr;
      mfn_value  = (plprcr_val & (0xf8000000)) >> (31- 4);
      mfd_value  = (plprcr_val & (0x07c00000)) >> (31- 9);
      s_value    = (plprcr_val & (0x00300000)) >> (31-11);
      mfi_value  = (plprcr_val & (0x000f0000)) >> (31-15);
      pdf_value  = (plprcr_val & (0x00000006)) >> (31-30);
      extclk = (((uint64_t)bsp_clock_speed)
		* ((pdf_value + 1) * (mfd_value + 1))
		/ (mfi_value * (mfd_value + 1) + mfn_value)
		* (1 << s_value));
    }
    else {
      /*
       * for MPC860/850 etc: simple PLL
       */
      uint32_t mf_value;
      mf_value  = m8xx.plprcr >> 20;
      extclk    = bsp_clock_speed / (mf_value+1);
    }
    pit_value = (extclk
		 / 1000
		 / 4
		 * rtems_configuration_get_microseconds_per_tick()
		 / 1000);
    m8xx.sccr |=  (1<<23);
    force_prescaler = true;
  }
  else {
    pit_value = (rtems_configuration_get_microseconds_per_tick() *
		 bsp_clicks_per_usec);

    m8xx.sccr &= ~(1<<23);
  }
if ((pit_value > 0xffff) || force_prescaler){
    /*
     * try to activate prescaler
     * NOTE: divider generates odd values now...
     */
    pit_value = pit_value / 128;
    m8xx.sccr |= (1<<24);
  }
  else {
    m8xx.sccr &= ~(1<<24);
  }

  if (pit_value > 0xffff) {           /* pit is only 16 bits long */
    rtems_fatal_error_occurred(-1);
  }
  m8xx.pitc = pit_value - 1;

  desiredLevel = BSP_get_clock_irq_level();
  /* set PIT irq level, enable PIT, PIT interrupts */
  /*  and clear int. status */
  m8xx.piscr = M8xx_PISCR_PIRQ(desiredLevel) |
    M8xx_PISCR_PTE | M8xx_PISCR_PS | M8xx_PISCR_PIE;
}
/*
 * Called via atexit()
 * Remove the clock interrupt handler by setting handler to NULL
 */
void
clockOff(void* unused)
{
  /* disable PIT and PIT interrupts */
  m8xx.piscr &= ~(M8xx_PISCR_PTE | M8xx_PISCR_PIE);
}

int clockIsOn(void* unused)
{
  if (m8xx.piscr & M8xx_PISCR_PIE) return 1;
  return 0;
}

/*
 * Called via atexit()
 * Remove the clock interrupt handler by setting handler to NULL
 */
void
Clock_exit(void)
{
  (void) BSP_disconnect_clock_handler ();
}

void Install_clock(rtems_isr_entry clock_isr)
{
  Clock_driver_ticks = 0;

  BSP_connect_clock_handler (clock_isr);
  atexit(Clock_exit);
}

void
ReInstall_clock(rtems_isr_entry new_clock_isr)
{
  BSP_connect_clock_handler (new_clock_isr);
}


rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  Install_clock( Clock_isr );

  /*
   * make major/minor avail to others such as shared memory driver
   */

  rtems_clock_major = major;
  rtems_clock_minor = minor;

  return RTEMS_SUCCESSFUL;
}
