/*
 *  Message Queue Manager
 *
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

#if defined(RTEMS_MULTIPROCESSING)
#define MESSAGE_QUEUE_MP_HANDLER _Message_queue_Core_message_queue_mp_support
#else
#define MESSAGE_QUEUE_MP_HANDLER NULL
#endif

rtems_status_code _Message_queue_Submit(
  Objects_Id                  id,
  void                       *buffer,
  unsigned32                  size,
  Message_queue_Submit_types  submit_type
)
{
  register Message_queue_Control  *the_message_queue;
  Objects_Locations                location;

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
          _CORE_message_queue_Send(
            &the_message_queue->message_queue,
            buffer,
            size,
            id,
            MESSAGE_QUEUE_MP_HANDLER,
            FALSE,   /* sender does not block */
            0        /* no timeout */
          );
          break;
        case MESSAGE_QUEUE_URGENT_REQUEST:
          _CORE_message_queue_Urgent(
            &the_message_queue->message_queue,
            buffer,
            size,
            id,
            MESSAGE_QUEUE_MP_HANDLER,
            FALSE,   /* sender does not block */
            0        /* no timeout */
          );
          break;
        default:
          return RTEMS_INTERNAL_ERROR;   /* should never get here */
      }

      _Thread_Enable_dispatch();
      return _Message_queue_Translate_core_message_queue_return_code(
        _Thread_Executing->Wait.return_code
      );
          
  }
  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
