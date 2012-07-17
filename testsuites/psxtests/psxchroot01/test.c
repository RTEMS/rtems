/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <rtems/libio.h>
#include <rtems/userenv.h>
#include <rtems/malloc.h>
#include <pmacros.h>
#include <rtems/libcsupport.h>

/* forward declarations to avoid warnings */
int test_main(void);

static void touch( char *file )
{
  int fd;

  rtems_test_assert( file );

  fd = open( file, O_RDWR|O_CREAT, 0777 );
  rtems_test_assert( fd != -1 );
  close( fd );
}

static int fileexists( char *file )
{
  int         status;
  struct stat statbuf;

  rtems_test_assert( file );

  status = stat( file, &statbuf );

  if ( status == -1 ) {
    /* printf( ": %s\n", strerror( errno ) ); */
    return 0;
  }
  return 1;
}

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int argc,
  char **argv
)
#endif
{
  static const uintptr_t global_location_size [] = {
    sizeof(rtems_filesystem_global_location_t)
  };

  int status;
  void *opaque;
/*
 *  This test is the C equivalent of this sequence.
#mkdir /one
#mkdir /one/one
#touch /one/one.test
#touch /one/two/two.test
# an error case to ENOTSUP from chroot
# chroot
#chroot /one
#if !fileexists(/one/one.test) echo "SUCCESSFUL"
#if fileexists(/two/two.test) echo "SUCCESSFUL"
#rtems_set_private_env() ! reset at the global environment
#if fileexists(/one/one.test) echo "SUCESSFUL"
#if !fileexists(/two/two.test) echo "SUCCESSFUL"
*/

  printf( "\n\n*** CHROOT01 TEST ***\n" );

  status = mkdir( "/one", 0777);
  rtems_test_assert( status == 0 );

  status = mkdir( "/one/one", 0777);
  rtems_test_assert( status == 0 );

  status = mkdir( "/one/two", 0777);
  rtems_test_assert( status == 0 );

  touch( "/one/one.test" );
  touch( "/one/two/two.test" );

  puts( "chroot with bad path - expect ENOENT" );
  status = chroot( "/three" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOENT );

  puts( "chroot with file - expect ENOTDIR" );
  status = chroot( "/one/one.test" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOTDIR );

  puts( "allocate most of memory - attempt to fail chroot - expect ENOMEM" );
  opaque = rtems_heap_greedy_allocate( global_location_size, 1 );

  status = chroot( "/one" );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  puts( "freeing the allocated memory" );
  rtems_heap_greedy_free( opaque );

  status = chroot( "/one" );
  rtems_test_assert( status == 0 );

  status = fileexists( "/one/one.test" );
  printf( "%s on /one/one.test\n", (!status) ? "SUCCESS" : "FAILURE" );

  status = fileexists( "/two/two.test" );
  printf( "%s on /two/two.test\n", (status) ? "SUCCESS" : "FAILURE" );

  puts( "Go back to global environment" );
  rtems_libio_use_global_env();

  status = fileexists( "/one/one.test" );
  printf( "%s on /one/one.test\n", ( status) ? "SUCCESS" : "FAILURE" );

  status = fileexists( "/two/two.test" );
  printf( "%s on /two/two.test\n", (!status) ? "SUCCESS" : "FAILURE" );

  printf( "*** END OF CHROOT01 TEST ***\n" );
  rtems_test_exit(0);
}
