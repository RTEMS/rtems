/**
 *  @file
 *
 *  This routine initializes the interval timer on the
 *  PowerPC 403 CPU.  The tick frequency is specified by the BSP.
 */

/*
 *  Original PPC403 Code from:
 *  Author: Andrew Bray <andy@i-cubed.co.uk>
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  Modifications for PPC405GP by Dennis Ehlin
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
 *  Modifications for deriving timer clock from cpu system clock by
 *              Thomas Doerfler <td@imd.m.isar.de>
 *  for these modifications:
 *  COPYRIGHT (c) 1997 by IMD, Puchheim, Germany.
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/libio.h>
#include <stdlib.h>                     /* for atexit() */
#include <rtems/bspIo.h>
#include <rtems/powerpc/powerpc.h>

/*
 * check, which exception handling code is present
 */

#include <bsp.h>

#include <bsp/vectors.h>
#include <bsp/irq.h>

extern uint32_t bsp_clicks_per_usec;
extern bool bsp_timer_internal_clock;

volatile uint32_t   Clock_driver_ticks;
static uint32_t   pit_value, tick_time;
static bool auto_restart;

void Clock_exit( void );

static inline uint32_t   get_itimer(void)
{
  register uint32_t   rc;

#ifndef ppc405 /* this is a ppc403 */
  __asm__ volatile ("mfspr %0, 0x3dd" : "=r" ((rc))); /* TBLO */
#else /* ppc405 */
  __asm__ volatile ("mfspr %0, 0x10c" : "=r" ((rc))); /* 405GP TBL */
#endif /* ppc405 */

  return rc;
}

/*
 *  ISR Handler
 */
void Clock_isr(void* handle)
{
  uint32_t   clicks_til_next_interrupt;
#if defined(BSP_PPC403_CLOCK_ISR_IRQ_LEVEL)
  uint32_t   l_orig = _ISR_Get_level();
#endif

  if (!auto_restart) {
    uint32_t   itimer_value;
    /*
     * setup for next interrupt; making sure the new value is reasonably
     * in the future.... in case we lost out on an interrupt somehow
     */
    itimer_value = get_itimer();
    tick_time += pit_value;

    /*
     * how far away is next interrupt *really*
     * It may be a long time; this subtraction works even if
     * Clock_clicks_interrupt < Clock_clicks_low_order via
     * the miracle of unsigned math.
     */
    clicks_til_next_interrupt = tick_time - itimer_value;

    /*
     * If it is too soon then bump it up.
     * This should only happen if CPU_HPPA_CLICKS_PER_TICK is too small.
     * But setting it low is useful for debug, so...
     */

    if (clicks_til_next_interrupt < 400) {
      tick_time = itimer_value + 1000;
      clicks_til_next_interrupt = 1000;
      /* XXX: count these! this should be rare */
    }

    /*
     * If it is too late, that means we missed the interrupt somehow.
     * Rather than wait 35-50s for a wrap, we just fudge it here.
     */

    if (clicks_til_next_interrupt > pit_value) {
      tick_time = itimer_value + 1000;
      clicks_til_next_interrupt = 1000;
      /* XXX: count these! this should never happen :-) */
    }

#ifndef ppc440
    __asm__ volatile ("mtspr 0x3db, %0" :: "r"
                      (clicks_til_next_interrupt)); /* PIT */
#else
    __asm__ volatile ("mtspr 0x016, %0" :: "r"
                      (clicks_til_next_interrupt)); /* Decrementer */
#endif
  }

#ifndef ppc440
    __asm__ volatile ( "mtspr 0x3d8, %0" :: "r" (0x08000000)); /* TSR */
#else /* Book E */
    __asm__ volatile ( "mtspr 0x150, %0" :: "r" (0x08000000)); /* TSR */
#endif

  Clock_driver_ticks++;

  /* Give BSP a chance to say if they want to re-enable interrupts */
#if defined(BSP_PPC403_CLOCK_ISR_IRQ_LEVEL)
  _ISR_Set_level(BSP_PPC403_CLOCK_ISR_IRQ_LEVEL);
#endif
  rtems_clock_tick();

#if defined(BSP_PPC403_CLOCK_ISR_IRQ_LEVEL)
  _ISR_Set_level(l_orig)
#endif
}

