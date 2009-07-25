/*
 *  IO Manager Dynamic Registration
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>

rtems_device_driver test_open(
     rtems_device_major_number  minor,
     rtems_device_minor_number  major,
     void                      *ignored
)
{
}

rtems_driver_address_table test_driver = {
  NULL,                               /* initialization procedure */
  test_open,                          /* open request procedure */
  NULL,                               /* close request procedure */
  NULL,                               /* read request procedure */
  NULL,                               /* write request procedure */
  NULL,                               /* special functions procedure */
};
rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code         sc;
  rtems_device_major_number registered;

  puts( "\n\n*** TEST 40 ***" );

  /*
   *  Register a driver with init == NULL and open != NULL
   */

  puts( "Init - rtems_io_register_driver - init == NULL, open != NULL OK" );
  sc = rtems_io_register_driver( 0, &test_driver, &registered );
  printf( "Init - Major slot returned = %d\n", (int) registered );
  directive_failed( sc, "rtems_io_register_driver" );

  puts( "Init - rtems_io_register_driver - used slot" );
  sc = rtems_io_register_driver( 1, &test_driver, &registered );
  fatal_directive_status(
    sc,
    RTEMS_RESOURCE_IN_USE,
    "rtems_io_register_driver slot in use"
  );

  puts( "*** END OF TEST 40 ***" );
  rtems_test_exit( 0 );
}


/* functions */

rtems_task Init(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/* more one more driver slot than are statically configured */
#define CONFIGURE_MAXIMUM_DRIVERS 4

#define CONFIGURE_MAXIMUM_TASKS  1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of include file */
