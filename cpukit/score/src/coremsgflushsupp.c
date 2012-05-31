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
 *  _CORE_message_queue_Flush_support
 *
 *  This message handler routine removes all messages from a message queue
 *  and returns them to the inactive message pool.  The number of messages
 *  flushed from the queue is returned
 *
 *  Input parameters:
 *    the_message_queue - pointer to message queue
 *
 *  Output parameters:
 *    returns - number of messages placed on inactive chain
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

uint32_t   _CORE_message_queue_Flush_support(
  CORE_message_queue_Control *the_message_queue
)
{
  ISR_Level   level;
  Chain_Node *inactive_head;
  Chain_Node *inactive_first;
  Chain_Node *message_queue_first;
  Chain_Node *message_queue_last;
  uint32_t    count;

  /*
   *  Currently, RTEMS supports no API that has both flush and blocking
   *  sends.  Thus, this routine assumes that there are no senders
   *  blocked waiting to send messages.  In the event, that an API is
   *  added that can flush a message queue when threads are blocked
   *  waiting to send, there are two basic behaviors envisioned:
   *
   *  (1) The thread queue of pending senders is a logical extension
   *  of the pending message queue.  In this case, it should be
   *  flushed using the _Thread_queue_Flush() service with a status
   *  such as CORE_MESSAGE_QUEUE_SENDER_FLUSHED (which currently does
   *  not exist).  This can be implemented without changing the "big-O"
   *  of the message flushing part of the routine.
   *
   *  (2) Only the actual messages queued should be purged.  In this case,
   *  the blocked sender threads must be allowed to send their messages.
   *  In this case, the implementation will be forced to individually
   *  dequeue the senders and queue their messages.  This will force
   *  this routine to have "big O(n)" where n is the number of blocked
   *  senders.  If there are more messages pending than senders blocked,
   *  then the existing flush code can be used to dispose of the remaining
   *  pending messages.
   *
   *  For now, though, we are very happy to have a small routine with
   *  fixed execution time that only deals with pending messages.
   */

  _ISR_Disable( level );
    inactive_head = _Chain_Head( &the_message_queue->Inactive_messages );
    inactive_first = inactive_head->next;
    message_queue_first = _Chain_First( &the_message_queue->Pending_messages );
    message_queue_last = _Chain_Last( &the_message_queue->Pending_messages );

    inactive_head->next = message_queue_first;
    message_queue_last->next = inactive_first;
    inactive_first->previous = message_queue_last;
    message_queue_first->previous = inactive_head;

    _Chain_Initialize_empty( &the_message_queue->Pending_messages );

    count = the_message_queue->number_of_pending_messages;
    the_message_queue->number_of_pending_messages = 0;
  _ISR_Enable( level );
  return count;
}
