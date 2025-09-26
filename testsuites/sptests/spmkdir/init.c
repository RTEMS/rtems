/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup tests
 *
 * @brief rtems_mkdir() test.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
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

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include <rtems/libio.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPMKDIR";

static void test_mkdir(const char *path, mode_t omode, int expected_rv)
{
  struct stat st;
  int rv = 0;
  mode_t current_umask = umask(0);
  mode_t dirmode = S_IFDIR | (omode & ~current_umask);

  umask(current_umask);

  rv = rtems_mkdir(path, omode);
  rtems_test_assert(rv == expected_rv);

  if (rv == 0) {
    rv = stat(path, &st);
    rtems_test_assert(rv == 0 && st.st_mode == dirmode);
  }
}

static rtems_task Init(rtems_task_argument argument)
{
  (void) argument;

  mode_t omode = S_IRWXU | S_IRWXG | S_IRWXO;
  int rv = 0;

  TEST_BEGIN();

  puts( "rtems_mkdir a - OK" );
  test_mkdir("a", omode, 0);
  puts( "rtems_mkdir a/b - OK" );
  test_mkdir("a/b", omode, 0);
  puts( "rtems_mkdir a/b/c/d/e/f/g/h/i - OK" );
  test_mkdir("a/b/c/d/e/f/g/h/i", omode, 0);
  puts( "rtems_mkdir a/b/c - OK" );
  test_mkdir("a/b/c", omode, 0);
  puts( "rtems_mkdir a/b/c/1 - OK" );
  test_mkdir("a/b/c/1", 0, 0);
  puts( "rtems_mkdir a/b/c/2 - OK" );
  test_mkdir("a/b/c/2", S_IRWXU, 0);
  puts( "rtems_mkdir a/b/c/3 - OK" );
  test_mkdir("a/b/c/3", S_IRWXG, 0);
  puts( "rtems_mkdir a/b/c/4 - OK" );
  test_mkdir("a/b/c/4", S_IRWXO, 0);
  puts( "rtems_mkdir a/b - OK" );
  test_mkdir("a/b", omode, 0);
  puts( "rtems_mkdir a - OK" );
  test_mkdir("a", omode, 0);
  puts( "rtems_mkdir a/b/x - OK" );
  test_mkdir("a/b/x", S_IRUSR, 0);
  puts( "rtems_mkdir a/b/x/y - expect failure" );
  test_mkdir("a/b/x/y", S_IRUSR, -1);
  puts( "mknod regular file a/n - OK" );  
  rv = mknod("a/n", S_IRWXU | S_IFREG, 0LL);
  puts( "rtems_mkdir a/n/b - expect failure" );
  test_mkdir("a/n/b", S_IRUSR, -1);

  puts( "Create node b and open in RDONLY mode - OK" );
  rv = open ("b", O_CREAT | O_RDONLY, omode);
  rtems_test_assert(rv >= 0);

  puts( "Closing b - OK" );
  rv = close(rv);
  rtems_test_assert(rv == 0);

  puts( "rtems_mkdir b - expect failure" );
  test_mkdir("b", omode, -1);
  rtems_test_assert(errno == EEXIST);

  TEST_END();

  exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
