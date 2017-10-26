/*
 * Copyright (c) 2016 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/*
 * From http://pubs.opengroup.org/onlinepubs/9699919799/functions/shm_open.html
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include "pritime.h"

#include <sys/mman.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>

const char rtems_test_name[] = "PSX SHM01";

#define MAX_LEN 10000
struct region {        /* Defines "structure" of shared memory */
  int len;
  char buf[MAX_LEN];
};

void *POSIX_Init(
  void *argument
)
{
  struct region *p;
  int fd;
  int err;
  char *name = "/shm";

  TEST_BEGIN();

  puts( "Init: shm_open" );
  fd = shm_open( name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR );
  if ( fd == -1 ) {
    printf ( "Error: %s\n", strerror(errno) );
    rtems_test_assert( fd != -1 );
  }

  puts( "Init: ftruncate" );
  err = ftruncate( fd, sizeof( struct region ) );
  if ( err == -1 ) {
    printf ( "Error: %s\n", strerror(errno) );
    rtems_test_assert( err != -1 );
  }

  puts( "Init: mmap" );
  p = mmap(
    NULL,
    sizeof( struct region ),
    PROT_READ | PROT_WRITE, MAP_SHARED,
    fd,
    0
  );

  if ( p != MAP_FAILED ) {
    puts( "Init: write to mapped region" );
    p->len = MAX_LEN;

    puts( "Init: munmap" );
    err = munmap( p, sizeof( struct region ) );
    if ( err == -1 ) {
      printf ( "Error: %s\n", strerror(errno) );
      rtems_test_assert( err != -1 );
    }
  } else {
    puts( "Init: FIXME: mmap() not supported" );
  }

  puts( "Init: close" );
  err = close( fd );
  if ( err == -1 ) {
    printf ( "Error: %s\n", strerror(errno) );
    rtems_test_assert( err != -1 );
  }

  TEST_END();

  rtems_test_exit(0);
  return 0;
}
