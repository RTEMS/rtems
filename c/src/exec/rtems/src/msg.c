/*
 *  Message Queue Manager
 *
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
#include <rtems/score/sysstate.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/rtems/status.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/support.h>

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

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Register_packet_processor(
    MP_PACKET_MESSAGE_QUEUE,
    _Message_queue_MP_Process_packet
  );
#endif

}

/*PAGE
 *
 *  _Message_queue_Allocate
 *
 *  Allocate a message queue and the space for its messages
 * 
 *  Input parameters:
 *    the_message_queue - the message queue to allocate message buffers
 *    count             - maximum message and reserved buffer count
 *    max_message_size  - maximum size of each message
 *
 *  Output parameters:
 *    the_message_queue - set if successful, NULL otherwise
 */

Message_queue_Control *_Message_queue_Allocate (
  unsigned32           count,
  unsigned32           max_message_size
)
{
  return 
    (Message_queue_Control *)_Objects_Allocate(&_Message_queue_Information);

}

/*PAGE
 *
 *  rtems_message_queue_create
 *
 *  This directive creates a message queue by allocating and initializing
 *  a message queue data structure.
 *
 *  Input parameters:
 *    name             - user defined queue name
 *    count            - maximum message and reserved buffer count
 *    max_message_size - maximum size of each message
 *    attribute_set    - process method
 *    id               - pointer to queue
 *
 *  Output parameters:
 *    id                - queue id
 *    RTEMS_SUCCESSFUL  - if successful
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
  CORE_message_queue_Attributes   the_message_queue_attributes;
  void                           *handler;
#if defined(RTEMS_MULTIPROCESSING)
  boolean                         is_global;
#endif

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

#if defined(RTEMS_MULTIPROCESSING)
  if ( (is_global = _Attributes_Is_global( attribute_set ) ) && 
       !_System_state_Is_multiprocessing )
    return RTEMS_MP_NOT_CONFIGURED;
#endif

  if (count == 0)
      return RTEMS_INVALID_NUMBER;

  if (max_message_size == 0)
      return RTEMS_INVALID_SIZE;

#if defined(RTEMS_MULTIPROCESSING)
#if 1
  /*
   * I am not 100% sure this should be an error.
   * It seems reasonable to create a que with a large max size,
   * and then just send smaller msgs from remote (or all) nodes.
   */
  
  if ( is_global && (_MPCI_table->maximum_packet_size < max_message_size) )
    return RTEMS_INVALID_SIZE;
