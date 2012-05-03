/*
 * Copyright 2010, Alin Rus <alin.codejunkie@gmail.com> 
 * 
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <aio.h>
#include <rtems/posix/aio_misc.h>
#include <errno.h>
#include <stdlib.h>
#include <rtems/system.h>
#include <rtems/seterr.h>

/* 
 *  aio_cancel
 *
 * Cancel an asynchronous I/O request
 *
 *  Input parameters:
 *        fildes - file descriptor
 *        aiocbp - asynchronous I/O control block
 *
 *  Output parameters: 
 *        AIO_CANCELED    - if the requested operation(s)
 *                          were canceled
 *        AIO_NOTCANCELED - if at least one of the requested
 *                          operation(s) cannot be canceled
 */


int aio_cancel(int fildes, struct aiocb  *aiocbp)
{
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
         
    r_chain = rtems_aio_search_fd (&aio_request_queue.work_req, fildes, 0);
    if (r_chain == NULL) {
      AIO_printf ("Request chain not on [WQ]\n");

      if (!rtems_chain_is_empty (&aio_request_queue.idle_req)) {
        r_chain = rtems_aio_search_fd (&aio_request_queue.idle_req, fildes, 0);
        if (r_chain == NULL) {
          pthread_mutex_unlock(&aio_request_queue.mutex);
          return AIO_ALLDONE;
        }

        AIO_printf ("Request chain on [IQ]\n");

        rtems_chain_extract (&r_chain->next_fd);        
        rtems_aio_remove_fd (r_chain);
        pthread_mutex_destroy (&r_chain->mutex);
        pthread_cond_destroy (&r_chain->mutex);
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
      
    r_chain = rtems_aio_search_fd (&aio_request_queue.work_req, fildes, 0);
    if (r_chain == NULL) {
      if (!rtems_chain_is_empty (&aio_request_queue.idle_req)) {
        r_chain = rtems_aio_search_fd (&aio_request_queue.idle_req, fildes, 0);
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
