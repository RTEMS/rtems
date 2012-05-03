/*
 * Copyright 2010-2011, Alin Rus <alin.codejunkie@gmail.com> 
 * 
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <rtems/posix/aio_misc.h>
#include <errno.h>

static void *rtems_aio_handle (void *arg);

rtems_aio_queue aio_request_queue;

/* 
 *  rtems_aio_init
 *
 * Initialize the request queue for aio
 *
 *  Input parameters:
 *        NONE
 *
 *  Output parameters: 
 *        0    -    if initialization succeeded
 */

int
rtems_aio_init (void)
{
  int result = 0;

  result = pthread_attr_init (&aio_request_queue.attr);
  if (result != 0)
    return result;

  result =
    pthread_attr_setdetachstate (&aio_request_queue.attr,
                                 PTHREAD_CREATE_DETACHED);
  if (result != 0)
    pthread_attr_destroy (&aio_request_queue.attr);


  result = pthread_mutex_init (&aio_request_queue.mutex, NULL);
  if (result != 0)
    pthread_attr_destroy (&aio_request_queue.attr);


  result = pthread_cond_init (&aio_request_queue.new_req, NULL);
  if (result != 0) {
    pthread_mutex_destroy (&aio_request_queue.mutex);
    pthread_attr_destroy (&aio_request_queue.attr);
  }

  rtems_chain_initialize_empty (&aio_request_queue.work_req);
  rtems_chain_initialize_empty (&aio_request_queue.idle_req);

  aio_request_queue.active_threads = 0;
  aio_request_queue.idle_threads = 0;
  aio_request_queue.initialized = AIO_QUEUE_INITIALIZED;

  return result;
}

/* 
 *  rtems_aio_search_fd
 *
 * Search and create chain of requests for given FD 
 *
 *  Input parameters:
 *        chain        - chain of FD chains
 *        fildes       - file descriptor to search
 *        create       - if 1 search and create
 *                     - if 0 just search
 *
 *  Output parameters: 
 *        r_chain      - NULL if create == 0 and there is
 *                       no chain for given fildes
 *                     - pointer to chain is there exists
 *                       a chain for given fildes
 *                     - pointer to newly create chain if
 *                       create == 1
 *
 */

rtems_aio_request_chain *
rtems_aio_search_fd (rtems_chain_control *chain, int fildes, int create)
{
  rtems_aio_request_chain *r_chain;
  rtems_chain_node *node;

  node = rtems_chain_first (chain);
  r_chain = (rtems_aio_request_chain *) node;

  while (r_chain->fildes < fildes && !rtems_chain_is_tail (chain, node)) {
    node = rtems_chain_next (node);
    r_chain = (rtems_aio_request_chain *) node;
  }

  if (r_chain->fildes == fildes)
    r_chain->new_fd = 0;
  else {
    if (create == 0)
      r_chain = NULL;
    else {
      r_chain = malloc (sizeof (rtems_aio_request_chain));
      rtems_chain_initialize_empty (&r_chain->perfd);

      if (rtems_chain_is_empty (chain))
        rtems_chain_prepend (chain, &r_chain->next_fd);
      else
        rtems_chain_insert (rtems_chain_previous (node), &r_chain->next_fd);

      r_chain->new_fd = 1;
	  r_chain->fildes = fildes;
    }
  }
  return r_chain;
}

/*
 *  rtems_aio_move_to_work 
 * 
 * Move chain of requests from IQ to WQ
 *
 *  Input parameters:
 *        r_chain      - chain of requests
 *
 *  Output paramteres:
 *        NONE
 */

static void
rtems_aio_move_to_work (rtems_aio_request_chain *r_chain)
{
  rtems_aio_request_chain *temp;
  rtems_chain_node *node;
  
  node = rtems_chain_first (&aio_request_queue.work_req);
  temp = (rtems_aio_request_chain *) node;

  while (temp->fildes < r_chain->fildes && 
	 !rtems_chain_is_tail (&aio_request_queue.work_req, node)) {
    node = rtems_chain_next (node);
    temp = (rtems_aio_request_chain *) node;
  }
  
  rtems_chain_insert (rtems_chain_previous (node), &r_chain->next_fd);
}
 

