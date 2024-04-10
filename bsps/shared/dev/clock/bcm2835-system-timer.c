/**
 * @file
 *
 * @ingroup RTEMSDriverClockImpl
 *
 * @brief This source file contains the implementation of the BCM2835 Clock
 *   Driver.
 */

/*
 * Copyright (c) 2013 Alan Cudmore
 * Copyright (c) 2016 Pavel Pisa
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
*/

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/raspberrypi.h>
#include <rtems/timecounter.h>

static struct timecounter raspberrypi_tc;

static uint32_t raspberrypi_clock_get_timecount(struct timecounter *tc)
{
  return BCM2835_REG(BCM2835_GPU_TIMER_CLO);
}

static void raspberrypi_clock_at_tick(void)
{
  uint32_t act_val;
  uint32_t next_cmp = BCM2835_REG(BCM2835_GPU_TIMER_C3);
  next_cmp += rtems_configuration_get_microseconds_per_tick();
  BCM2835_REG(BCM2835_GPU_TIMER_C3) = next_cmp;
  act_val = BCM2835_REG(BCM2835_GPU_TIMER_CLO);

  /*
   * Clear interrupt only if there is time left to the next tick.
   * If time of the next tick has already passed then interrupt
   * request stays active and fires immediately after current tick
   * processing is finished.
   */
  if ((int32_t)(next_cmp - act_val) > 0)
    BCM2835_REG(BCM2835_GPU_TIMER_CS) = BCM2835_GPU_TIMER_CS_M3;
}

static void raspberrypi_clock_handler_install_isr(
  rtems_interrupt_handler clock_isr
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    BCM2835_IRQ_ID_GPU_TIMER_M3,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    clock_isr,
    NULL
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static void raspberrypi_clock_initialize_hardware(void)
{
  uint32_t next_cmp = BCM2835_REG(BCM2835_GPU_TIMER_CLO);
  next_cmp += rtems_configuration_get_microseconds_per_tick();
  BCM2835_REG(BCM2835_GPU_TIMER_C3) = next_cmp;
  BCM2835_REG(BCM2835_GPU_TIMER_CS) = BCM2835_GPU_TIMER_CS_M3;

  raspberrypi_tc.tc_get_timecount = raspberrypi_clock_get_timecount;
  raspberrypi_tc.tc_counter_mask = 0xffffffff;
  raspberrypi_tc.tc_frequency = 1000000; /* 1 MHz */
  raspberrypi_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&raspberrypi_tc);
}

#define Clock_driver_support_at_tick(arg) raspberrypi_clock_at_tick()

#define Clock_driver_support_initialize_hardware() raspberrypi_clock_initialize_hardware()

#define Clock_driver_support_install_isr(clock_isr) \
  raspberrypi_clock_handler_install_isr(clock_isr)

#define CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR 1

#include "../../../shared/dev/clock/clockimpl.h"
