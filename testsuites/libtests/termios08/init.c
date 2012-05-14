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
#include <sys/ioctl.h>
#include <rtems/dumpbuf.h>
#include <rtems/termiostypes.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void open_it(void);
void close_it(void);
void write_it(void);
void change_lflag( const char *desc, int mask, int new );
void change_vmin_vtime( const char *desc, int min, int time );
void read_it(ssize_t expected);

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

void write_it(void)
{
  ssize_t sc;
  char    ch[10] = "PPPD TEST";

  puts( "write(PPPD TEST) - OK " );
  sc = write(Test_fd, ch, sizeof(ch));
  rtems_test_assert( sc == sizeof(ch) );
}

uint8_t read_helper_buffer[256];

void change_lflag( const char *desc, int mask, int new )
{
  int            rc;
  struct termios attr;

  printf( "Changing c_lflag to: %s\n", desc );
  rc = tcgetattr( Test_fd, &attr );
  rtems_test_assert( rc == 0 );

  attr.c_lflag &= ~mask;
  attr.c_lflag |= new;

  rc = tcsetattr( Test_fd, TCSANOW, &attr );
  rtems_test_assert( rc == 0 );
}

void change_vmin_vtime( const char *desc, int min, int time )
{
  int            rc;
  struct termios attr;

  printf( "Changing %s - VMIN=%d VTIME=%d\n", desc, min, time );
  rc = tcgetattr( Test_fd, &attr );
  rtems_test_assert( rc == 0 );

  attr.c_cc[VMIN] = min;
  attr.c_cc[VTIME] = time;

  rc = tcsetattr( Test_fd, TCSANOW, &attr );
  rtems_test_assert( rc == 0 );
}

void read_it( ssize_t expected )
{
  ssize_t rc;
  char    buf[32];

  rtems_test_assert( expected <= sizeof(buf) );

  printf( "read - %zd expected\n", expected );
  rc = read( Test_fd, buf, expected ); 
  if ( expected != rc )
    printf( "ERROR - expected=%zd rc=%zd\n", expected, rc );
  rtems_test_assert( expected == rc );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** TEST TERMIOS08 ***" );

  open_it();
  change_lflag( "non-canonical", ICANON, 0 );

  change_vmin_vtime( "to polling", 0, 0 );
  read_it( 0 );

  change_vmin_vtime( "to half-second timeout", 0, 5 );
  read_it( 0 );

  change_vmin_vtime( "to half-second timeout", 5, 3 );
  puts( "Enqueue 2 characters" );
  termios_test_driver_set_rx( "ab", 2 );
  read_it( 2 );

  change_vmin_vtime( "to half-second timeout", 5, 3 );
  puts( "Enqueue 1 character" );
  termios_test_driver_set_rx( "b", 1 );
  read_it( 1 );

  puts( "Enqueue 7 characters" );
  termios_test_driver_set_rx( "1234567", 7 );
  read_it( 5 );
  read_it( 2 );

  close_it();
  
  puts( "*** END OF TEST TERMIOS08 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS TERMIOS_TEST_DRIVER_TABLE_ENTRY

/* one for the console and one for the test port */
#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 3

/* we need to be able to open the test device */
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
