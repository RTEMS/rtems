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
#include "termios_testdriver_intr.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <rtems/dumpbuf.h>

const char rtems_test_name[] = "TERMIOS 4";

/* forward declarations to avoid warnings */
void write_helper(int fd, const char *c);
void read_helper(int fd, const char *expected);
void open_it(void);
void close_it(void);
rtems_task Init(rtems_task_argument argument);

void write_helper(
  int        fd,
  const char *c
)
{
  size_t   len;
  int      rc;
  
  len = strlen( c );
  printf( "Writing: %s", c );

  rc = write( fd, c, len );
  rtems_test_assert( rc == len );

  termios_test_driver_dump_tx("Transmitted");
}

uint8_t read_helper_buffer[256];

void read_helper(
  int         fd,
  const char *expected
)
{
  int    rc;
  size_t len;

  len = strlen( expected );

  termios_test_driver_set_rx( expected, len );
  printf( "\nReading (expected):\n" );
  rtems_print_buffer( (unsigned char *)expected, len-1 );

  rc = read( fd, read_helper_buffer, sizeof(read_helper_buffer) );
  rtems_test_assert( rc != -1 );

  printf( "Read %d bytes from read(2)\n", rc );
  rtems_print_buffer( read_helper_buffer, rc );

  termios_test_driver_dump_tx("As Read");
}

int Test_fd;

void open_it(void)
{
  /* open the file */
  puts( "open(" TERMIOS_TEST_DRIVER_DEVICE_NAME ") - OK " );
  Test_fd = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  rtems_test_assert( Test_fd != -1 );
}

void close_it(void)
{
  int rc;

  puts( "close(" TERMIOS_TEST_DRIVER_DEVICE_NAME ") - OK " );
  rc = close( Test_fd );
  rtems_test_assert( rc == 0 );
}

const char ExpectedOutput_1[] = "This is interrupt driven test output.\n";
const char ExpectedInput_1[] = "Blocking interrupt driven read.\n";
const char ExpectedInput_2[] = "Non-Blocking interrupt driven read.\n";

rtems_task Init(
  rtems_task_argument argument
)
{
  TEST_BEGIN();

  open_it();

  /* some basic cases */
  write_helper( Test_fd, ExpectedOutput_1 );
  read_helper( Test_fd, ExpectedInput_1 );
  termios_test_driver_set_rx_enqueue_now( true );
  read_helper( Test_fd, ExpectedInput_2 );

  close_it();

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS \
  TERMIOS_TEST_DRIVER_TABLE_ENTRY

/* we need to be able to open the test device */
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_MAXIMUM_TIMERS            2
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
