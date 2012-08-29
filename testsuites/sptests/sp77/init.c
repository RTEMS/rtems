/*
 *  COPYRIGHT (c) 2012.
 *  Krzysztof Miesowicz krzysztof.miesowicz@gmail.com
 *  
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <tmacros.h>
#include "test_support.h"

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** TEST sp77 ***" );

  rtems_status_code status;
  rtems_id id;

  status = rtems_message_queue_create(
    rtems_build_name( 'M', 'Q' , 'T', '1'),
    2,
    SIZE_MAX-2,
    RTEMS_DEFAULT_ATTRIBUTES,
      &id
  );
  
  fatal_directive_check_status_only(status , RTEMS_UNSATISFIED ,
				    "attempt to create message queue return: ");  
  puts( "*** END OF TEST sp77 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 1
#define CONFIGURE_MESSAGE_BUFFER_MEMORY (CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( \
						2,SIZE_MAX))

#include <rtems/confdefs.h>
/* end of file */
