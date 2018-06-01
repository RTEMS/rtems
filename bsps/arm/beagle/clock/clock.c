/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief Clock driver configuration.
 */

/*
 * Copyright (c) 2014 Ben Gras <beng@shrike-systems.com>.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/timecounter.h>
#include <bsp.h>

#include <libcpu/omap_timer.h>

static struct timecounter beagle_clock_tc;

static omap_timer_registers_t regs_v1 = {
  .TIDR = OMAP3_TIMER_TIDR,
  .TIOCP_CFG = OMAP3_TIMER_TIOCP_CFG,
  .TISTAT = OMAP3_TIMER_TISTAT,
  .TISR = OMAP3_TIMER_TISR,
  .TIER = OMAP3_TIMER_TIER,
  .TWER = OMAP3_TIMER_TWER,
  .TCLR = OMAP3_TIMER_TCLR,
  .TCRR = OMAP3_TIMER_TCRR,
  .TLDR = OMAP3_TIMER_TLDR,
  .TTGR = OMAP3_TIMER_TTGR,
  .TWPS = OMAP3_TIMER_TWPS,
  .TMAR = OMAP3_TIMER_TMAR,
  .TCAR1 = OMAP3_TIMER_TCAR1,
  .TSICR = OMAP3_TIMER_TSICR,
  .TCAR2 = OMAP3_TIMER_TCAR2,
  .TPIR = OMAP3_TIMER_TPIR,
  .TNIR = OMAP3_TIMER_TNIR,
  .TCVR = OMAP3_TIMER_TCVR,
  .TOCR = OMAP3_TIMER_TOCR,
  .TOWR = OMAP3_TIMER_TOWR,
};

#if IS_AM335X
/* AM335X has a different ip block for the non 1ms timers */
static omap_timer_registers_t regs_v2 = {
  .TIDR = AM335X_TIMER_TIDR,
  .TIOCP_CFG = AM335X_TIMER_TIOCP_CFG,
  .TISTAT = AM335X_TIMER_IRQSTATUS_RAW,
  .TISR = AM335X_TIMER_IRQSTATUS,
  .TIER = AM335X_TIMER_IRQENABLE_SET,
  .TWER = AM335X_TIMER_IRQWAKEEN,
  .TCLR = AM335X_TIMER_TCLR,
  .TCRR = AM335X_TIMER_TCRR,
  .TLDR = AM335X_TIMER_TLDR,
  .TTGR = AM335X_TIMER_TTGR,
  .TWPS = AM335X_TIMER_TWPS,
  .TMAR = AM335X_TIMER_TMAR,
  .TCAR1 = AM335X_TIMER_TCAR1,
  .TSICR = AM335X_TIMER_TSICR,
  .TCAR2 = AM335X_TIMER_TCAR2,
  .TPIR = -1,		/* UNDEF */
  .TNIR = -1,		/* UNDEF */
  .TCVR = -1,		/* UNDEF */
  .TOCR = -1,		/* UNDEF */
  .TOWR = -1		/* UNDEF */
};
#endif

/* which timers are in use? target-dependent.
 * initialize at compile time.
 */

#if IS_DM3730

static omap_timer_t dm37xx_timer = {
  .base = OMAP3_GPTIMER1_BASE,
  .irq_nr = OMAP3_GPT1_IRQ,
  .regs = &regs_v1
};

/* free running timer */
static omap_timer_t dm37xx_fr_timer = {
  .base = OMAP3_GPTIMER10_BASE,
  .irq_nr = OMAP3_GPT10_IRQ,
  .regs = &regs_v1
};

static struct omap_timer *fr_timer = &dm37xx_fr_timer;
static struct omap_timer *timer = &dm37xx_timer;

#endif

#if IS_AM335X

/* normal timer */
static omap_timer_t am335x_timer = {
  .base = AM335X_DMTIMER1_1MS_BASE,
  .irq_nr = AM335X_INT_TINT1_1MS,
  .regs = &regs_v1
};

/* free running timer */
static omap_timer_t am335x_fr_timer = {
  .base = AM335X_DMTIMER7_BASE,
  .irq_nr = AM335X_INT_TINT7,
  .regs = &regs_v2
};

static struct omap_timer *fr_timer = &am335x_fr_timer;
static struct omap_timer *timer = &am335x_timer;

#endif

#if IS_AM335X
#define FRCLOCK_HZ (16*1500000)
#endif

#if IS_DM3730
#define FRCLOCK_HZ (8*1625000)
#endif

#ifndef FRCLOCK_HZ
#error expected IS_AM335X or IS_DM3730 to be defined.
#endif

