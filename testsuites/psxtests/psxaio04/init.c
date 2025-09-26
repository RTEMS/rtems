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
#include <signal.h>

const char rtems_test_name[] = "PSXAIO 4";

#define BUFSIZE 512
#define WRONG_FD 404

/* forward declarations to avoid warnings */
static struct aiocb *create_aiocb( int fd );
static void free_aiocb( struct aiocb *aiocbp );
static void notify( union sigval sig );

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

static void notify( union sigval sig )
{
  kill( getpid(), sig.sival_int );
}

void *POSIX_Init( void *argument )
{
  (void) argument;

  int fd, status, received_signal, sig;
  struct aiocb *aiocbp;
  sigset_t sig_set;
  sig = SIGUSR1;

  rtems_aio_init();

  status = mkdir( "/tmp", S_IRWXU );
  rtems_test_assert( !status );
  
  fd = open(
    "/tmp/aio_fildes",
    O_RDWR|O_CREAT,
    S_IRWXU|S_IRWXG|S_IRWXO
  );
  rtems_test_assert( fd != -1 );

  TEST_BEGIN();

  aiocbp = create_aiocb( fd );
  aiocbp->aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  aiocbp->aio_sigevent.sigev_signo = SIGUSR1;

  status =sigemptyset( &sig_set );
  rtems_test_assert( status == 0 );

  status =sigaddset( &sig_set, sig );
  rtems_test_assert( status == 0 );

  status = sigprocmask( SIG_BLOCK, &sig_set, NULL );
  rtems_test_assert( status == 0 );

  status = aio_write( aiocbp );
  rtems_test_assert( status == 0 );

  status = sigwait( &sig_set, &received_signal );
  rtems_test_assert( status == 0 );

  rtems_test_assert( received_signal == sig );

  aiocbp = create_aiocb( fd );
  aiocbp->aio_sigevent.sigev_notify = SIGEV_THREAD;
  aiocbp->aio_sigevent.sigev_notify_attributes = NULL;
  aiocbp->aio_sigevent.sigev_notify_function = &notify;
  aiocbp->aio_sigevent.sigev_value.sival_int = sig;

  status =sigemptyset( &sig_set );
  rtems_test_assert( status == 0 );

  status =sigaddset( &sig_set, sig );
  rtems_test_assert( status == 0 );

  status = sigprocmask( SIG_BLOCK, &sig_set, NULL );
  rtems_test_assert( status == 0 );

  status = aio_write( aiocbp );
  rtems_test_assert( status == 0 );

  status = sigwait( &sig_set, &received_signal );
  rtems_test_assert( status == 0 );

  rtems_test_assert( received_signal == sig );

  TEST_END();

  free_aiocb( aiocbp );
  close( fd );
  rtems_test_exit( 0 );

  return NULL;
}
