/*
 *  Message Queue Manager
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/attr.h>
#include <rtems/core/chain.h>
#include <rtems/core/isr.h>
#include <rtems/rtems/message.h>
#include <rtems/core/object.h>
#include <rtems/rtems/options.h>
#include <rtems/core/states.h>
#include <rtems/rtems/support.h>
#include <rtems/core/thread.h>
#include <rtems/core/wkspace.h>
#include <rtems/core/mpci.h>
#include <rtems/sysstate.h>

/*PAGE
 *
 *  _Message_queue_Manager_initialization
 *
 *  This routine initializes all message queue manager related
 *  data structures.
 *
 *  Input parameters:
 *    maximum_message_queues - number of message queues to initialize
 *
 *  Output parameters:  NONE
 */

void _Message_queue_Manager_initialization(
  unsigned32 maximum_message_queues
)
{
  _Objects_Initialize_information(
    &_Message_queue_Information,
    OBJECTS_RTEMS_MESSAGE_QUEUES,
    TRUE,
    maximum_message_queues,
    sizeof( Message_queue_Control ),
    FALSE,
    RTEMS_MAXIMUM_NAME_LENGTH,
    FALSE
  );

  /*
   *  Register the MP Process Packet routine.
   */

  _MPCI_Register_packet_processor(
    MP_PACKET_MESSAGE_QUEUE,
    _Message_queue_MP_Process_packet
  );

}

/*PAGE
 *
 *  _Message_queue_Allocate
 *
 *  Allocate a message queue and the space for its messages
 */

Message_queue_Control *_Message_queue_Allocate (
  unsigned32          count,
  unsigned32          max_message_size
)
{
    Message_queue_Control *mq = 0;
    unsigned32 message_buffering_required;
    unsigned32 allocated_message_size;

    mq =
      (Message_queue_Control *)_Objects_Allocate(&_Message_queue_Information);

    if (mq == 0)
        goto failed;

    mq->maximum_message_size = max_message_size;

    /*
     * round size up to multiple of a ptr for chain init
     */
    
    allocated_message_size = max_message_size;
    if (allocated_message_size & (sizeof(unsigned32) - 1)) {
        allocated_message_size += sizeof(unsigned32);
        allocated_message_size &= ~(sizeof(unsigned32) - 1);
    }
    
    message_buffering_required = 
      count * (allocated_message_size + sizeof(Message_queue_Buffer_control));
  
    mq->message_buffers = 
      (Message_queue_Buffer *) _Workspace_Allocate(message_buffering_required);

    if (mq->message_buffers == 0)
        goto failed;
  
    _Chain_Initialize
      (&mq->Inactive_messages,
      mq->message_buffers,
      count,
      allocated_message_size + sizeof(Message_queue_Buffer_control)
    );
    return mq;

failed:
    if (mq)
        _Message_queue_Free(mq);
    return (Message_queue_Control *) 0;
}

/*PAGE
 *
 *  rtems_message_queue_create
 *
 *  This directive creates a message queue by allocating and initializing
 *  a message queue data structure.
 *
 *  Input parameters:
 *    name          - user defined queue name
 *    count         - maximum message and reserved buffer count
 *    max_message_size - maximum size of each message
 *    attribute_set - process method
 *    id            - pointer to queue
 *
 *  Output parameters:
 *    id                - queue id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_message_queue_create(
  rtems_name          name,
  unsigned32          count,
  unsigned32          max_message_size,
  rtems_attribute     attribute_set,
  Objects_Id         *id
)
{
  register Message_queue_Control *the_message_queue;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( _Attributes_Is_global( attribute_set ) && 
       !_System_state_Is_multiprocessing )
    return RTEMS_MP_NOT_CONFIGURED;

  if (count == 0)
      return RTEMS_INVALID_NUMBER;

  if (max_message_size == 0)
      return RTEMS_INVALID_SIZE;

#if 1
  /*
   * I am not 100% sure this should be an error.
   * It seems reasonable to create a que with a large max size,
   * and then just send smaller msgs from remote (or all) nodes.
   */
  
  if ( _Attributes_Is_global( attribute_set ) &&
       (_MPCI_table->maximum_packet_size < max_message_size))
  {
      return RTEMS_INVALID_SIZE;
  }
