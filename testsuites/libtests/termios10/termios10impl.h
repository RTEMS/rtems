/*
 *  COPYRIGHT (c) 1989-2012,2019.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  SPDX-License-Identifier: BSD-2-Clause
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include "test_support.h"

#ifdef INTERRUPT_DRIVEN
#include "../termios04/termios_testdriver_intr.h"
const char rtems_test_name[] = "TERMIOS 11 -- Interrupt driven";
#else
#include "../termios03/termios_testdriver_polled.h"
const char rtems_test_name[] = "TERMIOS 10 -- Polled";
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <rtems/dumpbuf.h>
#include <rtems/libio.h>

int Test_fd;

static void open_it(void)
{
  Test_fd = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  rtems_test_assert( Test_fd != -1 );
}

static void change_lflag( const char *desc, int mask, int new )
{ 
  int            rc;
  struct termios attr;
  
  if (desc) {
    printf( "Changing c_lflag to: %s\n", desc );
  }

  rc = tcgetattr( Test_fd, &attr );
  rtems_test_assert( rc == 0 );
  
  attr.c_lflag &= ~mask;
  attr.c_lflag |= new;
  
  rc = tcsetattr( Test_fd, TCSANOW, &attr );
  rtems_test_assert( rc == 0 );
}


static void read_it(ssize_t expected, int expected_intr)
{
  ssize_t rc;
  char    buf[32];

  rtems_test_assert( expected <= sizeof(buf) );

  rc = read( Test_fd, buf, expected ); 
  if (expected_intr) {
    rtems_test_assert( rc == -1 );
    rtems_test_assert( errno == EINTR );
  } else {
    if ( expected != rc )
      printf( "ERROR - expected=%zd rc=%zd\n", expected, rc );
    rtems_test_assert( expected == rc );
  }
}

static void close_it(void)
{
  int rc;

  rc = close( Test_fd );
  rtems_test_assert( rc == 0 );
}

volatile int sigint_occurred = 0;
volatile int sigquit_occurred = 0;

static void sigint_handler(int signo)
{
  rtems_test_assert(signo == SIGINT);
  sigint_occurred = 1; 
}

static void sigquit_handler(int signo)
{
  rtems_test_assert(signo == SIGQUIT);
  sigquit_occurred = 1; 
}

static void test_read_for_signal(
  const char *description,
  int         isig_value,
  char        c,
  int         sigint_expected,
  int         sigquit_expected
)
{
  char expected[3];

  printf("Test read for %s\n", description);

  expected[0] = c;
  expected[1] = '\n'; /* in canonical mode, so need \n for read to return */
  expected[2] = '\0';

  sigint_occurred  = 0;
  sigquit_occurred = 0;

  open_it();

  change_lflag(NULL, ISIG, isig_value);

  termios_test_driver_set_rx( expected, 2 );

  read_it(1, (sigint_expected || sigquit_expected));

  rtems_test_assert(sigint_occurred == sigint_expected);
  rtems_test_assert(sigquit_occurred == sigquit_expected);
  close_it();
}

/*
 * Use a POSIX init thread so signals are enabled.
 */
static void *POSIX_Init(void *argument)
{
  int rc;

  TEST_BEGIN();

  signal(SIGINT, sigint_handler);
  signal(SIGQUIT, sigquit_handler);

  puts( "Exercise default ISIG handler with ISIG enabled");
  test_read_for_signal("VKILL - no signals", ISIG, '\003', 0, 0);
  test_read_for_signal("VQUIT - no signals", ISIG, '\034', 0, 0);

  puts( "Exercise POSIX ISIG handler with ISIG enabled");
  rc = rtems_termios_register_isig_handler(rtems_termios_posix_isig_handler);
  rtems_test_assert( rc == 0 );
  test_read_for_signal("VKILL - signal caught", ISIG, '\003', 1, 0);
  test_read_for_signal("VQUIT - signal caught", ISIG, '\034', 0, 1);

  puts( "Exercise default ISIG handler with ISIG enabled");
  rc = rtems_termios_register_isig_handler(rtems_termios_default_isig_handler);
  rtems_test_assert( rc == 0 );
  test_read_for_signal("VKILL - signal caught", ISIG, '\003', 0, 0);
  test_read_for_signal("VQUIT - signal caught", ISIG, '\034', 0, 0);

  puts( "Exercise POSIX ISIG handler with ISIG disabled");
  rc = rtems_termios_register_isig_handler(rtems_termios_posix_isig_handler);
  rtems_test_assert( rc == 0 );
  test_read_for_signal("VKILL - signal caught", 0, '\003', 0, 0);
  test_read_for_signal("VQUIT - signal caught", 0, '\034', 0, 0);

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS \
  TERMIOS_TEST_DRIVER_TABLE_ENTRY

/* we need to be able to open the test device */
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_MAXIMUM_TIMERS            2
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
