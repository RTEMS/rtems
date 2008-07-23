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
 *
 *  $Id$
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

#ifdef BSP_PPC403_CLOCK_HOOK_EXCEPTION
#include <libcpu/raw_exception.h>
#include <bsp/vectors.h>
#include <bsp/ppc_exc_bspsupp.h>
#define PPC_HAS_CLASSIC_EXCEPTIONS FALSE
#else
#if !defined(ppc405)
#define PPC_HAS_CLASSIC_EXCEPTIONS TRUE
#else
#define PPC_HAS_CLASSIC_EXCEPTIONS FALSE
#include <bsp/irq.h>
#endif
#endif

volatile uint32_t   Clock_driver_ticks;
static uint32_t   pit_value, tick_time;
static rtems_boolean auto_restart;

void Clock_exit( void );
 
rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;
 
static inline uint32_t   get_itimer(void)
{
    register uint32_t   rc;

#ifndef ppc405 /* this is a ppc403 */
    asm volatile ("mfspr %0, 0x3dd" : "=r" ((rc))); /* TBLO */
#else /* ppc405 */
    asm volatile ("mfspr %0, 0x10c" : "=r" ((rc))); /* 405GP TBL */
#endif /* ppc405 */

    return rc;
}

/*
 *  ISR Handler
 */

#if PPC_HAS_CLASSIC_EXCEPTIONS
rtems_isr Clock_isr(rtems_vector_number vector)
#elif defined(BSP_PPC403_CLOCK_HOOK_EXCEPTION)
int  Clock_isr(struct _BSP_Exception_frame *f, unsigned int vector)
#else
void Clock_isr(void* handle)
#endif
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
 
    asm volatile ( "mtspr 0x3d8, %0" :: "r" (0x08000000)); /* TSR */
 
    Clock_driver_ticks++;

	/* Give BSP a chance to say if they want to re-enable interrupts */
 
#if defined(BSP_PPC403_CLOCK_ISR_IRQ_LEVEL)
	_ISR_Set_level(BSP_PPC403_CLOCK_ISR_IRQ_LEVEL);
#endif

    rtems_clock_tick();

#if defined(BSP_PPC403_CLOCK_ISR_IRQ_LEVEL)
	_ISR_Set_level(l_orig)
#endif

#if defined(BSP_PPC403_CLOCK_HOOK_EXCEPTION)
	return 0;
#endif
}

#if !PPC_HAS_CLASSIC_EXCEPTIONS && !defined(BSP_PPC403_CLOCK_HOOK_EXCEPTION)
int ClockIsOn(const rtems_irq_connect_data* unused)
{
    register uint32_t   tcr;
 
    asm volatile ("mfspr %0, 0x3da" : "=r" ((tcr))); /* TCR */
 
    return (tcr & 0x04000000) != 0;
}
#endif

void ClockOff(
#if PPC_HAS_CLASSIC_EXCEPTIONS || defined(BSP_PPC403_CLOCK_HOOK_EXCEPTION)
	      void
#else
	      const rtems_irq_connect_data* unused
#endif
	      )
{
    register uint32_t   tcr;
 
    asm volatile ("mfspr %0, 0x3da" : "=r" ((tcr))); /* TCR */
 
    tcr &= ~ 0x04400000;
 
    asm volatile ("mtspr 0x3da, %0" : "=r" ((tcr)) : "0" ((tcr))); /* TCR */ 
}

void ClockOn(
#if PPC_HAS_CLASSIC_EXCEPTIONS || defined(BSP_PPC403_CLOCK_HOOK_EXCEPTION)
	      void
#else
	      const rtems_irq_connect_data* unused
#endif
	      )
{
    uint32_t   iocr;
    register uint32_t   tcr;
    extern uint32_t bsp_clicks_per_usec;
    extern boolean bsp_timer_internal_clock;
#ifdef ppc403
    uint32_t   pvr;
#endif /* ppc403 */
 
    Clock_driver_ticks = 0;
 
#ifndef ppc405 /* this is a ppc403 */
    asm volatile ("mfdcr %0, 0xa0" : "=r" (iocr)); /* IOCR */
    if (bsp_timer_internal_clock) {
	iocr &= ~4; /* timer clocked from system clock */
    }
    else {
	iocr |= 4; /* select external timer clock */
    }
    asm volatile ("mtdcr 0xa0, %0" : "=r" (iocr) : "0" (iocr)); /* IOCR */
 
    asm volatile ("mfspr %0, 0x11f" : "=r" ((pvr))); /* PVR */
    if (((pvr & 0xffff0000) >> 16) != 0x0020)
      return; /* Not a ppc403 */
 
    if ((pvr & 0xff00) == 0x0000) /* 403GA */
#if 0 /* FIXME: in which processor versions will "autoload" work properly? */
      auto_restart = (pvr & 0x00f0) > 0x0000 ? 1 : 0;
#else 
    /* no known chip version supports auto restart of timer... */
    auto_restart = 0;
#endif
    else if ((pvr & 0xff00) == 0x0100) /* 403GB */
      auto_restart = 1;
 
#else /* ppc405 */
    asm volatile ("mfdcr %0, 0x0b2" : "=r" (iocr));  /*405GP CPC0_CR1 */
    if (bsp_timer_internal_clock) {
	iocr &=~0x800000	;/* timer clocked from system clock CETE*/
    }
    else {
	iocr |= 0x800000; /* select external timer clock CETE*/
    }
    asm volatile ("mtdcr 0x0b2, %0" : "=r" (iocr) : "0" (iocr)); /* 405GP CPC0_CR1 */

     /*
      * Enable auto restart
      */

    auto_restart=1;

#endif /* ppc405 */
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

    asm volatile ("mfspr %0, 0x3da" : "=r" ((tcr))); /* TCR */ 
    tcr = (tcr & ~0x04400000) | (auto_restart ? 0x04400000 : 0x04000000);
#if 1
    asm volatile ("mtspr 0x3da, %0" : "=r" ((tcr)) : "0" ((tcr))); /* TCR */
#endif

}



