/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief Clock driver configuration.
 */

/*
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>

#include <mpc55xx/regs.h>

#include <rtems/timecounter.h>

void Clock_isr(void *arg);

static rtems_timecounter_simple mpc55xx_tc;

#if defined(MPC55XX_CLOCK_EMIOS_CHANNEL)

#include <mpc55xx/emios.h>

static uint32_t mpc55xx_tc_get(rtems_timecounter_simple *tc)
{
  return EMIOS.CH [MPC55XX_CLOCK_EMIOS_CHANNEL].CCNTR.R;
}

static bool mpc55xx_tc_is_pending(rtems_timecounter_simple *tc)
{
  return EMIOS.CH [MPC55XX_CLOCK_EMIOS_CHANNEL].CSR.B.FLAG != 0;
}

static uint32_t mpc55xx_tc_get_timecount(struct timecounter *tc)
{
  return rtems_timecounter_simple_upcounter_get(
    tc,
    mpc55xx_tc_get,
    mpc55xx_tc_is_pending
  );
}

static void mpc55xx_tc_at_tick(rtems_timecounter_simple *tc)
{
  union EMIOS_CSR_tag csr = MPC55XX_ZERO_FLAGS;
  csr.B.FLAG = 1;
  EMIOS.CH [MPC55XX_CLOCK_EMIOS_CHANNEL].CSR.R = csr.R;
}

static void mpc55xx_tc_tick(void)
{
  rtems_timecounter_simple_upcounter_tick(
    &mpc55xx_tc,
    mpc55xx_tc_get,
    mpc55xx_tc_at_tick
  );
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
    bsp_fatal(MPC55XX_FATAL_CLOCK_EMIOS_IRQ_INSTALL);
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

  /* Apply prescaler */
  if (prescaler > 0) {
    interval /= (uint64_t) prescaler;
  } else {
    bsp_fatal(MPC55XX_FATAL_CLOCK_EMIOS_PRESCALER);
  }

  /* Check interval */
  if (interval == 0 || interval > MPC55XX_EMIOS_VALUE_MAX) {
    bsp_fatal(MPC55XX_FATAL_CLOCK_EMIOS_INTERVAL);
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
  #if MPC55XX_CHIP_FAMILY == 551
    ccr.B.MODE = MPC55XX_EMIOS_MODE_MCB_UP_INT_CLK;
  #else
    ccr.B.MODE = MPC55XX_EMIOS_MODE_MC_UP_INT_CLK;
  #endif
  ccr.B.UCPREN = 1;
  ccr.B.FEN = 1;
  ccr.B.FREN = 1;
  regs->CCR.R = ccr.R;

  rtems_timecounter_simple_install(
    &mpc55xx_tc,
    reference_clock,
    interval,
    mpc55xx_tc_get_timecount
  );
}

#elif defined(MPC55XX_CLOCK_PIT_CHANNEL)

static uint32_t mpc55xx_tc_get(rtems_timecounter_simple *tc)
{
  return PIT_RTI.CHANNEL [MPC55XX_CLOCK_PIT_CHANNEL].CVAL.R;
}

static bool mpc55xx_tc_is_pending(rtems_timecounter_simple *tc)
{
  return PIT_RTI.CHANNEL [MPC55XX_CLOCK_PIT_CHANNEL].TFLG.B.TIF != 0;
}

static uint32_t mpc55xx_tc_get_timecount(struct timecounter *tc)
{
  return rtems_timecounter_simple_downcounter_get(
    tc,
    mpc55xx_tc_get,
    mpc55xx_tc_is_pending
  );
}

static void mpc55xx_tc_at_tick(rtems_timecounter_simple *tc)
{
  volatile PIT_RTI_CHANNEL_tag *channel =
    &PIT_RTI.CHANNEL [MPC55XX_CLOCK_PIT_CHANNEL];
  PIT_RTI_TFLG_32B_tag tflg = { .B = { .TIF = 1 } };

  channel->TFLG.R = tflg.R;
}

static void mpc55xx_tc_tick(void)
{
  rtems_timecounter_simple_downcounter_tick(
    &mpc55xx_tc,
    mpc55xx_tc_get,
    mpc55xx_tc_at_tick
  );
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
    bsp_fatal(MPC55XX_FATAL_CLOCK_PIT_IRQ_INSTALL);
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

  PIT_RTI.PITMCR.R = pitmcr.R;
  channel->LDVAL.R = interval;
  channel->TCTRL.R = tctrl.R;

  rtems_timecounter_simple_install(
    &mpc55xx_tc,
    reference_clock,
    interval,
    mpc55xx_tc_get_timecount
  );
}

#endif

#define Clock_driver_timecounter_tick() mpc55xx_tc_tick()
#define Clock_driver_support_initialize_hardware() \
  mpc55xx_clock_initialize()
#define Clock_driver_support_install_isr(isr) \
  mpc55xx_clock_handler_install(isr)

/* Include shared source clock driver code */
#include "../../../shared/dev/clock/clockimpl.h"
