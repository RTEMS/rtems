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

#ifndef __RTEMS_MESSAGE_QUEUE_h
#define __RTEMS_MESSAGE_QUEUE_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/chain.h>
#include <rtems/object.h>
#include <rtems/threadq.h>

/*
 *  The following defines the data types needed to manipulate
 *  the contents of message buffers.
 */

typedef struct {
  unsigned32 field1;
  unsigned32 field2;
  unsigned32 field3;
  unsigned32 field4;
}  Message_queue_Buffer;

/*
 *  The following records define the organization of a message
 *  buffer.
 */

typedef struct {
  Chain_Node           Node;
  Message_queue_Buffer Contents;
}   Message_queue_Buffer_control;

/*
 *  The following records define the control block used to manage
 *  each message queue.
 */

typedef struct {
  Objects_Control      Object;
  Thread_queue_Control Wait_queue;
  rtems_attribute   attribute_set;
  unsigned32           maximum_pending_messages;
  unsigned32           number_of_pending_messages;
  Chain_Control        Pending_messages;
}   Message_queue_Control;

/*
 *  The following defines the information control block used to
 *  manage this class of objects.
 */

EXTERN Objects_Information  _Message_queue_Information;

/*
 *  The following defines the data structures used to
 *  manage the pool of inactive message buffers.
 */

EXTERN Chain_Control _Message_queue_Inactive_messages;

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
 *  _Message_queue_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _Message_queue_Manager_initialization(
  unsigned32 maximum_message_queues,
  unsigned32 maximum_messages
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
  Objects_Name        name,
  unsigned32          count,
  rtems_attribute  attribute_set,
  Objects_Id         *id
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
  Objects_Name  name,
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
 *  then the message buffer will be placed at the rear of the
 *  chain of pending messages for this message queue.
 */

rtems_status_code rtems_message_queue_send(
  Objects_Id            id,
  void                 *buffer
);

/*
 *  rtems_message_queue_urgent
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
 *  then the message buffer will be placed at the rear of the
 *  chain of pending messages for this message queue.
 */

rtems_status_code rtems_message_queue_urgent(
  Objects_Id            id,
  void                 *buffer
);

/*
 *  rtems_message_queue_broadcast
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
 *  then the message buffer will be placed at the rear of the
 *  chain of pending messages for this message queue.
 */

rtems_status_code rtems_message_queue_broadcast(
  Objects_Id            id,
  void                 *buffer,
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
  unsigned32            option_set,
  rtems_interval     timeout
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
 *  _Message_queue_Copy_buffer
 *
 *  DESCRIPTION:
 *
 *  This routine copies the contents of the source message buffer
 *  to the destination message buffer.
 */

STATIC INLINE void _Message_queue_Copy_buffer (
  Message_queue_Buffer *source,
  Message_queue_Buffer *destination
);

/*
 *  _Message_queue_Seize
 *
 *  DESCRIPTION:
 *
 *  This routine attempts to receive a message from the_message_queue.
 *  If a message is available or if the RTEMS_NO_WAIT option is enabled in
 *  option_set, then the routine returns.  Otherwise, the calling task
 *  is blocked until a message is available.  If a message is returned
 *  to the task, then buffer will contain its contents.
 */

boolean _Message_queue_Seize(
  Message_queue_Control *the_message_queue,
  unsigned32             option_set,
  Message_queue_Buffer  *buffer
);

/*
 *  _Message_queue_Flush_support
 *
 *  DESCRIPTION:
 *
 *  This routine flushes all outstanding messages and returns
 *  them to the inactive message chain.
 */

unsigned32 _Message_queue_Flush_support(
  Message_queue_Control *the_message_queue
);

/*
 *  _Message_queue_Submit
 *
 *  DESCRIPTION:
 *
 *  This routine provides the common foundation for the
 *  rtems_message_queue_send and rtems_message_queue_urgent directives.
 */

rtems_status_code _Message_queue_Submit(
  Objects_Id                  id,
  Message_queue_Buffer       *buffer,
  Message_queue_Submit_types  submit_type
);

/*
 *  _Message_queue_Allocate_message_buffer
 *
 *  DESCRIPTION:
 *
 *  This function allocates a message buffer from the inactive
 *  message buffer chain.
 */

STATIC INLINE Message_queue_Buffer_control *
  _Message_queue_Allocate_message_buffer ( void );

/*
 *  _Message_queue_Free_message_buffer
 *
 *  DESCRIPTION:
 *
 *  This routine frees a message buffer to the inactive
 *  message buffer chain.
 */

STATIC INLINE void _Message_queue_Free_message_buffer (
  Message_queue_Buffer_control *the_message
);

/*
 *  _Message_queue_Get_pending_message
 *
 *  DESCRIPTION:
 *
 *  This function removes the first message from the_message_queue
 *  and returns a pointer to it.
 */

STATIC INLINE
  Message_queue_Buffer_control *_Message_queue_Get_pending_message (
  Message_queue_Control *the_message_queue
);

/*
 *  _Message_queue_Append
 *
 *  DESCRIPTION:
 *
 *  This routine places the_message at the rear of the outstanding
 *  messages on the_message_queue.
 */

STATIC INLINE void _Message_queue_Append (
  Message_queue_Control        *the_message_queue,
  Message_queue_Buffer_control *the_message
);

/*
 *  _Message_queue_Prepend
 *
 *  DESCRIPTION:
 *
 *  This routine places the_message at the rear of the outstanding
 *  messages on the_message_queue.
 */

STATIC INLINE void _Message_queue_Prepend (
  Message_queue_Control        *the_message_queue,
  Message_queue_Buffer_control *the_message
);

/*
 *  _Message_queue_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function places the_message at the rear of the outstanding
 *  messages on the_message_queue.
 */

STATIC INLINE boolean _Message_queue_Is_null (
  Message_queue_Control *the_message_queue
);

/*
 *  _Message_queue_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a message queue control block from
 *  the inactive chain of free message queue control blocks.
 */

STATIC INLINE Message_queue_Control *_Message_queue_Allocate ( void );

/*
 *  _Message_queue_Free
 *
 *  DESCRIPTION:
 *
 *  This routine allocates a message queue control block from
 *  the inactive chain of free message queue control blocks.
 */

STATIC INLINE void _Message_queue_Free (
  Message_queue_Control *the_message_queue
);

/*
 *  _Message_queue_Get
 *
 *  DESCRIPTION:
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

STATIC INLINE Message_queue_Control *_Message_queue_Get (
  Objects_Id         id,
  Objects_Locations *location
);

#include <rtems/message.inl>
#include <rtems/msgmp.h>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
