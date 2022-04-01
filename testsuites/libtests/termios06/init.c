/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
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
#include "test_support.h"
#include "../termios03/termios_testdriver_polled.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <rtems/bspIo.h>
#include <rtems/dumpbuf.h>
#include <rtems/termiostypes.h>

const char rtems_test_name[] = "TERMIOS 6";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void open_it(void);
void Rx_Wake(struct termios *tty, void *arg);
void Tx_Wake(struct termios *tty, void *arg);
void set_wakeups(void);
void set_discipline(void);
void ioctl_it(void);
void close_it(void);
void write_it(void);
void read_helper(int fd, const char *expected);
void read_it(void);

void pppasyncattach(void);
void ppp_test_driver_set_rx( const char *expected, size_t len );

int Test_fd;
int InitialDiscipline;

void open_it(void)
{
  /* open the file */
  puts( "open(" TERMIOS_TEST_DRIVER_DEVICE_NAME ") - OK " );
  Test_fd = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  rtems_test_assert( Test_fd != -1 );
}

void Rx_Wake(
  struct termios *tty,
  void           *arg
)
{
  printk( "Rx_Wake - invoked\n" );
}

void Tx_Wake(
  struct termios *tty,
  void           *arg
)
{
  printk( "Tx_Wake - invoked\n" );
}

struct ttywakeup RxWake = { Rx_Wake, NULL };
struct ttywakeup TxWake = { Tx_Wake, NULL };

void set_wakeups(void)
{
  int sc;

  puts( "ioctl - RTEMS_IO_SNDWAKEUP - OK" );
  sc = ioctl( Test_fd, RTEMS_IO_SNDWAKEUP, &TxWake );
  rtems_test_assert( sc == 0 );

  puts( "ioctl - RTEMS_IO_RCVWAKEUP - OK" );
  sc = ioctl( Test_fd, RTEMS_IO_RCVWAKEUP, &RxWake );
  rtems_test_assert( sc == 0 );

}

void set_discipline(void)
{
  int pppdisc = PPPDISC;
  int sc;

  puts( "ioctl - TIOCGETD - OK" );
  sc = ioctl(Test_fd, TIOCGETD, &InitialDiscipline);
  rtems_test_assert( sc == 0 );

  puts( "ioctl - TIOCSETD - OK" );
  sc = ioctl(Test_fd, TIOCSETD, &pppdisc);
  rtems_test_assert( sc == 0 );

  puts( "ioctl - TIOCSETD - OK" );
  sc = ioctl(Test_fd, TIOCSETD, &pppdisc);
  rtems_test_assert( sc == 0 );
}

void ioctl_it(void)
{
  int rc;
  struct termios t;

  puts( "ioctl(" TERMIOS_TEST_DRIVER_DEVICE_NAME ") - OK " );
  rc = ioctl( Test_fd, 0xFFFF, NULL );
  rtems_test_assert( rc == 0 );

  puts( "tcgetattr(" TERMIOS_TEST_DRIVER_DEVICE_NAME ") - OK " );
  rc = tcgetattr( Test_fd, &t );
  rtems_test_assert( rc == 0 );

  puts( "Turn on flow control on output - OK" );
  t.c_iflag |= IXON;
  rc = tcsetattr( Test_fd, TCSANOW, &t );
  rtems_test_assert( rc == 0 );

  puts( "Turn off flow control on output - OK" );
  t.c_iflag &= ~IXON;
  rc = tcsetattr( Test_fd, TCSANOW, &t );
  rtems_test_assert( rc == 0 );

  puts( "Turn on flow control on input - OK" );
  t.c_iflag |= IXOFF;
  rc = tcsetattr( Test_fd, TCSANOW, &t );
  rtems_test_assert( rc == 0 );

  puts( "Turn off flow control on input - OK" );
  t.c_iflag &= ~IXOFF;
  rc = tcsetattr( Test_fd, TCSANOW, &t );
  rtems_test_assert( rc == 0 );
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

void read_helper(
  int         fd,
  const char *expected
)
{
  int    rc;
  size_t len;

  len = strlen( expected );

  ppp_test_driver_set_rx( expected, len );
  printf( "\nReading (expected):\n" );
  rtems_print_buffer( (unsigned char *)expected, len-1 );

  rc = read( fd, read_helper_buffer, sizeof(read_helper_buffer) );
  rtems_test_assert( rc != -1 );

  printf( "Read %d bytes from read(2)\n", rc );
  rtems_print_buffer( read_helper_buffer, rc );
}

void read_it(void)
{
  read_helper( Test_fd, "This is test PPP input." );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  TEST_BEGIN();

  pppasyncattach();
  open_it();
  set_wakeups();
  set_discipline();
  write_it();
  ioctl_it();
  read_it();
  close_it();

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS TERMIOS_TEST_DRIVER_TABLE_ENTRY

/* we need to be able to open the test device */
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
