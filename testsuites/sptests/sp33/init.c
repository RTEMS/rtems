/* spmonotonic -- sanity check the rate monotonic manager
 *
 * license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE .
 *
 * $Id$
 */

#define TEST_INIT
#include <tmacros.h>  /* includes bsp.h, stdio, etc... */

/* prototype */
rtems_task Init (rtems_task_argument ignored);

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_TASKS     1
#define CONFIGURE_MAXIMUM_BARRIERS  1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code status;
  rtems_name  name = rtems_build_name('B','A','R','1');
  rtems_id    id;
  uint32_t    released;

  puts("\n\n*** TEST 33 ***");

  /* create period */
  puts( "Create Barrier" );
  status = rtems_barrier_create( name, RTEMS_DEFAULT_ATTRIBUTES, 0, &id );
  directive_failed(status, "rtems_barrier_create");

  puts( "Wait on Barrier w/timeout" );
  status = rtems_barrier_wait( id, 10 );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_barrier_wait did not timeout"
  );

#if 0
  /* start period with initial value */
  status = rtems_barrier_release( id, &released );
  directive_failed(status, "rtems_barrier_release");
  if ( released != 0 ) {
    printf( "ERROR -- rtems_barrier_release -- released != 0, = %d", released);
    exit(0);
  }
#endif

  puts( "Delete barrier" );
  status = rtems_barrier_delete(id);
  directive_failed(status, "rtems_barrier_delete");

  /* the end */
  printf("*** END OF TEST SP33 ***\n");
  exit(0);
}
