/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIX_AIO
 *
 * @brief Private implementation for Asynchronous I/O.
 *
 * This file contains the implementation of private methods used for the processing of Asynchronous I/O requests.
 */

/*
 *  Copyright 2010-2011, Alin Rus <alin.codejunkie@gmail.com>
 *  Copyright 2024, Alessandro Nardin <ale.daluch@gmail.com>
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

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h> 
#include <rtems/posix/aio_misc.h>
#include <rtems/score/assert.h>
#include <errno.h>
#include <limits.h>

/**
 * @brief Thread processing AIO requests.
 * 
 * @param[in,out] arg A pointer to the chain for the FD to be worked on.
 * 
 * @retval NULL if an error occurs
 */
static void *rtems_aio_handle( void *arg );

/**
 * @brief Helper function por request processing
 * 
 * @param[in,out] req A pointer to a single request.
 *                    It will store the results of the request.
 */
static void rtems_aio_handle_helper( rtems_aio_request *req );

/**
 * @brief Move chain of requests from IQ to WQ
 * 
 * @param[in,out] r_chain the chain of requests to move in WQ
 */
static void rtems_aio_move_to_work( rtems_aio_request_chain *r_chain );

/**
 * @brief Add request to given FD chain.
 *
 * Inserts the request in a the fd chain, which is ordered by priority.
 * 
 * @param[in,out] chain A pointer to the chain of requests for a given FD.
 * @param[in,out] req   A pointer to a request (see aio_misc.h).
 */
static void rtems_aio_insert_prio(
  rtems_chain_control *chain,
  rtems_aio_request *req 
);

/**
 * @brief Wrapper for pthread_create() call.
 *
 * This function serves as a wrapper with the appropriate signature for a call 
 * to pthread_create(). It receives a pointer to a sigevent structure that 
 * contains a pointer to the function to be called and the parameters to be 
 * passed to it.
 * 
 * @param args Pointer to the sigevent struct containing a pointer to the 
 *             function and its parameters.
 * @return void*
 */
static void *rtems_aio_notify_function_wrapper( void *args );

/**
 * @brief Generates a notification.
 *
 * The signal is generated using a sigevent struct, as defined from the 
 * POSIX specifications.
 * 
 * @param sigp is a pointer to the sigevent struct that will be used 
 *             to generate the signal
 */
static void rtems_aio_notify( struct sigevent *sigp );

rtems_aio_queue aio_request_queue;

int rtems_aio_init( void )
{
  int result = 0;

  result = pthread_attr_init( &aio_request_queue.attr );
  if ( result != 0 ){
    return -1;
  }

  result = pthread_attr_setdetachstate(
    &aio_request_queue.attr,
    PTHREAD_CREATE_DETACHED
  );
  if ( result != 0 ) {
    pthread_attr_destroy( &aio_request_queue.attr );
    return -1;
  }

  result = pthread_mutex_init( &aio_request_queue.mutex, NULL );
  if ( result != 0 ) {
    pthread_attr_destroy( &aio_request_queue.attr );
    return -1;
  }

  pthread_mutex_lock( &aio_request_queue.mutex );

  result = pthread_cond_init( &aio_request_queue.new_req, NULL );
  if ( result != 0 ) {
    pthread_mutex_unlock( &aio_request_queue.mutex );
    pthread_mutex_destroy( &aio_request_queue.mutex );
    pthread_attr_destroy( &aio_request_queue.attr );
    return -1;
  }

  rtems_chain_initialize_empty( &aio_request_queue.work_req );
  rtems_chain_initialize_empty( &aio_request_queue.idle_req );

  aio_request_queue.active_threads = 0;
  aio_request_queue.idle_threads = 0;
  atomic_init( &aio_request_queue.queued_requests, 0 );
  aio_request_queue.initialized = AIO_QUEUE_INITIALIZED;

  pthread_mutex_unlock( &aio_request_queue.mutex );

  return 0;
}

