/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief POSIX Asynchronous Input and Output Private Support
 *
 * This defines private information for the AIO implementation.
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

#ifndef _AIO_MISC_H
#define _AIO_MISC_H

#include <stdio.h>
#include <string.h>
#include <aio.h>
#include <pthread.h>
#include <rtems.h>
#include <rtems/chain.h>
#include <rtems/seterr.h>
  
#ifdef __cplusplus
extern "C"
{
#endif

  /* Actual request being processed */
  typedef struct
  {
    rtems_chain_node next_prio; /* chain requests in order of priority */
    int policy;                 /* If _POSIX_PRIORITIZED_IO and 
		                   _POSIX_PRIORITY_SCHEDULING are defined */ 
    int priority;               /* see above */
    pthread_t caller_thread;    /* used for notification */
    struct aiocb *aiocbp;       /* aio control block */
  } rtems_aio_request;

  typedef struct
  {
    rtems_chain_node next_fd;   /* order fd chains in queue */
    rtems_chain_control perfd;  /* chain of requests for this fd */
    int fildes;                 /* file descriptor to be processed */
    int new_fd;                 /* if this is a newly created chain */
    pthread_mutex_t mutex;      
    pthread_cond_t cond;

  } rtems_aio_request_chain;

  typedef struct
  {
    pthread_mutex_t mutex;
    pthread_cond_t new_req;
    pthread_attr_t attr;

    rtems_chain_control work_req; /* chains being worked by active threads */
    rtems_chain_control idle_req; /* fd chains waiting to be processed */
    unsigned int initialized;     /* specific value if queue is initialized */
    int active_threads;           /* the number of active threads */
    int idle_threads;             /* number of idle threads */

  } rtems_aio_queue;

extern rtems_aio_queue aio_request_queue;

#define AIO_QUEUE_INITIALIZED 0xB00B

#ifndef AIO_MAX_THREADS
#define AIO_MAX_THREADS 5
#endif

#ifndef AIO_MAX_QUEUE_SIZE
#define AIO_MAX_QUEUE_SIZE 30
#endif

int rtems_aio_init (void);
int rtems_aio_enqueue (rtems_aio_request *req);
rtems_aio_request_chain *rtems_aio_search_fd 
(
  rtems_chain_control *chain,
  int fildes,
  int create
);
void rtems_aio_remove_fd (rtems_aio_request_chain *r_chain);
int rtems_aio_remove_req (rtems_chain_control *chain,
				 struct aiocb *aiocbp);

#ifdef RTEMS_DEBUG
#include <assert.h>

#define AIO_assert(_x) assert(_x)
#define AIO_printf(_x) printf(_x)
#else
#define AIO_assert(_x)
#define AIO_printf(_x)
#endif

#define rtems_aio_set_errno_return_minus_one( _error, _aiocbp ) \
  do { (_aiocbp)->error_code = (_error);			\
    (_aiocbp)->return_value = -1;				\
    rtems_set_errno_and_return_minus_one (_error);} while(0)

#ifdef __cplusplus
}
#endif

#endif