void Install_clock(
#if PPC_HAS_CLASSIC_EXCEPTIONS
		   rtems_isr_entry clock_isr
#elif defined(BSP_PPC403_CLOCK_HOOK_EXCEPTION)
           ppc_exc_handler_t clock_isr
#else
		   void (*clock_isr)(void *)
#endif
		   )
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

#if PPC_HAS_CLASSIC_EXCEPTIONS
 {
    rtems_isr_entry previous_isr;
    rtems_interrupt_catch(clock_isr, PPC_IRQ_PIT, &previous_isr);
    ClockOn();
 }
#elif defined(BSP_PPC403_CLOCK_HOOK_EXCEPTION)
 {
 	ppc_exc_set_handler( BSP_PPC403_CLOCK_HOOK_EXCEPTION, clock_isr );
    ClockOn();
 }
#else
 {
   rtems_irq_connect_data clockIrqConnData;
   clockIrqConnData.on   = ClockOn;
   clockIrqConnData.off  = ClockOff;
   clockIrqConnData.isOn = ClockIsOn;
   clockIrqConnData.name = BSP_PIT;
   clockIrqConnData.hdl  = clock_isr;
   if (!BSP_install_rtems_irq_handler (&clockIrqConnData)) {
     printk("Unable to connect Clock Irq handler\n");
     rtems_fatal_error_occurred(1);
   }
 }
#endif
    atexit(Clock_exit);
}

void
ReInstall_clock(
#if PPC_HAS_CLASSIC_EXCEPTIONS
		rtems_isr_entry new_clock_isr
#elif defined(BSP_PPC403_CLOCK_HOOK_EXCEPTION)
		ppc_exc_handler_t clock_isr
#else
		void (*new_clock_isr)(void *)
#endif
)
{
  uint32_t   isrlevel = 0;
  
  rtems_interrupt_disable(isrlevel);
  
#if PPC_HAS_CLASSIC_EXCEPTIONS
 {
   rtems_isr_entry previous_isr;
   rtems_interrupt_catch(new_clock_isr, PPC_IRQ_PIT, &previous_isr);
   ClockOn();
 }
#elif defined(BSP_PPC403_CLOCK_HOOK_EXCEPTION)
 {
 	ppc_exc_set_handler( BSP_PPC403_CLOCK_HOOK_EXCEPTION, clock_isr );
    ClockOn();
 }
#else
  {
    rtems_irq_connect_data clockIrqConnData;
    
    clockIrqConnData.name = BSP_PIT;
    if (!BSP_get_current_rtems_irq_handler(&clockIrqConnData)) {
      printk("Unable to stop system clock\n");
      rtems_fatal_error_occurred(1);
    }
    
    BSP_remove_rtems_irq_handler (&clockIrqConnData);
    
    clockIrqConnData.on   = ClockOn;
    clockIrqConnData.off  = ClockOff;
    clockIrqConnData.isOn = ClockIsOn;
    clockIrqConnData.name = BSP_PIT;
    clockIrqConnData.hdl  = new_clock_isr;

    if (!BSP_install_rtems_irq_handler (&clockIrqConnData)) {
      printk("Unable to connect Clock Irq handler\n");
      rtems_fatal_error_occurred(1);
    }
  }
#endif

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
#if PPC_HAS_CLASSIC_EXCEPTIONS
  ClockOff();
 
  (void) set_vector(0, PPC_IRQ_PIT, 1);
#elif defined(BSP_PPC403_CLOCK_HOOK_EXCEPTION)
  ClockOff();
  ppc_exc_set_handler( BSP_PPC403_CLOCK_HOOK_EXCEPTION, 0 );
#else
 {
    rtems_irq_connect_data clockIrqConnData;
    
    clockIrqConnData.name = BSP_PIT;
    if (!BSP_get_current_rtems_irq_handler(&clockIrqConnData)) {
      printk("Unable to stop system clock\n");
      rtems_fatal_error_occurred(1);
    }
    
    BSP_remove_rtems_irq_handler (&clockIrqConnData);
 }
#endif
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
 
rtems_device_driver Clock_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
    rtems_libio_ioctl_args_t *args = pargp;
 
    if (args == 0)
        goto done;
 
    /*
     * This is hokey, but until we get a defined interface
     * to do this, it will just be this simple...
     */
 
    if (args->command == rtems_build_name('I', 'S', 'R', ' '))
    {
#if PPC_HAS_CLASSIC_EXCEPTIONS
        Clock_isr(PPC_IRQ_PIT);
#elif defined(BSP_PPC403_CLOCK_HOOK_EXCEPTION)
        Clock_isr(NULL, BSP_PPC403_CLOCK_HOOK_EXCEPTION);
#else
        Clock_isr(NULL);
#endif
    }
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
        ReInstall_clock(args->buffer);
    }
 
done:
    return RTEMS_SUCCESSFUL;
}
