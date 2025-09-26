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
#include <rtems/chain.h>

const char rtems_test_name[] = "PSXAIO 2";

/* forward declarations to avoid warnings */
static struct aiocb *create_aiocb( int fd );
static void free_aiocb( struct aiocb *aiocbp );

#define BUFSIZE 32
#define FD_COUNT 10
#define WRONG_FD 666

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

  int fd[FD_COUNT];
  struct aiocb *aiocbp[FD_COUNT+1];
  int status, i, policy = SCHED_FIFO;
  char filename[BUFSIZE];
  struct sched_param param;

  status = rtems_aio_init();
  rtems_test_assert( status == 0 );

  param.sched_priority = 30;
  status = pthread_setschedparam( 
    pthread_self(),
    policy, &param 
  );
  rtems_test_assert( status == 0 );
 
  status = mkdir( "/tmp", S_IRWXU );
  rtems_test_assert( !status );
  
  TEST_BEGIN();

  for( i=0; i<FD_COUNT; i++ )
    {
      sprintf( filename, "/tmp/aio_fildes%d", i );
      fd[i] = open( 
        filename,
        O_RDWR|O_CREAT,
        S_IRWXU|S_IRWXG|S_IRWXO 
      );
      rtems_test_assert( fd[i] != -1 );
    }
  
  aiocbp[0] = create_aiocb( fd[0] );
  status = aio_write( aiocbp[0] );
  rtems_test_assert( status != -1 );

  aiocbp[1] = create_aiocb( fd[1] );
  status = aio_write( aiocbp[1] );
  rtems_test_assert( status != -1 );
  
  aiocbp[2] = create_aiocb( fd[1] );
  status = aio_read( aiocbp[2] );
  rtems_test_assert( status != -1 );
  
  aiocbp[3] = create_aiocb( fd[2] );
  status = aio_write( aiocbp[3] );
  rtems_test_assert( status != -1 );
  
  aiocbp[4] = create_aiocb( fd[3] );
  status = aio_write( aiocbp[4] );
  rtems_test_assert( status != -1 );
  
  aiocbp[5] = create_aiocb( fd[4] );
  status = aio_write( aiocbp[5] );
  rtems_test_assert( status != -1 );
  
  aiocbp[6] = create_aiocb( fd[5] );
  status = aio_write( aiocbp[6] );
  rtems_test_assert( status != -1 );
  
  aiocbp[7] = create_aiocb( fd[6] );
  status = aio_write( aiocbp[7] );
  rtems_test_assert( status != -1 );

  aiocbp[8] = create_aiocb( fd[6] ); 
  status = aio_read( aiocbp[8] );
  rtems_test_assert( status != -1 );

  aiocbp[9] = create_aiocb( fd[0] ); 
  status = aio_fsync( O_SYNC, aiocbp[9] );
  rtems_test_assert( status != -1 );

  aiocbp[9] = create_aiocb( fd[0] ); 
  status = aio_fsync( O_DSYNC, aiocbp[9] );
  rtems_test_assert( status != -1 );

  status = aio_cancel( WRONG_FD, NULL );
  rtems_test_assert( status == -1 );

  status = aio_cancel( fd[7], NULL );
  rtems_test_assert( status == AIO_ALLDONE );

  status = aio_cancel( fd[1], NULL );
  rtems_test_assert( status == AIO_CANCELED );

  status = aio_cancel( fd[6], NULL );
  rtems_test_assert( status == AIO_CANCELED );

  status = aio_cancel( fd[4], aiocbp[4] );
  rtems_test_assert( status == -1 );
 
  aiocbp[10] = create_aiocb( fd[9] );
  status = aio_cancel( fd[9], aiocbp[10] );
  rtems_test_assert( status == -1 );

  status = aio_cancel( fd[5], aiocbp[6] );
  rtems_test_assert( status == AIO_CANCELED );

  /* tests for aio error and aio test */

  int result = aio_error( aiocbp[6] );
  rtems_test_assert( result == ECANCELED );

  result = aio_return( aiocbp[6] );
  rtems_test_assert( result == -1 );

  result = aio_return( aiocbp[6] );
  rtems_test_assert( result == -1 );

  rtems_test_assert( errno == EINVAL );
  result = aio_error( aiocbp[6] );
  rtems_test_assert( result == -1 );
  rtems_test_assert( errno == EINVAL );

  status = aio_cancel( fd[0], aiocbp[9] );
  rtems_test_assert( status == AIO_CANCELED );

  status = aio_cancel( fd[5], aiocbp[6] );
  rtems_test_assert( status == AIO_ALLDONE );

  TEST_END();

  for ( i = 0; i < FD_COUNT; i++ )
    {
      close( fd[i] );
      free_aiocb( aiocbp[i] );
    }
  free_aiocb( aiocbp[i] );
  rtems_test_exit( 0 );

  return NULL;

}
