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
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "test_driver.h"
#include <rtems/devnull.h>

const char rtems_test_name[] = "DEVICEIO 1";

static rtems_task Init(
  rtems_task_argument argument
)
{
  int status;
  int fdr = 0, fdw = 0;
  char buf[10];

  TEST_BEGIN();

  puts( "Init - attempt to open the /dev/test WR mode -- OK" );
  fdw = open( "/dev/test", O_WRONLY );
  rtems_test_assert( fdw != -1 );
  
  puts( "Init - attempt to write to /dev/test - expect ENOSYS" );
  status = write( fdw, "data", 10 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOSYS );

  puts( "Init - attempt to open the /dev/test RD mode -- OK" );
  fdr = open( "/dev/test", O_RDONLY );
  rtems_test_assert( fdr != -1 );

  puts( "Init - attempt to read from /dev/test - expect ENOSYS" );
  status = read( fdr, buf, 10 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOSYS );

  puts( "Init - attempt ioctl on the device - expect ENOSYS" );
  status = ioctl( fdr, -1 );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOSYS );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS TEST_DRIVER_TABLE_ENTRY

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
