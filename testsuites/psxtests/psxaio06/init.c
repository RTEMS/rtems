/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright 2024, Alessandro Nardin <ale.daluch@gmail.com>
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

#define CONFIGURE_INIT
#include "system.h"
#include <rtems.h>
#include "tmacros.h"
#include <rtems/posix/aio_misc.h>
#include <aio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <fcntl.h>

const char rtems_test_name[] = "PSXAIO 6";

#define BUFSIZE 512

/* forward declarations to avoid warnings */
static struct aiocb *create_aiocb( int fd );
static void free_aiocb( struct aiocb *aiocbp );

static struct aiocb *create_aiocb( int fd )
{
  struct aiocb *aiocbp;

  aiocbp = malloc( sizeof( struct aiocb ) );
  memset( aiocbp, 0, sizeof( struct aiocb ) );
  aiocbp->aio_buf = malloc( BUFSIZE * sizeof( char ) );
  aiocbp->aio_nbytes = BUFSIZE;
  aiocbp->aio_offset = 0;
  aiocbp->aio_reqprio = 0;
  aiocbp->aio_fildes = fd;
  aiocbp->aio_sigevent.sigev_notify = SIGEV_NONE;
  aiocbp->aio_lio_opcode = LIO_READ;
  return aiocbp;
}

static void free_aiocb( struct aiocb *aiocbp )
{
  free( (void*) aiocbp->aio_buf );
  free( aiocbp );
}

void *POSIX_Init( void *argument )
{
  (void) argument;

  struct aiocb *aiocbp;
  struct timespec timespec;
  int fd, result;
  
  timespec.tv_sec = 0;
  timespec.tv_nsec = 1;

  rtems_aio_init();

  result = mkdir( "/tmp", S_IRWXU );
  rtems_test_assert( result == 0 );
  
  fd = open(
    "/tmp/aio_fildes",
    O_RDWR|O_CREAT,
    S_IRWXU|S_IRWXG|S_IRWXO
  );
  rtems_test_assert( fd != -1 );

  TEST_BEGIN();

  aiocbp = create_aiocb( fd );
  
  const struct aiocb *s_aiocbp = aiocbp;

  // EINVAL
  result = aio_suspend( NULL, 1, NULL );
  rtems_test_assert( result != 0 );
  rtems_test_assert( errno == EINVAL );  

  result = aio_suspend( &s_aiocbp, -1, NULL );
  rtems_test_assert( result != 0 );
  rtems_test_assert( errno == EINVAL ); 
  
  //NO ERROR
  result = aio_read( aiocbp );
  rtems_test_assert( result == 0 );

  result = aio_suspend( &s_aiocbp, 1, &timespec );
  rtems_test_assert( result == 0 );

  result = aio_error( s_aiocbp );
  rtems_test_assert( result != EINPROGRESS );

  TEST_END();

  free_aiocb( aiocbp );

  close( fd );
  rtems_test_exit( 0 );

  return NULL;
}
