/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <intrcritical.h>

static uint32_t Maximum;
static uint32_t Maximum_current;
static rtems_id Timer;
static rtems_timer_service_routine (*TSR)( rtems_id, void * );

static uint32_t interrupt_critical_remaining_units_of_tick( void )
{
  uint32_t       units = 0;
  rtems_interval initial = rtems_clock_get_ticks_since_boot();

  while ( initial == rtems_clock_get_ticks_since_boot() ) {
    ++units;
  }

  return units;
}

static bool interrupt_critical_busy_wait( void )
{
  uint32_t max = Maximum_current;
  uint32_t unit = 0;
  rtems_interval initial = rtems_clock_get_ticks_since_boot();

  while ( unit < max && initial == rtems_clock_get_ticks_since_boot() ) {
    ++unit;
  }

  if ( max > 0 ) {
    Maximum_current = max - 1;

    return false;
  } else {
    Maximum_current = Maximum;

    return true;
  }
}

void interrupt_critical_section_test_support_initialize(
  rtems_timer_service_routine (*tsr)( rtems_id, void * )
)
{
  Timer = 0;
  TSR   = tsr;
  if ( tsr ) {
    rtems_status_code rc;

    puts( "Support - rtems_timer_create - creating timer 1" );
    rc = rtems_timer_create( rtems_build_name( 'T', 'M', '1', ' ' ), &Timer );
    directive_failed( rc, "rtems_timer_create" );
  }

  /* Wait for tick change */
  interrupt_critical_remaining_units_of_tick();

  /* Get units for a hole tick */
  Maximum = interrupt_critical_remaining_units_of_tick();
  Maximum_current = Maximum;

  #if 0
    printf( "%d 0x%08x units\n", Maximum, Maximum );
  #endif
}

bool interrupt_critical_section_test_support_delay(void)
{
  if (TSR) {
    rtems_status_code rc;

    rc = rtems_timer_fire_after( Timer, 1, TSR, NULL );
    directive_failed( rc, "timer_fire_after failed" );
  }

  return interrupt_critical_busy_wait();
}
