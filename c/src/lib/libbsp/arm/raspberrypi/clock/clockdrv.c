/*
 * BCM2835 Clock driver
 *
 * Copyright (c) 2013 Alan Cudmore
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE
 *
*/

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/raspberrypi.h>

/* This is defined in ../../../shared/clockdrv_shell.h */
void Clock_isr(rtems_irq_hdl_param arg);

static void raspberrypi_clock_at_tick(void)
{
   BCM2835_REG(BCM2835_TIMER_CLI) = 0;
}

static void raspberrypi_clock_handler_install(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    BCM2835_IRQ_ID_TIMER_0,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static void raspberrypi_clock_initialize(void)
{
   BCM2835_REG(BCM2835_TIMER_CTL) = 0x003E0000;
   BCM2835_REG(BCM2835_TIMER_LOD) = 10000 - 1;
   BCM2835_REG(BCM2835_TIMER_RLD) = 10000 - 1;
   BCM2835_REG(BCM2835_TIMER_DIV) = BCM2835_TIMER_PRESCALE;
   BCM2835_REG(BCM2835_TIMER_CLI) = 0;
   BCM2835_REG(BCM2835_TIMER_CTL) = 0x003E00A2;
}

static void raspberrypi_clock_cleanup(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  /* Remove interrupt handler */
  sc = rtems_interrupt_handler_remove(
    BCM2835_IRQ_ID_TIMER_0,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

/*
 *  Return the nanoseconds since last tick
 */
static uint32_t raspberrypi_clock_nanoseconds_since_last_tick(void)
{
  return 0;
}

#define Clock_driver_support_at_tick() raspberrypi_clock_at_tick()

#define Clock_driver_support_initialize_hardware() raspberrypi_clock_initialize()

#define Clock_driver_support_install_isr(isr, old_isr) \
  do {                                                 \
    raspberrypi_clock_handler_install();               \
    old_isr = NULL;                                    \
  } while (0)

#define Clock_driver_support_shutdown_hardware() raspberrypi_clock_cleanup()

#define Clock_driver_nanoseconds_since_last_tick \
  raspberrypi_clock_nanoseconds_since_last_tick


#include "../../../shared/clockdrv_shell.h"
