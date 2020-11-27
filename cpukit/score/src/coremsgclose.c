/**
 * @file
 *
 * @ingroup RTEMSScoreMessageQueue
 *
 * @brief This source file contains the implementation of
 *   _CORE_message_queue_Close().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coremsgimpl.h>

static Thread_Control *_CORE_message_queue_Was_deleted(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  the_thread->Wait.return_code = STATUS_MESSAGE_QUEUE_WAS_DELETED;

  return the_thread;
}

void _CORE_message_queue_Close(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
)
{

  /*
   *  This will flush blocked threads whether they were blocked on
   *  a send or receive.
   */

  _Thread_queue_Flush_critical(
    &the_message_queue->Wait_queue.Queue,
    the_message_queue->operations,
    _CORE_message_queue_Was_deleted,
    queue_context
  );

  if ( the_message_queue->free_message_buffers != NULL ) {
    ( *the_message_queue->free_message_buffers )(
      the_message_queue->message_buffers
    );
  }

  _Thread_queue_Destroy( &the_message_queue->Wait_queue );
}
