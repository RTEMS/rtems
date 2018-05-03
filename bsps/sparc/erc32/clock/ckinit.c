/*
 *  This routine initializes the Real Time Clock Counter Timer which is
 *  part of the MEC on the ERC32 CPU.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */

#include <bsp.h>
#include <bspopts.h>
#include <rtems/sysinit.h>
#include <rtems/timecounter.h>
#include <rtems/score/sparcimpl.h>

extern int CLOCK_SPEED;

#define ERC32_REAL_TIME_CLOCK_FREQUENCY 1000000

static struct timecounter erc32_tc;

static void erc32_clock_init( void )
{
  struct timecounter *tc;

  tc = &erc32_tc;
  tc->tc_get_timecount = _SPARC_Get_timecount_clock;
  tc->tc_counter_mask = 0xffffffff;
  tc->tc_frequency = ERC32_REAL_TIME_CLOCK_FREQUENCY;
  tc->tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(tc);
}

uint32_t _CPU_Counter_frequency(void)
{
  return ERC32_REAL_TIME_CLOCK_FREQUENCY;
}

static void erc32_clock_at_tick( void )
{
  SPARC_Counter *counter;
  rtems_interrupt_level level;

  counter = &_SPARC_Counter_mutable;
  rtems_interrupt_local_disable(level);

  ERC32_Clear_interrupt( ERC32_INTERRUPT_REAL_TIME_CLOCK );
  counter->accumulated += counter->interval;

  rtems_interrupt_local_enable(level);
}

static void erc32_clock_initialize_early( void )
{
  SPARC_Counter *counter;

  /* approximately 1 us per countdown */
  ERC32_MEC.Real_Time_Clock_Scalar = CLOCK_SPEED - 1;
  ERC32_MEC.Real_Time_Clock_Counter =
    rtems_configuration_get_microseconds_per_tick();
  ERC32_MEC_Set_Real_Time_Clock_Timer_Control(
      ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING |
      ERC32_MEC_TIMER_COUNTER_LOAD_SCALER |
      ERC32_MEC_TIMER_COUNTER_LOAD_COUNTER
  );
  ERC32_MEC_Set_Real_Time_Clock_Timer_Control(
    ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING |
    ERC32_MEC_TIMER_COUNTER_RELOAD_AT_ZERO
  );

  counter = &_SPARC_Counter_mutable;
  counter->read_isr_disabled = _SPARC_Counter_read_clock_isr_disabled;
  counter->read = _SPARC_Counter_read_clock;
  counter->counter_register = &ERC32_MEC.Real_Time_Clock_Counter,
  counter->pending_register = &ERC32_MEC.Interrupt_Pending;
  counter->pending_mask = UINT32_C(1) << ERC32_INTERRUPT_REAL_TIME_CLOCK;
  counter->accumulated = rtems_configuration_get_microseconds_per_tick();
  counter->interval = rtems_configuration_get_microseconds_per_tick();
}

RTEMS_SYSINIT_ITEM(
  erc32_clock_initialize_early,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

/*
 *  The Real Time Clock Counter Timer uses this trap type.
 */
#define CLOCK_VECTOR ERC32_TRAP_TYPE( ERC32_INTERRUPT_REAL_TIME_CLOCK )

#define Clock_driver_support_install_isr( _new ) \
  set_vector( _new, CLOCK_VECTOR, 1 )

#define Clock_driver_support_set_interrupt_affinity( _online_processors ) \
  do { \
    (void) _online_processors; \
  } while (0)

#define Clock_driver_support_at_tick() erc32_clock_at_tick()

#define Clock_driver_support_initialize_hardware() erc32_clock_init()

#include "../../../shared/dev/clock/clockimpl.h"

SPARC_COUNTER_DEFINITION;