static void
omap3_frclock_init(void)
{
  uint32_t tisr;

#if IS_DM3730
  /* Stop timer */
  mmio_clear(fr_timer->base + fr_timer->regs->TCLR,
      OMAP3_TCLR_ST);

  /* Use functional clock source for GPTIMER10 */
  mmio_set(OMAP3_CM_CLKSEL_CORE, OMAP3_CLKSEL_GPT10);
#endif

#if IS_AM335X
  /* Disable the module and wait for the module to be disabled */
  set32(CM_PER_TIMER7_CLKCTRL, CM_MODULEMODE_MASK,
      CM_MODULEMODE_DISABLED);
  while ((mmio_read(CM_PER_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
      != CM_CLKCTRL_IDLEST_DISABLE);

  set32(CLKSEL_TIMER7_CLK, CLKSEL_TIMER7_CLK_SEL_MASK,
      CLKSEL_TIMER7_CLK_SEL_SEL2);
  while ((read32(CLKSEL_TIMER7_CLK) & CLKSEL_TIMER7_CLK_SEL_MASK)
      != CLKSEL_TIMER7_CLK_SEL_SEL2);

  /* enable the module and wait for the module to be ready */
  set32(CM_PER_TIMER7_CLKCTRL, CM_MODULEMODE_MASK,
      CM_MODULEMODE_ENABLE);
  while ((mmio_read(CM_PER_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
      != CM_CLKCTRL_IDLEST_FUNC);

  /* Stop timer */
  mmio_clear(fr_timer->base + fr_timer->regs->TCLR,
      OMAP3_TCLR_ST);
#endif

  /* Start and auto-reload at 0 */
  mmio_write(fr_timer->base + fr_timer->regs->TLDR, 0x0);
  mmio_write(fr_timer->base + fr_timer->regs->TCRR, 0x0);

  /* Set up overflow interrupt */
  tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
      OMAP3_TISR_TCAR_IT_FLAG;
  /* Clear interrupt status */
  mmio_write(fr_timer->base + fr_timer->regs->TISR, tisr);
  mmio_write(fr_timer->base + fr_timer->regs->TIER,
      OMAP3_TIER_OVF_IT_ENA);

  /* Start timer, without prescaler */
  mmio_set(fr_timer->base + fr_timer->regs->TCLR,
      OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST);
}

static uint32_t
beagle_clock_get_timecount(struct timecounter *tc)
{
  return mmio_read(fr_timer->base + fr_timer->regs->TCRR);
}

static void
beagle_clock_initialize(void)
{
  uint32_t freq = 1000000UL/rtems_configuration_get_microseconds_per_tick();

  /* we only support 1ms resolution */
  uint32_t tisr;
#if IS_DM3730
  /* Stop timer */
  mmio_clear(timer->base + timer->regs->TCLR, OMAP3_TCLR_ST);

  /* Use 32 KHz clock source for GPTIMER1 */
  mmio_clear(OMAP3_CM_CLKSEL_WKUP, OMAP3_CLKSEL_GPT1);
#endif

#if IS_AM335X
  /* disable the module and wait for the module to be disabled */
  set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_MASK,
      CM_MODULEMODE_DISABLED);
  while ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
      != CM_CLKCTRL_IDLEST_DISABLE);

  set32(CLKSEL_TIMER1MS_CLK, CLKSEL_TIMER1MS_CLK_SEL_MASK,
      CLKSEL_TIMER1MS_CLK_SEL_SEL2);
  while ((read32(CLKSEL_TIMER1MS_CLK) &
    CLKSEL_TIMER1MS_CLK_SEL_MASK) !=
      CLKSEL_TIMER1MS_CLK_SEL_SEL2);

  /* enable the module and wait for the module to be ready */
  set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_MASK,
      CM_MODULEMODE_ENABLE);
  while ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
      != CM_CLKCTRL_IDLEST_FUNC);

  /* Stop timer */
  mmio_clear(timer->base + timer->regs->TCLR, OMAP3_TCLR_ST);
#endif

  /* Use 1-ms tick mode for GPTIMER1 TRM 16.2.4.2.1 */
  mmio_write(timer->base + timer->regs->TPIR, 232000);
  mmio_write(timer->base + timer->regs->TNIR, -768000);
  mmio_write(timer->base + timer->regs->TLDR,
      0xffffffff - (32768 / freq) + 1);
  mmio_write(timer->base + timer->regs->TCRR,
      0xffffffff - (32768 / freq) + 1);

  /* Set up overflow interrupt */
  tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
      OMAP3_TISR_TCAR_IT_FLAG;
  /* Clear interrupt status */
  mmio_write(timer->base + timer->regs->TISR, tisr);
  mmio_write(timer->base + timer->regs->TIER, OMAP3_TIER_OVF_IT_ENA);

  /* Start timer */
  mmio_set(timer->base + timer->regs->TCLR,
      OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST);
  /* also initilize the free runnning timer */
  omap3_frclock_init();

#if IS_AM335X
  /* Disable AM335X watchdog */
  mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0xAAAA);
  while(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
  mmio_write(AM335X_WDT_BASE+AM335X_WDT_WSPR, 0x5555);
  while(mmio_read(AM335X_WDT_BASE+AM335X_WDT_WWPS) != 0) ;
#endif

  /* Install timecounter */ \
  beagle_clock_tc.tc_get_timecount = beagle_clock_get_timecount;
  beagle_clock_tc.tc_counter_mask = 0xffffffff;
  beagle_clock_tc.tc_frequency = FRCLOCK_HZ;
  beagle_clock_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&beagle_clock_tc);
}

static void beagle_clock_at_tick(void)
{
  mmio_write(timer->base + timer->regs->TISR,
    OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
      OMAP3_TISR_TCAR_IT_FLAG);
}

static rtems_interrupt_handler clock_isr = NULL;

static void beagle_clock_handler_install(rtems_interrupt_handler isr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    timer->irq_nr,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    isr,
    NULL
  );

  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
  clock_isr = isr;
}

#define Clock_driver_support_at_tick() beagle_clock_at_tick()
#define Clock_driver_support_initialize_hardware() beagle_clock_initialize()
#define Clock_driver_support_install_isr(isr) \
  beagle_clock_handler_install(isr)

/* Include shared source clock driver code */
#include "../../shared/dev/clock/clockimpl.h"
