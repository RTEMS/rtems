/**
 * @file clock.c
 *
 * @ingroup tms570
 *
 * @brief clock functions definitions.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 * by embedded brains GmbH and others
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/tms570-rti.h>
#include <rtems/counter.h>

/**
 *  holds HW counter value since last interrupt event
 *  sets in tms570_clock_driver_support_at_tick
 *  used in tms570_clock_driver_nanoseconds_since_last_tick
 */
static uint32_t tms570_rti_last_tick_fcr0;

/**
 *  @brief Initialize the HW peripheral for clock driver
 *
 *  Clock driver is implemented by RTI module
 *
 * @retval Void
 */
static void tms570_clock_driver_support_initialize_hardware( void )
{

  uint32_t microsec_per_tick = rtems_configuration_get_microseconds_per_tick();

  rtems_counter_initialize_converter(BSP_PLL_OUT_CLOCK);

  /* Hardware specific initialize */
  TMS570_RTI.RTIGCTRL = 0;
  TMS570_RTI.RTICPUC0 = BSP_PLL_OUT_CLOCK /1000000 / 2; /* prescaler */
  TMS570_RTI.RTITBCTRL = 2;
  TMS570_RTI.RTICAPCTRL = 0;
  TMS570_RTI.RTICOMPCTRL = 0;
  /* set counter to zero */
  TMS570_RTI.RTIUC0 = 0;
  TMS570_RTI.RTIFRC0 = 0;
  /* clear interrupts*/
  TMS570_RTI.RTICLEARINTENA = 0x00070f0f;
  TMS570_RTI.RTIINTFLAG = 0x0007000f;
  /* set timer */
  TMS570_RTI.RTICOMP0 = TMS570_RTI.RTIFRC0 + microsec_per_tick;
  TMS570_RTI.RTICOMP0CLR = TMS570_RTI.RTICOMP0 + microsec_per_tick / 2;
  TMS570_RTI.RTIUDCP0 = microsec_per_tick;
  /* enable interupt */
  TMS570_RTI.RTISETINTENA = 0x1;
  /* enable timer */
  TMS570_RTI.RTIGCTRL = 1;
}

/**
 * @brief Clears interrupt source
 *
 * @retval Void
 */
static void tms570_clock_driver_support_at_tick( void )
{
  TMS570_RTI.RTIINTFLAG = 0x00000001;
  tms570_rti_last_tick_fcr0 = TMS570_RTI.RTICOMP0 - TMS570_RTI.RTIUDCP0;
}

/**
 * @brief registers RTI interrupt handler
 *
 * @param[in] Clock_isr new ISR handler
 * @param[in] Old_ticker old ISR handler (unused and type broken)
 *
 * @retval Void
 */
static void tms570_clock_driver_support_install_isr(
  rtems_isr_entry Clock_isr
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    TMS570_IRQ_TIMER_0,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

/**
 * @brief disables RTI interrupt
 *
 * Called when closing clock driver
 *
 * @retval Void
 */
static void tms570_clock_driver_support_shutdown_hardware( void )
{
  /* turn off the timer interrupts */
  TMS570_RTI.RTICLEARINTENA = 0x20000;
}

/**
 * @brief returns the nanoseconds since last tick
 *
 * Return the nanoseconds since last tick
 *
 * @retval x nanoseconds
 *
 */
static uint32_t tms570_clock_driver_nanoseconds_since_last_tick( void )
{
  uint32_t actual_fcr0 = TMS570_RTI.RTIFRC0;
  uint32_t usec_since_tick;

  usec_since_tick = actual_fcr0 - tms570_rti_last_tick_fcr0;

  return usec_since_tick * 1000;
}

#define Clock_driver_support_initialize_hardware \
                        tms570_clock_driver_support_initialize_hardware
#define Clock_driver_support_at_tick \
                        tms570_clock_driver_support_at_tick
#define Clock_driver_support_initialize_hardware \
                        tms570_clock_driver_support_initialize_hardware
#define Clock_driver_support_shutdown_hardware \
                        tms570_clock_driver_support_shutdown_hardware
#define Clock_driver_nanoseconds_since_last_tick \
                        tms570_clock_driver_nanoseconds_since_last_tick

#define Clock_driver_support_install_isr(Clock_isr, Old_ticker ) \
              tms570_clock_driver_support_install_isr( Clock_isr )

void Clock_isr(void *arg); /* to supress warning */

#include "../../../shared/clockdrv_shell.h"
