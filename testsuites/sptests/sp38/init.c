/*
 *  Classic API Signal to Task from ISR
 *
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

#define CONFIGURE_INIT
#include "system.h"

volatile bool signal_sent;
volatile bool signal_processed;

rtems_id main_task;
void signal_handler(rtems_signal_set signals);
rtems_timer_service_routine test_signal_from_isr(
  rtems_id  timer,
  void     *arg
);

void signal_handler(
  rtems_signal_set signals
)
{
  signal_processed = TRUE;
}

rtems_timer_service_routine test_signal_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  rtems_status_code     status;

  status = rtems_signal_send( main_task, 0x0a0b0c0d );
  directive_failed_with_level( status, "rtems_signal_send", 1 );

  signal_sent = TRUE;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code     status;
  rtems_id              timer;
  rtems_interval        start;
  rtems_interval        now;

  puts( "\n\n*** TEST 38 ***" );

  main_task = rtems_task_self();

  /*
   *  Timer used in multiple ways
   */
  status = rtems_timer_create( 1, &timer );
  directive_failed( status, "rtems_timer_create" );

  /*
   *  Get starting time
   */
  start = rtems_clock_get_ticks_since_boot();

  status = rtems_signal_catch( signal_handler, RTEMS_DEFAULT_MODES );
  directive_failed( status, "rtems_signal_catch" );
  puts( "rtems_signal_catch - handler installed" );

  /*
   * Test Signal from ISR
   */
  signal_sent = FALSE;

  status = rtems_timer_fire_after( timer, 10, test_signal_from_isr, NULL );
  directive_failed( status, "timer_fire_after failed" );

  while (1) {
    now = rtems_clock_get_ticks_since_boot();
    if ( (now-start) > 100 ) {
      puts( "Signal from ISR did not get processed\n" );
      rtems_test_exit( 0 );
    }
    if ( signal_processed )
      break;
  }

  puts( "Signal sent from ISR has been processed" );
  puts( "*** END OF TEST 38 ***" );
  rtems_test_exit( 0 );
}
