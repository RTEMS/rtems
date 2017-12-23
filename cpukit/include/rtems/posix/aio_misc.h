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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _AIO_MISC_H
#define _AIO_MISC_H

#include <stdio.h>
#include <string.h>
#include <aio.h>
#include <pthread.h>
#include <rtems.h>
#include <rtems/chain.h>
#include <rtems/system.h>
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
