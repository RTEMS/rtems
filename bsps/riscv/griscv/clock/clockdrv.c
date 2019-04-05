/*
 *  Clock Tick Device Driver
 *
 *  This routine initializes GRLIB gptimer 1 which used for the clock tick.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for GRLIB BSP.
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
#include <amba.h>
#include <bsp/irq.h>
#include <bspopts.h>
#include <bsp/fatal.h>
#include <rtems/rtems/intr.h>
#include <grlib/ambapp.h>
#include <rtems/score/profiling.h>
#include <rtems/timecounter.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/riscv-utility.h>

volatile uint32_t _RISCV_Counter_register;

/* The GRLIB BSP Timer driver can rely on the Driver Manager if the
 * DrvMgr is initialized during startup. Otherwise the classic driver
 * must be used.
 *
 * The DrvMgr Clock driver is located in the shared/timer directory
 */
#ifndef RTEMS_DRVMGR_STARTUP

/* GRLIB Timer system interrupt number */
static int clkirq;

static void (*grlib_tc_tick)(void);

static struct timecounter grlib_tc;

#ifdef RTEMS_PROFILING
#define IRQMP_TIMESTAMP_S1_S2 ((1U << 25) | (1U << 26))

static void grlib_tc_tick_irqmp_timestamp(void)
{
  volatile struct irqmp_timestamp_regs *irqmp_ts =
    &GRLIB_IrqCtrl_Regs->timestamp[0];
  unsigned int first = irqmp_ts->assertion;
  unsigned int second = irqmp_ts->counter;

  irqmp_ts->control |= IRQMP_TIMESTAMP_S1_S2;

  _Profiling_Update_max_interrupt_delay(_Per_CPU_Get(), second - first);

  rtems_timecounter_tick();
}
#endif

static void grlib_tc_tick_irqmp_timestamp_init(void)
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
    &GRLIB_IrqCtrl_Regs->timestamp[0];
  unsigned int ks = 1U << 5;

  irqmp_ts->control = ks | IRQMP_TIMESTAMP_S1_S2 | (unsigned int) clkirq;

  if (done) {
    grlib_tc_tick = grlib_tc_tick_irqmp_timestamp;
  }
#endif

  rtems_timecounter_tick();
}

static void grlib_tc_do_tick(void)
{
  (*grlib_tc_tick)();
}

#define Adjust_clkirq_for_node() do { clkirq += GRLIB_CLOCK_INDEX; } while(0)

#define Clock_driver_support_find_timer() \
  do { \
    /* Assume timer found during BSP initialization */ \
    if (GRLIB_Timer_Regs) { \
      clkirq = (GRLIB_Timer_Regs->cfg & 0xf8) >> 3; \
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

uint32_t _CPU_Counter_frequency( void )
{
  return grlib_up_counter_frequency();
}


static uint32_t _RISCV_Get_timecount_csr(struct timecounter *tc)
{
  return read_csr(time);
}

static void grlib_clock_initialize(void)
{
  volatile struct gptimer_regs *gpt;
  struct timecounter *tc;

  gpt = GRLIB_Timer_Regs;
  tc = &grlib_tc;

  gpt->timer[GRLIB_CLOCK_INDEX].reload =
    rtems_configuration_get_microseconds_per_tick() - 1;
  gpt->timer[GRLIB_CLOCK_INDEX].ctrl =
    GPTIMER_TIMER_CTRL_EN | GPTIMER_TIMER_CTRL_RS |
      GPTIMER_TIMER_CTRL_LD | GPTIMER_TIMER_CTRL_IE;

    /* Use the RISCV time register as up-counter */
  tc->tc_get_timecount = _RISCV_Get_timecount_csr;
  tc->tc_frequency = grlib_up_counter_frequency();

#ifdef RTEMS_PROFILING
  volatile struct irqmp_timestamp_regs *irqmp_ts =
    &GRLIB_IrqCtrl_Regs->timestamp[0];

    if (!grlib_irqmp_has_timestamp(irqmp_ts)) {
      bsp_fatal(GRLIB_FATAL_CLOCK_NO_IRQMP_TIMESTAMP_SUPPORT);
    }
#endif

  grlib_tc_tick = grlib_tc_tick_irqmp_timestamp_init;

  tc->tc_counter_mask = 0xffffffff;
  tc->tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(tc);
}

CPU_Counter_ticks _CPU_Counter_read( void )
{
  unsigned long timec;

  __asm__ volatile ( "csrr %0, time" : "=&r" ( timec ) );

  return timec;
}

#define Clock_driver_support_initialize_hardware() \
  grlib_clock_initialize()

#define Clock_driver_timecounter_tick() grlib_tc_do_tick()

#include "../../../shared/dev/clock/clockimpl.h"

#endif
