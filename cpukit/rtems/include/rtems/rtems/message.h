/*  message.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Message Queue Manager.  This manager provides a mechanism for
 *  communication and synchronization between tasks using messages.
 *
 *  Directives provided are:
 *
 *     + create a queue
 *     + get ID of a queue
 *     + delete a queue
 *     + put a message at the rear of a queue
 *     + put a message at the front of a queue
 *     + broadcast N messages to a queue
 *     + receive message from a queue
 *     + flush all messages on a queue
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

#ifndef __RTEMS_MESSAGE_QUEUE_h
#define __RTEMS_MESSAGE_QUEUE_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>
#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/threadq.h>
#include <rtems/score/coremsg.h>

/*
 *  The following enumerated type details the modes in which a message
 *  may be submitted to a message queue.  The message may be posted
 *  in a send or urgent fashion.
 */
 
typedef enum {
  MESSAGE_QUEUE_SEND_REQUEST   = 0,
  MESSAGE_QUEUE_URGENT_REQUEST = 1
}  Message_queue_Submit_types;

/*
 *  The following records define the control block used to manage
 *  each message queue.
 */

typedef struct {
  Objects_Control             Object;
  rtems_attribute             attribute_set;
  CORE_message_queue_Control  message_queue;
}   Message_queue_Control;

/*
 *  The following defines the information control block used to
 *  manage this class of objects.
 */

RTEMS_EXTERN Objects_Information  _Message_queue_Information;

/*
 *  _Message_queue_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _Message_queue_Manager_initialization(
  unsigned32 maximum_message_queues
);

/*
 *  rtems_message_queue_create
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_message_queue_create directive.  The
 *  message queue will have the name name.  If the attribute_set indicates
 *  that the message queue is to be limited in the number of messages
 *  that can be outstanding, then count indicates the maximum number of
 *  messages that will be held.  It returns the id of the created
 *  message queue in ID.
 */

rtems_status_code rtems_message_queue_create(
  rtems_name       name,
  unsigned32       count,
  unsigned32       max_message_size,
  rtems_attribute  attribute_set,
  Objects_Id      *id
);

/*
 *  rtems_message_queue_ident
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_message_queue_ident directive.
 *  This directive returns the message queue ID associated with NAME.
 *  If more than one message queue is named name, then the message
 *  queue to which the ID belongs is arbitrary.  node indicates the
 *  extent of the search for the ID of the message queue named name.
 *  The search can be limited to a particular node or allowed to
 *  encompass all nodes.
 */

rtems_status_code rtems_message_queue_ident(
  rtems_name    name,
  unsigned32    node,
  Objects_Id   *id
);

/*
 *  rtems_message_queue_delete
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_message_queue_delete directive.  The
 *  message queue indicated by ID is deleted.
 */

rtems_status_code rtems_message_queue_delete(
  Objects_Id id
);

/*
 *  rtems_message_queue_send
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_message_queue_send directive.
 *  This directive sends the message buffer to the message queue
 *  indicated by ID.  If one or more tasks is blocked waiting
 *  to receive a message from this message queue, then one will
 *  receive the message.  The task selected to receive the
 *  message is based on the task queue discipline algorithm in
 *  use by this particular message queue.  If no tasks are waiting,
 *  then the message buffer will be placed at the REAR of the
 *  chain of pending messages for this message queue.
 */

rtems_status_code rtems_message_queue_send(
  Objects_Id            id,
  void                 *buffer,
  unsigned32            size
);

/*
 *  rtems_message_queue_urgent
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_message_queue_urgent directive.
 *  This directive has the same behavior as rtems_message_queue_send
 *  except that if no tasks are waiting, the message buffer will
 *  be placed at the FRONT of the chain of pending messages rather
 *  than at the REAR.
 */

rtems_status_code rtems_message_queue_urgent(
  Objects_Id            id,
  void                 *buffer,
  unsigned32            size
);

/*
 *  rtems_message_queue_broadcast
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_message_queue_broadcast directive.
 *  This directive sends the message buffer to all of the tasks blocked
 *  waiting for a message on the message queue indicated by ID.  
 *  If no tasks are waiting, then the message buffer will not be queued.
 */

rtems_status_code rtems_message_queue_broadcast(
  Objects_Id            id,
  void                 *buffer,
  unsigned32            size,
  unsigned32           *count
);

/*
 *  rtems_message_queue_receive
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_message_queue_receive directive.
 *  This directive is invoked when the calling task wishes to receive
 *  a message from the message queue indicated by ID.  The received
 *  message is to be placed in buffer.  If no messages are outstanding
 *  and the option_set indicates that the task is willing to block,
 *  then the task will be blocked until a message arrives or until,
 *  optionally, timeout clock ticks have passed.
 */

rtems_status_code rtems_message_queue_receive(
  Objects_Id            id,
  void                 *buffer,
  unsigned32           *size,
  unsigned32            option_set,
  rtems_interval        timeout
);

/*
 *  rtems_message_queue_flush
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_message_queue_flush directive.
 *  This directive takes all outstanding messages for the message
 *  queue indicated by ID and returns them to the inactive message
 *  chain.  The number of messages flushed is returned in COUNT.
 */

rtems_status_code rtems_message_queue_flush(
  Objects_Id  id,
  unsigned32 *count
);

/*
 *  rtems_message_queue_get_number_pending
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_message_queue_get_number_pending
 *  directive.  This directive returns the number of pending 
 *  messages for the message queue indicated by ID 
 *  chain.  The number of messages pending is returned in COUNT.
 */

rtems_status_code rtems_message_queue_get_number_pending(
  Objects_Id  id,
  unsigned32 *count
);


/*
 *  _Message_queue_Submit
 *
 *  DESCRIPTION:
 *
 *  This routine implements the directives rtems_message_queue_send
 *  and rtems_message_queue_urgent.  It processes a message that is
 *  to be submitted to the designated message queue.  The message will
 *  either be processed as a send send message which it will be inserted
 *  at the rear of the queue or it will be processed as an urgent message
 *  which will be inserted at the front of the queue.
 */
 
rtems_status_code _Message_queue_Submit(
  Objects_Id                  id,
  void                       *buffer,
  unsigned32                  size,
  Message_queue_Submit_types  submit_type
);

/*
 *  _Message_queue_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a message queue control block from
 *  the inactive chain of free message queue control blocks.
 */

Message_queue_Control *_Message_queue_Allocate (
    unsigned32          count,
    unsigned32          max_message_size
);

/*
 *  _Message_queue_Translate_core_message_queue_return_code
 *
 *  DESCRIPTION:
 *
 *  This function returns a RTEMS status code based on the core message queue
 *  status code specified.
 */
 
rtems_status_code _Message_queue_Translate_core_message_queue_return_code (
  unsigned32 the_message_queue_status
);

/*
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
);
#endif

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/message.inl>
#endif
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/msgmp.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