#endif
       
  _Thread_Disable_dispatch();              /* protects object pointer */

  the_message_queue = _Message_queue_Allocate(count, max_message_size);

  if ( !the_message_queue ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

  if ( _Attributes_Is_global( attribute_set ) &&
       !( _Objects_MP_Allocate_and_open( &_Message_queue_Information, name,
                            the_message_queue->Object.id, FALSE ) ) ) {
    _Message_queue_Free( the_message_queue );
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

  the_message_queue->maximum_pending_messages = count;

  the_message_queue->attribute_set = attribute_set;
  the_message_queue->number_of_pending_messages = 0;

  _Chain_Initialize_empty( &the_message_queue->Pending_messages );

  _Thread_queue_Initialize(
    &the_message_queue->Wait_queue,
    OBJECTS_RTEMS_MESSAGE_QUEUES,
    _Attributes_Is_priority( attribute_set ) ? 
       THREAD_QUEUE_DISCIPLINE_PRIORITY : THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_MESSAGE,
    _Message_queue_MP_Send_extract_proxy,
    RTEMS_TIMEOUT
  );

  _Objects_Open(
    &_Message_queue_Information,
    &the_message_queue->Object,
    &name
  );

  *id = the_message_queue->Object.id;

  if ( _Attributes_Is_global( attribute_set ) )
    _Message_queue_MP_Send_process_packet(
      MESSAGE_QUEUE_MP_ANNOUNCE_CREATE,
      the_message_queue->Object.id,
      name,
      0
    );

  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  rtems_message_queue_ident
 *
 *  This directive returns the system ID associated with
 *  the message queue name.
 *
 *  Input parameters:
 *    name - user defined message queue name
 *    node - node(s) to be searched
 *    id   - pointer to message queue id
 *
 *  Output parameters:
 *    *id               - message queue id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_message_queue_ident(
  rtems_name    name,
  unsigned32    node,
  Objects_Id   *id
)
{
  Objects_Name_to_id_errors  status;

  status = _Objects_Name_to_id(
    &_Message_queue_Information,
    &name,
    node,
    id 
  );

  return _Status_Object_name_errors_to_status[ status ];
}

/*PAGE
 *
 *  rtems_message_queue_delete
 *
 *  This directive allows a thread to delete the message queue specified
 *  by the given queue identifier.
 *
 *  Input parameters:
 *    id - queue id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_message_queue_delete(
  Objects_Id id
)
{
  register Message_queue_Control *the_message_queue;
  Objects_Locations               location;

  the_message_queue = _Message_queue_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    case OBJECTS_LOCAL:
      _Objects_Close( &_Message_queue_Information,
                      &the_message_queue->Object );

      if ( the_message_queue->number_of_pending_messages != 0 )
        (void) _Message_queue_Flush_support( the_message_queue );
      else
        _Thread_queue_Flush(
          &the_message_queue->Wait_queue,
          _Message_queue_MP_Send_object_was_deleted,
          RTEMS_OBJECT_WAS_DELETED
        );

      _Message_queue_Free( the_message_queue );

      if ( _Attributes_Is_global( the_message_queue->attribute_set ) ) {
        _Objects_MP_Close(
          &_Message_queue_Information,
          the_message_queue->Object.id
        );

        _Message_queue_MP_Send_process_packet(
          MESSAGE_QUEUE_MP_ANNOUNCE_DELETE,
          the_message_queue->Object.id,
          0,                                 /* Not used */
          0
        );
      }

      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_message_queue_send
 *
 *  This routine implements the directives q_send.  It sends a
 *  message to the specified message queue.
 *
 *  Input parameters:
 *    id     - pointer to message queue
 *    buffer - pointer to message buffer
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_message_queue_send(
  Objects_Id            id,
  void                 *buffer,
  unsigned32            size
)
{
  return _Message_queue_Submit(id, buffer, size, MESSAGE_QUEUE_SEND_REQUEST);
}

