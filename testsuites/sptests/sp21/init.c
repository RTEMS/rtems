/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
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

#include <tmacros.h>
#include <rtems/devnull.h>
#include <rtems/devzero.h>
#include <rtems/libio.h>

const char rtems_test_name[] = "SP 21";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void do_test_io_manager(void);
void do_test_zero_driver(void);

#define PRIurtems_device_major_number PRIu32

#define STUB_DRIVER_MAJOR     1
#define ZERO_DRIVER_MAJOR     2
#define NO_DRIVER_MAJOR       3
#define INVALID_DRIVER_MAJOR  UINT32_MAX

/* driver entries to use with io_register */
rtems_driver_address_table GoodDriver = DEVNULL_DRIVER_TABLE_ENTRY;
rtems_driver_address_table BadDriver_Nothing = {
  NULL, NULL, NULL, NULL, NULL, NULL
};

void do_test_io_manager(void)
{
  rtems_status_code         status;
  rtems_device_major_number registered;
  rtems_device_major_number registered_worked;

  puts( "-----  TESTING THE NULL DRIVER CHECKS  -----" );

  status = rtems_io_initialize( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_initialize" );
  puts( "rtems_io_initialize  - NULL DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_open( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_open" );
  puts( "rtems_io_open        - NULL DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_close( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_close" );
  puts( "rtems_io_close       - NULL DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_read( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_read" );
  puts( "rtems_io_read        - NULL DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_write( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_write" );
  puts( "rtems_io_write       - NULL DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_control( NO_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_control" );
  puts( "rtems_io_control     - NULL DRIVER RTEMS_SUCCESSFUL" );

  puts( "-----  TESTING THE I/O MANAGER DIRECTIVES  -----" );

  status = rtems_io_initialize( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_initialize" );
  puts( "rtems_io_initialize  - STUB DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_open( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_open" );
  puts( "rtems_io_open        - STUB DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_close( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_close" );
  puts( "rtems_io_close       - STUB DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_read( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_read" );
  puts( "rtems_io_read        - STUB DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_write( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_write" );
  puts( "rtems_io_write       - STUB DRIVER RTEMS_SUCCESSFUL" );

  status = rtems_io_control( STUB_DRIVER_MAJOR, 0, NULL );
  directive_failed( status, "rtems_io_control" );
  puts( "rtems_io_control     - STUB DRIVER RTEMS_SUCCESSFUL" );

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
  puts( "rtems_io_initialize  - RTEMS_INVALID_NUMBER" );

  status = rtems_io_open( INVALID_DRIVER_MAJOR, 0, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_open"
  );
  puts( "rtems_io_open        - RTEMS_INVALID_NUMBER" );

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
   "rtems_io_register_driver - RTEMS_INVALID_ADDRESS - NULL registered"
  );

  status = rtems_io_register_driver( 0, NULL, &registered );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_io_register_driver"
  );
  puts(
    "rtems_io_register_driver - RTEMS_INVALID_ADDRESS - NULL device"
  );

  status = rtems_io_register_driver( 0, &BadDriver_Nothing, &registered );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_io_register_driver"
  );
  puts(
    "rtems_io_register_driver - RTEMS_INVALID_ADDRESS - no callouts"
  );

  status = rtems_io_register_driver(
    INVALID_DRIVER_MAJOR, &GoodDriver, &registered );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_register_driver"
  );
  puts( "rtems_io_register_driver - RTEMS_INVALID_NUMBER - bad major" );

  /* this has to succeed to overwrite NULL driver entry required above. */
  status = rtems_io_register_driver( 0, &GoodDriver, &registered_worked );
  directive_failed( status, "rtems_io_register_driver" );
  puts( "rtems_io_register_driver - RTEMS_SUCCESSFUL - overwrite NULL" );

  status = rtems_io_register_driver( 0, &GoodDriver, &registered );
  if ( status == RTEMS_SUCCESSFUL )
    printf(
      "registered major = %" PRIurtems_device_major_number "\n", registered );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_io_register_driver"
  );
  puts( "rtems_io_register_driver - RTEMS_TOO_MANY - no slots" );

  /* there should be a driver at major 1 -- clock, console, or stub */
  status = rtems_io_register_driver( 1, &GoodDriver, &registered );
  fatal_directive_status(
    status,
    RTEMS_RESOURCE_IN_USE,
    "rtems_io_register_driver"
  );
  puts(
    "rtems_io_register_driver - RTEMS_RESOURCE_IN_USE - major in use"
  );

  /* this should unregister the device driver we registered above */
  status = rtems_io_unregister_driver( registered_worked );
  directive_failed( status, "rtems_io_unregister_driver" );
  puts( "rtems_io_unregister_driver - RTEMS_SUCCESSFUL" );
}

void do_test_zero_driver(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  char in = 'I';
  rtems_libio_rw_args_t rw_in = {
    .buffer = &in,
    .count = sizeof(in)
  };
  char out = 'O';
  rtems_libio_rw_args_t rw_out = {
    .buffer = &out,
    .count = sizeof(out)
  };

  puts( "-----  TESTING THE ZERO DRIVER CHECKS  -----" );

  sc = rtems_io_initialize( ZERO_DRIVER_MAJOR, 0, NULL );
  rtems_test_assert( sc == RTEMS_TOO_MANY );

  sc = rtems_io_open( ZERO_DRIVER_MAJOR, 0, NULL );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_io_close( ZERO_DRIVER_MAJOR, 0, NULL );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_io_read( ZERO_DRIVER_MAJOR, 0, &rw_in );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( in == 0 );
  rtems_test_assert( rw_in.bytes_moved == sizeof(in) );

  sc = rtems_io_write( ZERO_DRIVER_MAJOR, 0, &rw_out );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( out == 'O' );
  rtems_test_assert( rw_out.bytes_moved == sizeof(out) );

  sc = rtems_io_control( ZERO_DRIVER_MAJOR, 0, NULL );
  rtems_test_assert( sc == RTEMS_IO_ERROR );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  TEST_BEGIN();

  do_test_io_manager();

  do_test_zero_driver();

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_NULL_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
