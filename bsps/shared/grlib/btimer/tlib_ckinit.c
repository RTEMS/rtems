/*
 *  Clock Tick Device Driver using Timer Library implemented
 *  by the GRLIB GPTIMER / LEON2 Timer drivers.
 *
 *  COPYRIGHT (c) 2010 - 2017.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

/*
 * This is an implementation of the RTEMS "clockdrv_shell" interface for
 * LEON2/3/4 systems using the Driver Manager. It is clock hardware agnostic
 * and compatible with SMP and UP. Availability of free running counters is
 * probed and selected as needed.
 */
#include <rtems.h>
#include <rtems/timecounter.h>
#include <rtems/clockdrv.h>
#include <stdlib.h>
#include <bsp.h>
#include <grlib/tlib.h>

#ifdef RTEMS_DRVMGR_STARTUP

#if defined(LEON3)
#include <leon.h>
#endif

struct ops {
  /*
   * Set up the free running counter using the Timecounter or Simple
   * Timecounter interface.
   */
  rtems_device_driver (*initialize_counter)(void);

  /*
   * Hardware-specific support at tick interrupt which runs early in Clock_isr.
   * It can for example be used to check if interrupt was actually caused by
   * the timer hardware. If return value is not RTEMS_SUCCESSFUL then Clock_isr
   * returns immediately. at_tick can be initialized with NULL.
   */
  rtems_device_driver (*at_tick)(void);

  /*
   * Typically calls rtems_timecounter_tick(). A specialized clock driver may
   * use for example rtems_timecounter_tick_simple() instead.
   */
  void (*timecounter_tick)(void);

  /*
   * Called when the clock driver exits. It can be used to stop functionality
   * started by initialize_counter. The tick timer is stopped by default.
   * shutdown_hardware can be initialized with NULL
   */
  void (*shutdown_hardware)(void);
};

/*
 * Different implementation depending on available free running counter for the
 * timecounter.
 *
 * NOTE: The clock interface is not compatible with shared interrupts on the
 * clock (tick) timer in SMP configuration.
 */

#ifndef RTEMS_SMP
/* "simple timecounter" interface. Only for non-SMP. */
static const struct ops ops_simple;
#else
/* Hardware support up-counter using LEON3 %asr23. */
static const struct ops ops_timetag;
/* Timestamp counter available in some IRQ(A)MP instantiations. */
static const struct ops ops_irqamp;
/* Separate GPTIMER subtimer as timecounter */
static const struct ops ops_subtimer;
#endif

struct clock_priv {
  const struct ops *ops;
  /*
   * Timer number in Timer Library for tick timer used by this interface.
   * Defaults to the first Timer in the System.
   */
  int tlib_tick_index;
  /* Timer number for timecounter timer if separate GPTIMER subtimer is used */
  int tlib_counter_index;
  void *tlib_tick;
  void *tlib_counter;
  rtems_timecounter_simple tc_simple;
  struct timecounter tc;
};
static struct clock_priv priv;

/** Common interface **/

/* Set system clock timer instance */
void Clock_timer_register(int timer_number)
{
  priv.tlib_tick_index = timer_number;
  priv.tlib_counter_index = timer_number + 1;
}

static rtems_device_driver tlib_clock_find_timer(void)
{
  /* Take Timer that should be used as system timer. */
  priv.tlib_tick = tlib_open(priv.tlib_tick_index);
  if (priv.tlib_tick == NULL) {
    /* System Clock Timer not found */
    return RTEMS_NOT_DEFINED;
  }

  /* Select which operation set to use */
#ifndef RTEMS_SMP
  priv.ops = &ops_simple;
#else
  /* When on LEON3 try to use dedicated hardware free running counter. */
  leon3_up_counter_enable();
  if (leon3_up_counter_is_available()) {
    priv.ops = &ops_timetag;
    return RTEMS_SUCCESSFUL;
  } else {
    volatile struct irqmp_timestamp_regs *irqmp_ts;

    irqmp_ts = &LEON3_IrqCtrl_Regs->timestamp[0];
    if (leon3_irqmp_has_timestamp(irqmp_ts)) {
      priv.ops = &ops_irqamp;
      return RTEMS_SUCCESSFUL;
    }
  }

  /* Take another subtimer as the final option. */
  priv.ops = &ops_subtimer;
#endif

  return RTEMS_SUCCESSFUL;
}