/*PAGE
 *
 *  rtems_message_queue_urgent
 *
 *  This routine implements the directives q_urgent.  It urgents a
 *  message to the specified message queue.
 *
 *  Input parameters:
 *    id     - pointer to message queue
 *    buffer - pointer to message buffer
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_message_queue_urgent(
  Objects_Id            id,
  void                 *buffer,
  unsigned32            size
)
{
  return _Message_queue_Submit(id, buffer, size, MESSAGE_QUEUE_URGENT_REQUEST);
}

/*PAGE
 *
 *  rtems_message_queue_broadcast
 *
 *  This directive sends a message for every thread waiting on the queue
 *  designated by id.
 *
 *  Input parameters:
 *    id     - pointer to message queue
 *    buffer - pointer to message buffer
 *    count  - pointer to area to store number of threads made ready
 *
 *  Output parameters:
 *    count             - number of threads made ready
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_message_queue_broadcast(
  Objects_Id            id,
  void                 *buffer,
  unsigned32            size,
  unsigned32           *count
)
{
  register Message_queue_Control *the_message_queue;
  Objects_Locations               location;
  Thread_Control                 *the_thread;
  unsigned32                      number_broadcasted;

  the_message_queue = _Message_queue_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:
      _Thread_Executing->Wait.return_argument = count;

      return
        _Message_queue_MP_Send_request_packet(
          MESSAGE_QUEUE_MP_BROADCAST_REQUEST,
          id,
          buffer,
          &size,
          0,                               /* option_set not used */
          MPCI_DEFAULT_TIMEOUT
        );

    case OBJECTS_LOCAL:
    {
      Thread_Wait_information *waitp;
      unsigned32 constrained_size;

      number_broadcasted = 0;
      while ( (the_thread =
                 _Thread_queue_Dequeue(&the_message_queue->Wait_queue)) ) {
        waitp = &the_thread->Wait;
        number_broadcasted += 1;

        constrained_size = size;
        if (size > the_message_queue->maximum_message_size)
            constrained_size = the_message_queue->maximum_message_size;

        _Message_queue_Copy_buffer(buffer,
                                   waitp->return_argument,
                                   constrained_size);

        *(rtems_unsigned32 *)the_thread->Wait.return_argument_1 = size;
        
        if ( !_Objects_Is_local_id( the_thread->Object.id ) ) {
          the_thread->receive_packet->return_code = RTEMS_SUCCESSFUL;

          _Message_queue_MP_Send_response_packet(
            MESSAGE_QUEUE_MP_RECEIVE_RESPONSE,
            id,
            the_thread
          );
        }
      }
      _Thread_Enable_dispatch();
      *count = number_broadcasted;
      return RTEMS_SUCCESSFUL;
    }

    default:
      return RTEMS_INTERNAL_ERROR;
  }
}

