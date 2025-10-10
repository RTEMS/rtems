/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Function Initiates a List of I/O Requests with Single Function Call 
 */

/*
 *  6.7.4 List Directed I/O, P1003.1b-1993, p. 158
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
#include <fcntl.h>
#include <rtems/posix/aio_misc.h>
#include <rtems/seterr.h>
#include <stdlib.h>
#include <pthread.h>
#include <rtems/score/basedefs.h>
#include <rtems/seterr.h>

int lio_listio(
  int              mode,
  struct aiocb    *__restrict const  list[__restrict],
  int              nent,
  struct sigevent *__restrict sig
)
{
#ifdef RTEMS_POSIX_API 
  int result, error;
  listcb *listcbp;
  rtems_aio_request *req;

  /* Errors on parameters */
  if ( list == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( mode != LIO_WAIT && mode != LIO_NOWAIT ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( nent < 1 || nent > AIO_LISTIO_MAX ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( nent + atomic_load( &aio_request_queue.queued_requests ) > RTEMS_AIO_MAX ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /* Initialize list control block */
  listcbp = malloc( sizeof( listcb ) );
  if ( listcbp == NULL ) {
    rtems_set_errno_and_return_minus_one( EAGAIN );
  }

  result = pthread_mutex_init( &listcbp->mutex, NULL );
  if ( result != 0 ) {
    free( listcbp );
    rtems_set_errno_and_return_minus_one( EAGAIN );
  }

  listcbp->requests_left = 1;

  /* Set up notification for list completion (wait or no wait) */
  if ( mode == LIO_WAIT ) {
    listcbp->notification_type = AIO_LIO_EVENT;
    listcbp->lio_notification.task_id = rtems_task_self();

  } else if ( mode == LIO_NOWAIT ) {

    if ( sig == NULL ) {
      listcbp->notification_type = AIO_LIO_NO_NOTIFY;
    } else {
      if ( rtems_aio_check_sigevent( sig ) == 0 ) {
        free( listcbp );
        rtems_set_errno_and_return_minus_one( EINVAL );
      }

      listcbp->notification_type = AIO_LIO_SIGEV;
      listcbp->lio_notification.sigp = sig;
    }
  }
  
  /* Initialize and enqueue each request */
  error = 0;

  for( int i = 0; i < nent; i++ ) { 
    if ( list[i] == NULL ) {
      continue;
    }

    switch ( list[i]->aio_lio_opcode ) {
      case LIO_READ:
        req = init_read_req( list[i] );
        if ( req == NULL ) {
          error = 1;
          list[i]->return_value = -1;
          list[i]->error_code = errno;
          list[i]->return_status = AIO_NOTRETURNED;
          break;
        }

        req->listcbp = listcbp;
        result = rtems_aio_enqueue( req );
        if ( result != 0 ) {
          error = 1;
          list[i]->return_value = -1;
          list[i]->error_code = EAGAIN;
          list[i]->return_status = AIO_NOTRETURNED;
          break;
        }

        listcbp->requests_left++;
        break;

      case LIO_WRITE:
        req = init_write_req( list[i] );
        if ( req == NULL ) {
          error = 1;
          list[i]->error_code = errno;
          list[i]->return_value = -1;
          list[i]->return_status = AIO_NOTRETURNED;
          break;
        }
        
        req->listcbp = listcbp;
        result = rtems_aio_enqueue( req );
        if ( result != 0 ) {
          error = 1;
          list[i]->return_value = -1;
          list[i]->error_code = EAGAIN;
          list[i]->return_status = AIO_NOTRETURNED;
          break;
        }

        listcbp->requests_left++;
        break;
    }
  }
  rtems_aio_completed_list_op( listcbp );

  if ( mode == LIO_NOWAIT ) {
    if ( error == 1 ) {
      rtems_set_errno_and_return_minus_one( EIO ); 
    }
  } else if ( mode == LIO_WAIT ) {
    rtems_event_set event_out; 
    rtems_event_system_receive(
      RTEMS_EVENT_SYSTEM_LIO_LIST_COMPLETED,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT,
      &event_out
    );

    for ( int i = 0; i < nent; i++ ) { 
      if ( list[i] != NULL ) {
        if (
          list[i]->aio_lio_opcode == LIO_READ ||
          list[i]->aio_lio_opcode == LIO_WRITE 
        ) {
          switch ( list[i]->error_code ) {
            case 0:
              break;
            case EINPROGRESS:
              rtems_set_errno_and_return_minus_one( EINTR );
              break;
            default:
              rtems_set_errno_and_return_minus_one( EIO );
          }
        }
      }
    }
  }

  return 0;
#else
  (void) mode;
  (void) list;
  (void) nent;
  (void) sig;

  rtems_set_errno_and_return_minus_one( ENOSYS );
#endif
}

