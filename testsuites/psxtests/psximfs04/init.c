/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2025 On-Line Application Research Corporation (OAR)
 * Copyright (c) 2025 Bhavya Shah <bhavyashah8443@gmail.com>
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <fcntl.h>
#include <errno.h>
#include <rtems/libcsupport.h>

#define BLOCK_SIZE 1024
const char rtems_test_name[] = "PSXIMFS 4";

#define FILE_NAME "large_file"

rtems_task Init(rtems_task_argument argument);

rtems_task Init(rtems_task_argument argument)
{
  int TestFd, rv;

  (void) argument;

  TEST_BEGIN();

  puts( "open(" FILE_NAME ") - OK " );
  TestFd = open( FILE_NAME, O_CREAT | O_RDWR, 0777 );
  rtems_test_assert( TestFd != -1 );

  puts( "close(" FILE_NAME ") - OK " );
  rv = close( TestFd );
  rtems_test_assert( rv == 0 );

  struct statvfs imfs_statvfs;
  puts( "statvfs(" FILE_NAME ") - OK " );
  rv = statvfs( FILE_NAME, &imfs_statvfs );
  rtems_test_assert( imfs_statvfs.f_bsize == BLOCK_SIZE );

  rv = unlink( FILE_NAME );
  rtems_test_assert( rv == 0 );

  TEST_END();

  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK BLOCK_SIZE
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */

