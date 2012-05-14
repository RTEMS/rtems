/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include "test_support.h"
#include "termios_testdriver_polled.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <rtems/dumpbuf.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void write_helper(int fd, const char *c);
void read_helper(int fd, const char *expected);
void open_it(void);
void close_it(void);
void change_iflag(const char *desc, int mask, int new);

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

  termios_test_driver_dump_tx("Echoed");
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

void change_iflag( const char *desc, int mask, int new )
{
  int            rc;
  struct termios attr;

  printf( "Changing c_iflag to: %s\n", desc );
  rc = tcgetattr( Test_fd, &attr );
  rtems_test_assert( rc == 0 );

  attr.c_iflag &= ~mask;
  attr.c_iflag |= new;

  rc = tcsetattr( Test_fd, TCSANOW, &attr );
  rtems_test_assert( rc == 0 );
}

const char ExpectedOutput_1[] = "This is test output.\n";
const char ExpectedInput_1[] = "Test input this is.\n";
const char ExpectedInput_2[] = "1235\b456.\n";
const char ExpectedInput_3[] = "tab\ttab.\n";
const char ExpectedInput_4[] = "cr\r.";
const char ExpectedInput_5[] = "aBcDeFgH.\n";
const char ExpectedInput_6[] = "Testing VERASE\177.\n";
const char ExpectedInput_7[] = "Testing VKILL\025.\n";
const char ExpectedInput_8[] = "\177Testing VERASE in column 1.\n";
const char ExpectedInput_9[] = "\t tab \tTesting VKILL after tab.\025\n";

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** TEST TERMIOS03 ***" );

  open_it();

  /* some basic cases */
  write_helper( Test_fd, ExpectedOutput_1 );
  read_helper( Test_fd, ExpectedInput_1 );
  read_helper( Test_fd, ExpectedInput_2 );
  read_helper( Test_fd, ExpectedInput_3 );
  read_helper( Test_fd, ExpectedInput_4 );

  /* test to lower case input mapping */
  read_helper( Test_fd, ExpectedInput_5 );
  change_iflag( "Enable to lower case mapping on input", IUCLC, IUCLC );
  read_helper( Test_fd, ExpectedInput_5 );
  change_iflag( "Disable to lower case mapping on input", IUCLC, 0 );

  read_helper( Test_fd, ExpectedInput_6 );
  read_helper( Test_fd, ExpectedInput_7 );
  read_helper( Test_fd, ExpectedInput_8 );
  read_helper( Test_fd, ExpectedInput_9 );

  puts( "" );
  close_it();

  puts( "*** END OF TEST TERMIOS03 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS \
  TERMIOS_TEST_DRIVER_TABLE_ENTRY

/* include an extra slot for registering the termios one dynamically */
#define CONFIGURE_MAXIMUM_DRIVERS 3

/* one for the console and one for the test port */
#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 3

/* we need to be able to open the test device */
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
