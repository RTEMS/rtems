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

/*
 *  _CORE_message_queue_Close
 *
 *  This function closes a message by returning all allocated space and
 *  flushing the message_queue's task wait queue.
 *
 *  Input parameters:
 *    the_message_queue      - the message_queue to be flushed
 *    remote_extract_callout - function to invoke remotely
 *    status                 - status to pass to thread
 *
 *  Output parameters:  NONE
 */

void _CORE_message_queue_Close(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Flush_callout  remote_extract_callout,
  uint32_t                    status
)
{

  /*
   *  This will flush blocked threads whether they were blocked on
   *  a send or receive.
   */

  _Thread_queue_Flush(
    &the_message_queue->Wait_queue,
    remote_extract_callout,
    status
  );

  /*
   *  This removes all messages from the pending message queue.  Since
   *  we just flushed all waiting threads, we don't have to worry about
   *  the flush satisfying any blocked senders as a side-effect.
   */

  if ( the_message_queue->number_of_pending_messages != 0 )
    (void) _CORE_message_queue_Flush_support( the_message_queue );

  (void) _Workspace_Free( the_message_queue->message_buffers );

}