static rtems_device_driver tlib_clock_initialize_hardware(void)
{
  /* Set tick rate in number of "Base-Frequency ticks" */
  tlib_set_freq(priv.tlib_tick, rtems_configuration_get_microseconds_per_tick());
  priv.ops->initialize_counter();
  tlib_start(priv.tlib_tick, 0);

  return RTEMS_SUCCESSFUL;
}

static rtems_device_driver tlib_clock_at_tick(void)
{
  if (priv.ops->at_tick) {
    return priv.ops->at_tick();
  }

  return RTEMS_SUCCESSFUL;
}

static void tlib_clock_timecounter_tick(void)
{
  priv.ops->timecounter_tick();
}

static void tlib_clock_install_isr(rtems_isr *isr)
{
  int flags = 0;

#ifdef RTEMS_SMP
  /* We shall broadcast the clock interrupt to all processors. */
  flags = TLIB_FLAGS_BROADCAST;
#endif
  tlib_irq_register(priv.tlib_tick, isr, NULL, flags);
}

#ifndef RTEMS_SMP
/** Simple counter **/
static uint32_t simple_tlib_tc_get(rtems_timecounter_simple *tc)
{
  unsigned int clicks = 0;

  if (priv.tlib_tick != NULL) {
    tlib_get_counter(priv.tlib_tick, &clicks);
  }

  return clicks;
}

static bool simple_tlib_tc_is_pending(rtems_timecounter_simple *tc)
{
  bool pending = false;

  if (priv.tlib_tick != NULL) {
    pending = tlib_interrupt_pending(priv.tlib_tick, 0) != 0;
  }

  return pending;
}

static uint32_t simple_tlib_tc_get_timecount(struct timecounter *tc)
{
  return rtems_timecounter_simple_downcounter_get(
    tc,
    simple_tlib_tc_get,
    simple_tlib_tc_is_pending
  );
}

static rtems_device_driver simple_initialize_counter(void)
{
  uint64_t frequency;
  unsigned int tick_hz;

  frequency = 1000000;
  tick_hz = rtems_configuration_get_microseconds_per_tick();

  rtems_timecounter_simple_install(
    &priv.tc_simple,
    frequency,
    tick_hz,
    simple_tlib_tc_get_timecount
  );

  return RTEMS_NOT_DEFINED;
}

static void simple_tlib_tc_at_tick(rtems_timecounter_simple *tc)
{
  /* Nothing to do */
}

/*
 * Support for shared interrupts. Ack IRQ at source, only handle interrupts
 * generated from the tick-timer. This is called early in Clock_isr.
 */
static rtems_device_driver simple_at_tick(void)
{
  if (tlib_interrupt_pending(priv.tlib_tick, 1) == 0) {
    return RTEMS_NOT_DEFINED;
  }
  return RTEMS_SUCCESSFUL;
}

static void simple_timecounter_tick(void)
{
  rtems_timecounter_simple_downcounter_tick(
    &priv.tc_simple,
    simple_tlib_tc_get,
    simple_tlib_tc_at_tick
  );
}

static const struct ops ops_simple = {
  .initialize_counter = simple_initialize_counter,
  .at_tick            = simple_at_tick,
  .timecounter_tick   = simple_timecounter_tick,
  .shutdown_hardware  = NULL,
};

#else

/** Subtimer as counter **/
static uint32_t subtimer_get_timecount(struct timecounter *tc)
{
  unsigned int counter;

  tlib_get_counter(priv.tlib_counter, &counter);

  return 0xffffffff - counter;
}

static rtems_device_driver subtimer_initialize_counter(void)
{
  unsigned int mask;
  unsigned int basefreq;

  if (priv.tlib_counter_index == priv.tlib_tick_index) {
    priv.tlib_counter_index = priv.tlib_tick_index + 1;
  }
  /* Take Timer that should be used as timecounter upcounter timer. */
  priv.tlib_counter = tlib_open(priv.tlib_counter_index);
  if (priv.tlib_counter == NULL) {
    /* Timecounter timer not found */
    return RTEMS_NOT_DEFINED;
  }

  /* Configure free running counter: GPTIMER */
  tlib_get_freq(priv.tlib_counter, &basefreq, NULL);
  tlib_get_widthmask(priv.tlib_counter, &mask);

  priv.tc.tc_get_timecount = subtimer_get_timecount;
  priv.tc.tc_counter_mask = mask;
  priv.tc.tc_frequency = basefreq;
  priv.tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&priv.tc);
  /* Start free running counter */
  tlib_start(priv.tlib_counter, 0);

  return RTEMS_SUCCESSFUL;
}

