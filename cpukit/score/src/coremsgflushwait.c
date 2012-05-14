/*
 *  CORE Message Queue Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the CORE Message Queue Handler.
 *  This core object provides task synchronization and communication functions
 *  via messages passed to queue objects.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

#if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
  /*
   *  _CORE_message_queue_Flush_waiting_threads
   *
   *  This function flushes the message_queue's task wait queue.  The number
   *  of messages flushed from the queue is returned.
   *
   *  Input parameters:
   *    the_message_queue - the message_queue to be flushed
   *
   *  Output parameters:
   *    returns - the number of messages flushed from the queue
   */

  void _CORE_message_queue_Flush_waiting_threads(
    CORE_message_queue_Control *the_message_queue
  )
  {
    /* XXX this is not supported for global message queues */

    /*
     *  IF there are no pending messages,
     *  THEN threads may be blocked waiting to RECEIVE a message,
     *
     *  IF the pending message queue is full
     *  THEN threads may be blocked waiting to SEND a message
     *
     *  But in either case, we will return "unsatisfied nowait"
     *  to indicate that the blocking condition was not satisfied
     *  and that the blocking state was canceled.
     */

    _Thread_queue_Flush(
      &the_message_queue->Wait_queue,
      NULL,
      CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT
    );
  }
#endif
