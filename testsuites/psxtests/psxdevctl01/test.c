/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  This test exercises the posix_devctl() method.
 */

/*
 *  COPYRIGHT (c) 2016.
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

#define _POSIX_26_C_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"
#include <errno.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include <devctl.h>

#include <unistd.h>
#include <fcntl.h>

const char rtems_test_name[] = "PSXDEVCTL 1";

/* forward declarations to avoid warnings */
int test_main(void);

/*
 *  main entry point to the test
 */

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int    argc,
  char **argv
)
#endif
{
  int     status;
  int     fd;
  int     dcmd;
  int     dev_data;
  void   *dev_data_ptr;
  size_t  nbyte;

  TEST_BEGIN();

  puts( "posix_devctl() SOCKCLOSE on invalid file descriptor -- EBADF" );
  fd = -1;
  dcmd = SOCKCLOSE;
  dev_data_ptr = NULL;
  nbyte = 0;
  status = posix_devctl( fd, dcmd, dev_data_ptr, nbyte, NULL );
  rtems_test_assert( status == EBADF );

  /*
   * Create a file, open it, and close it via posix_devctl().
   * Then verify it is really closed.
   */
  puts( "posix_devctl() SOCKCLOSE on valid file descriptor -- OK" );
  fd = open("tmp_for_close", O_CREAT | O_RDWR, S_IRWXU );
  rtems_test_assert( fd != -1 );

  dcmd = SOCKCLOSE;
  dev_data_ptr = NULL;
  nbyte = 0;
  status = posix_devctl( fd, dcmd, dev_data_ptr, nbyte, NULL );
  rtems_test_assert( status == 0 );

  status = close( fd );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );

  puts( "posix_devctl() FIONBIO with invalid nbyte -- EINVAL" );
  fd = 0;
  dcmd = FIONBIO;
  dev_data_ptr = NULL;
  nbyte = 0;
  status = posix_devctl( fd, dcmd, dev_data_ptr, nbyte, NULL );
  rtems_test_assert( status == EINVAL );

  puts( "posix_devctl() FIONBIO with invalid file descriptor -- EBADF" );
  fd = -1;
  dcmd = FIONBIO;
  dev_data_ptr = NULL;
  nbyte = sizeof(int);
  status = posix_devctl( fd, dcmd, dev_data_ptr, nbyte, NULL );
  rtems_test_assert( status == EBADF );

  puts( "posix_devctl() FIONBIO flag not zero -- 0" );
  fd = 0;
  dcmd = FIONBIO;
  dev_data = 1;
  dev_data_ptr = &dev_data;
  nbyte = sizeof(int);
  status = posix_devctl( fd, dcmd, dev_data_ptr, nbyte, NULL );
  rtems_test_assert( status == 0 );

  puts( "posix_devctl() FIONBIO flag is zero -- 0" );
  fd = 0;
  dcmd = FIONBIO;
  dev_data = 0;
  dev_data_ptr = &dev_data;
  nbyte = sizeof(int);
  status = posix_devctl( fd, dcmd, dev_data_ptr, nbyte, NULL );
  rtems_test_assert( status == 0 );

  puts( "posix_devctl() dcmd not valid value -- EBADF" );
  fd = 0;
  dcmd = 1;
  dev_data = 0;
  dev_data_ptr = &dev_data;
  nbyte = sizeof(int);
  status = posix_devctl( fd, dcmd, dev_data_ptr, nbyte, NULL );
  rtems_test_assert( status == EBADF );

  TEST_END();
  exit(0);
}
