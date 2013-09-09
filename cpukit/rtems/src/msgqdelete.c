/**
 *  @file
 *
 *  @brief RTEMS Delete Message Queue
 *  @ingroup ClassicMessageQueue
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/coremsgimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/messageimpl.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/support.h>

rtems_status_code rtems_message_queue_delete(
  rtems_id id
)
{
  register Message_queue_Control *the_message_queue;
  Objects_Locations               location;

  the_message_queue = _Message_queue_Get( id, &location );
  switch ( location ) {

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
      _Objects_Put( &the_message_queue->Object );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
