/*  threadq.h
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of objects.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __THREAD_QUEUE_h
#define __THREAD_QUEUE_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/tqdata.h>

#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

/*
 *  Constant for indefinite wait.
 */
 
#define THREAD_QUEUE_WAIT_FOREVER  WATCHDOG_NO_TIMEOUT

/*
 *  The following type defines the callout used when a remote task
 *  is extracted from a local thread queue.
 */

typedef void ( *Thread_queue_Flush_callout )(
                 Thread_Control *
             );

/*
 *  The following type defines the callout used when a local task
 *  is extracted from a remote thread queue (i.e. it's proxy must
 *  extracted from the remote queue).
 */
 
#if 0
typedef void ( *Thread_queue_Extract_callout )(
                 Thread_Control *
             );

SCORE_EXTERN Thread_queue_Extract_callout  
  _Thread_queue_Extract_table[ OBJECTS_CLASSES_LAST + 1 ];
#endif

/*
 *  _Thread_queue_Dequeue
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to a thread waiting on
 *  the_thread_queue.  The selection of this thread is based on
 *  the discipline of the_thread_queue.  If no threads are waiting
 *  on the_thread_queue, then NULL is returned.
 */

Thread_Control *_Thread_queue_Dequeue(
  Thread_queue_Control *the_thread_queue
);

/*
 *  _Thread_queue_Enqueue
 *
 *  DESCRIPTION:
 *
 *  This routine enqueues the currently executing thread on
 *  the_thread_queue with an optional timeout.
 */

void _Thread_queue_Enqueue(
  Thread_queue_Control *the_thread_queue,
  Watchdog_Interval     timeout
);

/*
 *  _Thread_queue_Extract
 *
 *  DESCRIPTION:
 *
 *  This routine removes the_thread from the_thread_queue
 *  and cancels any timeouts associated with this blocking.
 */

void _Thread_queue_Extract(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
);

/*
 *  _Thread_queue_Extract_with_proxy
 *
 *  DESCRIPTION:
 *
 *  This routine extracts the_thread from the_thread_queue
 *  and ensures that if there is a proxy for this task on 
 *  another node, it is also dealt with.
 */
 
boolean _Thread_queue_Extract_with_proxy(
  Thread_Control       *the_thread
);

/*
 *  _Thread_queue_First
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to the "first" thread
 *  on the_thread_queue.  The "first" thread is selected
 *  based on the discipline of the_thread_queue.
 */

Thread_Control *_Thread_queue_First(
  Thread_queue_Control *the_thread_queue
);

/*
 *  _Thread_queue_Flush
 *
 *  DESCRIPTION:
 *
 *  This routine unblocks all threads blocked on the_thread_queue
 *  and cancels any associated timeouts.
 */

void _Thread_queue_Flush(
  Thread_queue_Control       *the_thread_queue,
  Thread_queue_Flush_callout  remote_extract_callout,
  unsigned32                  status
);

/*
 *  _Thread_queue_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the_thread_queue based on the
 *  discipline indicated in attribute_set.  The state set on
 *  threads which block on the_thread_queue is state.
 */

void _Thread_queue_Initialize(
  Thread_queue_Control         *the_thread_queue,
  Thread_queue_Disciplines      the_discipline,
  States_Control                state,
  unsigned32                    timeout_status
);

/*
 *  _Thread_queue_Dequeue_priority
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to the highest priority
 *  thread waiting on the_thread_queue.  If no threads are waiting
 *  on the_thread_queue, then NULL is returned.
 */

Thread_Control *_Thread_queue_Dequeue_priority(
  Thread_queue_Control *the_thread_queue
);

/*
 *  _Thread_queue_Enqueue_priority
 *
 *  DESCRIPTION:
 *
 *  This routine enqueues the currently executing thread on
 *  the_thread_queue with an optional timeout using the
 *  priority discipline.
 */

void _Thread_queue_Enqueue_priority(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  Watchdog_Interval     timeout
);

/*
 *  _Thread_queue_Extract_priority
 *
 *  DESCRIPTION:
 *
 *  This routine removes the_thread from the_thread_queue
 *  and cancels any timeouts associated with this blocking.
 */

void _Thread_queue_Extract_priority(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
);

/*
 *  _Thread_queue_First_priority
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to the "first" thread
 *  on the_thread_queue.  The "first" thread is the highest
 *  priority thread waiting on the_thread_queue.
 */

Thread_Control *_Thread_queue_First_priority(
  Thread_queue_Control *the_thread_queue
);

/*
 *  _Thread_queue_Dequeue_FIFO
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to the thread which has
 *  been waiting the longest on  the_thread_queue.  If no
 *  threads are waiting on the_thread_queue, then NULL is returned.
 */

Thread_Control *_Thread_queue_Dequeue_fifo(
  Thread_queue_Control *the_thread_queue
);

/*
 *  _Thread_queue_Enqueue_FIFO
 *
 *  DESCRIPTION:
 *
 *  This routine enqueues the currently executing thread on
 *  the_thread_queue with an optional timeout using the
 *  FIFO discipline.
 */

void _Thread_queue_Enqueue_fifo(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  Watchdog_Interval     timeout
);

/*
 *  _Thread_queue_Extract_FIFO
 *
 *  DESCRIPTION:
 *
 *  This routine removes the_thread from the_thread_queue
 *  and cancels any timeouts associated with this blocking.
 */

void _Thread_queue_Extract_fifo(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
);

/*
 *  _Thread_queue_First_FIFO
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to the "first" thread
 *  on the_thread_queue.  The first thread is the thread
 *  which has been waiting longest on the_thread_queue.
 */

Thread_Control *_Thread_queue_First_fifo(
  Thread_queue_Control *the_thread_queue
);

/*
 *  _Thread_queue_timeout
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a task's request has not
 *  been satisfied after the timeout interval specified to
 *  enqueue.  The task represented by ID will be unblocked and
 *  its status code will be set in it's control block to indicate
 *  that a timeout has occurred.
 */

void _Thread_queue_Timeout (
  Objects_Id  id,
  void       *ignored
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
