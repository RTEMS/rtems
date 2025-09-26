/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright 2010, Alin Rus <alin.codejunkie@gmail.com>
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

const char rtems_test_name[] = "PSXAIO 1";

#define BUFSIZE 512
#define WRONG_FD 404

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

  int result, fd;
  struct aiocb *aiocbp;
  int status;

  rtems_aio_init();

  status = mkdir( "/tmp", S_IRWXU );
  rtems_test_assert( !status );
  
  fd = open( "/tmp/aio_fildes", O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO );
  rtems_test_assert( fd != -1 );

  TEST_BEGIN();

  /* NULL aiocbp */

  result = aio_write( NULL );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );

  /* NULL aiocbp */

  result = aio_read( NULL );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );

  /* NULL aiocbp */

  result = aio_fsync( O_SYNC, NULL );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );

  /* NULL aiocbp */

  result = aio_return( NULL );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == ENOENT );

  /* NULL aiocbp */

  result = aio_error( NULL );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );

  aiocbp = create_aiocb( WRONG_FD );

  /* Bad file descriptor */
  
  result = aio_write( aiocbp );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EBADF );

  /* Bad file descriptor */

  result = aio_read( aiocbp );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EBADF );

  /* Bad file descriptor */
  
  result = aio_cancel( WRONG_FD, NULL );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EBADF );
 
  /* Bad file descriptor */

  result = aio_fsync( O_SYNC, aiocbp );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EBADF );
  
  free_aiocb( aiocbp );

  aiocbp = create_aiocb( fd );
  aiocbp->aio_offset = -1;

  /* Invalid offset */

  result = aio_write( aiocbp );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );

  /* Invalid offset */

  result = aio_read( aiocbp );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );

  free_aiocb( aiocbp );

  aiocbp = create_aiocb( fd );
  aiocbp->aio_reqprio = AIO_PRIO_DELTA_MAX + 1;

  /* Invalid request priority */

  result = aio_write( aiocbp );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );

  /* Invalid request priority */

  result = aio_read( aiocbp );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );

  /* Invalid operation to aio_fsync */

  result = aio_fsync( -1, aiocbp );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );

  /* Invalid request aio_cancel */

  aiocbp->aio_fildes = WRONG_FD;
  result = aio_cancel( fd, aiocbp );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );
  aiocbp->aio_fildes = fd;
  
  /* Bad sigevent */

  aiocbp->aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  aiocbp->aio_sigevent.sigev_notify_function = NULL;
  result = aio_fsync( O_SYNC, aiocbp );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );

  /* Bad sigevent */

  aiocbp->aio_sigevent.sigev_notify = SIGEV_THREAD;
  aiocbp->aio_sigevent.sigev_signo = 0;
  result = aio_fsync( O_SYNC, aiocbp );
  status = errno;
  rtems_test_assert( result == -1 );
  rtems_test_assert( status == EINVAL );

  free_aiocb( aiocbp );

  TEST_END();

  close( fd );
  rtems_test_exit( 0 );

  return NULL;
}
