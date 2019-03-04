/**
 * @file
 * @ingroup RTEMSBSPsSPARCLEON2
 * @brief Clock Tick Device Driver
 *
 *  This routine initializes LEON timer 1 which used for the clock tick.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON BSP
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bspopts.h>
#include <rtems/sysinit.h>
#include <rtems/timecounter.h>
#include <rtems/score/sparcimpl.h>

extern int CLOCK_SPEED;

#define LEON2_TIMER_1_FREQUENCY 1000000

static struct timecounter leon2_tc;

static void leon2_clock_init( void )
{
  struct timecounter *tc;

  tc = &leon2_tc;
  tc->tc_get_timecount = _SPARC_Get_timecount_clock;
  tc->tc_counter_mask = 0xffffffff;
  tc->tc_frequency = LEON2_TIMER_1_FREQUENCY;
  tc->tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(tc);
}

static void leon2_clock_at_tick( void )
{
  SPARC_Counter *counter;
  rtems_interrupt_level level;

  counter = &_SPARC_Counter_mutable;
  rtems_interrupt_local_disable(level);

  LEON_Clear_interrupt( LEON_INTERRUPT_TIMER1 );
  counter->accumulated += counter->interval;

  rtems_interrupt_local_enable(level);
}

static void leon2_clock_initialize_early( void )
{
  SPARC_Counter *counter;

  LEON_REG.Timer_Reload_1 =
    rtems_configuration_get_microseconds_per_tick() - 1;
  LEON_REG.Timer_Control_1 = (
    LEON_REG_TIMER_COUNTER_ENABLE_COUNTING |
      LEON_REG_TIMER_COUNTER_RELOAD_AT_ZERO |
      LEON_REG_TIMER_COUNTER_LOAD_COUNTER
  );

  counter = &_SPARC_Counter_mutable;
  counter->read_isr_disabled = _SPARC_Counter_read_clock_isr_disabled;
  counter->read = _SPARC_Counter_read_clock;
  counter->counter_register = &LEON_REG.Timer_Counter_1;
  counter->pending_register = &LEON_REG.Interrupt_Pending;
  counter->pending_mask = UINT32_C(1) << LEON_INTERRUPT_TIMER1;
  counter->accumulated = rtems_configuration_get_microseconds_per_tick();
  counter->interval = rtems_configuration_get_microseconds_per_tick();
}

RTEMS_SYSINIT_ITEM(
  leon2_clock_initialize_early,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

uint32_t _CPU_Counter_frequency(void)
{
  return LEON2_TIMER_1_FREQUENCY;
}

/*
 *  The Real Time Clock Counter Timer uses this trap type.
 */

#define CLOCK_VECTOR LEON_TRAP_TYPE( LEON_INTERRUPT_TIMER1 )

#define Clock_driver_support_install_isr( _new ) \
  set_vector( _new, CLOCK_VECTOR, 1 )

#define Clock_driver_support_at_tick() leon2_clock_at_tick()

#define Clock_driver_support_initialize_hardware() leon2_clock_init()

#include "../../../shared/dev/clock/clockimpl.h"

SPARC_COUNTER_DEFINITION;
