/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Clock driver configuration.
 */

/*
 * Copyright (c) 2009-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>

#include <mpc55xx/regs.h>

static uint64_t mpc55xx_clock_factor;

#if defined(MPC55XX_CLOCK_EMIOS_CHANNEL)

#include <mpc55xx/emios.h>

static void mpc55xx_clock_at_tick(void)
{
  union EMIOS_CSR_tag csr = MPC55XX_ZERO_FLAGS;
  csr.B.FLAG = 1;
  EMIOS.CH [MPC55XX_CLOCK_EMIOS_CHANNEL].CSR.R = csr.R;
}

static void mpc55xx_clock_handler_install(rtems_isr_entry isr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = mpc55xx_interrupt_handler_install(
    MPC55XX_IRQ_EMIOS(MPC55XX_CLOCK_EMIOS_CHANNEL),
    "clock",
    RTEMS_INTERRUPT_UNIQUE,
    MPC55XX_INTC_MIN_PRIORITY,
    (rtems_interrupt_handler) isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static void mpc55xx_clock_initialize(void)
{
  volatile struct EMIOS_CH_tag *regs = &EMIOS.CH [MPC55XX_CLOCK_EMIOS_CHANNEL];
  union EMIOS_CCR_tag ccr = MPC55XX_ZERO_FLAGS;
  union EMIOS_CSR_tag csr = MPC55XX_ZERO_FLAGS;
  unsigned prescaler = mpc55xx_emios_global_prescaler();
  uint64_t reference_clock = bsp_clock_speed;
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint64_t interval = (reference_clock * us_per_tick) / 1000000;

  mpc55xx_clock_factor = (1000000000ULL << 32) / reference_clock;

  /* Apply prescaler */
  if (prescaler > 0) {
    interval /= (uint64_t) prescaler;
  } else {
    rtems_fatal_error_occurred(0xdeadbeef);
  }

  /* Check interval */
  if (interval == 0 || interval > MPC55XX_EMIOS_VALUE_MAX) {
    rtems_fatal_error_occurred(0xdeadbeef);
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

  /* Set internal counter start value */
  regs->CCNTR.R = 1;

  /* Set timer period */
  regs->CADR.R = (uint32_t) interval - 1;

  /* Set control register */
  #if MPC55XX_CHIP_TYPE / 10 == 551
    ccr.B.MODE = MPC55XX_EMIOS_MODE_MCB_UP_INT_CLK;
  #else
    ccr.B.MODE = MPC55XX_EMIOS_MODE_MC_UP_INT_CLK;
  #endif
  ccr.B.UCPREN = 1;
  ccr.B.FEN = 1;
  ccr.B.FREN = 1;
  regs->CCR.R = ccr.R;
}

static void mpc55xx_clock_cleanup(void)
{
  volatile struct EMIOS_CH_tag *regs = &EMIOS.CH [MPC55XX_CLOCK_EMIOS_CHANNEL];
  union EMIOS_CCR_tag ccr = MPC55XX_ZERO_FLAGS;

  /* Set channel in GPIO mode */
  ccr.B.MODE = MPC55XX_EMIOS_MODE_GPIO_INPUT;
  regs->CCR.R = ccr.R;
}

static uint32_t mpc55xx_clock_nanoseconds_since_last_tick(void)
{
  volatile struct EMIOS_CH_tag *regs = &EMIOS.CH [MPC55XX_CLOCK_EMIOS_CHANNEL];
  uint64_t c = regs->CCNTR.R;
  union EMIOS_CSR_tag csr = { .R = regs->CSR.R };
  uint64_t k = mpc55xx_clock_factor;

  if (csr.B.FLAG != 0) {
    c = regs->CCNTR.R + regs->CADR.R + 1;
  }

  return (uint32_t) ((c * k) >> 32);
}

#elif defined(MPC55XX_CLOCK_PIT_CHANNEL)

static void mpc55xx_clock_at_tick(void)
{
  volatile PIT_RTI_CHANNEL_tag *channel =
    &PIT_RTI.CHANNEL [MPC55XX_CLOCK_PIT_CHANNEL];
  PIT_RTI_TFLG_32B_tag tflg = { .B = { .TIF = 1 } };

  channel->TFLG.R = tflg.R;
}

static void mpc55xx_clock_handler_install(rtems_isr_entry isr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = mpc55xx_interrupt_handler_install(
    MPC55XX_IRQ_PIT_CHANNEL(MPC55XX_CLOCK_PIT_CHANNEL),
    "clock",
    RTEMS_INTERRUPT_UNIQUE,
    MPC55XX_INTC_MIN_PRIORITY,
    (rtems_interrupt_handler) isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static void mpc55xx_clock_initialize(void)
{
  volatile PIT_RTI_CHANNEL_tag *channel =
    &PIT_RTI.CHANNEL [MPC55XX_CLOCK_PIT_CHANNEL];
  uint64_t reference_clock = bsp_clock_speed;
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint64_t interval = (reference_clock * us_per_tick) / 1000000;
  PIT_RTI_PITMCR_32B_tag pitmcr = { .B = { .FRZ = 1 } };
  PIT_RTI_TCTRL_32B_tag tctrl = { .B = { .TIE = 1, .TEN = 1 } };

  mpc55xx_clock_factor = (1000000000ULL << 32) / reference_clock;

  PIT_RTI.PITMCR.R = pitmcr.R;
  channel->LDVAL.R = interval;
  channel->TCTRL.R = tctrl.R;
}

static void mpc55xx_clock_cleanup(void)
{
  volatile PIT_RTI_CHANNEL_tag *channel =
    &PIT_RTI.CHANNEL [MPC55XX_CLOCK_PIT_CHANNEL];

  channel->TCTRL.R = 0;
}

static uint32_t mpc55xx_clock_nanoseconds_since_last_tick(void)
{
  volatile PIT_RTI_CHANNEL_tag *channel =
    &PIT_RTI.CHANNEL [MPC55XX_CLOCK_PIT_CHANNEL];
  uint32_t c = channel->CVAL.R;
  uint32_t i = channel->LDVAL.R;
  uint64_t k = mpc55xx_clock_factor;

  if (channel->TFLG.B.TIF != 0) {
    c = channel->CVAL.R - i;
  }

  return (uint32_t) (((i - c) * k) >> 32);
}

#endif

#define Clock_driver_support_at_tick() \
  mpc55xx_clock_at_tick()
#define Clock_driver_support_initialize_hardware() \
  mpc55xx_clock_initialize()
#define Clock_driver_support_install_isr(isr, old_isr) \
  do { \
    mpc55xx_clock_handler_install(isr); \
    old_isr = NULL; \
  } while (0)
#define Clock_driver_support_shutdown_hardware() \
  mpc55xx_clock_cleanup()
#define Clock_driver_nanoseconds_since_last_tick \
  mpc55xx_clock_nanoseconds_since_last_tick

/* Include shared source clock driver code */
#include "../../../../libbsp/shared/clockdrv_shell.h"
