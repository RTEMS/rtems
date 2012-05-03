/*  clock.c
 *
 *  This routine initializes the interval timer on the
 *  PowerPC 403 CPU.  The tick frequency is specified by the bsp.
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
 *  Derived from c/src/lib/libcpu/hppa1.1/clock/clock.c:
 *
 *  Modifications for deriving timer clock from cpu system clock by
 *              Thomas Doerfler <td@imd.m.isar.de>
 *  for these modifications:
 *  COPYRIGHT (c) 1997 by IMD, Puchheim, Germany.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications for PPC405GP by Dennis Ehlin
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
#include <bsp/irq_supp.h>

volatile uint32_t   Clock_driver_ticks;
static uint32_t   pit_value, tick_time;
static bool auto_restart;

void Clock_exit( void );

/*
 * These are set by clock driver during its init
 */

rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

static inline uint32_t   get_itimer(void)
{
    register uint32_t   rc;

    asm volatile ("mfspr %0, 0x10c" : "=r" ((rc))); /* 405GP TBL */

    return rc;
}

/*
 *  ISR Handler
 */

int Clock_isr(BSP_Exception_frame *f, unsigned int vector)
{
    uint32_t   clicks_til_next_interrupt;
#if defined(BSP_PPC403_CLOCK_ISR_IRQ_LEVEL)
    uint32_t   l_orig = _ISR_Get_level();
#endif
    if (!auto_restart)
    {
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

      if (clicks_til_next_interrupt < 400)
      {
        tick_time = itimer_value + 1000;
        clicks_til_next_interrupt = 1000;
        /* XXX: count these! this should be rare */
      }

      /*
       * If it is too late, that means we missed the interrupt somehow.
       * Rather than wait 35-50s for a wrap, we just fudge it here.
       */

      if (clicks_til_next_interrupt > pit_value)
      {
        tick_time = itimer_value + 1000;
        clicks_til_next_interrupt = 1000;
        /* XXX: count these! this should never happen :-) */
      }

      asm volatile ("mtspr 0x3db, %0" :: "r"
                         (clicks_til_next_interrupt)); /* PIT */
    }

    /* Clear the Programmable Interrupt Status */
    asm volatile ( "mtspr 0x3d8, %0" :: "r" (0x08000000)); /* TSR */

    Clock_driver_ticks++;

    rtems_clock_tick();

    return 0;
}

void ClockOff(void)
{
    register uint32_t   tcr;

    asm volatile ("mfspr %0, 0x3da" : "=r" ((tcr))); /* TCR */

    tcr &= ~ 0x04400000;

    asm volatile ("mtspr 0x3da, %0" : "=r" ((tcr)) : "0" ((tcr))); /* TCR */
}

void ClockOn(void)
{
    uint32_t   iocr;
    register uint32_t   tcr;

    Clock_driver_ticks = 0;

    asm volatile ("mfdcr %0, 0x0b2" : "=r" (iocr));  /*405GP CPC0_CR1 */
    if (bsp_timer_internal_clock) {
      iocr &=~0x800000;               /* timer clocked from system clock CETE*/
    }
    else {
      iocr |= 0x800000;               /* select external timer clock CETE*/
    }
    asm volatile ("mtdcr 0x0b2, %0" : "=r" (iocr) : "0" (iocr)); /* 405GP CPC0_CR1 */

     /*
      * Enable auto restart
      */

    auto_restart = true;

    pit_value = rtems_configuration_get_microseconds_per_tick() *
      bsp_clicks_per_usec;

     /*
      * Set PIT value
      */

    asm volatile ("mtspr 0x3db, %0" : : "r" (pit_value)); /* PIT */

     /*
      * Set timer to autoreload, bit TCR->ARE = 1  0x0400000
      * Enable PIT interrupt, bit TCR->PIE = 1     0x4000000
      */
    tick_time = get_itimer() + pit_value;

    asm volatile ("mfspr %0, 0x3da" : "=r" ((tcr)));               /* TCR */
    tcr = (tcr & ~0x04400000) | (auto_restart ? 0x04400000 : 0x04000000);
    asm volatile ("mtspr 0x3da, %0" : "=r" ((tcr)) : "0" ((tcr))); /* TCR */
}



void Install_clock(ppc_exc_handler_t clock_isr)
{
#ifdef ppc403
   uint32_t   pvr;
#endif /* ppc403 */

   Clock_driver_ticks = 0;

   /*
    * initialize the interval here
    * First tick is set to right amount of time in the future
    * Future ticks will be incremented over last value set
    * in order to provide consistent clicks in the face of
    * interrupt overhead
    */

   ppc_exc_set_handler( BSP_PPC403_CLOCK_HOOK_EXCEPTION, clock_isr );
   ClockOn();

   atexit(Clock_exit);
}

void
ReInstall_clock(ppc_exc_handler_t clock_isr)
{
  uint32_t   isrlevel = 0;

  rtems_interrupt_disable(isrlevel);

   ppc_exc_set_handler( BSP_PPC403_CLOCK_HOOK_EXCEPTION, clock_isr );
   ClockOn();

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
  ClockOff();
  ppc_exc_set_handler( BSP_PPC403_CLOCK_HOOK_EXCEPTION, 0 );
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
