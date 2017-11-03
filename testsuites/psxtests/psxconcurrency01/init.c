/*
 *  COPYRIGHT (c) 2016
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pmacros.h>
#include <pthread.h>
#include <errno.h>

const char rtems_test_name[] = "PSXCONCURRENCY01";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(
  void *argument
)
{
  int  sc;

  TEST_BEGIN();

  puts( "Init: pthread_getconcurrency - initial value of 0" );
  sc = pthread_getconcurrency();
  rtems_test_assert( sc == 0 );

  puts( "Init: pthread_setconcurrency - set value to 2" );
  sc = pthread_setconcurrency( 2 );
  rtems_test_assert( sc == 0 );

  puts( "Init: pthread_getconcurrency - confirm new value of 2" );
  sc = pthread_getconcurrency();
  rtems_test_assert( sc == 2 );

  TEST_END();
  rtems_test_exit(0);
}


/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
