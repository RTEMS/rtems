/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP SIGNAL ERROR 01";

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  TEST_BEGIN();
  /* send invalid id */
  status = rtems_signal_send( 100, RTEMS_SIGNAL_1 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_signal_send with illegal id"
  );
  puts( "TA1 - rtems_signal_send - RTEMS_INVALID_ID" );

  /* no signal in set */
  status = rtems_signal_send( RTEMS_SELF, 0 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_signal_send with no signals"
  );
  puts( "TA1 - rtems_signal_send - RTEMS_INVALID_NUMBER" );

  /* no signal handler */
  status = rtems_signal_send( RTEMS_SELF, RTEMS_SIGNAL_16 );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_signal_send with no handler"
  );
  puts( "TA1 - rtems_signal_send - RTEMS_NOT_DEFINED" );

  TEST_END();
}
