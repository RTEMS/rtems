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

#include "tmacros.h"
#include <termios.h>
#include <errno.h>
#include <unistd.h>

const char rtems_test_name[] = "TERMIOS 2";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument ignored
)
{
  int sc;
  pid_t pid;
  char *term_name_p;
  char term_name[32];

  TEST_BEGIN();

  puts( "tcdrain(12) - EBADF" );
  sc = tcdrain(12);
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EBADF );

  puts( "tcdrain(stdin) - OK" );
  sc = tcdrain(0);
  rtems_test_assert( !sc );

  puts( "tcdrain(stdout) - OK" );
  tcdrain(1);
  rtems_test_assert( !sc );

  puts( "tcdrain(stderr) - OK" );
  tcdrain(2);
  rtems_test_assert( !sc );

  puts( "" );

  /***** TEST TCFLOW *****/
  puts( "tcflow(stdin, TCOOFF) - ENOTSUP" );
  errno = 0;
  sc = tcflow( 0, TCOOFF );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == ENOTSUP );

  puts( "tcflow(stdin, TCOON) - ENOTSUP" );
  errno = 0;
  sc = tcflow( 0, TCOON );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == ENOTSUP );

  puts( "tcflow(stdin, TCIOFF) - ENOTSUP" );
  errno = 0;
  sc = tcflow( 0, TCIOFF );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == ENOTSUP );

  puts( "tcflow(stdin, TCION) - ENOTSUP" );
  errno = 0;
  sc = tcflow( 0, TCION );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == ENOTSUP );

  puts( "tcflow(stdin, 22) - EINVAL" );
  errno = 0;
  sc = tcflow( 0, 22 );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "" );

  /***** TEST TCFLUSH *****/
  puts( "tcflush(stdin, TCIFLUSH) - OK" );
  errno = 0;
  sc = tcflush( 0, TCIFLUSH );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( errno == 0 );

  puts( "tcflush(stdin, TCOFLUSH) - OK" );
  sc = tcflush( 0, TCOFLUSH );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( errno == 0 );

  puts( "tcflush(stdin, TCIOFLUSH) - OK" );
  sc = tcflush( 0, TCIOFLUSH );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( errno == 0 );

  puts( "tcflush(stdin, 22) - EINVAL" );
  errno = 0;
  sc = tcflush( 0, 22 );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "" );

  /***** TEST TCGETPGRP *****/
  puts( "tcgetpgrp( 1 ) - OK" );
  pid = tcgetpgrp(1);
  rtems_test_assert( pid == getpid() );

  puts( "tcsetpgrp( 1, 3 ) - OK" );
  sc = tcsetpgrp( 1, 3 );
  rtems_test_assert( !sc );

  puts( "" );

  /***** TEST TCSENDBREAK *****/
  puts( "tcsendbreak( 1, 0 ) - OK" );
  sc = tcsendbreak( 1, 0 );
  rtems_test_assert( !sc );

  puts( "" );

  /***** TEST CTERMID *****/
  puts( "ctermid( NULL ) - OK" );
  term_name_p = ctermid( NULL );
  rtems_test_assert( term_name_p );
  printf( "ctermid ==> %s\n", term_name_p );

  puts( "ctermid( term_name ) - OK" );
  term_name_p = ctermid( term_name );
  rtems_test_assert( term_name_p == term_name );
  printf( "ctermid ==> %s\n", term_name_p );

  TEST_END();
  exit( 0 );
}


/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
