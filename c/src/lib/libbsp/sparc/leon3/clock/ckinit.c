/*
 *  Clock Tick Device Driver
 *
 *  This routine initializes LEON timer 1 which used for the clock tick.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bspopts.h>
#include <bsp/fatal.h>
#include <rtems/rtems/intr.h>
#include <ambapp.h>
#include <rtems/score/profiling.h>
#include <rtems/timecounter.h>

/* The LEON3 BSP Timer driver can rely on the Driver Manager if the
 * DrvMgr is initialized during startup. Otherwise the classic driver
 * must be used.
 *
 * The DrvMgr Clock driver is located in the shared/timer directory
 */
#ifndef RTEMS_DRVMGR_STARTUP

#if SIMSPARC_FAST_IDLE==1
#define CLOCK_DRIVER_USE_FAST_IDLE 1
#endif

/* LEON3 Timer system interrupt number */
static int clkirq;

static bool leon3_tc_use_irqmp;

static rtems_timecounter_simple leon3_tc;

static uint32_t leon3_tc_get(rtems_timecounter_simple *tc)
{
  return LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].value;
}

static bool leon3_tc_is_pending(rtems_timecounter_simple *tc)
{
  return LEON_Is_interrupt_pending(clkirq);
}

static uint32_t leon3_tc_get_timecount(struct timecounter *tc)
{
  return rtems_timecounter_simple_downcounter_get(
    tc,
    leon3_tc_get,
    leon3_tc_is_pending
  );
}

static uint32_t leon3_tc_get_timecount_irqmp(struct timecounter *tc)
{
  return LEON3_IrqCtrl_Regs->timestamp[0].counter;
}

static void leon3_clock_profiling_interrupt_delay(void)
{
#ifdef RTEMS_PROFILING
  /*
   * We need a small state machine to ignore the first clock interrupt, since
   * it contains the sequential system initialization time.  Do the timestamp
   * initialization on the fly.
   */
  static int state = 1;

  volatile struct irqmp_timestamp_regs *irqmp_ts =
    &LEON3_IrqCtrl_Regs->timestamp[0];
  unsigned int s1_s2 = (1U << 25) | (1U << 26);

  if (state == 0) {
    unsigned int first = irqmp_ts->assertion;
    unsigned int second = irqmp_ts->counter;

    irqmp_ts->control |= s1_s2;

    _Profiling_Update_max_interrupt_delay(_Per_CPU_Get(), second - first);
  } else if (state == 1 && leon3_irqmp_has_timestamp(irqmp_ts)) {
    unsigned int ks = 1U << 5;

    state = 0;

    irqmp_ts->control = ks | s1_s2 | (unsigned int) clkirq;
  } else if (state == 1) {
    state = 2;
  }
#endif
}

static void leon3_tc_at_tick( rtems_timecounter_simple *tc )
{
  leon3_clock_profiling_interrupt_delay();
}

static void leon3_tc_tick(void)
{
  if (leon3_tc_use_irqmp) {
    leon3_clock_profiling_interrupt_delay();
    rtems_timecounter_tick();
  } else {
    rtems_timecounter_simple_downcounter_tick(
      &leon3_tc,
      leon3_tc_get,
      leon3_tc_at_tick
    );
  }
}

#define Adjust_clkirq_for_node() do { clkirq += LEON3_CLOCK_INDEX; } while(0)

#define Clock_driver_support_find_timer() \
  do { \
    /* Assume timer found during BSP initialization */ \
    if (LEON3_Timer_Regs) { \
      clkirq = (LEON3_Timer_Regs->cfg & 0xf8) >> 3; \
      \
      Adjust_clkirq_for_node(); \
    } \
  } while (0)

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    (_old) = NULL; \
    bsp_clock_handler_install(_new); \
  } while(0)

static void bsp_clock_handler_install(rtems_isr *new)
{
  rtems_status_code sc;

  sc = rtems_interrupt_handler_install(
    clkirq,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    new,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal(RTEMS_FATAL_SOURCE_BSP, LEON3_FATAL_CLOCK_INITIALIZATION);
  }
}

static void leon3_clock_initialize(void)
{
  volatile struct irqmp_timestamp_regs *irqmp_ts =
    &LEON3_IrqCtrl_Regs->timestamp[0];

  LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].reload =
    rtems_configuration_get_microseconds_per_tick() - 1;
  LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].ctrl =
    GPTIMER_TIMER_CTRL_EN | GPTIMER_TIMER_CTRL_RS |
      GPTIMER_TIMER_CTRL_LD | GPTIMER_TIMER_CTRL_IE;

  if (leon3_irqmp_has_timestamp(irqmp_ts)) {
    leon3_tc.tc.tc_get_timecount = leon3_tc_get_timecount_irqmp;
    leon3_tc.tc.tc_counter_mask = 0xffffffff;
    leon3_tc.tc.tc_frequency = ambapp_freq_get(&ambapp_plb, LEON3_Timer_Adev);
    leon3_tc.tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
    leon3_tc_use_irqmp = true;
    rtems_timecounter_install(&leon3_tc.tc);
  } else {
    rtems_timecounter_simple_install(
      &leon3_tc,
      LEON3_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER,
      rtems_configuration_get_microseconds_per_tick(),
      leon3_tc_get_timecount
    );
  }
}

#define Clock_driver_support_initialize_hardware() \
  leon3_clock_initialize()

#define Clock_driver_support_shutdown_hardware() \
  do { \
    LEON_Mask_interrupt(LEON_TRAP_TYPE(clkirq)); \
    LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].ctrl = 0; \
  } while (0)

#define Clock_driver_timecounter_tick() leon3_tc_tick()

#include "../../../shared/clockdrv_shell.h"

#endif
