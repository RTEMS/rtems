/*
 * Use the last periodic interval timer (PIT2) as the system clock.
 */

#include <rtems.h>
#include <rtems/timecounter.h>
#include <bsp.h>

/*
 * Use INTC1 base
 */
#define CLOCK_VECTOR (128+46)

static rtems_timecounter_simple mcf5329_tc;

static uint32_t mcf5329_tc_get(rtems_timecounter_simple *tc)
{
  (void) tc;

  return MCF_PIT3_PCNTR;
}

static bool mcf5329_tc_is_pending(rtems_timecounter_simple *tc)
{
  (void) tc;

  return (MCF_PIT3_PCSR & MCF_PIT_PCSR_PIF) != 0;
}

static uint32_t mcf5329_tc_get_timecount(struct timecounter *tc)
{
  return rtems_timecounter_simple_downcounter_get(
    tc,
    mcf5329_tc_get,
    mcf5329_tc_is_pending
  );
}

static void mcf5329_tc_at_tick(rtems_timecounter_simple *tc)
{
  (void) tc;

  MCF_PIT3_PCSR |= MCF_PIT_PCSR_PIF;
}

static void mcf5329_tc_tick(void)
{
  rtems_timecounter_simple_downcounter_tick(
    &mcf5329_tc,
    mcf5329_tc_get,
    mcf5329_tc_at_tick
  );
}

/*
 * Attach clock interrupt handler
 */
#define Clock_driver_support_install_isr( _new ) \
    rtems_interrupt_handler_install( \
        CLOCK_VECTOR, \
        "Install clock interrupt", \
        RTEMS_INTERRUPT_UNIQUE, \
        (void *)_new, \
        NULL \
    );

/*
 * Set up the clock hardware
 *
 * We need to have 1 interrupt every rtems_configuration_get_microseconds_per_tick()
 */
static void Clock_driver_support_initialize_hardware(void)
{
  int level;
  uint32_t pmr;
  uint32_t preScaleCode = 0;
  uint32_t clk = bsp_get_BUS_clock_speed();
  uint32_t tps = 1000000 / rtems_configuration_get_microseconds_per_tick();

  while (preScaleCode < 15) {
    pmr = (clk >> preScaleCode) / tps;
    if (pmr < (1 << 15))
      break;
    preScaleCode++;
  }
  MCF_INTC1_ICR46 = MCF_INTC_ICR_IL(PIT3_IRQ_LEVEL);

  rtems_interrupt_disable(level);
  MCF_INTC1_IMRH &= ~MCF_INTC_IMRH_INT_MASK46;
  MCF_PIT3_PCSR &= ~MCF_PIT_PCSR_EN;
  rtems_interrupt_enable(level);

  MCF_PIT3_PCSR = MCF_PIT_PCSR_PRE(preScaleCode) |
    MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_RLD;
  MCF_PIT3_PMR = pmr;
  MCF_PIT3_PCSR = MCF_PIT_PCSR_PRE(preScaleCode) |
    MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_EN;

  rtems_timecounter_simple_install(
    &mcf5329_tc,
    clk >> preScaleCode,
    pmr,
    mcf5329_tc_get_timecount
  );
}

#define Clock_driver_timecounter_tick(arg) mcf5329_tc_tick()

#include "../../../shared/dev/clock/clockimpl.h"
