/*
 *  IO Manager Dynamic Registration
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2009 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

static rtems_device_driver test_open(
     rtems_device_major_number  minor,
     rtems_device_minor_number  major,
     void                      *ignored
)
{
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
    _Thread_Disable_dispatch(); \
    rtems_interrupt_disable( level ); \
    ++_ISR_Nest_level; \
  } while (0)

#define test_interrupt_context_leave( level ) \
  do { \
    --_ISR_Nest_level; \
    rtems_interrupt_enable( level ); \
    _Thread_Enable_dispatch(); \
  } while (0)

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code         sc;
  rtems_device_major_number registered;
  rtems_device_major_number registered_not;
  rtems_device_major_number invalid_major = _IO_Number_of_drivers + 1;
  rtems_interrupt_level level;
  bool do_interrupt_context_test = true;

  puts( "\n\n*** TEST 40 ***" );

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

  puts( "*** END OF TEST 40 ***" );
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/* more one more driver slot than are statically configured */
#define CONFIGURE_MAXIMUM_DRIVERS 3

#define CONFIGURE_MAXIMUM_TASKS  1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of include file */