rtems_aio_request *init_write_req( struct aiocb* aiocbp )
{
  rtems_aio_request *req;
  int mode;

  if ( aiocbp == NULL ) {
    errno = EINVAL;
    return NULL;
  }
  
  mode = fcntl( aiocbp->aio_fildes, F_GETFL );
  if (
    ( mode&O_ACCMODE ) != O_WRONLY &&
    ( mode&O_ACCMODE ) != O_RDWR 
  ) {
    errno = EBADF;
    return NULL;
  }

  if ( aiocbp->aio_reqprio < 0 || aiocbp->aio_reqprio > AIO_PRIO_DELTA_MAX ) {
    errno = EINVAL;
    return NULL;
  }

  if ( aiocbp->aio_offset < 0 ) {
    errno = EINVAL;
    return NULL;
  }

  if ( rtems_aio_check_sigevent( &aiocbp->aio_sigevent ) == 0 ) {
    errno = EINVAL;
    return NULL;
  }

  req = malloc( sizeof( rtems_aio_request ) );
  if ( req == NULL ) {
    errno = EAGAIN;
    return NULL;
  }

  req->aiocbp = aiocbp;
  req->op_type = AIO_OP_WRITE;
  req->listcbp = NULL;

  return req;
}

rtems_aio_request *init_read_req( struct aiocb* aiocbp )
{
  rtems_aio_request *req;
  int mode;

  if ( aiocbp == NULL ) {
    errno = EINVAL;
    return NULL;
  }

  mode = fcntl( aiocbp->aio_fildes, F_GETFL );
  if (
      ( mode&O_ACCMODE ) != O_RDONLY &&
      ( mode&O_ACCMODE ) != O_RDWR
  ) {
    errno = EBADF;
    return NULL;
  }

  if ( aiocbp->aio_reqprio < 0 || aiocbp->aio_reqprio > AIO_PRIO_DELTA_MAX ) {
    errno = EINVAL;
    return NULL;
  }

  if ( aiocbp->aio_offset < 0 ) {
    errno = EINVAL;
    return NULL;
  }

  if ( rtems_aio_check_sigevent( &aiocbp->aio_sigevent ) == 0 ) {
    errno = EINVAL;
    return NULL;
  }

  req = malloc( sizeof( rtems_aio_request ) );
  if ( req == NULL ) {
    errno = EAGAIN;
    return NULL;
  }

  req->aiocbp = aiocbp;
  req->op_type = AIO_OP_READ;
  req->listcbp = NULL;

  return req;
}

void rtems_aio_completed_list_op( listcb *listcbp )
{
  if (listcbp == NULL)
    return;

  pthread_mutex_lock( &listcbp->mutex );

  if( --listcbp->requests_left == 0 ){
    switch ( listcbp->notification_type ) {
      case AIO_LIO_NO_NOTIFY:
        break;
      case AIO_LIO_SIGEV:
        rtems_aio_notify( listcbp->lio_notification.sigp );
        break;
      case AIO_LIO_EVENT:
        rtems_event_system_send(
          listcbp->lio_notification.task_id,
          RTEMS_EVENT_SYSTEM_LIO_LIST_COMPLETED
        );
        break;
    }
    pthread_mutex_unlock( &listcbp->mutex );
    pthread_mutex_destroy( &listcbp->mutex );
    free( listcbp );
  } else {
    pthread_mutex_unlock( &listcbp->mutex );
  }
}

void rtems_aio_update_suspendcbp( rtems_aio_suspendcb *suspendcbp )
{
  
  int send_event = 0;
  int finished = 0;
  rtems_id id;
  if ( suspendcbp == NULL )
    return;

  pthread_mutex_lock( &suspendcbp->mutex );
  
  id = suspendcbp->task_id;
  
  if ( --suspendcbp->requests_left == 0 ) {
    finished = 1;
  }

  if ( suspendcbp->notified == !AIO_SIGNALED ) {
    send_event = 1;
    suspendcbp->notified = AIO_SIGNALED;
  }
  
  pthread_mutex_unlock( &suspendcbp->mutex );

  if ( send_event ) {
    rtems_event_system_send(
      id,
      RTEMS_EVENT_SYSTEM_AIO_SUSPENSION_TERMINATED
    );
  }

  if( finished ) {
    pthread_mutex_destroy( &suspendcbp->mutex );
    free( suspendcbp );
  }
}