/* 
 *  rtems_aio_insert_prio
 *
 * Add request to given FD chain. The chain is ordered
 * by priority
 *
 *  Input parameters:
 *        chain        - chain of requests for a given FD
 *        req          - request (see aio_misc.h)
 *
 *  Output parameters: 
 *        NONE
 */

static void
rtems_aio_insert_prio (rtems_chain_control *chain, rtems_aio_request *req)
{
  rtems_chain_node *node;

  AIO_printf ("FD exists \n");
  node = rtems_chain_first (chain);

  if (rtems_chain_is_empty (chain)) {
    AIO_printf ("First in chain \n");
    rtems_chain_prepend (chain, &req->next_prio);
  } else {
    AIO_printf ("Add by priority \n");
    int prio = ((rtems_aio_request *) node)->aiocbp->aio_reqprio;

    while (req->aiocbp->aio_reqprio > prio &&
           !rtems_chain_is_tail (chain, node)) {
      node = rtems_chain_next (node);
      prio = ((rtems_aio_request *) node)->aiocbp->aio_reqprio;
    }

    rtems_chain_insert (node->previous, &req->next_prio);

  }
}

/* 
 *  rtems_aio_remove_fd
 *
 * Removes all the requests in a fd chain 
 *
 *  Input parameters:
 *        r_chain        - pointer to the fd chain request
 * 
 *  Output parameters: 
 *        NONE
 */

void rtems_aio_remove_fd (rtems_aio_request_chain *r_chain)
{
  rtems_chain_control *chain;
  rtems_chain_node *node;
  chain = &r_chain->perfd;
  node = rtems_chain_first (chain);
  
  while (!rtems_chain_is_tail (chain, node))
    {
      rtems_chain_extract (node);
      rtems_aio_request *req = (rtems_aio_request *) node;
      node = rtems_chain_next (node);
      req->aiocbp->error_code = ECANCELED;
      req->aiocbp->return_value = -1;
      free (req);
    }
}

/* 
 *  rtems_aio_remove_req
 *
 * Removes request from given chain 
 *
 *  Input parameters:
 *        chain      - pointer to fd chain which may contain
 *                     the request
 *        aiocbp     - pointer to request that needs to be
 *                     canceled
 * 
 *  Output parameters: 
 *         AIO_NOTCANCELED   - if request was not canceled
 *         AIO_CANCELED      - if request was canceled
 */

int rtems_aio_remove_req (rtems_chain_control *chain, struct aiocb *aiocbp)
{
  if (rtems_chain_is_empty (chain))
    return AIO_ALLDONE;

  rtems_chain_node *node = rtems_chain_first (chain);
  rtems_aio_request *current;
  
  current = (rtems_aio_request *) node;

  while (!rtems_chain_is_tail (chain, node) && current->aiocbp != aiocbp) {
    node = rtems_chain_next (node);
    current = (rtems_aio_request *) node;
  }
  
  if (rtems_chain_is_tail (chain, node))
    return AIO_NOTCANCELED;
  else
    {
      rtems_chain_extract (node);
      current->aiocbp->error_code = ECANCELED;
      current->aiocbp->return_value = -1;
      free (current); 
    }
    
  return AIO_CANCELED;
}

/* 
 *  rtems_aio_enqueue
 *
 * Enqueue requests, and creates threads to process them 
 *
 *  Input parameters:
 *        req        - see aio_misc.h
 * 
 *  Output parameters: 
 *         0         - if request was added to queue
 *         errno     - otherwise
 */

