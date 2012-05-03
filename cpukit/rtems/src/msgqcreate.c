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
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

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

/*
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
  rtems_name       name,
  uint32_t         count,
  size_t           max_message_size,
  rtems_attribute  attribute_set,
  rtems_id        *id
)
{
  register Message_queue_Control *the_message_queue;
  CORE_message_queue_Attributes   the_msgq_attributes;
#if defined(RTEMS_MULTIPROCESSING)
  bool                            is_global;
  size_t                          max_packet_payload_size;
#endif

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( !id )
    return RTEMS_INVALID_ADDRESS;

#if defined(RTEMS_MULTIPROCESSING)
  if ( (is_global = _Attributes_Is_global( attribute_set ) ) &&
       !_System_state_Is_multiprocessing )
    return RTEMS_MP_NOT_CONFIGURED;
#endif

  if ( count == 0 )
      return RTEMS_INVALID_NUMBER;

  if ( max_message_size == 0 )
      return RTEMS_INVALID_SIZE;

#if defined(RTEMS_MULTIPROCESSING)
#if 1
  /*
   * I am not 100% sure this should be an error.
   * It seems reasonable to create a que with a large max size,
   * and then just send smaller msgs from remote (or all) nodes.
   */

  max_packet_payload_size = _MPCI_table->maximum_packet_size
    - MESSAGE_QUEUE_MP_PACKET_SIZE;
  if ( is_global && max_packet_payload_size < max_message_size )
    return RTEMS_INVALID_SIZE;
#endif
#endif

  _Thread_Disable_dispatch();              /* protects object pointer */

  the_message_queue = _Message_queue_Allocate();

  if ( !the_message_queue ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( is_global &&
    !( _Objects_MP_Allocate_and_open( &_Message_queue_Information,
                              name, the_message_queue->Object.id, false ) ) ) {
    _Message_queue_Free( the_message_queue );
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }
#endif

  the_message_queue->attribute_set = attribute_set;

  if (_Attributes_Is_priority( attribute_set ) )
    the_msgq_attributes.discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY;
  else
    the_msgq_attributes.discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO;

  if ( ! _CORE_message_queue_Initialize(
           &the_message_queue->message_queue,
           &the_msgq_attributes,
           count,
           max_message_size
         ) ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( is_global )
        _Objects_MP_Close(
          &_Message_queue_Information, the_message_queue->Object.id);
#endif

    _Message_queue_Free( the_message_queue );
    _Thread_Enable_dispatch();
    return RTEMS_UNSATISFIED;
  }

  _Objects_Open(
    &_Message_queue_Information,
    &the_message_queue->Object,
    (Objects_Name) name
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
