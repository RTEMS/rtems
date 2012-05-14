/**
 *  @file
 *  
 *  This file contains the clock driver initialization for the Hurricane BSP.
 */

/*
 *  Author:     Craig Lebakken <craigl@transition.com>
 *
 *  COPYRIGHT (c) 1996 by Transition Networks Inc.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Transition Networks not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      Transition Networks makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libbsp/no_cpu/no_bsp/clock/ckinit.c
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*
 *  Rather than deleting this, it is commented out to (hopefully) help
 *  the submitter send updates.
 *
 *  static char _sccsid[] = "@(#)ckinit.c 08/20/96     1.3\n";
 */


#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>

extern uint32_t bsp_clicks_per_microsecond;

#define EXT_INT1    0x800  /* external interrupt 5 */

#include "clock.h"

rtems_isr USC_isr(void *unused);

void reset_wdt(void);
void enable_wdi(void);
void init_hbt(void);
void enable_hbi(void);
void disable_hbi(void);

void Clock_exit(void);
rtems_isr Clock_isr(rtems_vector_number vector);
rtems_isr User_Clock_isr(rtems_vector_number vector);
void Install_clock(rtems_isr_entry clock_isr);


/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR_MASK    EXT_INT1
#define CLOCK_VECTOR         MIPS_INTERRUPT_BASE + 0x3

/*
 *  Clock_driver_ticks is a monotonically increasing counter of the
 *  number of clock ticks since the driver was initialized.
 */

volatile uint32_t Clock_driver_ticks;

/*
 *  Clock_isrs is the number of clock ISRs until the next invocation of
 *  the RTEMS clock tick routine.  The clock tick device driver
 *  gets an interrupt once a millisecond and counts down until the
 *  length of time between the user configured microseconds per tick
 *  has passed.
 */

uint32_t Clock_isrs;              /* ISRs until next tick */

/*
 *  The previous ISR on this clock tick interrupt vector.
 */

rtems_isr_entry  Old_ticker;

void Clock_exit( void );

static uint32_t mips_timer_rate = 0;

/*
 *  Isr Handler
 */

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
/*
 * bump the number of clock driver ticks since initialization
 *
 * determine if it is time to announce the passing of tick as configured
 * to RTEMS through the rtems_clock_tick directive
 *
 * perform any timer dependent tasks
 */

  reset_wdt();    /* Reset hardware watchdog timer */

  Clock_driver_ticks += 1;

  rtems_clock_tick();
}

/* User callback shell (set from Clock_Control) */
static void (*user_callback)(void);

rtems_isr User_Clock_isr(
  rtems_vector_number vector
)
{
   if (user_callback)
      user_callback();
}

/*
 *  Install_clock
 *
 *  Install a clock tick handleR and reprograms the chip.  This
 *  is used to initially establish the clock tick.
 */

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  /*
  *  Initialize the clock tick device driver variables
  */

  Clock_driver_ticks = 0;
  Clock_isrs = rtems_configuration_get_milliseconds_per_tick();

  mips_timer_rate = rtems_configuration_get_microseconds_per_tick() *
           bsp_clicks_per_microsecond;

  /*
  *  Hardware specific initialize goes here
  */

  /* Set up USC heartbeat timer to generate interrupts */
  disable_hbi();      /* Disable heartbeat interrupt in USC */

  /* Install interrupt handler */
  rtems_interrupt_handler_install( 
    CLOCK_VECTOR,
    "clock",
    0,
    USC_isr,
    NULL
  );

  init_hbt();        /* Initialize heartbeat timer */

  reset_wdt();      /* Reset watchdog timer */

  enable_wdi();      /* Enable watchdog interrupt in USC */

  enable_hbi();      /* Enable heartbeat interrupt in USC */

              /* Enable USC interrupt in MIPS processor */
  mips_enable_in_interrupt_mask(CLOCK_VECTOR_MASK);

  /*
  *  Schedule the clock cleanup routine to execute if the application exits.
  */

  atexit( Clock_exit );
}

/*
 *  Clean up before the application exits
 */

void Clock_exit( void )
{
  /* mips: turn off the timer interrupts */
  mips_disable_in_interrupt_mask(~CLOCK_VECTOR_MASK);
}

/*
 *  Clock_initialize
 *
 *  Device driver entry point for clock tick driver initialization.
 */

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  Install_clock( Clock_isr );

  return RTEMS_SUCCESSFUL;
}

