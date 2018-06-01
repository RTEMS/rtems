/**
 * @file
 * @ingroup sparc_leon2
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
#include <rtems/timecounter.h>
#include <rtems/score/sparcimpl.h>

static rtems_timecounter_simple leon2_tc;

static uint32_t leon2_tc_get( rtems_timecounter_simple *tc )
{
  return LEON_REG.Timer_Counter_1;
}

static bool leon2_tc_is_pending( rtems_timecounter_simple *tc )
{
  return LEON_Is_interrupt_pending( LEON_INTERRUPT_TIMER1 );
}

static uint32_t leon2_tc_get_timecount( struct timecounter *tc )
{
  return rtems_timecounter_simple_downcounter_get(
    tc,
    leon2_tc_get,
    leon2_tc_is_pending
  );
}

static void leon2_tc_at_tick( rtems_timecounter_simple *tc )
{
  /* Nothing to do */
}

static void leon2_tc_tick( void )
{
  rtems_timecounter_simple_downcounter_tick(
    &leon2_tc,
    leon2_tc_get,
    leon2_tc_at_tick
  );
}

/*
 *  The Real Time Clock Counter Timer uses this trap type.
 */

#define CLOCK_VECTOR LEON_TRAP_TYPE( LEON_INTERRUPT_TIMER1 )

#define Clock_driver_support_install_isr( _new ) \
  set_vector( _new, CLOCK_VECTOR, 1 )

extern int CLOCK_SPEED;

#define Clock_driver_support_initialize_hardware() \
  do { \
    LEON_REG.Timer_Reload_1 = \
        rtems_configuration_get_microseconds_per_tick() - 1; \
    \
    LEON_REG.Timer_Control_1 = ( \
      LEON_REG_TIMER_COUNTER_ENABLE_COUNTING |  \
        LEON_REG_TIMER_COUNTER_RELOAD_AT_ZERO | \
        LEON_REG_TIMER_COUNTER_LOAD_COUNTER  \
    ); \
    rtems_timecounter_simple_install( \
      &leon2_tc, \
      1000000, \
      rtems_configuration_get_microseconds_per_tick(), \
      leon2_tc_get_timecount \
    ); \
  } while (0)

#define Clock_driver_timecounter_tick() leon2_tc_tick()

#include "../../../shared/dev/clock/clockimpl.h"

SPARC_COUNTER_DEFINITION;
