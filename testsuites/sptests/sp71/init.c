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
#include "test_support.h"

#define PER_ALLOCATION 8000

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id          id;
  long              created;

  puts( "\n\n*** TEST 71 ***" );

  created = 0;
  do {
    status = rtems_port_create(
      rtems_build_name( 'P', 'O', 'R', 'T' ),
      (void *) 0x1000,
      (void *) 0x2000,
      1024,
      &id
    );
    if ( status == RTEMS_TOO_MANY )
      break;
    directive_failed( status, "rtems_task_create" );
    created++;
  } while (1);

  printf(
    "%ld ports created using %d per allocation -- need %ld\n",
    created,
    PER_ALLOCATION,
    (long) OBJECTS_ID_FINAL_INDEX
  );

  if ( (created + PER_ALLOCATION) > OBJECTS_ID_FINAL_INDEX ) {
    puts( "Test case hit" );
    puts( "*** END OF TEST 71 ***" );
  } else {
    puts( "Test case NOT hit" );
  }
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_MAXIMUM_PORTS rtems_resource_unlimited(PER_ALLOCATION)
#define CONFIGURE_UNIFIED_WORK_AREAS
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