int
rtems_aio_enqueue (rtems_aio_request *req)
{

  rtems_aio_request_chain *r_chain;
  rtems_chain_control *chain;
  pthread_t thid;
  int result, policy;
  struct sched_param param;

  /* The queue should be initialized */
  AIO_assert (aio_request_queue.initialized == AIO_QUEUE_INITIALIZED);

  result = pthread_mutex_lock (&aio_request_queue.mutex);
  if (result != 0) {
    free (req);
    return result;
  }

  /* _POSIX_PRIORITIZED_IO and _POSIX_PRIORITY_SCHEDULING are defined, 
     we can use aio_reqprio to lower the priority of the request */
  pthread_getschedparam (pthread_self(), &policy, &param);

  req->caller_thread = pthread_self ();
  req->priority = param.sched_priority - req->aiocbp->aio_reqprio;
  req->policy = policy;
  req->aiocbp->error_code = EINPROGRESS;
  req->aiocbp->return_value = 0;

  if ((aio_request_queue.idle_threads == 0) &&
      aio_request_queue.active_threads < AIO_MAX_THREADS)
    /* we still have empty places on the active_threads chain */
    {
      chain = &aio_request_queue.work_req;
      r_chain = rtems_aio_search_fd (chain, req->aiocbp->aio_fildes, 1);
      
      if (r_chain->new_fd == 1) {
	rtems_chain_prepend (&r_chain->perfd, &req->next_prio);
	r_chain->new_fd = 0;
	pthread_mutex_init (&r_chain->mutex, NULL);
	pthread_cond_init (&r_chain->cond, NULL);
	    
	AIO_printf ("New thread \n");
	result = pthread_create (&thid, &aio_request_queue.attr,
				 rtems_aio_handle, (void *) r_chain);
	if (result != 0) {
	  pthread_mutex_unlock (&aio_request_queue.mutex);
	  return result;
	}
	++aio_request_queue.active_threads;
      }
      else {
	/* put request in the fd chain it belongs to */
	pthread_mutex_lock (&r_chain->mutex);
	rtems_aio_insert_prio (&r_chain->perfd, req);
	pthread_cond_signal (&r_chain->cond);
	pthread_mutex_unlock (&r_chain->mutex);
      }
    }
  else 
    {
      /* the maximum number of threads has been already created
	 even though some of them might be idle.
	 The request belongs to one of the active fd chain */
      r_chain = rtems_aio_search_fd (&aio_request_queue.work_req,
				     req->aiocbp->aio_fildes, 0);
      if (r_chain != NULL)
	{
	  pthread_mutex_lock (&r_chain->mutex);
	  rtems_aio_insert_prio (&r_chain->perfd, req);
	  pthread_cond_signal (&r_chain->cond);
	  pthread_mutex_unlock (&r_chain->mutex);
	    
	} else {
      
	/* or to the idle chain */
	chain = &aio_request_queue.idle_req;
	r_chain = rtems_aio_search_fd (chain, req->aiocbp->aio_fildes, 1);
      
	if (r_chain->new_fd == 1) {
	  /* If this is a new fd chain we signal the idle threads that
	     might be waiting for requests */
	  AIO_printf (" New chain on waiting queue \n ");
	  rtems_chain_prepend (&r_chain->perfd, &req->next_prio);
	  r_chain->new_fd = 0;
	  pthread_mutex_init (&r_chain->mutex, NULL);
	  pthread_cond_init (&r_chain->cond, NULL);
	} else
	  /* just insert the request in the existing fd chain */
	  rtems_aio_insert_prio (&r_chain->perfd, req);
	if (aio_request_queue.idle_threads > 0)
	  pthread_cond_signal (&aio_request_queue.new_req);
      }
    }

  pthread_mutex_unlock (&aio_request_queue.mutex);
  return 0;
}

/* 
 *  rtems_aio_handle
 *
 * Thread processing requests 
 *
 *  Input parameters:
 *        arg        - the chain for the fd to be worked on
 * 
 *  Output parameters: 
 *        NULL       - if error
 */

