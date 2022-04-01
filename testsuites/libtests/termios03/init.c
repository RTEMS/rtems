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
#include "termios_testdriver_polled.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <rtems/dumpbuf.h>
#include <rtems/termiostypes.h>

const char rtems_test_name[] = "TERMIOS 3";

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
  TEST_BEGIN();

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
#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