int ClockIsOn(const rtems_irq_connect_data* unused)
{
  register uint32_t   tcr;

#ifndef ppc440
  __asm__ volatile ("mfspr %0, 0x3da" : "=r" ((tcr))); /* TCR */
#else /* Book E */
  __asm__ volatile ("mfspr %0, 0x154" : "=r" ((tcr))); /* TCR */
#endif

  return (tcr & 0x04000000) != 0;
}

void ClockOff(const rtems_irq_connect_data* unused)
{
  register uint32_t   tcr;

#ifndef ppc440
  __asm__ volatile ("mfspr %0, 0x3da" : "=r" ((tcr))); /* TCR */
  tcr &= ~ 0x04400000;
  __asm__ volatile ("mtspr 0x3da, %0" : "=r" ((tcr)) : "0" ((tcr))); /* TCR */
#else /* Book E */
  __asm__ volatile ("mfspr %0, 0x154" : "=r" ((tcr))); /* TCR */
  tcr &= ~ 0x04400000;
  __asm__ volatile ("mtspr 0x154, %0" : "=r" ((tcr)) : "0" ((tcr))); /* TCR */
#endif
}

void ClockOn(const rtems_irq_connect_data* unused)
{
  uint32_t   iocr;
  register uint32_t   tcr;
#ifdef ppc403
  uint32_t   pvr;
#endif /* ppc403 */

  Clock_driver_ticks = 0;

#ifdef ppc403 /* this is a ppc403 */
  __asm__ volatile ("mfdcr %0, 0xa0" : "=r" (iocr)); /* IOCR */
  if (bsp_timer_internal_clock) {
    iocr &= ~4; /* timer clocked from system clock */
  } else {
    iocr |= 4; /* select external timer clock */
  }
  __asm__ volatile ("mtdcr 0xa0, %0" : "=r" (iocr) : "0" (iocr)); /* IOCR */

  __asm__ volatile ("mfspr %0, 0x11f" : "=r" ((pvr))); /* PVR */
  if (((pvr & 0xffff0000) >> 16) != 0x0020)
    return; /* Not a ppc403 */

  if ((pvr & 0xff00) == 0x0000) /* 403GA */
#if 0 /* FIXME: in which processor versions will "autoload" work properly? */
   auto_restart = (pvr & 0x00f0) > 0x0000 ? true : false;
#else
   /* no known chip version supports auto restart of timer... */
   auto_restart = false;
#endif
   else if ((pvr & 0xff00) == 0x0100) /* 403GB */
     auto_restart = true;

#elif defined(ppc405) /* ppc405 */
  __asm__ volatile ("mfdcr %0, 0x0b2" : "=r" (iocr));  /*405GP CPC0_CR1 */
  if (bsp_timer_internal_clock) {
    iocr &=~0x800000; /* timer clocked from system clock CETE */
  } else {
    iocr |= 0x800000; /* select external timer clock CETE */
  }
  /* 405GP CPC0_CR1 */
  __asm__ volatile ("mtdcr 0x0b2, %0" : "=r" (iocr) : "0" (iocr));

  /*
   * Enable auto restart
   */
  auto_restart = true;
#else
  /* PPC440 */
  __asm__ volatile ("mfspr %0, 0x378" : "=r" (iocr));  /* 440 CCR1 */
  if (bsp_timer_internal_clock) {
    iocr &= ~0x00000100;           /* timer clocked from system clock CETE */
  } else {
    iocr |= 0x00000100;           /* select external timer clock CETE */
  }
  __asm__ volatile ("mtspr 0x378, %0" : "=r" (iocr) : "0" (iocr)); /*440 CCR1*/
#endif
  pit_value = rtems_configuration_get_microseconds_per_tick() *
                bsp_clicks_per_usec;

  /*
   * Set PIT value
   */
#ifndef ppc440
  __asm__ volatile ("mtspr 0x3db, %0" : : "r" (pit_value)); /* PIT */
#else /* Book E */
  __asm__ volatile ("mtspr 0x016, %0" : : "r" (pit_value)); /* Decrementer */
#endif

   /*
    * Set timer to autoreload, bit TCR->ARE = 1  0x0400000
    * Enable PIT interrupt, bit TCR->PIE = 1     0x4000000
    */
  tick_time = get_itimer() + pit_value;

#ifndef ppc440
  __asm__ volatile ("mfspr %0, 0x3da" : "=r" ((tcr))); /* TCR */
  tcr = (tcr & ~0x04400000) | (auto_restart ? 0x04400000 : 0x04000000);
  __asm__ volatile ("mtspr 0x3da, %0" : "=r" ((tcr)) : "0" ((tcr))); /* TCR */
#else /* Book E */
  __asm__ volatile ("mfspr %0, 0x154" : "=r" ((tcr)));               /* TCR */
  tcr = (tcr & ~0x04400000) | (auto_restart ? 0x04400000 : 0x04000000);
  __asm__ volatile ("mtspr 0x154, %0" : "=r" ((tcr)) : "0" ((tcr))); /* TCR */
#endif
}