static void subtimer_timecounter_tick(void)
{
  rtems_timecounter_tick();
}

static void subtimer_shutdown_hardware(void)
{
  if (priv.tlib_counter) {
    tlib_stop(priv.tlib_counter);
    priv.tlib_counter = NULL;
  }
}

static const struct ops ops_subtimer = {
  .initialize_counter = subtimer_initialize_counter,
  .timecounter_tick   = subtimer_timecounter_tick,
  .shutdown_hardware  = subtimer_shutdown_hardware,
};

/** DSU timetag as counter **/
static uint32_t timetag_get_timecount(struct timecounter *tc)
{
  return leon3_up_counter_low();
}

static rtems_device_driver timetag_initialize_counter(void)
{
  /* Configure free running counter: timetag */
  priv.tc.tc_get_timecount = timetag_get_timecount;
  priv.tc.tc_counter_mask = 0xffffffff;
  priv.tc.tc_frequency = leon3_up_counter_frequency();
  priv.tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&priv.tc);

  return RTEMS_SUCCESSFUL;
}

static void timetag_timecounter_tick(void)
{
  rtems_timecounter_tick();
}

static const struct ops ops_timetag = {
  .initialize_counter = timetag_initialize_counter,
  .at_tick            = NULL,
  .timecounter_tick   = timetag_timecounter_tick,
  .shutdown_hardware  = NULL,
};

/** IRQ(A)MP timestamp as counter **/
static uint32_t irqamp_get_timecount(struct timecounter *tc)
{
  return LEON3_IrqCtrl_Regs->timestamp[0].counter;
}

static rtems_device_driver irqamp_initialize_counter(void)
{
  volatile struct irqmp_timestamp_regs *irqmp_ts;
  static const uint32_t A_TSISEL_FIELD = 0xf;

  /* Configure free running counter: timetag */
  priv.tc.tc_get_timecount = irqamp_get_timecount;
  priv.tc.tc_counter_mask = 0xffffffff;
  priv.tc.tc_frequency = leon3_up_counter_frequency();
  priv.tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&priv.tc);

  /*
   * The counter increments whenever a TSISEL field in a Timestamp Control
   * Register is non-zero.
   */
  irqmp_ts = &LEON3_IrqCtrl_Regs->timestamp[0];
  irqmp_ts->control = A_TSISEL_FIELD;

  return RTEMS_SUCCESSFUL;
}

static void irqamp_timecounter_tick(void)
{
  rtems_timecounter_tick();
}

static const struct ops ops_irqamp = {
  .initialize_counter = irqamp_initialize_counter,
  .at_tick            = NULL,
  .timecounter_tick   = irqamp_timecounter_tick,
  .shutdown_hardware  = NULL,
};
#endif

/** Interface to the Clock Driver Shell (dev/clock/clockimpl.h) **/
#define Clock_driver_support_find_timer() \
  do { \
    rtems_device_driver ret; \
    ret = tlib_clock_find_timer(); \
    if (RTEMS_SUCCESSFUL != ret) { \
      return ret; \
    } \
  } while (0)

#define Clock_driver_support_install_isr( isr ) \
  tlib_clock_install_isr( isr )

#define Clock_driver_support_set_interrupt_affinity(online_processors) \
  /* Done by tlib_clock_install_isr() */

#define Clock_driver_support_initialize_hardware() \
  do { \
    rtems_device_driver ret; \
    ret = tlib_clock_initialize_hardware(); \
    if (RTEMS_SUCCESSFUL != ret) { \
      return ret; \
    } \
  } while (0)

#define Clock_driver_timecounter_tick() \
  tlib_clock_timecounter_tick()

#define Clock_driver_support_at_tick() \
  do { \
    rtems_device_driver ret; \
    ret = tlib_clock_at_tick(); \
    if (RTEMS_SUCCESSFUL != ret) { \
      return; \
    } \
  } while (0)

#include "../../../shared/dev/clock/clockimpl.h"

#endif /* RTEMS_DRVMGR_STARTUP */

