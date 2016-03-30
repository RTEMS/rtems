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

void _CORE_message_queue_Do_close(
  CORE_message_queue_Control *the_message_queue,
  uint32_t                    status
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

  _Thread_queue_Flush(
    &the_message_queue->Wait_queue,
    the_message_queue->operations,
    status,
    mp_callout,
    mp_id
  );

  /*
   *  This removes all messages from the pending message queue.  Since
   *  we just flushed all waiting threads, we don't have to worry about
   *  the flush satisfying any blocked senders as a side-effect.
   */

  _ISR_lock_ISR_disable( &lock_context );
  (void) _CORE_message_queue_Flush( the_message_queue, &lock_context );

  (void) _Workspace_Free( the_message_queue->message_buffers );

  _Thread_queue_Destroy( &the_message_queue->Wait_queue );
}