rtems_aio_request_chain *rtems_aio_search_fd(
  rtems_chain_control *chain,
  int fildes,
  int create
)
{
  rtems_aio_request_chain *r_chain;
  rtems_chain_node *node;

  node = rtems_chain_first( chain );
  r_chain = (rtems_aio_request_chain *) node;

  while ( r_chain->fildes < fildes && !rtems_chain_is_tail( chain, node ) ) {
    node = rtems_chain_next( node );
    r_chain = (rtems_aio_request_chain *) node;
  }

  if ( r_chain->fildes == fildes ) {
    r_chain->new_fd = 0;
  } else {
    if ( create == 0 ) {
      r_chain = NULL;
    } else {
      r_chain = calloc( 1, sizeof( rtems_aio_request_chain ) );
      rtems_chain_initialize_empty( &r_chain->perfd );
      rtems_chain_initialize_node( &r_chain->next_fd );

      if ( rtems_chain_is_empty( chain ) )
        rtems_chain_prepend( chain, &r_chain->next_fd );
      else
        rtems_chain_insert( rtems_chain_previous( node ), &r_chain->next_fd );

      r_chain->new_fd = 1;
      r_chain->fildes = fildes;
    }
  }
  return r_chain;
}

static void rtems_aio_move_to_work( rtems_aio_request_chain *r_chain )
{
  rtems_chain_control *work_req_chain = &aio_request_queue.work_req;
  rtems_aio_request_chain *temp;
  rtems_chain_node *node;

  node = rtems_chain_first( work_req_chain );
  temp = (rtems_aio_request_chain *) node;

  while (
    temp->fildes < r_chain->fildes &&
    !rtems_chain_is_tail( work_req_chain, node )
  ) {
    node = rtems_chain_next( node );
    temp = (rtems_aio_request_chain *) node;
  }

  rtems_chain_insert( rtems_chain_previous( node ), &r_chain->next_fd );
}

static void rtems_aio_insert_prio(
  rtems_chain_control *chain,
  rtems_aio_request *req
)
{
  rtems_chain_node *node;

  AIO_printf( "FD exists \n" );
  node = rtems_chain_first( chain );

  if ( rtems_chain_is_empty( chain ) ) {
    AIO_printf( "First in chain \n" );
    rtems_chain_prepend( chain, &req->next_prio );
  } else {

    if ( req->op_type == AIO_OP_SYNC ) {
      AIO_printf( "Sync request. Append to end of chain \n" );
      rtems_chain_append( chain, &req->next_prio );
    } else {
      AIO_printf( "Add by priority \n" );
      int prio = ((rtems_aio_request *) node)->aiocbp->aio_reqprio;

      while (
        req->aiocbp->aio_reqprio > prio &&
        !rtems_chain_is_tail( chain, node )
      ) {
        node = rtems_chain_next( node );
        prio = ((rtems_aio_request *) node)->aiocbp->aio_reqprio;
      }

      rtems_chain_insert( node->previous, &req->next_prio );
    }
  }
}

void rtems_aio_remove_fd( rtems_aio_request_chain *r_chain )
{
  rtems_chain_control *chain;
  rtems_chain_node *node;
  chain = &r_chain->perfd;
  node = rtems_chain_first( chain );
  
  while ( !rtems_chain_is_tail( chain, node ) ) {
    rtems_aio_request *req = (rtems_aio_request *) node;
    node = rtems_chain_next( node );
    rtems_chain_extract( &req->next_prio );
    req->aiocbp->error_code = ECANCELED;
    req->aiocbp->return_value = -1;
    atomic_fetch_sub( &aio_request_queue.queued_requests, 1 );
    rtems_aio_completed_list_op( req->listcbp );
    free( req );
  }
}

int rtems_aio_remove_req( rtems_chain_control *chain, struct aiocb *aiocbp )
{
  if ( rtems_chain_is_empty( chain ) )
    return AIO_ALLDONE;
  
  rtems_aio_request *request = rtems_aio_search_in_chain( aiocbp, chain );
  if ( request == NULL ) {
    return AIO_NOTCANCELED;
  }
  
  rtems_chain_node *node = (rtems_chain_node *) request;
  rtems_chain_extract( node );

  request->aiocbp->error_code = ECANCELED;
  request->aiocbp->return_value = -1;
  rtems_aio_completed_list_op( request->listcbp );
  rtems_aio_update_suspendcbp( request->suspendcbp );
  atomic_fetch_sub( &aio_request_queue.queued_requests, 1 );
  free( request );

  return AIO_CANCELED;
}

