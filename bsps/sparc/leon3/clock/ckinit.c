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
 *  Copyright (c) 2014, 2018 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bspopts.h>
#include <bsp/fatal.h>
#include <rtems/rtems/intr.h>
#include <grlib/ambapp.h>
#include <rtems/score/profiling.h>
#include <rtems/score/sparcimpl.h>
#include <rtems/timecounter.h>

/* The LEON3 BSP Timer driver can rely on the Driver Manager if the
 * DrvMgr is initialized during startup. Otherwise the classic driver
 * must be used.
 *
 * The DrvMgr Clock driver is located in the shared/timer directory
 */
#ifndef RTEMS_DRVMGR_STARTUP

/* LEON3 Timer system interrupt number */
static int clkirq;

static void (*leon3_tc_tick)(void);

static struct timecounter leon3_tc;

#ifdef RTEMS_PROFILING
#define IRQMP_TIMESTAMP_S1_S2 ((1U << 25) | (1U << 26))

static void leon3_tc_tick_irqmp_timestamp(void)
{
  volatile struct irqmp_timestamp_regs *irqmp_ts =
    &LEON3_IrqCtrl_Regs->timestamp[0];
  unsigned int first = irqmp_ts->assertion;
  unsigned int second = irqmp_ts->counter;

  irqmp_ts->control |= IRQMP_TIMESTAMP_S1_S2;

  _Profiling_Update_max_interrupt_delay(_Per_CPU_Get(), second - first);

  rtems_timecounter_tick();
}
#endif

static void leon3_tc_tick_irqmp_timestamp_init(void)
{
#ifdef RTEMS_PROFILING
  /*
   * Ignore the first clock interrupt, since it contains the sequential system
   * initialization time.  Do the timestamp initialization on the fly.
   */

#ifdef RTEMS_SMP
  static Atomic_Uint counter = ATOMIC_INITIALIZER_UINT(0);

  bool done =
    _Atomic_Fetch_add_uint(&counter, 1, ATOMIC_ORDER_RELAXED)
      == rtems_scheduler_get_processor_maximum() - 1;
#else
  bool done = true;
#endif

  volatile struct irqmp_timestamp_regs *irqmp_ts =
    &LEON3_IrqCtrl_Regs->timestamp[0];
  unsigned int ks = 1U << 5;

  irqmp_ts->control = ks | IRQMP_TIMESTAMP_S1_S2 | (unsigned int) clkirq;

  if (done) {
    leon3_tc_tick = leon3_tc_tick_irqmp_timestamp;
  }
#endif

  rtems_timecounter_tick();
}

static void leon3_tc_tick_default(void)
{
#ifndef RTEMS_SMP
  SPARC_Counter *counter;
  rtems_interrupt_level level;

  counter = &_SPARC_Counter_mutable;
  rtems_interrupt_local_disable(level);

  LEON3_IrqCtrl_Regs->iclear = counter->pending_mask;
  counter->accumulated += counter->interval;

  rtems_interrupt_local_enable(level);
#endif

  rtems_timecounter_tick();
}

static void leon3_tc_do_tick(void)
{
  (*leon3_tc_tick)();
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

#define Clock_driver_support_install_isr( _new ) \
  bsp_clock_handler_install(_new)

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

#define Clock_driver_support_set_interrupt_affinity(online_processors) \
  bsp_interrupt_set_affinity(clkirq, online_processors)

static void leon3_clock_initialize(void)
{
  volatile struct irqmp_timestamp_regs *irqmp_ts;
  volatile struct gptimer_regs *gpt;
  struct timecounter *tc;

  irqmp_ts = &LEON3_IrqCtrl_Regs->timestamp[0];
  gpt = LEON3_Timer_Regs;
  tc = &leon3_tc;

  gpt->timer[LEON3_CLOCK_INDEX].reload =
    rtems_configuration_get_microseconds_per_tick() - 1;
  gpt->timer[LEON3_CLOCK_INDEX].ctrl =
    GPTIMER_TIMER_CTRL_EN | GPTIMER_TIMER_CTRL_RS |
      GPTIMER_TIMER_CTRL_LD | GPTIMER_TIMER_CTRL_IE;

  leon3_up_counter_enable();

  if (leon3_up_counter_is_available()) {
    /* Use the LEON4 up-counter if available */
    tc->tc_get_timecount = _SPARC_Get_timecount_asr23;
    tc->tc_frequency = leon3_up_counter_frequency();

#ifdef RTEMS_PROFILING
    if (!leon3_irqmp_has_timestamp(irqmp_ts)) {
      bsp_fatal(LEON3_FATAL_CLOCK_NO_IRQMP_TIMESTAMP_SUPPORT);
    }
#endif

    leon3_tc_tick = leon3_tc_tick_irqmp_timestamp_init;
  } else if (leon3_irqmp_has_timestamp(irqmp_ts)) {
    /* Use the interrupt controller timestamp counter if available */
    tc->tc_get_timecount = _SPARC_Get_timecount_up;
    tc->tc_frequency = ambapp_freq_get(&ambapp_plb, LEON3_Timer_Adev);

    leon3_tc_tick = leon3_tc_tick_irqmp_timestamp_init;

    /*
     * At least one TSISEL field must be non-zero to enable the timestamp
     * counter.  Use an arbitrary interrupt source.
     */
    irqmp_ts->control = 0x1;
  } else {
#ifdef RTEMS_SMP
    /*
     * The GR712RC for example has no timestamp unit in the interrupt
     * controller.  At least on SMP configurations we must use a second timer
     * in free running mode for the timecounter.
     */
    gpt->timer[LEON3_COUNTER_GPTIMER_INDEX].ctrl =
      GPTIMER_TIMER_CTRL_EN | GPTIMER_TIMER_CTRL_IE;

    tc->tc_get_timecount = _SPARC_Get_timecount_down;
#else
    SPARC_Counter *counter;

    counter = &_SPARC_Counter_mutable;
    counter->read_isr_disabled = _SPARC_Counter_read_clock_isr_disabled;
    counter->read = _SPARC_Counter_read_clock;
    counter->counter_register = &gpt->timer[LEON3_CLOCK_INDEX].value;
    counter->pending_register = &LEON3_IrqCtrl_Regs->ipend;
    counter->pending_mask = UINT32_C(1) << clkirq;
    counter->accumulated = rtems_configuration_get_microseconds_per_tick();
    counter->interval = rtems_configuration_get_microseconds_per_tick();

    tc->tc_get_timecount = _SPARC_Get_timecount_clock;
#endif

    tc->tc_frequency = LEON3_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER,
    leon3_tc_tick = leon3_tc_tick_default;
  }

  tc->tc_counter_mask = 0xffffffff;
  tc->tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(tc);
}

#define Clock_driver_support_initialize_hardware() \
  leon3_clock_initialize()

#define Clock_driver_timecounter_tick() leon3_tc_do_tick()

#include "../../../shared/dev/clock/clockimpl.h"

#endif