static void *
rtems_aio_handle (void *arg)
{

  rtems_aio_request_chain *r_chain = arg;
  rtems_aio_request *req;
  rtems_chain_control *chain;
  rtems_chain_node *node;
  int result, policy;
  struct sched_param param;

  AIO_printf ("Thread started\n");
 
  while (1) {
    
    /* acquire the mutex of the current fd chain.
       we don't need to lock the queue mutex since we can
       add requests to idle fd chains or even active ones
       if the working request has been extracted from the
       chain */
    result = pthread_mutex_lock (&r_chain->mutex);
    if (result != 0)
      return NULL;
    
    chain = &r_chain->perfd;    

    /* If the locked chain is not empty, take the first
       request extract it, unlock the chain and process 
       the request, in this way the user can supply more
       requests to this fd chain */
    if (!rtems_chain_is_empty (chain)) {

      AIO_printf ("Get new request from not empty chain\n");	
      node = rtems_chain_first (chain);
      req = (rtems_aio_request *) node;
      
      /* See _POSIX_PRIORITIZE_IO and _POSIX_PRIORITY_SCHEDULING
	 discussion in rtems_aio_enqueue () */
      pthread_getschedparam (pthread_self(), &policy, &param);
      param.sched_priority = req->priority;
      pthread_setschedparam (pthread_self(), req->policy, &param);

      rtems_chain_extract (node);

      pthread_mutex_unlock (&r_chain->mutex);

      switch (req->aiocbp->aio_lio_opcode) {
      case LIO_READ:
	AIO_printf ("read\n");
        result = pread (req->aiocbp->aio_fildes,
                        (void *) req->aiocbp->aio_buf,
                        req->aiocbp->aio_nbytes, req->aiocbp->aio_offset);
        break;

      case LIO_WRITE:
	AIO_printf ("write\n");
        result = pwrite (req->aiocbp->aio_fildes,
                         (void *) req->aiocbp->aio_buf,
                         req->aiocbp->aio_nbytes, req->aiocbp->aio_offset);
        break;
        
      case LIO_SYNC:
	AIO_printf ("sync\n");
      	result = fsync (req->aiocbp->aio_fildes);
      	break;

      default:
        result = -1;
      }
      if (result == -1) {
        req->aiocbp->return_value = -1;
	req->aiocbp->error_code = errno;
      } else {
        req->aiocbp->return_value = result;
        req->aiocbp->error_code = 0;
      }

      // notification needed for lio

    } else {
      /* If the fd chain is empty we unlock the fd chain
	 and we lock the queue chain, this will ensure that
	 we have at most one request comming to our fd chain
	 when we check. 
	 
	 If there was no request added sleep for 3 seconds and
	 wait for a signal on chain, this will unlock the queue.
	 The fd chain is already unlocked */

      struct timespec timeout;
      
      AIO_printf ("Chain is empty [WQ], wait for work\n");
     
      pthread_mutex_unlock (&r_chain->mutex);
      pthread_mutex_lock (&aio_request_queue.mutex);
      
      if (rtems_chain_is_empty (chain))
	{
	  clock_gettime (CLOCK_REALTIME, &timeout);
	  timeout.tv_sec += 3;
	  timeout.tv_nsec = 0;
	  result = pthread_cond_timedwait (&r_chain->cond,
					   &aio_request_queue.mutex,
					   &timeout);

	  /* If no requests were added to the chain we delete the fd chain from 
	     the queue and start working with idle fd chains */
	  if (result == ETIMEDOUT) {
	    rtems_chain_extract (&r_chain->next_fd);
	    pthread_mutex_destroy (&r_chain->mutex);
	    pthread_cond_destroy (&r_chain->cond);
	    free (r_chain);
	    
	    /* If the idle chain is empty sleep for 3 seconds and wait for a 
	       signal. The thread now becomes idle. */
	    if (rtems_chain_is_empty (&aio_request_queue.idle_req)) {
	      AIO_printf ("Chain is empty [IQ], wait for work\n");	      

	      ++aio_request_queue.idle_threads;
	      --aio_request_queue.active_threads;
	      clock_gettime (CLOCK_REALTIME, &timeout);
	      timeout.tv_sec += 3;
	      timeout.tv_nsec = 0;

	      result = pthread_cond_timedwait (&aio_request_queue.new_req,
					       &aio_request_queue.mutex,
					       &timeout);
	      
	      /* If no new fd chain was added in the idle requests
		 then this thread is finished */
	      if (result == ETIMEDOUT) {
		AIO_printf ("Etimeout\n");
		--aio_request_queue.idle_threads;
		pthread_mutex_unlock (&aio_request_queue.mutex);
		return NULL;
	      }
	    }
	    /* Otherwise move this chain to the working chain and 
	       start the loop all over again */
	    AIO_printf ("Work on idle\n");
	    --aio_request_queue.idle_threads;
	    ++aio_request_queue.active_threads;

	    node = rtems_chain_first (&aio_request_queue.idle_req);
	    rtems_chain_extract (node);

	    r_chain = (rtems_aio_request_chain *) node;
	    rtems_aio_move_to_work (r_chain);
	    
	  }
	}
      /* If there was a request added in the initial fd chain then release
	 the mutex and process it */
      pthread_mutex_unlock (&aio_request_queue.mutex);
      
    }
  }
  
  AIO_printf ("Thread finished\n");
  return NULL;
}
