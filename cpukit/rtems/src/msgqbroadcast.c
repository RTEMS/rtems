/*
 *  Message Queue Manager
 *
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
