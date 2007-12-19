/*  Task_1
 *
 *  This routine serves as a test task.  It tests the I/O manager.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "system.h"

#define STUB_DRIVER_MAJOR     0x2
#define NO_DRIVER_MAJOR       0x3
#define INVALID_DRIVER_MAJOR  \
     (rtems_configuration_get_number_of_device_drivers() + 1)

/* driver entries to use with io_register */
rtems_driver_address_table GoodDriver = DEVNULL_DRIVER_TABLE_ENTRY;
rtems_driver_address_table BadDriver_Nothing = {
  NULL, NULL, NULL, NULL, NULL, NULL
};

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code         status;
  rtems_device_major_number registered;
  rtems_device_major_number registered_worked;

  puts( "-----  TESTING THE NULL DRIVER CHECKS  -----" );

  status = rtems_io_initialize( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_initialize" );
  puts( "TA1 - rtems_io_initialize  - NULL DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_open( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_open" );
  puts( "TA1 - rtems_io_open        - NULL DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_close( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_close" );
  puts( "TA1 - rtems_io_close       - NULL DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_read( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_read" );
  puts( "TA1 - rtems_io_read        - NULL DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_write( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_write" );
  puts( "TA1 - rtems_io_write       - NULL DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_control( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_control" );
  puts( "TA1 - rtems_io_control     - NULL DRIVER RTEMS_SUCCESSFUL" );

  puts( "-----  TESTING THE I/O MANAGER DIRECTIVES  -----" );

  status = rtems_io_initialize( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_initialize" );
  puts( "TA1 - rtems_io_initialize  - STUB DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_open( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_open" );
  puts( "TA1 - rtems_io_open        - STUB DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_close( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_close" );
  puts( "TA1 - rtems_io_close       - STUB DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_read( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_read" );
  puts( "TA1 - rtems_io_read        - STUB DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_write( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_write" );
  puts( "TA1 - rtems_io_write       - STUB DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_control( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_control" );
  puts( "TA1 - rtems_io_control     - STUB DRIVER RTEMS_SUCCESSFUL" );


  /*
   *  Invalid major number
   */
  puts( "-----  RETURNING INVALID MAJOR NUMBER -----" );

  status = rtems_io_initialize( INVALID_DRIVER_MAJOR, 0, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_initialize"
  );
  puts( "TA1 - rtems_io_initialize  - RTEMS_INVALID_NUMBER" );

  status = rtems_io_open( INVALID_DRIVER_MAJOR, 0, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_open"
  );
  puts( "TA1 - rtems_io_open        - RTEMS_INVALID_NUMBER" );

  /*
   *  Exercise the io register driver directive
   */
  puts( "-----  EXERCISE DRIVER REGISTER -----" );

  status = rtems_io_register_driver( 0, &GoodDriver, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_io_register_driver"
  );
  puts(
   "TA1 - rtems_io_register_driver - RTEMS_INVALID_ADDRESS - NULL registered"
  );

  status = rtems_io_register_driver( 0, NULL, &registered );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_io_register_driver"
  );
  puts(
    "TA1 - rtems_io_register_driver - RTEMS_INVALID_ADDRESS - NULL device"
  );

  status = rtems_io_register_driver( 0, &BadDriver_Nothing, &registered );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_io_register_driver"
  );
  puts(
    "TA1 - rtems_io_register_driver - RTEMS_INVALID_ADDRESS - no callouts"
  );

  status = rtems_io_register_driver(
    INVALID_DRIVER_MAJOR, &GoodDriver, &registered );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_register_driver"
  );
  puts( "TA1 - rtems_io_register_driver - RTEMS_INVALID_NUMBER - bad major" );

  /* this has to succeed to overwrite NULL driver entry required above. */
  status = rtems_io_register_driver( 0, &GoodDriver, &registered_worked );
  directive_failed( status, "rtems_io_register_driver" );
  puts( "TA1 - rtems_io_register_driver - RTEMS_SUCCESSFUL - overwrite NULL" );

  status = rtems_io_register_driver( 0, &GoodDriver, &registered );
  if ( status == RTEMS_SUCCESSFUL )
    printf( "registered major = %d\n", registered );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_io_register_driver"
  );
  puts( "TA1 - rtems_io_register_driver - RTEMS_TOO_MANY - no slots" );

  /* there should be a driver at major 1 -- clock, console, or stub */
  status = rtems_io_register_driver( 1, &GoodDriver, &registered );
  fatal_directive_status(
    status,
    RTEMS_RESOURCE_IN_USE,
    "rtems_io_register_driver"
  );
  puts(
    "TA1 - rtems_io_register_driver - RTEMS_RESOURCE_IN_USE - major in use"
  );

  /* this should unregister the device driver we registered above */
  status = rtems_io_unregister_driver( registered_worked );
  directive_failed( status, "rtems_io_unregister_driver" );
  puts( "TA1 - rtems_io_unregister_driver - RTEMS_SUCCESSFUL" );

  puts( "*** END OF TEST 21 ***" );
  rtems_test_exit( 0 );
}
