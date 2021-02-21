/**
 * @file
 *
 * @ingroup RTEMSImplClassicMessage
 *
 * @brief This source file contains the implementation of
 *   rtems_message_queue_construct().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/messageimpl.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/rtems/support.h>
#include <rtems/score/coremsgimpl.h>
#include <rtems/sysinit.h>

static void *_Message_queue_Get_buffers(
  CORE_message_queue_Control *the_message_queue,
  size_t                      size,
  const void                 *arg
)
{
  const rtems_message_queue_config *config;

  config = arg;

  if ( config->storage_size != size ) {
    return NULL;
  }

  the_message_queue->free_message_buffers = config->storage_free;
  return config->storage_area;
}

rtems_status_code rtems_message_queue_construct(
  const rtems_message_queue_config *config,
  rtems_id                         *id
)
{
  return _Message_queue_Create( config, id, _Message_queue_Get_buffers );
}

rtems_status_code _Message_queue_Create(
  const rtems_message_queue_config    *config,
  rtems_id                            *id,
  CORE_message_queue_Allocate_buffers  allocate_buffers
)
{
  Message_queue_Control          *the_message_queue;
  CORE_message_queue_Disciplines  discipline;
  Status_Control                  status;
#if defined(RTEMS_MULTIPROCESSING)
  bool                            is_global;
#endif

  if ( !rtems_is_name_valid( config->name ) ) {
    return RTEMS_INVALID_NAME;
  }

  if ( id == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( config->maximum_pending_messages == 0 ) {
    return RTEMS_INVALID_NUMBER;
  }

  if ( config->maximum_message_size == 0 ) {
    return RTEMS_INVALID_SIZE;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( _System_state_Is_multiprocessing ) {
    is_global = _Attributes_Is_global( config->attributes );
  } else {
    is_global = false;
  }

#if 1
  /*
   * I am not 100% sure this should be an error.
   * It seems reasonable to create a que with a large max size,
   * and then just send smaller msgs from remote (or all) nodes.
   */
  if ( is_global ) {
    size_t max_packet_payload_size = _MPCI_table->maximum_packet_size
      - MESSAGE_QUEUE_MP_PACKET_SIZE;

    if ( config->maximum_message_size > max_packet_payload_size ) {
      return RTEMS_INVALID_SIZE;
    }
  }
#endif
#endif

  the_message_queue = _Message_queue_Allocate();

  if ( !the_message_queue ) {
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if (
    is_global
      && !_Objects_MP_Allocate_and_open(
            &_Message_queue_Information,
            config->name,
            the_message_queue->Object.id,
            false
          )
  ) {
    _Message_queue_Free( the_message_queue );
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

  the_message_queue->is_global = is_global;
#endif

  if ( _Attributes_Is_priority( config->attributes ) ) {
    discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY;
  } else {
    discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO;
  }

  status = _CORE_message_queue_Initialize(
    &the_message_queue->message_queue,
    discipline,
    config->maximum_pending_messages,
    config->maximum_message_size,
    allocate_buffers,
    config
  );

  if ( status != STATUS_SUCCESSFUL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( is_global )
        _Objects_MP_Close(
          &_Message_queue_Information, the_message_queue->Object.id);
#endif

    _Message_queue_Free( the_message_queue );
    _Objects_Allocator_unlock();
    return _Status_Get( status );
  }

  *id = _Objects_Open_u32(
    &_Message_queue_Information,
    &the_message_queue->Object,
    config->name
  );

#if defined(RTEMS_MULTIPROCESSING)
  if ( is_global )
    _Message_queue_MP_Send_process_packet(
      MESSAGE_QUEUE_MP_ANNOUNCE_CREATE,
      the_message_queue->Object.id,
      config->name,
      0
    );
#endif

  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}

static void _Message_queue_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Message_queue_Information);
}

RTEMS_SYSINIT_ITEM(
  _Message_queue_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_MESSAGE_QUEUE,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
