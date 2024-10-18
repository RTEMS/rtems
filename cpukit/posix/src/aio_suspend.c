/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIX_AIO
 *
 * @brief Wait for Asynchronous I/O Request.
 */

/*
 *  Copyright 2024, Alessandro Nardin <ale.daluch@gmail.com>
 * 
 *  COPYRIGHT (c) 1989-2007.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <aio.h>
#include <errno.h>
#include <pthread.h>
#include <rtems/posix/aio_misc.h>
#include <stdlib.h>

#include <rtems/score/basedefs.h>
#include <rtems/seterr.h>
#include <rtems/timespec.h>

int aio_suspend(
  const struct aiocb  * const list[],
  int                     nent,
  const struct timespec  *timeout
)
{
  rtems_chain_control *idle_req_chain = &aio_request_queue.idle_req;
  rtems_chain_control *work_req_chain = &aio_request_queue.work_req;
  rtems_aio_suspendcb *suspendcbp;
  rtems_aio_request_chain *r_chain;
  rtems_aio_request *request;
  rtems_event_set event_out;
  int result, op_num, i;

  /* Controls over invalid parameters */
  if ( list == NULL ){
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( nent <= 0 ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /* Initialize suspendcb */
  suspendcbp = malloc( sizeof( rtems_aio_suspendcb ) );
  if ( suspendcbp == NULL ) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  result = pthread_mutex_init( &suspendcbp->mutex, NULL );
  if ( result != 0 ) {
    free( suspendcbp );
    if ( errno == ENOMEM ) {
      rtems_set_errno_and_return_minus_one( ENOMEM );
    }
    rtems_set_errno_and_return_minus_one( EAGAIN );
  }

  pthread_mutex_lock( &suspendcbp->mutex );
  suspendcbp->requests_left = 0;
  suspendcbp->task_id = rtems_task_self();
  suspendcbp->notified = !AIO_SIGNALED;

  /* Lock request queue */
  pthread_mutex_lock( &aio_request_queue.mutex );

  /* Iterate over each request */
  for ( i = 0; i < nent; i++ ) {
    if ( list[i] == NULL ) {
      continue;
    }

    /* Search fd_chain in work_req_chain */
    r_chain = rtems_aio_search_fd( work_req_chain, list[i]->aio_fildes, 0 );
    
    /* If not found search in idle_req_chain */
    if ( r_chain == NULL ) {
      r_chain = rtems_aio_search_fd( idle_req_chain, list[i]->aio_fildes, 0 );
    }

    /* If not found continue */
    if ( r_chain == NULL ) {
      continue;
    }

    /* Search request in fd_chain */
    request = rtems_aio_search_in_chain( list[i], &r_chain->perfd );
      
    if ( request != NULL ) {
      if ( request->suspendcbp == NULL ) {
        request->suspendcbp = suspendcbp;
        suspendcbp->requests_left++;

      } else {
        pthread_mutex_lock( &request->suspendcbp->mutex );
        if ( request->suspendcbp->notified == AIO_SIGNALED ) {
          pthread_mutex_unlock( &request->suspendcbp->mutex );

          rtems_aio_update_suspendcbp( request->suspendcbp );
          request->suspendcbp = suspendcbp;
          suspendcbp->requests_left++;

        } else{
          pthread_mutex_unlock( &request->suspendcbp->mutex );
        }
      }
    }
    /* Request not present */
  }

  if ( suspendcbp->requests_left <= 0 ) {
    pthread_mutex_unlock( &aio_request_queue.mutex );
    pthread_mutex_unlock( &suspendcbp->mutex );
    pthread_mutex_destroy( &suspendcbp->mutex );
    free( suspendcbp );
    return 0;
  }
  op_num = suspendcbp->requests_left;

  /* Unlock request queue and suspendcb */
  pthread_mutex_unlock( &aio_request_queue.mutex );
  pthread_mutex_unlock( &suspendcbp->mutex );

  /* Wait */
  result = rtems_event_system_receive(
    RTEMS_EVENT_SYSTEM_AIO_SUSPENSION_TERMINATED,
    RTEMS_WAIT,
    ( timeout == NULL )? RTEMS_NO_TIMEOUT : rtems_timespec_to_ticks( timeout ),
    &event_out
  ); 

  /* Timeout control */
  if (result == RTEMS_TIMEOUT) {
    rtems_set_errno_and_return_minus_one( EAGAIN );
  }

  /* Control to ensure at least one operation completed */
  for ( i = 0; i < nent; i++) {
    if ( list[i] != NULL && list[i]->error_code == EINPROGRESS ) {
      op_num --;
    }
  }
  if ( op_num <= 0 ) {
    rtems_set_errno_and_return_minus_one( EINTR ); 
  }

  return 0;
}

