/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>
#include <intrcritical.h>

static uint32_t Maximum;
static uint32_t Maximum_current;
static rtems_id Timer;
static rtems_timer_service_routine (*TSR)( rtems_id, void * );

void interrupt_critical_section_test_support_initialize(
  rtems_timer_service_routine (*tsr)( rtems_id, void * )
)
{
  Watchdog_Interval initial;
  uint32_t          counter;

  initial = _Watchdog_Ticks_since_boot;

  Timer = 0;
  TSR   = tsr;
  if ( tsr ) {
    rtems_status_code rc;

    puts( "Support - rtems_timer_create - creating timer 1" );
    rc = rtems_timer_create( rtems_build_name( 'T', 'M', '1', ' ' ), &Timer );
    directive_failed( rc, "rtems_timer_create" );
  }

  /*
   *  Wait for starting point
   */
  for (counter=0 ; initial == _Watchdog_Ticks_since_boot ; counter++)
    ;

  initial = _Watchdog_Ticks_since_boot;
  /*
   *  Wait for ending point
   */
  for (counter=0 ; initial == _Watchdog_Ticks_since_boot ; counter++)
    ;

  Maximum         = counter;
  Maximum_current = counter;

  #if 0
    printf( "%d 0x%08x counts\n", counter, counter );
  #endif
}

bool interrupt_critical_section_test_support_delay(void)
{
  uint32_t          counter;
  Watchdog_Interval initial;
  bool              retval;

  if (TSR) {
    rtems_status_code rc;

    rc = rtems_timer_fire_after( Timer, 1, TSR, NULL );
    directive_failed( rc, "timer_fire_after failed" );
  }

  /*
   *  Count down
   */
  if ( !Maximum_current ) {
    Maximum_current = Maximum;
    retval = true;
  } else
    retval = false;

  initial = _Watchdog_Ticks_since_boot;
  for (counter=Maximum_current ; counter ; counter--)
    if ( initial != _Watchdog_Ticks_since_boot )
      break;

  Maximum_current--;

  /*
   *  Return so the test can try to generate the condition
   */
  return retval;
}
