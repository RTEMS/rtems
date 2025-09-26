/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  IO Manager Dynamic Registration
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2009 embedded brains GmbH & Co. KG
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

#include <rtems/ioimpl.h>

const char rtems_test_name[] = "SP 40";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

static rtems_device_driver test_open(
     rtems_device_major_number  minor,
     rtems_device_minor_number  major,
     void                      *ignored
)
{
  (void) minor;
  (void) major;
  (void) ignored;

  return RTEMS_IO_ERROR;
}

static rtems_driver_address_table test_driver = {
  .initialization_entry = NULL,
  .open_entry = test_open,
  .close_entry = NULL,
  .read_entry = NULL,
  .write_entry = NULL,
  .control_entry = NULL
};

#define test_interrupt_context_enter( level ) \
  do { \
    rtems_interrupt_local_disable( level ); \
    ++_ISR_Nest_level; \
  } while (0)

#define test_interrupt_context_leave( level ) \
  do { \
    --_ISR_Nest_level; \
    rtems_interrupt_local_enable( level ); \
  } while (0)

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code         sc;
  rtems_device_major_number registered;
  rtems_device_major_number registered_not;
  rtems_device_major_number invalid_major = _IO_Number_of_drivers + 1;
  rtems_interrupt_level level;
  bool do_interrupt_context_test = true;

  TEST_BEGIN();

  /* Ensure that this test works as expected */
  test_interrupt_context_enter( level );
  do_interrupt_context_test = rtems_interrupt_is_in_progress();
  test_interrupt_context_leave( level );

  /*
   *  Register a driver with init == NULL and open != NULL
   */

  puts( "Init - rtems_io_register_driver - init == NULL, open != NULL OK" );
  sc = rtems_io_register_driver( 0, &test_driver, &registered );
  printf( "Init - Major slot returned = %d\n", (int) registered );
  directive_failed( sc, "rtems_io_register_driver" );

  puts( "Init - rtems_io_register_driver - init == NULL, open != NULL OK" );
  sc = rtems_io_register_driver( 0, &test_driver, &registered_not );
  printf( "Init - Major slot returned = %d\n", (int) registered_not );
  fatal_directive_status(
    sc,
    RTEMS_TOO_MANY,
    "rtems_io_register_driver too many"
  );

  puts( "Init - rtems_io_register_driver - used slot" );
  sc = rtems_io_register_driver( registered, &test_driver, &registered_not );
  fatal_directive_status(
    sc,
    RTEMS_RESOURCE_IN_USE,
    "rtems_io_register_driver slot in use"
  );

  puts( "Init - rtems_io_unregister_driver - used slot" );
  sc = rtems_io_unregister_driver( registered );
  directive_failed( sc, "rtems_io_unregister_driver" );

  puts( "Init - rtems_io_register_driver - free slot" );
  sc = rtems_io_register_driver( registered, &test_driver, &registered );
  directive_failed( sc, "rtems_io_register_driver" );

  puts( "Init - rtems_io_register_driver - called from interrupt context" );
  if ( do_interrupt_context_test ) {
    test_interrupt_context_enter( level );
    sc = rtems_io_register_driver( 0, NULL, NULL );
    test_interrupt_context_leave( level );
    fatal_directive_status(
      sc,
      RTEMS_CALLED_FROM_ISR,
      "rtems_io_register_driver"
    );
  }

  puts( "Init - rtems_io_register_driver - invalid registered major pointer" );
  sc = rtems_io_register_driver( 0, NULL, NULL );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_ADDRESS,
    "rtems_io_register_driver"
  );

  puts( "Init - rtems_io_register_driver - invalid driver table pointer" );
  sc = rtems_io_register_driver( 0, NULL, &registered );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_ADDRESS,
    "rtems_io_register_driver"
  );

  puts( "Init - rtems_io_register_driver - invalid empty driver table" );
  test_driver.open_entry = NULL;
  sc = rtems_io_register_driver( 0, &test_driver, &registered );
  test_driver.open_entry = test_open;
  fatal_directive_status(
    sc,
    RTEMS_INVALID_ADDRESS,
    "rtems_io_register_driver"
  );

  puts( "Init - rtems_io_register_driver - invalid major" );
  sc = rtems_io_register_driver( invalid_major, &test_driver, &registered );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_NUMBER,
    "rtems_io_register_driver"
  );

  puts( "Init - rtems_io_unregister_driver - called from interrupt context" );
  if ( do_interrupt_context_test ) {
    test_interrupt_context_enter( level );
    sc = rtems_io_unregister_driver( 0 );
    test_interrupt_context_leave( level );
    fatal_directive_status(
      sc,
      RTEMS_CALLED_FROM_ISR,
      "rtems_io_unregister_driver"
    );
  }

  puts( "Init - rtems_io_unregister_driver - invalid major number" );
  sc = rtems_io_unregister_driver( invalid_major );
  fatal_directive_status(
    sc,
    RTEMS_UNSATISFIED,
    "rtems_io_unregister_driver"
  );

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER

/* more one more driver slot than are statically configured */
#define CONFIGURE_MAXIMUM_DRIVERS 2

#define CONFIGURE_MAXIMUM_TASKS  1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of include file */
