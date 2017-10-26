/**
 *  @file
 *
 *  This test exercises the posix_devctl() method.
 */

/*
 *  COPYRIGHT (c) 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _POSIX_26_C_SOURCE
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
  int     dev_info;

  TEST_BEGIN();

  puts( "posix_devctl() FIONBIO on stdin return dev_info -- EBADF" );
  fd = 0;
  dcmd = FIONBIO;
  dev_data_ptr = &dev_data;
  nbyte = sizeof(dev_data);
  status = posix_devctl( fd, dcmd, dev_data_ptr, nbyte, &dev_info );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );
  rtems_test_assert( dev_info == 0 );

  puts( "posix_devctl() FIONBIO on stdin NULL dev_info -- EBADF" );
  fd = 0;
  dcmd = FIONBIO;
  dev_data_ptr = NULL;
  nbyte = 0;
  status = posix_devctl( fd, dcmd, dev_data_ptr, nbyte, NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EBADF );

  TEST_END();
  exit(0);
}
