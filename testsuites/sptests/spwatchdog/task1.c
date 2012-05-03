/*  Task_1
 *
 *  This routine serves as a test task.  XXX
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_id          tmid;
  rtems_status_code status;

/* Get id */

  puts( "TA1 - rtems_timer_ident - identing timer 1" );
  status = rtems_timer_ident( Timer_name[ 1 ], &tmid );
  directive_failed( status, "rtems_timer_ident" );
  printf( "TA1 - timer 1 has id (0x%" PRIxrtems_id ")\n", tmid );

/* after which is allowed to fire */

  Print_time();

  puts( "TA1 - rtems_timer_fire_after - timer 1 in 3 seconds" );
  status = rtems_timer_fire_after(
    tmid,
    3 * rtems_clock_get_ticks_per_second(),
    Delayed_resume,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  puts( "TA1 - rtems_task_suspend( RTEMS_SELF )" );
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend" );

  Print_time();

/* after which is reset and allowed to fire */

  puts( "TA1 - rtems_timer_fire_after - timer 1 in 3 seconds" );
  status = rtems_timer_fire_after(
    tmid,
    3 * rtems_clock_get_ticks_per_second(),
    Delayed_resume,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  puts( "TA1 - rtems_task_wake_after - 1 second" );
  status = rtems_task_wake_after( 1 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  Print_time();

  puts( "TA1 - rtems_timer_reset - timer 1" );
  status = rtems_timer_reset( tmid );
  directive_failed( status, "rtems_timer_reset" );

  puts( "TA1 - _Watchdog_Report_chain - with name"  );
  _Watchdog_Report_chain( "_Watchdog_Ticks_chain", & _Watchdog_Ticks_chain );

  puts( "TA1 - _Watchdog_Report_chain - no name"  );
  _Watchdog_Report_chain( NULL, & _Watchdog_Ticks_chain);

  puts( "TA1 - _Watchdog_Report - with name"  );
  _Watchdog_Report("first", (Watchdog_Control *)_Chain_First(&_Watchdog_Ticks_chain));

  puts( "TA1 - _Watchdog_Report - no name"  );
  _Watchdog_Report( NULL, (Watchdog_Control *)_Chain_First(&_Watchdog_Ticks_chain) );

  puts( "TA1 - timer_deleting - timer 1" );
  status = rtems_timer_delete( tmid );
  directive_failed( status, "rtems_timer_delete" );

  puts( "*** END OF RTEMS WATCHDOG PROGRAM ***" );
  rtems_test_exit( 0 );
}
