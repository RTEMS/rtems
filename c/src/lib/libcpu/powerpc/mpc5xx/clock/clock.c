/* clock.c
 *
 *  This routine initializes the PIT on the MPC5xx.
 *  The tick frequency is specified by the bsp.
 *
 *
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from c/src/lib/libcpu/powerpc/mpc8xx/clock/clock.c:
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
#include <libcpu/irq.h>

#include <stdlib.h>                     /* for atexit() */
#include <mpc5xx.h>

volatile uint32_t Clock_driver_ticks;
extern int BSP_connect_clock_handler(rtems_isr_entry);
extern int BSP_disconnect_clock_handler(void);
extern uint32_t bsp_clicks_per_usec;

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
  usiu.piscrk = USIU_UNLOCK_KEY;
  usiu.piscr |= USIU_PISCR_PS;			/* acknowledge interrupt */
  usiu.piscrk = 0;

  Clock_driver_ticks++;
  rtems_clock_tick();
}

void clockOn(void* unused)
{
  unsigned desiredLevel;
  uint32_t pit_value;

  /* calculate and set modulus */
  pit_value = (rtems_configuration_get_microseconds_per_tick() *
               bsp_clicks_per_usec) - 1 ;

  if (pit_value > 0xffff) {           /* pit is only 16 bits long */
    rtems_fatal_error_occurred(-1);
  }
  usiu.sccrk = USIU_UNLOCK_KEY;
  usiu.sccr &= ~USIU_SCCR_RTDIV;	/* RTC and PIT clock is divided by 4 */
  usiu.sccrk = 0;

  usiu.pitck = USIU_UNLOCK_KEY;
  usiu.pitc = pit_value;
  usiu.pitck = 0;

  /* set PIT irq level, enable PIT, PIT interrupts */
  /*  and clear int. status */
  desiredLevel = CPU_irq_level_from_symbolic_name(CPU_PERIODIC_TIMER);

  usiu.piscrk = USIU_UNLOCK_KEY;
  usiu.piscr = USIU_PISCR_PIRQ(desiredLevel) 	/* set interrupt priority */
             | USIU_PISCR_PS			/* acknowledge interrupt */
             | USIU_PISCR_PIE			/* enable interrupt */
             | USIU_PISCR_PITF			/* freeze during debug */
             | USIU_PISCR_PTE;			/* enable timer */
  usiu.piscrk = 0;
}

void
clockOff(void* unused)
{
  /* disable PIT and PIT interrupts */
  usiu.piscrk = USIU_UNLOCK_KEY;
  usiu.piscr &= ~(USIU_PISCR_PTE | USIU_PISCR_PIE);
  usiu.piscrk = 0;
}

int clockIsOn(void* unused)
{
  if (usiu.piscr & USIU_PISCR_PIE)
    return 1;
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
