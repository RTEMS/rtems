/*
 * Use the last periodic interval timer (PIT2) as the system clock.
 */

#include <rtems.h>
#include <rtems/timecounter.h>
#include <bsp.h>

/*
 * Use INTC0 base
 */
#define CLOCK_VECTOR (64+56)

static rtems_timecounter_simple mcf52235_tc;

static uint32_t mcf52235_tc_get(rtems_timecounter_simple *tc)
{
  return MCF_PIT1_PCNTR;
}

static bool mcf52235_tc_is_pending(rtems_timecounter_simple *tc)
{
  return (MCF_PIT1_PCSR & MCF_PIT_PCSR_PIF) != 0;
}

static uint32_t mcf52235_tc_get_timecount(struct timecounter *tc)
{
  return rtems_timecounter_simple_downcounter_get(
    tc,
    mcf52235_tc_get,
    mcf52235_tc_is_pending
  );
}

static void mcf52235_tc_at_tick(rtems_timecounter_simple *tc)
{
  MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;
}

static void mcf52235_tc_tick(void)
{
  rtems_timecounter_simple_downcounter_tick(
    &mcf52235_tc,
    mcf52235_tc_get,
    mcf52235_tc_at_tick
  );
}

/*
 * Attach clock interrupt handler
 */
#define Clock_driver_support_install_isr( _new ) \
  set_vector(_new, CLOCK_VECTOR, 1)

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
  uint32_t clk = bsp_get_CPU_clock_speed() >> 1;
  uint32_t tps = 1000000 / rtems_configuration_get_microseconds_per_tick();

  while (preScaleCode < 15) {
    pmr = (clk >> preScaleCode) / tps;
    if (pmr < (1 << 15))
      break;
    preScaleCode++;
  }

  MCF_INTC0_ICR56 = MCF_INTC_ICR_IL(PIT3_IRQ_LEVEL) |
    MCF_INTC_ICR_IP(PIT3_IRQ_PRIORITY);
  rtems_interrupt_disable(level);
  MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_MASK56;
  MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN;
  rtems_interrupt_enable(level);

  MCF_PIT1_PCSR = MCF_PIT_PCSR_PRE(preScaleCode) |
    MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_RLD;
  MCF_PIT1_PMR = pmr;
  MCF_PIT1_PCSR = MCF_PIT_PCSR_PRE(preScaleCode) |
    MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_EN;

  rtems_timecounter_simple_install(
    &mcf52235_tc,
    clk >> preScaleCode,
    pmr,
    mcf52235_tc_get_timecount
  );
}

#define Clock_driver_timecounter_tick() mcf52235_tc_tick()

#include "../../../shared/dev/clock/clockimpl.h"
