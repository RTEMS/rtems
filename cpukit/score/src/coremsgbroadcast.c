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
 *  _CORE_message_queue_Broadcast
 *
 *  This function sends a message for every thread waiting on the queue and
 *  returns the number of threads made ready by the message.
 *
 *  Input parameters:
 *    the_message_queue            - message is submitted to this message queue
 *    buffer                       - pointer to message buffer
 *    size                         - size in bytes of message to send
 *    id                           - id of message queue
 *    api_message_queue_mp_support - api specific mp support callout
 *    count                        - area to store number of threads made ready
 *
 *  Output parameters:
 *    count                         - number of threads made ready
 *    CORE_MESSAGE_QUEUE_SUCCESSFUL - if successful
 *    error code                    - if unsuccessful
 */

CORE_message_queue_Status _CORE_message_queue_Broadcast(
  CORE_message_queue_Control                *the_message_queue,
  void                                      *buffer,
  unsigned32                                 size,
  Objects_Id                                 id,
  CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support,
  unsigned32                                *count
)
{
  Thread_Control          *the_thread;
  unsigned32               number_broadcasted;
  Thread_Wait_information *waitp;
  unsigned32               constrained_size;

  /*
   *  If there are pending messages, then there can't be threads
   *  waiting for us to send them a message.
   *
   *  NOTE: This check is critical because threads can block on
   *        send and receive and this ensures that we are broadcasting
   *        the message to threads waiting to receive -- not to send.
   */

  if ( the_message_queue->number_of_pending_messages != 0 ) {
    *count = 0;
    return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
  }

  /*
   *  There must be no pending messages if there is a thread waiting to
   *  receive a message.
   */

  number_broadcasted = 0;
  while ((the_thread = _Thread_queue_Dequeue(&the_message_queue->Wait_queue))) {
    waitp = &the_thread->Wait;
    number_broadcasted += 1;

    constrained_size = size;
    if ( size > the_message_queue->maximum_message_size )
        constrained_size = the_message_queue->maximum_message_size;

    _CORE_message_queue_Copy_buffer(
      buffer,
      waitp->return_argument,
      constrained_size
    );

    *(unsigned32 *)the_thread->Wait.return_argument_1 = size;

#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      (*api_message_queue_mp_support) ( the_thread, id );
#endif

  }
  *count = number_broadcasted;
  return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
}