int rtems_aio_enqueue( rtems_aio_request *req )
{
  rtems_aio_request_chain *r_chain;
  rtems_chain_control *chain;
  pthread_t thid;
  int result, policy;
  struct sched_param param;

  /* The queue should be initialized */
  AIO_assert( aio_request_queue.initialized == AIO_QUEUE_INITIALIZED );

  result = pthread_mutex_lock( &aio_request_queue.mutex );
  if ( result != 0 ) {
    free( req );
    return result;
  }

  /* _POSIX_PRIORITIZED_IO and _POSIX_PRIORITY_SCHEDULING are defined,
     we can use aio_reqprio to lower the priority of the request */
  pthread_getschedparam( pthread_self(), &policy, &param );

  rtems_chain_initialize_node( &req->next_prio );
  req->caller_thread = pthread_self();
  req->priority = param.sched_priority - req->aiocbp->aio_reqprio;
  req->policy = policy;
  req->aiocbp->error_code = EINPROGRESS;
  req->aiocbp->return_value = 0;
  req->aiocbp->return_status = AIO_NOTRETURNED;
  atomic_fetch_add( &aio_request_queue.queued_requests, 1 );

  if (
    aio_request_queue.idle_threads == 0 &&
    aio_request_queue.active_threads < AIO_MAX_THREADS
  ) {
    /* we still have empty places on the active_threads chain */
    chain = &aio_request_queue.work_req;
    r_chain = rtems_aio_search_fd( chain, req->aiocbp->aio_fildes, 1 );

    if ( r_chain->new_fd == 1 ) {
      rtems_chain_prepend( &r_chain->perfd, &req->next_prio );
      r_chain->new_fd = 0;
      pthread_mutex_init( &r_chain->mutex, NULL );
      pthread_cond_init( &r_chain->cond, NULL );

      AIO_printf( "New thread \n" );
      result = pthread_create(
        &thid,
        &aio_request_queue.attr,
        rtems_aio_handle,
        (void *) r_chain
      );
      if ( result != 0 ) {
        pthread_mutex_unlock( &aio_request_queue.mutex );
        return result;
      }
      ++aio_request_queue.active_threads;
    } else {
      /* put request in the fd chain it belongs to */
      pthread_mutex_lock( &r_chain->mutex );
      rtems_aio_insert_prio( &r_chain->perfd, req );
      pthread_cond_signal( &r_chain->cond );
      pthread_mutex_unlock( &r_chain->mutex );
    }
  } else {
    /* The maximum number of threads has been already created
       even though some of them might be idle. The request
       belongs to one of the active fd chain */
    r_chain = rtems_aio_search_fd(
      &aio_request_queue.work_req,
      req->aiocbp->aio_fildes,
      0
    );
    if (r_chain != NULL) {
      pthread_mutex_lock( &r_chain->mutex );
      rtems_aio_insert_prio( &r_chain->perfd, req );
      pthread_cond_signal( &r_chain->cond );
      pthread_mutex_unlock( &r_chain->mutex );
    } else {

      /* or to the idle chain */
      chain = &aio_request_queue.idle_req;
      r_chain = rtems_aio_search_fd( chain, req->aiocbp->aio_fildes, 1 );

      if ( r_chain->new_fd == 1 ) {
        /* If this is a new fd chain we signal the idle threads that
          might be waiting for requests */
        AIO_printf( " New chain on waiting queue \n " );
        rtems_chain_prepend( &r_chain->perfd, &req->next_prio );
        r_chain->new_fd = 0;
        pthread_mutex_init( &r_chain->mutex, NULL );
        pthread_cond_init( &r_chain->cond, NULL );
      } else
        /* just insert the request in the existing fd chain */
        rtems_aio_insert_prio( &r_chain->perfd, req );

      if ( aio_request_queue.idle_threads > 0 )
        pthread_cond_signal( &aio_request_queue.new_req );
    }
  }

  pthread_mutex_unlock( &aio_request_queue.mutex );
  return 0;
}

int rtems_aio_check_sigevent( struct sigevent *sigp )
{
  _Assert( sigp != NULL );

  switch ( sigp->sigev_notify ) {
    case SIGEV_NONE:
      break;

    case SIGEV_SIGNAL:
      if ( sigp->sigev_signo < 1 || sigp->sigev_signo > 32 ) {
        return 0;
      }
      break;

    case SIGEV_THREAD:
      if ( sigp->sigev_notify_function == NULL ) {
        return 0;
      }
      break;

    default:
      return 0;
  }

  return 1;
}

