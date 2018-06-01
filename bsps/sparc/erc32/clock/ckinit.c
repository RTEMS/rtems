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
#include <rtems/counter.h>
#include <rtems/timecounter.h>
#include <rtems/score/sparcimpl.h>

#define ERC32_REAL_TIME_CLOCK_FREQUENCY 1000000

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

extern int CLOCK_SPEED;

static rtems_timecounter_simple erc32_tc;

static uint32_t erc32_tc_get( rtems_timecounter_simple *tc )
{
  return ERC32_MEC.Real_Time_Clock_Counter;
}

static bool erc32_tc_is_pending( rtems_timecounter_simple *tc )
{
  return ERC32_Is_interrupt_pending( ERC32_INTERRUPT_REAL_TIME_CLOCK );
}

static uint32_t erc32_tc_get_timecount( struct timecounter *tc )
{
  return rtems_timecounter_simple_downcounter_get(
    tc,
    erc32_tc_get,
    erc32_tc_is_pending
  );
}

static void erc32_tc_at_tick( rtems_timecounter_simple *tc )
{
  /* Nothing to do */
}

static void erc32_tc_tick( void )
{
  rtems_timecounter_simple_downcounter_tick(
    &erc32_tc,
    erc32_tc_get,
    erc32_tc_at_tick
  );
}

static void erc32_counter_initialize( void )
{
  _SPARC_Counter_initialize(
    _SPARC_Counter_read_address,
    _SPARC_Counter_difference_clock_period,
    &ERC32_MEC.Real_Time_Clock_Counter
  );
}

uint32_t _CPU_Counter_frequency(void)
{
  return ERC32_REAL_TIME_CLOCK_FREQUENCY;
}

#define Clock_driver_support_initialize_hardware() \
  do { \
    /* approximately 1 us per countdown */ \
    ERC32_MEC.Real_Time_Clock_Scalar  = CLOCK_SPEED - 1; \
    ERC32_MEC.Real_Time_Clock_Counter = \
      rtems_configuration_get_microseconds_per_tick(); \
    \
    ERC32_MEC_Set_Real_Time_Clock_Timer_Control( \
        ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING | \
        ERC32_MEC_TIMER_COUNTER_LOAD_SCALER | \
        ERC32_MEC_TIMER_COUNTER_LOAD_COUNTER \
    ); \
    \
    ERC32_MEC_Set_Real_Time_Clock_Timer_Control( \
        ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING | \
        ERC32_MEC_TIMER_COUNTER_RELOAD_AT_ZERO \
    );  \
    rtems_timecounter_simple_install( \
        &erc32_tc, \
        ERC32_REAL_TIME_CLOCK_FREQUENCY, \
        rtems_configuration_get_microseconds_per_tick(), \
        erc32_tc_get_timecount \
    ); \
    erc32_counter_initialize(); \
  } while (0)

#define Clock_driver_timecounter_tick() erc32_tc_tick()

#include "../../../shared/dev/clock/clockimpl.h"

SPARC_COUNTER_DEFINITION;
