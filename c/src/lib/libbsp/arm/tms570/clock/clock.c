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
#include <rtems/timecounter.h>

static struct timecounter tms570_rti_tc;

static uint32_t tms570_rti_get_timecount(struct timecounter *tc)
{
  return TMS570_RTI.RTIFRC0;
}

#ifndef TMS570_PREFERRED_TC_FREQUENCY
/*
 * Define preferred main time base counter frequency
 * The value of 1MHz is the best matching RTEMS
 * timing system because then there is no need
 * to scale RTEMS configuration microseconds_per_tick
 * parameter
 */
#define TMS570_PREFERRED_TC_FREQUENCY 1000000
#endif /* TMS570_PREFERRED_TC_FREQUENCY */

/**
 *  @brief Initialize the HW peripheral for clock driver
 *
 *  Clock driver is implemented by RTI module
 *
 * @retval Void
 */
static void tms570_clock_driver_support_initialize_hardware( void )
{

  uint32_t microsec_per_tick;
  uint32_t tc_frequency;
  uint32_t tc_prescaler;
  uint32_t tc_increments_per_tick;

  microsec_per_tick = rtems_configuration_get_microseconds_per_tick();
  tc_frequency = TMS570_PREFERRED_TC_FREQUENCY;

  rtems_counter_initialize_converter(BSP_PLL_OUT_CLOCK);

  tc_prescaler = (BSP_PLL_OUT_CLOCK + tc_frequency) / (tc_frequency * 2);

  /* Recompute actual most close frequency which can be realized */
  tc_frequency = (BSP_PLL_OUT_CLOCK + tc_prescaler) / (tc_prescaler * 2);

  /*
   * Recompute tick period to adjust for configurable or exact
   * preferred time base 1 usec resolution.
   */
  tc_increments_per_tick = ((uint64_t)microsec_per_tick * tc_frequency +
                           500000) / 1000000;

  /* Hardware specific initialize */
  TMS570_RTI.RTIGCTRL = 0;
  TMS570_RTI.RTICPUC0 = tc_prescaler - 1;
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
  TMS570_RTI.RTICOMP0 = TMS570_RTI.RTIFRC0 + tc_increments_per_tick;
  TMS570_RTI.RTICOMP0CLR = TMS570_RTI.RTICOMP0 + tc_increments_per_tick / 2;
  TMS570_RTI.RTIUDCP0 = tc_increments_per_tick;
  /* enable interupt */
  TMS570_RTI.RTISETINTENA = 0x1;
  /* enable timer */
  TMS570_RTI.RTIGCTRL = 1;
  /* set timecounter */
  tms570_rti_tc.tc_get_timecount = tms570_rti_get_timecount;
  tms570_rti_tc.tc_counter_mask = 0xffffffff;
  tms570_rti_tc.tc_frequency = tc_frequency;
  tms570_rti_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&tms570_rti_tc);
}

/**
 * @brief Clears interrupt source
 *
 * @retval Void
 */
static void tms570_clock_driver_support_at_tick( void )
{
  TMS570_RTI.RTIINTFLAG = 0x00000001;
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

#define Clock_driver_support_initialize_hardware \
                        tms570_clock_driver_support_initialize_hardware
#define Clock_driver_support_at_tick \
                        tms570_clock_driver_support_at_tick
#define Clock_driver_support_initialize_hardware \
                        tms570_clock_driver_support_initialize_hardware
#define Clock_driver_support_shutdown_hardware \
                        tms570_clock_driver_support_shutdown_hardware

#define Clock_driver_support_install_isr(Clock_isr, Old_ticker ) \
              tms570_clock_driver_support_install_isr( Clock_isr )

void Clock_isr(void *arg); /* to supress warning */

#include "../../../shared/clockdrv_shell.h"
