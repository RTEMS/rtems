/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief Clock driver configuration.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

#include <bsp.h>
#include <bsp/lpc24xx.h>
#include <bsp/irq.h>
#include <bsp/system-clocks.h>

/* This is defined in ../../../shared/clockdrv_shell.h */
rtems_isr Clock_isr( rtems_vector_number vector);

#define Clock_driver_support_at_tick() \
 do { \
   T0IR = TIR_MR0; \
 } while (0)

static void lpc24xx_clock_handler_install( void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    LPC24XX_IRQ_TIMER_0,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  RTEMS_CHECK_SC_VOID( sc, "Install clock interrupt handler");
}

static void lpc24xx_clock_initialize( void)
{
  rtems_interrupt_level level;
  uint64_t interval = ((uint64_t) lpc24xx_cclk()
    * (uint64_t) rtems_configuration_get_microseconds_per_tick()) / 1000000;

  /* Set timer pclk to cclk */
  rtems_interrupt_disable( level);
  SET_PCLKSEL0_PCLK_TIMER0( PCLKSEL0, 1);
  rtems_interrupt_enable( level);

  /* Reset timer */
  T0TCR = TCR_RST;

  /* Clear interrupt flags */
  T0IR = 0xff;

  /* Set timer mode */
  T0CCR = 0;

  /* Timer is incremented every pclk tick */
  T0PR = 0;

  /* Set match registers */
  T0MR0 = (uint32_t) interval;
  T0MR1 = 0xdeadbeef;
  T0MR2 = 0xdeadbeef;
  T0MR3 = 0xdeadbeef;

  /* Generate interrupt and reset counter on match with MR0 */
  T0MCR = TMCR_MR0I | TMCR_MR0R;

  /* No external match */
  T0EMR = 0;

  /* Enable timer */
  T0TCR = TCR_EN;
}

static void lpc24xx_clock_cleanup( void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  /* Disable timer */
  T0TCR = 0;

  /* Remove interrupt handler */
  sc = rtems_interrupt_handler_remove(
    LPC24XX_IRQ_TIMER_0,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  RTEMS_CHECK_SC_VOID( sc, "Remove clock interrupt handler");
}

static uint32_t lpc24xx_clock_nanoseconds_since_last_tick( void)
{
  uint64_t clock = lpc24xx_cclk();
  uint32_t clicks = T0TC;
  uint64_t ns = ((uint64_t) clicks * 1000000000) / clock;
  
  return (uint32_t) ns;
}
 
#define Clock_driver_support_initialize_hardware() lpc24xx_clock_initialize()

#define Clock_driver_support_install_isr( isr, old_isr) lpc24xx_clock_handler_install()

#define Clock_driver_support_shutdown_hardware() lpc24xx_clock_cleanup()
  
#define Clock_driver_nanoseconds_since_last_tick lpc24xx_clock_nanoseconds_since_last_tick

/* Include shared source clock driver code */
#include "../../../../libbsp/shared/clockdrv_shell.h"
