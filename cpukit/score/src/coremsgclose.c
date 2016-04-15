/**
 *  @file
 *
 *  @brief Close a Message Queue
 *  @ingroup ScoreMessageQueue
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coremsgimpl.h>
#include <rtems/score/wkspace.h>

static Thread_Control *_CORE_message_queue_Was_deleted(
  Thread_Control     *the_thread,
  Thread_queue_Queue *queue,
  ISR_lock_Context   *lock_context
)
{
  the_thread->Wait.return_code = CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED;

  return the_thread;
}

void _CORE_message_queue_Do_close(
  CORE_message_queue_Control *the_message_queue
#if defined(RTEMS_MULTIPROCESSING)
  ,
  Thread_queue_MP_callout     mp_callout,
  Objects_Id                  mp_id
#endif
)
{
  ISR_lock_Context lock_context;

  /*
   *  This will flush blocked threads whether they were blocked on
   *  a send or receive.
   */

  _CORE_message_queue_Acquire( the_message_queue, &lock_context );
  _Thread_queue_Flush_critical(
    &the_message_queue->Wait_queue.Queue,
    the_message_queue->operations,
    _CORE_message_queue_Was_deleted,
    mp_callout,
    mp_id,
    &lock_context
  );

  (void) _Workspace_Free( the_message_queue->message_buffers );

  _Thread_queue_Destroy( &the_message_queue->Wait_queue );
}
