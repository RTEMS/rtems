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
#include <rtems/rtems/status.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/message.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/interr.h>

void _Message_queue_Manager_initialization(
  unsigned32 maximum_message_queues
)
{
}

rtems_status_code rtems_message_queue_create(
  rtems_name          name,
  unsigned32          count,
  unsigned32          max_message_size,
  rtems_attribute     attribute_set,
  Objects_Id         *id
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_message_queue_ident(
  rtems_name    name,
  unsigned32    node,
  Objects_Id   *id
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_message_queue_delete(
  Objects_Id id
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_message_queue_send(
  Objects_Id            id,
  void                 *buffer,
  unsigned32            size
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_message_queue_urgent(
  Objects_Id            id,
  void                 *buffer,
  unsigned32            size
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_message_queue_broadcast(
  Objects_Id            id,
  void                 *buffer,
  unsigned32            size,
  unsigned32           *count
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_message_queue_receive(
  Objects_Id            id,
  void                 *buffer,
  unsigned32           *size_p,
  unsigned32            option_set,
  rtems_interval        timeout
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_message_queue_flush(
  Objects_Id  id,
  unsigned32 *count
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

unsigned32 _Message_queue_Flush_support(
  Message_queue_Control *the_message_queue
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
  return 0;
}

boolean _Message_queue_Seize(
  Message_queue_Control  *the_message_queue,
  rtems_option            option_set,
  void                   *buffer,
  unsigned32             *size_p
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
  _Thread_Executing->Wait.return_code = RTEMS_UNSATISFIED;
  return TRUE;
}

rtems_status_code _Message_queue_Submit(
  Objects_Id                  id,
  void                       *buffer,
  unsigned32                  size,
  Message_queue_Submit_types  submit_type
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}