void Install_clock(
  void (*clock_isr)(void *)
)
{
  rtems_irq_connect_data clockIrqConnData;

  Clock_driver_ticks = 0;

  /*
   * initialize the interval here
   * First tick is set to right amount of time in the future
   * Future ticks will be incremented over last value set
   * in order to provide consistent clicks in the face of
   * interrupt overhead
   */
  clockIrqConnData.on   = ClockOn;
  clockIrqConnData.off  = ClockOff;
  clockIrqConnData.isOn = ClockIsOn;
#if defined(ppc440) || defined(ppc405)
  clockIrqConnData.name = BSP_PIT;
#else
  clockIrqConnData.name = BSP_DECREMENTER;
#endif
  clockIrqConnData.hdl  = clock_isr;
  if (!BSP_install_rtems_irq_handler (&clockIrqConnData)) {
    printk("Unable to connect Clock Irq handler\n");
    rtems_fatal_error_occurred(1);
  }

  atexit(Clock_exit);
}

void ReInstall_clock(
  void (*new_clock_isr)(void *)
)
{
  uint32_t   isrlevel = 0;
  rtems_irq_connect_data clockIrqConnData;

  rtems_interrupt_disable(isrlevel);

#if defined(ppc440) || defined(ppc405)
  clockIrqConnData.name = BSP_PIT;
#else
  clockIrqConnData.name = BSP_DECREMENTER;
#endif
  if (!BSP_get_current_rtems_irq_handler(&clockIrqConnData)) {
    printk("Unable to stop system clock\n");
    rtems_fatal_error_occurred(1);
  }

  BSP_remove_rtems_irq_handler (&clockIrqConnData);

  clockIrqConnData.on   = ClockOn;
  clockIrqConnData.off  = ClockOff;
  clockIrqConnData.isOn = ClockIsOn;
#if defined(ppc440) || defined(ppc405)
  clockIrqConnData.name = BSP_PIT;
#else
  clockIrqConnData.name = BSP_DECREMENTER;
#endif
  clockIrqConnData.hdl  = new_clock_isr;

  if (!BSP_install_rtems_irq_handler (&clockIrqConnData)) {
    printk("Unable to connect Clock Irq handler\n");
    rtems_fatal_error_occurred(1);
  }

  rtems_interrupt_enable(isrlevel);
}

/*
 * Called via atexit()
 * Remove the clock interrupt handler by setting handler to NULL
 *
 * This will not work on the 405GP because
 * when bit's are set in TCR they can only be unset by a reset
 */
void Clock_exit(void)
{
  rtems_irq_connect_data clockIrqConnData;

#if defined(ppc440) || defined(ppc405)
  clockIrqConnData.name = BSP_PIT;
#else
  clockIrqConnData.name = BSP_DECREMENTER;
#endif
  if (!BSP_get_current_rtems_irq_handler(&clockIrqConnData)) {
    printk("Unable to stop system clock\n");
    rtems_fatal_error_occurred(1);
  }

  BSP_remove_rtems_irq_handler (&clockIrqConnData);
}

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  Install_clock( Clock_isr );

  return RTEMS_SUCCESSFUL;
}
