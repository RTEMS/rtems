/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/bspIo.h>

#include <tmacros.h>

rtems_task Init( rtems_task_argument argument );

const char rtems_test_name[] = "SPFATAL " FATAL_ERROR_TEST_NAME;

static void print_test_begin_message(void)
{
  static bool done = false;

  if (!done) {
    done = true;
    TEST_BEGIN();
  }
}

rtems_task Init(
  rtems_task_argument argument
)
{
  print_test_begin_message();
  force_error();
  printk( "Fatal error (%s) NOT hit\n", FATAL_ERROR_DESCRIPTION );
  rtems_test_exit(0);
}

#ifdef FATAL_ERROR_EXPECTED_ERROR
static void Put_Error( uint32_t source, uint32_t error )
{
  if ( source == INTERNAL_ERROR_CORE ) {
    printk( rtems_internal_error_text( error ) );
  }
  else if (source == INTERNAL_ERROR_RTEMS_API ){
    if (error >  RTEMS_NOT_IMPLEMENTED )
      printk("Unknown Internal RTEMS Error (0x%08" PRIx32 ")", error);
    else
      printk( "%s", rtems_status_text( error ) );
  }
}
#endif

static void Put_Source( rtems_fatal_source source )
{
  printk( "%s", rtems_fatal_source_text( source ) );
}

static bool is_expected_error( rtems_fatal_code error )
{
#ifdef FATAL_ERROR_EXPECTED_ERROR
  return error == FATAL_ERROR_EXPECTED_ERROR;
#else /* FATAL_ERROR_EXPECTED_ERROR */
  return FATAL_ERROR_EXPECTED_ERROR_CHECK( error );
#endif /* FATAL_ERROR_EXPECTED_ERROR */
}

static void Fatal_extension(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   error
)
{
  print_test_begin_message();
  printk( "Fatal error (%s) hit\n", FATAL_ERROR_DESCRIPTION );

  if ( source != FATAL_ERROR_EXPECTED_SOURCE ){
    printk( "ERROR==> Fatal Extension source Expected (");
    Put_Source( FATAL_ERROR_EXPECTED_SOURCE );
    printk( ") received (");
    Put_Source( source );
    printk( ")\n" );
  }

  if ( always_set_to_false )
    printk(
      "ERROR==> Fatal Extension is internal set to true expected false\n"
    );

#ifdef FATAL_ERROR_EXPECTED_ERROR
  if ( error !=  FATAL_ERROR_EXPECTED_ERROR ) {
    printk( "ERROR==> Fatal Error Expected (");
    Put_Error( source, FATAL_ERROR_EXPECTED_ERROR );
    printk( ") received (");
    Put_Error( source, error );
    printk( ")\n" );
  }
#endif /* FATAL_ERROR_EXPECTED_ERROR */

  if (
    source == FATAL_ERROR_EXPECTED_SOURCE
      && !always_set_to_false
      && is_expected_error( error )
  ) {
    TEST_END();
  }
}

#define CONFIGURE_INIT

#define CONFIGURE_INITIAL_EXTENSIONS \
  { \
    NULL,                    /* create  */ \
    NULL,                    /* start   */ \
    NULL,                    /* restart */ \
    NULL,                    /* delete  */ \
    NULL,                    /* switch  */ \
    NULL,                    /* begin   */ \
    NULL,                    /* exitted */ \
    Fatal_extension          /* fatal   */ \
  }, \
  RTEMS_TEST_INITIAL_EXTENSION

/* extra parameters may be in testcase.h */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/* always need an Init task, some cases need more tasks */
#ifndef SPFATAL_TEST_CASE_EXTRA_TASKS
#define SPFATAL_TEST_CASE_EXTRA_TASKS 0
#endif
#define CONFIGURE_MAXIMUM_TASKS \
  (SPFATAL_TEST_CASE_EXTRA_TASKS + 1)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
