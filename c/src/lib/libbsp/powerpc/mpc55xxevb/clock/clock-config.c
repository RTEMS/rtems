/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Clock driver configuration.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <mpc55xx/regs.h>
#include <mpc55xx/emios.h>

#include <rtems.h>

#include <bsp.h>
#include <bsp/irq.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

#define MPC55XX_CLOCK_EMIOS_CHANNEL (MPC55XX_EMIOS_CHANNEL_NUMBER - 1)

/* This is defined in clockdrv_shell.h */
rtems_isr Clock_isr( rtems_vector_number vector);

#define Clock_driver_support_at_tick() \
 do { \
    union EMIOS_CSR_tag csr = MPC55XX_ZERO_FLAGS; \
    csr.B.FLAG = 1; \
    EMIOS.CH [MPC55XX_CLOCK_EMIOS_CHANNEL].CSR.R = csr.R; \
 } while (0)

static void mpc55xx_clock_handler_install( rtems_isr_entry isr, 
					   rtems_isr_entry *old_isr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = mpc55xx_interrupt_handler_install(
    MPC55XX_IRQ_EMIOS_GET_REQUEST( MPC55XX_CLOCK_EMIOS_CHANNEL),
    "clock",
    RTEMS_INTERRUPT_UNIQUE,
    MPC55XX_INTC_MIN_PRIORITY,
    (rtems_interrupt_handler) isr,
    NULL
  );
 *old_isr = NULL;
  RTEMS_CHECK_SC_VOID( sc, "install clock interrupt handler");
}

static void mpc55xx_clock_initialize( void)
{
  volatile struct EMIOS_CH_tag *regs = &EMIOS.CH [MPC55XX_CLOCK_EMIOS_CHANNEL];
  union EMIOS_CCR_tag ccr = MPC55XX_ZERO_FLAGS;
  union EMIOS_CSR_tag csr = MPC55XX_ZERO_FLAGS;
  unsigned prescaler = mpc55xx_emios_global_prescaler();
  uint64_t interval = ((uint64_t) bsp_clock_speed
    * (uint64_t) rtems_configuration_get_microseconds_per_tick()) / 1000000;

  /* Apply prescaler */
  if (prescaler > 0) {
    interval /= (uint64_t) prescaler;
  } else {
    RTEMS_SYSLOG_ERROR( "unexpected global eMIOS prescaler\n");
  }

  /* Check interval */
  if (interval == 0 || interval > MPC55XX_EMIOS_VALUE_MAX) {
    interval = MPC55XX_EMIOS_VALUE_MAX;
    RTEMS_SYSLOG_ERROR( "clock timer interval out of range\n");
  }

  /* Configure eMIOS channel */

  /* Set channel in GPIO mode */
  ccr.B.MODE = MPC55XX_EMIOS_MODE_GPIO_INPUT;
  regs->CCR.R = ccr.R;

  /* Clear status flags */
  csr.B.OVR = 1;
  csr.B.OVFL = 1;
  csr.B.FLAG = 1;
  regs->CSR.R = csr.R;

  /* Set timer period */
  regs->CADR.R = (uint32_t) interval - 1;

  /* Set unused registers */
  regs->CBDR.R = 0;
  regs->CCNTR.R = 0;
  regs->ALTCADR.R = 0;

  /* Set control register */
  ccr.B.MODE = MPC55XX_EMIOS_MODE_MCB_UP_INT_CLK;
  ccr.B.UCPREN = 1;
  ccr.B.FEN = 1;
  ccr.B.FREN = 1;
  regs->CCR.R = ccr.R;
}

static void mpc55xx_clock_cleanup( void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  volatile struct EMIOS_CH_tag *regs = &EMIOS.CH [MPC55XX_CLOCK_EMIOS_CHANNEL];
  union EMIOS_CCR_tag ccr = MPC55XX_ZERO_FLAGS;

  /* Set channel in GPIO mode */
  ccr.B.MODE = MPC55XX_EMIOS_MODE_GPIO_INPUT;
  regs->CCR.R = ccr.R;

  /* Remove interrupt handler */
  sc = rtems_interrupt_handler_remove(
    MPC55XX_IRQ_EMIOS_GET_REQUEST( MPC55XX_CLOCK_EMIOS_CHANNEL),
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  RTEMS_CHECK_SC_VOID( sc, "remove clock interrupt handler");
}

static uint32_t mpc55xx_clock_nanoseconds_since_last_tick( void)
{
  uint64_t clicks = EMIOS.CH [MPC55XX_CLOCK_EMIOS_CHANNEL].CCNTR.R;
  uint64_t clock = bsp_clock_speed;
  uint64_t ns = (clicks * 1000000000) / clock;

  return (uint32_t) ns;
}

#define Clock_driver_support_initialize_hardware() mpc55xx_clock_initialize()

#define Clock_driver_support_install_isr( isr, old_isr) \
  mpc55xx_clock_handler_install(isr,&old_isr)

#define Clock_driver_support_shutdown_hardware() mpc55xx_clock_cleanup()

#define Clock_driver_nanoseconds_since_last_tick \
  mpc55xx_clock_nanoseconds_since_last_tick

/* Include shared source clock driver code */
#include "../../../../libbsp/shared/clockdrv_shell.h"