/*PAGE
 *
 *  rtems_message_queue_receive
 *
 *  This directive dequeues a message from the designated message queue
 *  and copies it into the requesting thread's buffer.
 *
 *  Input parameters:
 *    id         - queue id
 *    buffer     - pointer to message buffer
 *    option_set - options on receive
 *    timeout    - number of ticks to wait
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_message_queue_receive(
  Objects_Id            id,
  void                 *buffer,
  unsigned32           *size_p,
  unsigned32            option_set,
  rtems_interval        timeout
)
{
  register Message_queue_Control *the_message_queue;
  Objects_Locations               location;

  the_message_queue = _Message_queue_Get( id, &location );
  switch ( location ) {

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_REMOTE:
      return _Message_queue_MP_Send_request_packet(
          MESSAGE_QUEUE_MP_RECEIVE_REQUEST,
          id,
          buffer,
          size_p,
          option_set,
          timeout
        );

    case OBJECTS_LOCAL:
      if ( ! _Message_queue_Seize(the_message_queue,
                                  option_set,
                                  buffer,
                                  size_p))
      {
        _Thread_queue_Enqueue( &the_message_queue->Wait_queue, timeout );
      }
      _Thread_Enable_dispatch();
      return _Thread_Executing->Wait.return_code;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_message_queue_flush
 *
 *  This directive removes all pending messages from a queue and returns
 *  the number of messages removed.  If no messages were present then
 *  a count of zero is returned.
 *
 *  Input parameters:
 *    id    - queue id
 *    count - return area for count
 *
 *  Output parameters:
 *    count             - number of messages removed ( 0 = empty queue )
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_message_queue_flush(
  Objects_Id  id,
  unsigned32 *count
)
{
  register Message_queue_Control *the_message_queue;
  Objects_Locations               location;

  the_message_queue = _Message_queue_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:
      _Thread_Executing->Wait.return_argument = count;

      return
        _Message_queue_MP_Send_request_packet(
          MESSAGE_QUEUE_MP_FLUSH_REQUEST,
          id,
          0,                               /* buffer not used */
          0,                               /* size_p */
          0,                               /* option_set not used */
          MPCI_DEFAULT_TIMEOUT
        );

    case OBJECTS_LOCAL:
      if ( the_message_queue->number_of_pending_messages != 0 )
        *count = _Message_queue_Flush_support( the_message_queue );
      else
        *count = 0;
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  _Message_queue_Seize
 *
 *  This kernel routine dequeues a message, copies the message buffer to
 *  a given destination buffer, and frees the message buffer to the
 *  inactive message pool.
 *
 *  Input parameters:
 *    the_message_queue - pointer to message queue
 *    option_set        - options on receive
 *    the_buffer        - pointer to message buffer to be filled
 *
 *  Output parameters:
 *    TRUE  - if message received or RTEMS_NO_WAIT and no message
 *    FALSE - if thread is to block
 *
 *  NOTE: Dependent on BUFFER_LENGTH
 *
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 */

boolean _Message_queue_Seize(
  Message_queue_Control  *the_message_queue,
  rtems_option            option_set,
  void                   *buffer,
  unsigned32             *size_p
)
{
  ISR_Level                     level;
  Message_queue_Buffer_control *the_message;
  Thread_Control               *executing;

  executing = _Thread_Executing;
  executing->Wait.return_code = RTEMS_SUCCESSFUL;
  _ISR_Disable( level );
  if ( the_message_queue->number_of_pending_messages != 0 ) {
    the_message_queue->number_of_pending_messages -= 1;

    the_message = _Message_queue_Get_pending_message( the_message_queue );
    _ISR_Enable( level );
    *size_p = the_message->Contents.size;
    _Message_queue_Copy_buffer( the_message->Contents.buffer, buffer, *size_p );
    _Message_queue_Free_message_buffer(the_message_queue, the_message );
    return( TRUE );
  }

  if ( _Options_Is_no_wait( option_set ) ) {
    _ISR_Enable( level );
    executing->Wait.return_code = RTEMS_UNSATISFIED;
    return( TRUE );
  }

  the_message_queue->Wait_queue.sync = TRUE;
  executing->Wait.queue              = &the_message_queue->Wait_queue;
  executing->Wait.id                 = the_message_queue->Object.id;
  executing->Wait.option             = option_set;
  executing->Wait.return_argument    = (void *)buffer;
  executing->Wait.return_argument_1  = (void *)size_p;
  _ISR_Enable( level );
  return FALSE;
}

