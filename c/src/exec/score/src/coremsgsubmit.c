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
 *  _CORE_message_queue_Submit
 *
 *  This routine implements the send and urgent message functions. It
 *  processes a message that is to be submitted to the designated
 *  message queue.  The message will either be processed as a
 *  send message which it will be inserted at the rear of the queue
 *  or it will be processed as an urgent message which will be inserted
 *  at the front of the queue.
 *
 *  Input parameters:
 *    the_message_queue            - message is submitted to this message queue
 *    buffer                       - pointer to message buffer
 *    size                         - size in bytes of message to send
 *    id                           - id of message queue
 *    api_message_queue_mp_support - api specific mp support callout
 *    submit_type                  - send or urgent message
 *
 *  Output parameters:
 *    CORE_MESSAGE_QUEUE_SUCCESSFUL - if successful
 *    error code                    - if unsuccessful
 */

CORE_message_queue_Status _CORE_message_queue_Submit(
  CORE_message_queue_Control                *the_message_queue,
  void                                      *buffer,
  unsigned32                                 size,
  Objects_Id                                 id,
  CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support,
  CORE_message_queue_Submit_types            submit_type
)
{
  CORE_message_queue_Buffer_control   *the_message;
  Thread_Control                      *the_thread;

  if ( size > the_message_queue->maximum_message_size )
    return CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE;

  /*
   * Is there a thread currently waiting on this message queue?
   */
      
  the_thread = _Thread_queue_Dequeue( &the_message_queue->Wait_queue );
  if ( the_thread )
  {
    _CORE_message_queue_Copy_buffer(
      buffer,
      the_thread->Wait.return_argument,
      size
    );
    *(unsigned32 *)the_thread->Wait.return_argument_1 = size;
    the_thread->Wait.count = submit_type;
    
#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) )
      (*api_message_queue_mp_support) ( the_thread, id );
#endif

    return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
  }

  /*
   * No one waiting on this one currently.
   * Allocate a message buffer and store it away
   */

  if ( the_message_queue->number_of_pending_messages ==
       the_message_queue->maximum_pending_messages ) {
    return CORE_MESSAGE_QUEUE_STATUS_TOO_MANY;
  }

  the_message = _CORE_message_queue_Allocate_message_buffer(the_message_queue);
  if ( the_message == 0)
    return CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED;

  _CORE_message_queue_Copy_buffer( buffer, the_message->Contents.buffer, size );
  the_message->Contents.size = size;
  the_message->priority  = submit_type;

  the_message_queue->number_of_pending_messages += 1;

  switch ( submit_type ) {
    case CORE_MESSAGE_QUEUE_SEND_REQUEST:
      _CORE_message_queue_Append( the_message_queue, the_message );
      break;
    case CORE_MESSAGE_QUEUE_URGENT_REQUEST:
      _CORE_message_queue_Prepend( the_message_queue, the_message );
      break;
    default:
      /* XXX interrupt critical section needs to be addressed */
      {
        CORE_message_queue_Buffer_control *this_message;
        Chain_Node                        *the_node;

        the_message->priority = submit_type;
        for ( the_node = the_message_queue->Pending_messages.first ;
           !_Chain_Is_tail( &the_message_queue->Pending_messages, the_node ) ;
           the_node = the_node->next ) {

          this_message = (CORE_message_queue_Buffer_control *) the_node;

          if ( this_message->priority >= the_message->priority )
            continue;

          _Chain_Insert( the_node, &the_message->Node );
          break;
        }
      }
      break;
  }

  /*
   *  According to POSIX, does this happen before or after the message
   *  is actually enqueued.  It is logical to think afterwards, because
   *  the message is actually in the queue at this point.
   */

  if ( the_message_queue->number_of_pending_messages == 1 && 
       the_message_queue->notify_handler )
    (*the_message_queue->notify_handler)( the_message_queue->notify_argument );
  
  return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
}