static void *rtems_aio_notify_function_wrapper( void *args )
{
  struct sigevent *sig = ( struct sigevent * ) args;
  void (*notify_function)( union sigval ) = sig->sigev_notify_function;  
  union sigval param = ( union sigval ) sig->sigev_value;

  notify_function( param );

  pthread_exit( NULL );
}

static void rtems_aio_notify( struct sigevent *sigp ) 
{
  int result;

  _Assert( sigp != NULL );

  switch ( sigp->sigev_notify ) {
#ifdef RTEMS_POSIX_API
    case SIGEV_SIGNAL:
      result = sigqueue(
        getpid(),
        sigp->sigev_signo,
        sigp->sigev_value
      );
      _Assert_Unused_variable_equals( result, 0 );
      break;
#endif
    case SIGEV_THREAD:
      pthread_t thread;
      pthread_attr_t attr;
      pthread_attr_t *attrp = sigp->sigev_notify_attributes;
      
      if ( attrp == NULL ) {
        attrp = &attr;

        result = pthread_attr_init( attrp );
        _Assert_Unused_variable_equals( result, 0 );

        result = pthread_attr_setdetachstate(
          attrp, 
          PTHREAD_CREATE_DETACHED
        );
        _Assert_Unused_variable_equals( result, 0 );
      }

      result = pthread_create( 
        &thread,
        attrp,
        rtems_aio_notify_function_wrapper,
        sigp
      );
      _Assert_Unused_variable_equals( result, 0 );
      break;
  }
}

static void *rtems_aio_handle( void *arg )
{
  rtems_aio_request_chain *r_chain = arg;
  rtems_aio_request *req;
  rtems_chain_control *chain;
  rtems_chain_node *node;
  int result, policy;
  struct sched_param param;

  AIO_printf( "Thread started\n" );

  while (1) {

    /* acquire the mutex of the current fd chain.
       we don't need to lock the queue mutex since we can
       add requests to idle fd chains or even active ones
       if the working request has been extracted from the
       chain */
    result = pthread_mutex_lock( &r_chain->mutex );
    if ( result != 0 )
      return NULL;

    chain = &r_chain->perfd;

    /* If the locked chain is not empty, take the first
       request extract it, unlock the chain and process
       the request, in this way the user can supply more
       requests to this fd chain */
    if ( !rtems_chain_is_empty( chain ) ) {

      AIO_printf( "Get new request from not empty chain\n" );	
      node = rtems_chain_first( chain );
      req = (rtems_aio_request *) node;

      /* See _POSIX_PRIORITIZE_IO and _POSIX_PRIORITY_SCHEDULING
         discussion in rtems_aio_enqueue() */
      pthread_getschedparam(
        pthread_self(),
        &policy,
        &param
      );
      param.sched_priority = req->priority;
      pthread_setschedparam( pthread_self(), req->policy, &param );

      rtems_chain_extract( node );

      pthread_mutex_unlock( &r_chain->mutex );

      /* perform the requested operation*/
      rtems_aio_handle_helper( req );

      /* update queued_requests */
      atomic_fetch_sub( &aio_request_queue.queued_requests, 1 );

      /* notification for request completion */
      rtems_aio_notify( &req->aiocbp->aio_sigevent );

      /* notification for list completion */
      rtems_aio_completed_list_op( req->listcbp );

      /* notification for aio_suspend() */
      rtems_aio_update_suspendcbp( req->suspendcbp );

      req->listcbp = NULL;
      req->suspendcbp = NULL;

      free(req);

    } else {
      /* If the fd chain is empty we unlock the fd chain and we lock
         the queue chain, this will ensure that we have at most
         one request coming to our fd chain when we check. 

         If there was no request added sleep for 3 seconds and wait
         for a signal on chain, this will unlock the queue.
         The fd chain is already unlocked */

      struct timespec timeout;

      AIO_printf( "Chain is empty [WQ], wait for work\n" );

      pthread_mutex_unlock( &r_chain->mutex );
      pthread_mutex_lock( &aio_request_queue.mutex );

      if ( rtems_chain_is_empty( chain ) ) {
        clock_gettime( CLOCK_REALTIME, &timeout );
        timeout.tv_sec += 3;
        timeout.tv_nsec = 0;
        result = pthread_cond_timedwait(
          &r_chain->cond,
          &aio_request_queue.mutex,
          &timeout
        );

        /* If no requests were added to the chain we delete the fd chain from
          the queue and start working with idle fd chains */
        if ( result == ETIMEDOUT ) {
          rtems_chain_extract( &r_chain->next_fd );
          pthread_mutex_destroy( &r_chain->mutex );
          pthread_cond_destroy( &r_chain->cond );
          free( r_chain );

          /* If the idle chain is empty sleep for 3 seconds and wait for a
            signal. The thread now becomes idle. */
          if ( rtems_chain_is_empty( &aio_request_queue.idle_req ) ) {
            AIO_printf( "Chain is empty [IQ], wait for work\n" );

            ++aio_request_queue.idle_threads;
            --aio_request_queue.active_threads;
            clock_gettime( CLOCK_REALTIME, &timeout );
            timeout.tv_sec += 3;
            timeout.tv_nsec = 0;

            /* Coverity detected an error, but after review,
              it was deemed a false positive.

              The concern was whether there would be an issue
              if the thread was interrupted while waiting.
              According to the POSIX specification of
              pthread_cond_timedwait:

              "If a signal is delivered to a thread waiting
              on a condition variable, the thread either resumes
              waiting as if not interrupted or returns zero due
              to spurious wakeup."

              Both scenarios are safe:

              - If pthread_cond_timedwait resumes waiting,
                there are no issues.
              - If it returns zero, the thread continues
                as if an element was added. Since no element
                is added, in the next iteration of the loop
                handling requests, the queue stays empty, and
                the thread waits again. */
                
            result = pthread_cond_timedwait(
              &aio_request_queue.new_req,
              &aio_request_queue.mutex,
              &timeout
            );

            /* If no new fd chain was added in the idle requests
               then this thread is finished */
            if ( result == ETIMEDOUT ) {
              AIO_printf( "Etimeout\n" );
              --aio_request_queue.idle_threads;
              pthread_mutex_unlock( &aio_request_queue.mutex );
              return NULL;
            }
          }
          /* Otherwise move this chain to the working chain and
             start the loop all over again */
          AIO_printf( "Work on idle\n" );
          --aio_request_queue.idle_threads;
          ++aio_request_queue.active_threads;

          node = rtems_chain_first( &aio_request_queue.idle_req );
          rtems_chain_extract( node );

          r_chain = (rtems_aio_request_chain *) node;
          rtems_aio_move_to_work( r_chain );

        }
      }
      /* If there was a request added in the initial fd chain then release
         the mutex and process it */
      pthread_mutex_unlock( &aio_request_queue.mutex );

    }
  }

  AIO_printf( "Thread finished\n" );
  return NULL;
}

