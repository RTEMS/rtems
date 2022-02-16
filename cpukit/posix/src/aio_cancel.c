/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Cancel Asynchronous I/O Operation
 *  @ingroup POSIX_AIO
 */

/*
 * Copyright 2010, Alin Rus <alin.codejunkie@gmail.com> 
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
#include <rtems/posix/aio_misc.h>
#include <errno.h>
#include <stdlib.h>
#include <rtems/seterr.h>

int aio_cancel(int fildes, struct aiocb  *aiocbp)
{
  rtems_chain_control *idle_req_chain = &aio_request_queue.idle_req;
  rtems_chain_control *work_req_chain = &aio_request_queue.work_req;
  rtems_aio_request_chain *r_chain;
  int result;
  
  pthread_mutex_lock (&aio_request_queue.mutex);

  if (fcntl (fildes, F_GETFD) < 0) {
    pthread_mutex_unlock(&aio_request_queue.mutex);
    rtems_set_errno_and_return_minus_one (EBADF);
  }

  /* if aiocbp is NULL remove all request for given file descriptor */
  if (aiocbp == NULL) {
    AIO_printf ("Cancel all requests\n");        
         
    r_chain = rtems_aio_search_fd (work_req_chain, fildes, 0);
    if (r_chain == NULL) {
      AIO_printf ("Request chain not on [WQ]\n");

      if (!rtems_chain_is_empty (idle_req_chain)) {
        r_chain = rtems_aio_search_fd (idle_req_chain, fildes, 0);
        if (r_chain == NULL) {
          pthread_mutex_unlock(&aio_request_queue.mutex);
          return AIO_ALLDONE;
        }

        AIO_printf ("Request chain on [IQ]\n");

        rtems_chain_extract (&r_chain->next_fd);
        rtems_aio_remove_fd (r_chain);
        pthread_mutex_destroy (&r_chain->mutex);
        pthread_cond_destroy (&r_chain->cond);
        free (r_chain);

        pthread_mutex_unlock (&aio_request_queue.mutex);
        return AIO_CANCELED;
      }

      pthread_mutex_unlock (&aio_request_queue.mutex);
      return AIO_ALLDONE;
    }

    AIO_printf ("Request chain on [WQ]\n");

    pthread_mutex_lock (&r_chain->mutex);
    rtems_chain_extract (&r_chain->next_fd);
    rtems_aio_remove_fd (r_chain);
    pthread_mutex_unlock (&r_chain->mutex);
    pthread_mutex_unlock (&aio_request_queue.mutex);
    return AIO_CANCELED;
  } else {
    AIO_printf ("Cancel request\n");

    if (aiocbp->aio_fildes != fildes) {
      pthread_mutex_unlock (&aio_request_queue.mutex);
      rtems_set_errno_and_return_minus_one (EINVAL);
    }
      
    r_chain = rtems_aio_search_fd (work_req_chain, fildes, 0);
    if (r_chain == NULL) {
      if (!rtems_chain_is_empty (idle_req_chain)) {
        r_chain = rtems_aio_search_fd (idle_req_chain, fildes, 0);
        if (r_chain == NULL) { 
          pthread_mutex_unlock (&aio_request_queue.mutex);
          rtems_set_errno_and_return_minus_one (EINVAL);
        }      
           
        AIO_printf ("Request on [IQ]\n");                     
   
        result = rtems_aio_remove_req (&r_chain->perfd, aiocbp);
        pthread_mutex_unlock (&aio_request_queue.mutex);
        return result;
      } else {
        pthread_mutex_unlock (&aio_request_queue.mutex);
        return AIO_ALLDONE;
      }
    }  
      AIO_printf ("Request on [WQ]\n");
      
      pthread_mutex_lock (&r_chain->mutex);
      result = rtems_aio_remove_req (&r_chain->perfd, aiocbp);
      pthread_mutex_unlock (&r_chain->mutex);
      pthread_mutex_unlock (&aio_request_queue.mutex);
      return result;
  }
  return AIO_ALLDONE;
}
