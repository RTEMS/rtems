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
#define INVALID_DRIVER_MAJOR  (Configuration.number_of_device_drivers + 1)
/* #define INVALID_DRIVER_MAJOR  0xffffffff */

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code status;

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

  puts( "*** END OF TEST 21 ***" );
  rtems_test_exit( 0 );
}