static void rtems_aio_handle_helper( rtems_aio_request *req )
{
  int result;

  switch ( req->op_type ) {
    case AIO_OP_READ:
      AIO_printf( "read\n" );
      result = pread(
        req->aiocbp->aio_fildes,
        (void *) req->aiocbp->aio_buf,
        req->aiocbp->aio_nbytes, req->aiocbp->aio_offset
      );
      break;

    case AIO_OP_WRITE:
      AIO_printf( "write\n" );
      result = pwrite(
        req->aiocbp->aio_fildes,
        (void *) req->aiocbp->aio_buf,
        req->aiocbp->aio_nbytes, req->aiocbp->aio_offset
      );
      break;

    case AIO_OP_SYNC:
      AIO_printf( "sync\n" );
      result = fsync( req->aiocbp->aio_fildes );
      break;
    
    case AIO_OP_DSYNC:
      AIO_printf( "data sync\n" );
      result = fdatasync( req->aiocbp->aio_fildes );
      break;

    default:
      result = -1;
  }

  if ( result < 0 ) {
    req->aiocbp->return_value = -1;
    req->aiocbp->error_code = errno;
  } else {
    req->aiocbp->return_value = result;
    req->aiocbp->error_code = 0;
  }
}

rtems_aio_request * rtems_aio_search_in_chain(
  const struct aiocb* aiocbp,
  rtems_chain_control *fd_chain
)
{
  rtems_aio_request *current;
  rtems_chain_node *node;

  node = rtems_chain_first( fd_chain );
  current = (rtems_aio_request *) node;

  while ( !rtems_chain_is_tail( fd_chain, node ) && current->aiocbp != aiocbp ) {
    node = rtems_chain_next( node );
    current = (rtems_aio_request *) node;
  }
      
  if ( rtems_chain_is_tail( fd_chain, node ) ) {
    return NULL;
  }

  return current;
}
