/**
 * @file
 *
 * @ingroup bsp_clock
 *
 * @brief or1k clock support.
 */

/*
 * generic_or1k Clock driver
 *
 * COPYRIGHT (c) 2014-2015 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/generic_or1k.h>
#include <rtems/score/cpu.h>
#include <rtems/score/or1k-utility.h>
#include <rtems/timecounter.h>

/* The number of clock cycles before generating a tick timer interrupt. */
#define TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT     0x09ED9
#define OR1K_CLOCK_CYCLE_TIME_NANOSECONDS     10

static struct timecounter or1ksim_tc;

/* CPU counter */
static CPU_Counter_ticks cpu_counter_ticks;

static void generic_or1k_clock_at_tick(void)
{
  uint32_t TTMR;

 /* For TTMR register,
  * The least significant 28 bits are the number of clock cycles
  * before generating a tick timer interrupt. While the most
  * significant 4 bits are used for mode configuration, tick timer
  * interrupt enable and pending interrupts status.
  */
  TTMR = (CPU_OR1K_SPR_TTMR_MODE_RESTART | CPU_OR1K_SPR_TTMR_IE |
           (TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT & CPU_OR1K_SPR_TTMR_TP_MASK)
         ) & ~(CPU_OR1K_SPR_TTMR_IP);

  _OR1K_mtspr(CPU_OR1K_SPR_TTMR, TTMR);
  _OR1K_mtspr(CPU_OR1K_SPR_TTCR, 0);

  cpu_counter_ticks += TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT;
}

static void generic_or1k_clock_handler_install(CPU_ISR_handler new_isr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  _CPU_ISR_install_vector(OR1K_EXCEPTION_TICK_TIMER,
                          new_isr,
                          NULL);

  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static uint32_t or1ksim_get_timecount(struct timecounter *tc)
{
  uint32_t ticks_since_last_timer_interrupt;

  ticks_since_last_timer_interrupt = _OR1K_mfspr(CPU_OR1K_SPR_TTCR);

  return cpu_counter_ticks + ticks_since_last_timer_interrupt;
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return or1ksim_get_timecount(NULL);
}

static void generic_or1k_clock_initialize(void)
{
  uint64_t frequency = (1000000000 / OR1K_CLOCK_CYCLE_TIME_NANOSECONDS);
  uint32_t TTMR;

 /* For TTMR register,
  * The least significant 28 bits are the number of clock cycles
  * before generating a tick timer interrupt. While the most
  * significant 4 bits are used for mode configuration, tick timer
  * interrupt enable and pending interrupts status.
  */

  /* FIXME: Long interval should pass since initializing the tick timer
   * registers fires exceptions dispite interrupts has not been enabled yet.
   */
  TTMR = (CPU_OR1K_SPR_TTMR_MODE_RESTART | CPU_OR1K_SPR_TTMR_IE |
           (0xFFED9 & CPU_OR1K_SPR_TTMR_TP_MASK)
         ) & ~(CPU_OR1K_SPR_TTMR_IP);

  _OR1K_mtspr(CPU_OR1K_SPR_TTMR, TTMR);
  _OR1K_mtspr(CPU_OR1K_SPR_TTCR, 0);

  /* Initialize timecounter */
  or1ksim_tc.tc_get_timecount = or1ksim_get_timecount;
  or1ksim_tc.tc_counter_mask = 0xffffffff;
  or1ksim_tc.tc_frequency = frequency;
  or1ksim_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&or1ksim_tc);
}

#define Clock_driver_support_at_tick() generic_or1k_clock_at_tick()

#define Clock_driver_support_initialize_hardware() generic_or1k_clock_initialize()

#define Clock_driver_support_install_isr(isr) \
  generic_or1k_clock_handler_install(isr)

#include "../../../shared/dev/clock/clockimpl.h"
