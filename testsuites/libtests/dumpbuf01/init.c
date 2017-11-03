/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include "test_support.h"
#include <rtems/dumpbuf.h>

const char rtems_test_name[] = "DUMPBUF 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void do_test(int length);

unsigned char Buffer[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890\n";

void do_test(
  int length
)
{
  printf( "====== Printing %d Bytes ======\n", length );
  rtems_print_buffer( Buffer, length );
  printf( "===============================\n\n" );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  int i;

  TEST_BEGIN();

  for ( i = 0 ; i < sizeof(Buffer) ; i++ ) {
    do_test( i );
  }
  do_test( -1 );
  
  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
