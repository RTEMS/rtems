/*
 *  CORE Message Queue Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the CORE Message Queue Handler.
 *  This core object provides task synchronization and communication functions
 *  via messages passed to queue objects.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
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
 *  _CORE_message_queue_Initialize
 *
 *  This routine initializes a newly created message queue based on the
 *  specified data.
 *
 *  Input parameters:
 *    the_message_queue            - the message queue to initialize
 *    the_class                    - the API specific object class
 *    the_message_queue_attributes - the message queue's attributes
 *    maximum_pending_messages     - maximum message and reserved buffer count
 *    maximum_message_size         - maximum size of each message
 *    proxy_extract_callout        - remote extract support
 *
 *  Output parameters:
 *    TRUE   - if the message queue is initialized
 *    FALSE  - if the message queue is NOT initialized
 */

boolean _CORE_message_queue_Initialize(
  CORE_message_queue_Control    *the_message_queue,
  Objects_Classes                the_class,
  CORE_message_queue_Attributes *the_message_queue_attributes,
  unsigned32                     maximum_pending_messages,
  unsigned32                     maximum_message_size,
  Thread_queue_Extract_callout   proxy_extract_callout
)
{
  unsigned32 message_buffering_required;
  unsigned32 allocated_message_size;

  the_message_queue->maximum_pending_messages   = maximum_pending_messages;
  the_message_queue->number_of_pending_messages = 0;
  the_message_queue->maximum_message_size       = maximum_message_size;
  _CORE_message_queue_Set_notify( the_message_queue, NULL, NULL );
 
  /*
   * round size up to multiple of a ptr for chain init
   */
 
  allocated_message_size = maximum_message_size;
  if (allocated_message_size & (sizeof(unsigned32) - 1)) {
      allocated_message_size += sizeof(unsigned32);
      allocated_message_size &= ~(sizeof(unsigned32) - 1);
  }
   
  message_buffering_required = maximum_pending_messages *
       (allocated_message_size + sizeof(CORE_message_queue_Buffer_control));
 
  the_message_queue->message_buffers = (CORE_message_queue_Buffer *) 
     _Workspace_Allocate( message_buffering_required );
 
  if (the_message_queue->message_buffers == 0)
    return FALSE;
 
  _Chain_Initialize (
    &the_message_queue->Inactive_messages,
    the_message_queue->message_buffers,
    maximum_pending_messages,
    allocated_message_size + sizeof( CORE_message_queue_Buffer_control )
  );
 
  _Chain_Initialize_empty( &the_message_queue->Pending_messages );
 
  _Thread_queue_Initialize(
    &the_message_queue->Wait_queue,
    the_class,
    _CORE_message_queue_Is_priority( the_message_queue_attributes ) ?
       THREAD_QUEUE_DISCIPLINE_PRIORITY : THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_MESSAGE,
    proxy_extract_callout,
    CORE_MESSAGE_QUEUE_STATUS_TIMEOUT
  );

  return TRUE;
}

/*PAGE
 *
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
  unsigned32                  status
)
{
 
  if ( the_message_queue->number_of_pending_messages != 0 )
    (void) _CORE_message_queue_Flush_support( the_message_queue );
  else
    _Thread_queue_Flush(
      &the_message_queue->Wait_queue,
      remote_extract_callout,
      status
    );

  (void) _Workspace_Free( the_message_queue->message_buffers );

}

/*PAGE
 *
 *  _CORE_message_queue_Flush
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
 
unsigned32 _CORE_message_queue_Flush(
  CORE_message_queue_Control *the_message_queue
)
{
  if ( the_message_queue->number_of_pending_messages != 0 )
    return _CORE_message_queue_Flush_support( the_message_queue );
  else
    return 0;
}

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

/*PAGE
 *
 *  _CORE_message_queue_Seize
 *
 *  This kernel routine dequeues a message, copies the message buffer to
 *  a given destination buffer, and frees the message buffer to the
 *  inactive message pool.  The thread will be blocked if wait is TRUE,
 *  otherwise an error will be given to the thread if no messages are available.
 *
 *  Input parameters:
 *    the_message_queue - pointer to message queue
 *    id                - id of object we are waitig on
 *    buffer            - pointer to message buffer to be filled
 *    size              - pointer to the size of buffer to be filled
 *    wait              - TRUE if wait is allowed, FALSE otherwise
 *    timeout           - time to wait for a message
 *
 *  Output parameters:  NONE
 *
 *  NOTE: Dependent on BUFFER_LENGTH
 *
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 */

void _CORE_message_queue_Seize(
  CORE_message_queue_Control *the_message_queue,
  Objects_Id                  id,
  void                       *buffer,
  unsigned32                 *size,
  boolean                     wait,
  Watchdog_Interval           timeout
)
{
  ISR_Level                          level;
  CORE_message_queue_Buffer_control *the_message;
  Thread_Control                    *executing;

  executing = _Thread_Executing;
  executing->Wait.return_code = CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
  _ISR_Disable( level );
  if ( the_message_queue->number_of_pending_messages != 0 ) {
    the_message_queue->number_of_pending_messages -= 1;

    the_message = _CORE_message_queue_Get_pending_message( the_message_queue );
    _ISR_Enable( level );
    *size = the_message->Contents.size;
    _CORE_message_queue_Copy_buffer(the_message->Contents.buffer,buffer,*size );
    _CORE_message_queue_Free_message_buffer(the_message_queue, the_message );
    return;
  }

  if ( !wait ) {
    _ISR_Enable( level );
    executing->Wait.return_code = CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT;
    return;
  }

  _Thread_queue_Enter_critical_section( &the_message_queue->Wait_queue );
  executing->Wait.queue              = &the_message_queue->Wait_queue;
  executing->Wait.id                 = id;
  executing->Wait.return_argument    = (void *)buffer;
  executing->Wait.return_argument_1  = (void *)size;
  _ISR_Enable( level );

  _Thread_queue_Enqueue( &the_message_queue->Wait_queue, timeout );
}

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
