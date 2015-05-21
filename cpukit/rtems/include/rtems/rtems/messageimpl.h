/**
 * @file rtems/rtems/message.inl
 *
 *  This include file contains the static inline implementation of all
 *  inlined routines in the Message Manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_MESSAGEIMPL_H
#define _RTEMS_RTEMS_MESSAGEIMPL_H

#include <rtems/rtems/message.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/coremsgimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicMessageQueueImpl Classic Message Queue Implementation
 *
 *  @ingroup ClassicMessageQueue
 *
 *  @{
 */

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_MESSAGE_EXTERN
#define RTEMS_MESSAGE_EXTERN extern
#endif

/**
 *  The following enumerated type details the modes in which a message
 *  may be submitted to a message queue.  The message may be posted
 *  in a send or urgent fashion.
 */
typedef enum {
  /**
   *  This value indicates the user wants to send the message using the
   *  normal message insertion protocol (FIFO or priority).
   */
  MESSAGE_QUEUE_SEND_REQUEST   = 0,
  /**
   *  This value indicates the user considers the message to be urgent
   *  and wants it inserted at the head of the pending message queue.
   */
  MESSAGE_QUEUE_URGENT_REQUEST = 1
}  Message_queue_Submit_types;

/**
 *  The following defines the information control block used to
 *  manage this class of objects.
 */
RTEMS_MESSAGE_EXTERN Objects_Information  _Message_queue_Information;

/**
 *  @brief Message Queue Manager Initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Message_queue_Manager_initialization(void);

/**
 *  @brief Message_queue_Submit
 *
 *  This routine implements the directives rtems_message_queue_send
 *  and rtems_message_queue_urgent.  It processes a message that is
 *  to be submitted to the designated message queue.  The message will
 *  either be processed as a send send message which it will be inserted
 *  at the rear of the queue or it will be processed as an urgent message
 *  which will be inserted at the front of the queue.
 */
rtems_status_code _Message_queue_Submit(
  rtems_id                    id,
  const void                 *buffer,
  size_t                      size,
  Message_queue_Submit_types  submit_type
);

/**
 * @brief Message queue Translate Core Message Queue Return Code
 *
 * This function returns a RTEMS status code based on
 * @a the_message_queue_status.
 *
 * @param[in] the_message_queue_status is the status code to translate
 *
 * @retval translated RTEMS status code
 */
rtems_status_code _Message_queue_Translate_core_message_queue_return_code (
  uint32_t   the_message_queue_status
);

/**
 *  @brief Deallocates a message queue control block into
 *  the inactive chain of free message queue control blocks.
 *
 *  This routine deallocates a message queue control block into
 *  the inactive chain of free message queue control blocks.
 */
RTEMS_INLINE_ROUTINE void _Message_queue_Free (
  Message_queue_Control *the_message_queue
)
{
  _Objects_Free( &_Message_queue_Information, &the_message_queue->Object );
}

/**
 *  @brief Maps message queue IDs to message queue control blocks.
 *
 *  This function maps message queue IDs to message queue control
 *  blocks.  If ID corresponds to a local message queue, then it
 *  returns the_message_queue control pointer which maps to ID
 *  and location is set to OBJECTS_LOCAL.  If the message queue ID is
 *  global and resides on a remote node, then location is set
 *  to OBJECTS_REMOTE, and the_message_queue is undefined.
 *  Otherwise, location is set to OBJECTS_ERROR and
 *  the_message_queue is undefined.
 */
RTEMS_INLINE_ROUTINE Message_queue_Control *_Message_queue_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Message_queue_Control *)
     _Objects_Get( &_Message_queue_Information, id, location );
}

RTEMS_INLINE_ROUTINE Message_queue_Control *
_Message_queue_Get_interrupt_disable(
  Objects_Id         id,
  Objects_Locations *location,
  ISR_lock_Context  *lock_context
)
{
  return (Message_queue_Control *) _Objects_Get_isr_disable(
    &_Message_queue_Information,
    id,
    location,
    lock_context
  );
}

RTEMS_INLINE_ROUTINE Message_queue_Control *_Message_queue_Allocate( void )
{
  return (Message_queue_Control *)
    _Objects_Allocate( &_Message_queue_Information );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/msgmp.h>
#endif

#endif
/* end of include file */
