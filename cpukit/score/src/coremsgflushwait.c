/**
 * @file
 *
 * @brief Flush Waiting Threads.
 *
 * @ingroup ScoreMessageQueue
 */

/*
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
#include <rtems/score/coremsgimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

#if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)

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