#endif
#endif
       
  _Thread_Disable_dispatch();              /* protects object pointer */

  the_message_queue = _Message_queue_Allocate( count, max_message_size );

  if ( !the_message_queue ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( is_global &&
    !( _Objects_MP_Allocate_and_open( &_Message_queue_Information,
                              name, the_message_queue->Object.id, FALSE ) ) ) {
    _Message_queue_Free( the_message_queue );
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }
#endif

  the_message_queue->attribute_set = attribute_set;

  if (_Attributes_Is_priority( attribute_set ) )
    the_message_queue_attributes.discipline = 
                                      CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY;
  else
    the_message_queue_attributes.discipline =
                                      CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO;

  handler = NULL;
#if defined(RTEMS_MULTIPROCESSING)
  handler = _Message_queue_MP_Send_extract_proxy;
#endif

  if ( ! _CORE_message_queue_Initialize(
           &the_message_queue->message_queue,
           OBJECTS_RTEMS_MESSAGE_QUEUES,
           &the_message_queue_attributes,
           count,
           max_message_size,
           handler ) ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( is_global )
        _Objects_MP_Close(
          &_Message_queue_Information, the_message_queue->Object.id);
#endif

    _Message_queue_Free( the_message_queue );
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

  _Objects_Open(
    &_Message_queue_Information,
    &the_message_queue->Object,
    &name
  );

  *id = the_message_queue->Object.id;

#if defined(RTEMS_MULTIPROCESSING)
  if ( is_global )
    _Message_queue_MP_Send_process_packet(
      MESSAGE_QUEUE_MP_ANNOUNCE_CREATE,
      the_message_queue->Object.id,
      name,
      0
    );
#endif

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

    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      _Objects_Close( &_Message_queue_Information,
                      &the_message_queue->Object );

      _CORE_message_queue_Close(
        &the_message_queue->message_queue,
#if defined(RTEMS_MULTIPROCESSING)
        _Message_queue_MP_Send_object_was_deleted,
#else
        NULL,
#endif
        CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED
      );

      _Message_queue_Free( the_message_queue );

#if defined(RTEMS_MULTIPROCESSING)
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
#endif

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
 *    size   - size of message to sent urgently
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
  return( _Message_queue_Submit(id, buffer, size, MESSAGE_QUEUE_SEND_REQUEST) );
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
 *    size   - size of message to sent urgently
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_message_queue_urgent(
  Objects_Id            id,
  void                 *buffer,
  unsigned32            size
)
{
  return(_Message_queue_Submit(id, buffer, size, MESSAGE_QUEUE_URGENT_REQUEST));
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
 *    size   - size of message to broadcast
 *    count  - pointer to area to store number of threads made ready
 *
 *  Output parameters:
 *    count             - number of threads made ready
 *    RTEMS_SUCCESSFUL  - if successful
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
  CORE_message_queue_Status       core_status;

  the_message_queue = _Message_queue_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
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
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      core_status = _CORE_message_queue_Broadcast(
                      &the_message_queue->message_queue,
                      buffer,
                      size,
                      id,
#if defined(RTEMS_MULTIPROCESSING)
                      _Message_queue_Core_message_queue_mp_support,
#else
                      NULL,
#endif
                      count
                    );
                      
      _Thread_Enable_dispatch();
      return 
        _Message_queue_Translate_core_message_queue_return_code( core_status );

  }
  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
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
 *    size       - size of message receive
 *    option_set - options on receive
 *    timeout    - number of ticks to wait
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_message_queue_receive(
  Objects_Id            id,
  void                 *buffer,
  unsigned32           *size,
  unsigned32            option_set,
  rtems_interval        timeout
)
{
  register Message_queue_Control *the_message_queue;
  Objects_Locations               location;
  boolean                         wait;

  the_message_queue = _Message_queue_Get( id, &location );
  switch ( location ) {

    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      return _Message_queue_MP_Send_request_packet(
          MESSAGE_QUEUE_MP_RECEIVE_REQUEST,
          id,
          buffer,
          size,
          option_set,
          timeout
        );
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      if ( _Options_Is_no_wait( option_set ) )
        wait = FALSE;
      else
        wait = TRUE;
 
      _CORE_message_queue_Seize(
        &the_message_queue->message_queue,
        the_message_queue->Object.id,
        buffer,
        size,
        wait,
        timeout
      );
      _Thread_Enable_dispatch();
      return( _Message_queue_Translate_core_message_queue_return_code(
                  _Thread_Executing->Wait.return_code ) );

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
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Executing->Wait.return_argument = count;

      return
        _Message_queue_MP_Send_request_packet(
          MESSAGE_QUEUE_MP_FLUSH_REQUEST,
          id,
          0,                               /* buffer not used */
          0,                               /* size */
          0,                               /* option_set not used */
          MPCI_DEFAULT_TIMEOUT
        );
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      *count = _CORE_message_queue_Flush( &the_message_queue->message_queue );
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_message_queue_get_number_pending
 *
 *  This directive returns the number of messages pending.
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

rtems_status_code rtems_message_queue_get_number_pending(
  Objects_Id  id,
  unsigned32 *count
)
{
  register Message_queue_Control *the_message_queue;
  Objects_Locations               location;

  the_message_queue = _Message_queue_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Executing->Wait.return_argument = count;

      return _Message_queue_MP_Send_request_packet(
          MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_REQUEST,
          id,
          0,                               /* buffer not used */
          0,                               /* size */
          0,                               /* option_set not used */
          MPCI_DEFAULT_TIMEOUT
        );
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      *count = the_message_queue->message_queue.number_of_pending_messages;
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  _Message_queue_Submit
 *
 *  This routine implements the directives rtems_message_queue_send
 *  and rtems_message_queue_urgent.  It processes a message that is
 *  to be submitted to the designated message queue.  The message will
 *  either be processed as a send send message which it will be inserted
 *  at the rear of the queue or it will be processed as an urgent message
 *  which will be inserted at the front of the queue.
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
  register Message_queue_Control  *the_message_queue;
  Objects_Locations                location;
  CORE_message_queue_Status        core_status;

  the_message_queue = _Message_queue_Get( id, &location );
  switch ( location )
  {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      switch ( submit_type ) {
        case MESSAGE_QUEUE_SEND_REQUEST:
          return _Message_queue_MP_Send_request_packet(
              MESSAGE_QUEUE_MP_SEND_REQUEST,
              id,
              buffer,
              &size,
              0,                               /* option_set */
              MPCI_DEFAULT_TIMEOUT
            );

        case MESSAGE_QUEUE_URGENT_REQUEST:
          return _Message_queue_MP_Send_request_packet(
              MESSAGE_QUEUE_MP_URGENT_REQUEST,
              id,
              buffer,
              &size,
              0,                               /* option_set */
              MPCI_DEFAULT_TIMEOUT
            );
      }
      break;
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      switch ( submit_type ) {
        case MESSAGE_QUEUE_SEND_REQUEST:
          core_status = _CORE_message_queue_Send(
                          &the_message_queue->message_queue,
                          buffer,
                          size,
                          id,
#if defined(RTEMS_MULTIPROCESSING)
                          _Message_queue_Core_message_queue_mp_support
#else
                          NULL
#endif
                        );
          break;
        case MESSAGE_QUEUE_URGENT_REQUEST:
          core_status = _CORE_message_queue_Urgent(
                          &the_message_queue->message_queue,
                          buffer,
                          size,
                          id,
#if defined(RTEMS_MULTIPROCESSING)
                          _Message_queue_Core_message_queue_mp_support
#else
                          NULL
#endif
                        );
          break;
        default:
          core_status = CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
          return RTEMS_INTERNAL_ERROR;   /* should never get here */
      }

      _Thread_Enable_dispatch();
      return _Message_queue_Translate_core_message_queue_return_code(
                core_status );
          
  }
  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  _Message_queue_Translate_core_message_queue_return_code
 *
 *  Input parameters:
 *    the_message_queue_status - message_queue status code to translate
 *
 *  Output parameters:
 *    rtems status code - translated RTEMS status code
 *
 */
 
rtems_status_code _Message_queue_Translate_core_message_queue_return_code (
  unsigned32 the_message_queue_status
)
{
  switch ( the_message_queue_status ) {
    case  CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL:
      return RTEMS_SUCCESSFUL;
    case  CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE:
      return RTEMS_INVALID_SIZE;
    case  CORE_MESSAGE_QUEUE_STATUS_TOO_MANY:
      return RTEMS_TOO_MANY;
    case CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED:
      return RTEMS_UNSATISFIED;
    case CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT:
      return RTEMS_UNSATISFIED;
    case CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED:
      return RTEMS_OBJECT_WAS_DELETED;
    case CORE_MESSAGE_QUEUE_STATUS_TIMEOUT:
      return RTEMS_TIMEOUT;
    case THREAD_STATUS_PROXY_BLOCKING:
      return RTEMS_PROXY_BLOCKING;
  }
  _Internal_error_Occurred(         /* XXX */
    INTERNAL_ERROR_RTEMS_API,
    TRUE,
    the_message_queue_status
  );
  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  _Message_queue_Core_message_queue_mp_support
 *
 *  Input parameters:
 *    the_thread - the remote thread the message was submitted to
 *    id         - id of the message queue
 *
 *  Output parameters: NONE
 */
 
#if defined(RTEMS_MULTIPROCESSING)
void  _Message_queue_Core_message_queue_mp_support (
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  the_thread->receive_packet->return_code = RTEMS_SUCCESSFUL;
 
  _Message_queue_MP_Send_response_packet(
    MESSAGE_QUEUE_MP_RECEIVE_RESPONSE,
    id,
    the_thread
  );
}
#endif
