/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @ingroup POSIX_AIO
 *
 * @brief POSIX Asynchronous I/O Private Support
 *
 * This defines private information for the AIO implementation.
 */

/*
 *  Copyright 2010, Alin Rus <alin.codejunkie@gmail.com>
 * 
 *  COPYRIGHT (c) 1989-2011.
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
extern "C" {
#endif

/**
 * @brief The request being processed
 */
typedef struct
{
  /** @brief Chain requests in order of priority */
  rtems_chain_node next_prio;

  /** @brief If _POSIX_PRIORITIZED_IO and _POSIX_PRIORITY_SCHEDULING are defined */ 
  int policy;

  /** @brief see above */
  int priority;

  /** @brief Used for notification */
  pthread_t caller_thread;

  /** @brief Aio control block */
  struct aiocb *aiocbp;
} rtems_aio_request;

/**
 * @brief A chain of requests for the same FD
 */
typedef struct
{
  /** @brief Order fd chains in queue */
  rtems_chain_node next_fd;

  /** @brief Chain of requests for this fd */
  rtems_chain_control perfd;

  /** @brief File descriptor to be processed */
  int fildes;

  /** @brief Indicates if this is a newly created chain */
  int new_fd;

  pthread_mutex_t mutex;
  pthread_cond_t cond;

} rtems_aio_request_chain;

/**
 * @brief The queue of all the requests in progress and waiting to be processed
 */
typedef struct
{
  pthread_mutex_t mutex;
  pthread_cond_t new_req;
  pthread_attr_t attr;

  /** @brief Chains being worked by active threads */
  rtems_chain_control work_req;

  /** @brief Chains waiting to be processed */
  rtems_chain_control idle_req;
  
  /** @brief Specific value if queue is initialized */
  unsigned int initialized;

  /** @brief The number of active threads */
  int active_threads;

  /** @brief The number of idle threads */
  int idle_threads;

} rtems_aio_queue;

extern rtems_aio_queue aio_request_queue;

#define AIO_QUEUE_INITIALIZED 0xB00B

#ifndef AIO_MAX_THREADS
#define AIO_MAX_THREADS 5
#endif

#ifndef AIO_MAX_QUEUE_SIZE
#define AIO_MAX_QUEUE_SIZE 30
#endif

/**
 * @brief Initialize the request queue for AIO Operations.
 * 
 * @retval 0 The queue has bees succesfully initialized.
 */
int rtems_aio_init( void );

/**
 * @brief Enqueue requests, and creates threads to process them.
 * 
 * @param[in,out] req A pointer to the request.
 * 
 * @retval 0 if the request was added to the queue, errno otherwise.
 */
int rtems_aio_enqueue( rtems_aio_request *req );

/**
 * @brief Search for and create a chain of requests for a given file descriptor.
 * 
 * @param[in,out] chain   A pointer to a chain of FD chains.
 * @param[in] fildes The file descriptor to search for.
 * @param[in] create  If create == 0, the function just searches for the given FD.
 *                    If create == 1, the function creates a new chain if none is found.
 * 
 * @retval NULL If create == 0 and no chain is found for the given FD.
 * @return A pointer to the chain if a chain for the given FD exists.
 * @return A pointer to a newly created chain if create == 1 and no chain
 *         is found for the given FD.
 */
rtems_aio_request_chain *rtems_aio_search_fd(
  rtems_chain_control *chain,
  int fildes,
  int create
);

/**
 * @brief Removes all the requests in a FD chain.
 * 
 * @param[in,out] r_chain A pointer to a chain of requests for a given FD
 */
void rtems_aio_remove_fd( rtems_aio_request_chain *r_chain );

/**
 * @brief Remove request from given chain
 * 
 * @param[in,out] chain  A pointer to the FD chain that may contain the request
 * @param[in,out] aiocbp A pointer to the AIO control block of the request.
 * 
 * @retval AIO_CANCELED The request was canceled.
 * @retval AIO_NOTCANCELED The request was not canceled.
 */
int rtems_aio_remove_req(
  rtems_chain_control *chain,
	struct aiocb *aiocbp
);

#ifdef RTEMS_DEBUG
#include <assert.h>

#define AIO_assert(_x) assert(_x)
#define AIO_printf(_x) printf(_x)
#else
#define AIO_assert(_x)
#define AIO_printf(_x)
#endif

#ifdef __cplusplus
}
#endif

#endif

