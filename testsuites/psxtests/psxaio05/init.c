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

#include <stdio.h>
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
#include <signal.h>

const char rtems_test_name[] = "PSXAIO 5";

#define BUFSIZE 512
#define WRONG_FD 404
#define BAD_MODE 38576

/* forward declarations to avoid warnings */
static struct aiocb *create_aiocb( int fd );
static void free_aiocb( struct aiocb *aiocbp );
static void test_einval( struct aiocb** aiocbp );
static void test_eio( struct aiocb** aiocbp );
static void test_no_wait( struct aiocb** aiocbp );
static void test_wait( struct aiocb** aiocbp );

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

static void test_einval( struct aiocb** aiocbp )
{
  int status;

  status = lio_listio( LIO_WAIT, NULL, 1, NULL);
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );
  errno = 0;

  status = lio_listio( BAD_MODE, aiocbp, 1, NULL);
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );
  errno = 0;

  status = lio_listio( LIO_WAIT, aiocbp, -9, NULL);
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );
  errno = 0;

  status = lio_listio( LIO_WAIT, aiocbp, 547, NULL);
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );
  errno = 0;
}

static void test_eio( struct aiocb** aiocbp )
{
  int status;

  status = lio_listio( LIO_NOWAIT, &aiocbp[4], 1, NULL);
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EIO );
  errno = 0;

  status = lio_listio( LIO_WAIT, &aiocbp[4], 1, NULL);
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EIO );
  errno = 0;
}

static void test_no_wait( struct aiocb** aiocbp )
{
  int status, received_signal, sig;
  struct sigevent sigev;
  sigset_t sig_set;
  
  sig = SIGUSR1;
  sigev.sigev_notify = SIGEV_SIGNAL;
  sigev.sigev_signo = sig;
  sigev.sigev_value.sival_ptr  = NULL;

  status = sigemptyset( &sig_set );
  rtems_test_assert( status == 0 );

  status = sigaddset( &sig_set, sig );
  rtems_test_assert( status == 0 );

  status = sigprocmask( SIG_BLOCK, &sig_set, NULL );
  rtems_test_assert( status == 0 );

  status = lio_listio( LIO_NOWAIT, aiocbp, 4, &sigev);
  rtems_test_assert( status == 0 );

  status = sigwait( &sig_set, &received_signal );
  rtems_test_assert( status == 0 );
  rtems_test_assert( received_signal == sig );

  status = sigprocmask( SIG_UNBLOCK, &sig_set, NULL );
  rtems_test_assert( status == 0 );
}

static void test_wait( struct aiocb** aiocbp )
{
  int status;

  status = lio_listio( LIO_WAIT, aiocbp, 4, NULL);
  rtems_test_assert( status == 0 );
}

void *POSIX_Init( void *argument )
{
  (void) argument;

  struct aiocb *aiocbp[] = { NULL, NULL, NULL, NULL, NULL };
  int fd, result;

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

  for (int i = 0; i<4; i++ ) {
    aiocbp[i] = create_aiocb( fd );
  }
  aiocbp[4] = create_aiocb( WRONG_FD );

  test_einval( aiocbp );

  test_eio( aiocbp );

  test_no_wait( aiocbp );

  test_wait( aiocbp );

  TEST_END();

  for (int i = 0; i<5; i++ ) {
    free_aiocb(aiocbp[i]);
  }

  close( fd );
  rtems_test_exit( 0 );

  return NULL;
}
