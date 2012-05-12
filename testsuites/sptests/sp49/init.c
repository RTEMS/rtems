/*
 *  COPYRIGHT (c) 1989-2012.
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

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(rtems_task_argument ignored)
{
  rtems_status_code sc;
  rtems_id          q;
  uint32_t          flushed;

  puts( "\n\n*** TEST 49 ***" );

  puts( "Create Message Queue" );
  sc = rtems_message_queue_create(
    rtems_build_name('m', 's', 'g', ' '),
    1,
    sizeof(uint32_t),
    RTEMS_DEFAULT_ATTRIBUTES,
    &q
  );
  directive_failed( sc, "rtems_message_queue_create" );

  puts( "Flush Message Queue using Task Self ID" );
  sc = rtems_message_queue_flush( rtems_task_self(), &flushed );
  fatal_directive_status( sc, RTEMS_INVALID_ID, "flush" );

  puts( "Flush returned INVALID_ID as expected" );

  puts( "*** END OF TEST 49 ***" );
  rtems_test_exit( 0 );
}

/* configuration stuff */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              1
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES     1
#define CONFIGURE_MESSAGE_BUFFER_MEMORY      256 /* overkill */

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
