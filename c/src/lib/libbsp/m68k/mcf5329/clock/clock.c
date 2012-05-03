/*
 * Use the last periodic interval timer (PIT2) as the system clock.
 */

#include <rtems.h>
#include <bsp.h>

/*
 * Use INTC1 base
 */
#define CLOCK_VECTOR (128+46)

static uint32_t s_pcntrAtTick = 0;
static uint32_t s_nanoScale = 0;

/*
 * Provide nanosecond extension
 */
static uint32_t bsp_clock_nanoseconds_since_last_tick(void)
{
  uint32_t i;

  if (MCF_PIT3_PCSR & MCF_PIT_PCSR_PIF) {
    i = s_pcntrAtTick + (MCF_PIT3_PMR - MCF_PIT3_PCNTR);
  } else {
    i = s_pcntrAtTick - MCF_PIT3_PCNTR;
  }
  return i * s_nanoScale;
}

#define Clock_driver_nanoseconds_since_last_tick bsp_clock_nanoseconds_since_last_tick

/*
 * Periodic interval timer interrupt handler
 */
#define Clock_driver_support_at_tick()             \
    do {                                           \
        s_pcntrAtTick = MCF_PIT3_PCNTR;            \
        MCF_PIT3_PCSR |= MCF_PIT_PCSR_PIF;         \
    } while (0)                                    \


/*
 * Attach clock interrupt handler
 */
#define Clock_driver_support_install_isr( _new, _old )             \
    do {                                                           \
        _old = (rtems_isr_entry)set_vector(_new, CLOCK_VECTOR, 1); \
    } while(0)

/*
 * Turn off the clock
 */
static void Clock_driver_support_shutdown_hardware(void)
{
  MCF_PIT3_PCSR &= ~MCF_PIT_PCSR_EN;
}

/*
 * Set up the clock hardware
 *
 * We need to have 1 interrupt every BSP_Configuration.microseconds_per_tick
 */
static void Clock_driver_support_initialize_hardware(void)
{
  int level;
  uint32_t pmr;
  uint32_t preScaleCode = 0;
  uint32_t clk = bsp_get_BUS_clock_speed();
  uint32_t tps = 1000000 / Configuration.microseconds_per_tick;

  while (preScaleCode < 15) {
    pmr = (clk >> preScaleCode) / tps;
    if (pmr < (1 << 15))
      break;
    preScaleCode++;
  }
  s_nanoScale = 1000000000 / (clk >> preScaleCode);

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
  s_pcntrAtTick = MCF_PIT3_PCNTR;
}

#include "../../../shared/clockdrv_shell.h"
