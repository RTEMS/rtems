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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif

/*PAGE
 *
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

unsigned32 _CORE_message_queue_Flush_support(
  CORE_message_queue_Control *the_message_queue
)
{
  ISR_Level   level;
  Chain_Node *inactive_first;
  Chain_Node *message_queue_first;
  Chain_Node *message_queue_last;
  unsigned32  count;

  _ISR_Disable( level );
    inactive_first      = the_message_queue->Inactive_messages.first;
    message_queue_first = the_message_queue->Pending_messages.first;
    message_queue_last  = the_message_queue->Pending_messages.last;

    the_message_queue->Inactive_messages.first = message_queue_first;
    message_queue_last->next = inactive_first;
    inactive_first->previous = message_queue_last;
    message_queue_first->previous          =
               _Chain_Head( &the_message_queue->Inactive_messages );

    _Chain_Initialize_empty( &the_message_queue->Pending_messages );

    count = the_message_queue->number_of_pending_messages;
    the_message_queue->number_of_pending_messages = 0;
  _ISR_Enable( level );
  return count;
}

