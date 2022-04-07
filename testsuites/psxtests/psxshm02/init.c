/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 Gedare Bloom.
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

const char rtems_test_name[] = "PSX SHM02";

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

  if( p != MAP_FAILED ) {
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