/*PAGE
 *
 *  _Message_queue_Flush_support
 *
 *  This message manager routine removes all messages from a message queue
 *  and returns them to the inactive message pool.
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

unsigned32 _Message_queue_Flush_support(
  Message_queue_Control *the_message_queue
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
 *  _Message_queue_Submit
 *
 *  This routine implements the directives q_send and q_urgent.  It
 *  processes a message that is to be submitted to the designated
 *  message queue.  The message will either be processed as a send
 *  send message which it will be inserted at the rear of the queue
 *  or it will be processed as an urgent message which will be inserted
 *  at the front of the queue.
 *
 *  Input parameters:
 *    id          - pointer to message queue
 *    buffer      - pointer to message buffer
 *    size        - size in bytes of message to send
 *    submit_type - send or urgent message
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code _Message_queue_Submit(
  Objects_Id                  id,
  void                       *buffer,
  unsigned32                  size,
  Message_queue_Submit_types  submit_type
)
{
  register Message_queue_Control *the_message_queue;
  Objects_Locations               location;
  Thread_Control                 *the_thread;
  Message_queue_Buffer_control   *the_message;

  the_message_queue = _Message_queue_Get( id, &location );
  switch ( location )
  {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_REMOTE:
      switch ( submit_type ) {
        case MESSAGE_QUEUE_SEND_REQUEST:
          return
            _Message_queue_MP_Send_request_packet(
              MESSAGE_QUEUE_MP_SEND_REQUEST,
              id,
              buffer,
              &size,
              0,                               /* option_set */
              MPCI_DEFAULT_TIMEOUT
            );

        case MESSAGE_QUEUE_URGENT_REQUEST:
          return
            _Message_queue_MP_Send_request_packet(
              MESSAGE_QUEUE_MP_URGENT_REQUEST,
              id,
              buffer,
              &size,
              0,                               /* option_set */
              MPCI_DEFAULT_TIMEOUT
            );
      }

    case OBJECTS_LOCAL:
      if (size > the_message_queue->maximum_message_size)
      {
          _Thread_Enable_dispatch();
          return RTEMS_INVALID_SIZE;
      }

      /*
       * Is there a thread currently waiting on this message queue?
       */
      
      the_thread = _Thread_queue_Dequeue( &the_message_queue->Wait_queue );
      if ( the_thread )
      {
        _Message_queue_Copy_buffer(
          buffer,
          the_thread->Wait.return_argument,
          size
        );
        *(rtems_unsigned32 *)the_thread->Wait.return_argument_1 = size;
        
        if ( !_Objects_Is_local_id( the_thread->Object.id ) ) {
          the_thread->receive_packet->return_code = RTEMS_SUCCESSFUL;

          _Message_queue_MP_Send_response_packet(
            MESSAGE_QUEUE_MP_RECEIVE_RESPONSE,
            id,
            the_thread
          );

        }
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }

      /*
       * No one waiting on this one currently.
       * Allocate a message buffer and store it away
       */

      if ( the_message_queue->number_of_pending_messages ==
           the_message_queue->maximum_pending_messages ) {
        _Thread_Enable_dispatch();
        return RTEMS_TOO_MANY;
      }

      the_message = _Message_queue_Allocate_message_buffer(the_message_queue);
      if ( the_message == 0) {
        _Thread_Enable_dispatch();
        return RTEMS_UNSATISFIED;
      }

      _Message_queue_Copy_buffer( buffer, the_message->Contents.buffer, size );
      the_message->Contents.size = size;
      
      the_message_queue->number_of_pending_messages += 1;

      switch ( submit_type ) {
        case MESSAGE_QUEUE_SEND_REQUEST:
          _Message_queue_Append( the_message_queue, the_message );
          break;
        case MESSAGE_QUEUE_URGENT_REQUEST:
          _Message_queue_Prepend( the_message_queue, the_message );
          break;
      }

      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
          
    default:
      return RTEMS_INTERNAL_ERROR;       /* And they were such nice boys, too! */
  }
}
