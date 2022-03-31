/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Classic API Signal to Task from ISR
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP 38";

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

  TEST_BEGIN();

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
  TEST_END();
  rtems_test_exit( 0 );
}
