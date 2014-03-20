/**
 * @file rtems/rtems/message.h
 *
 * @defgroup ClassicMessageQueue Message Queues
 *
 * @ingroup ClassicRTEMS
 * @brief Message Queue Manager
 *
 * This include file contains all the constants and structures associated
 * with the Message Queue Manager. This manager provides a mechanism for
 * communication and synchronization between tasks using messages.
 *
 * Directives provided are:
 *
 * - create a queue
 * - get ID of a queue
 * - delete a queue
 * - put a message at the rear of a queue
 * - put a message at the front of a queue
 * - broadcast N messages to a queue
 * - receive message from a queue
 * - flush all messages on a queue
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_MESSAGE_H
#define _RTEMS_RTEMS_MESSAGE_H

#include <rtems/rtems/types.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/object.h>
#include <rtems/score/coremsg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @ingroup ClassicMessageQueueImpl
 *
 *  The following records define the control block used to manage
 *  each message queue.
 */
typedef struct {
  /** This field is the inherited object characteristics. */
  Objects_Control             Object;
  /** This field is the attribute set as defined by the API. */
  rtems_attribute             attribute_set;
  /** This field is the instance of the SuperCore Message Queue. */
  CORE_message_queue_Control  message_queue;
}   Message_queue_Control;

/**
 *  @defgroup ClassicMessageQueue Message Queues
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality related to the Classic API Message Queue
 *  Manager.
 */
/**@{*/

/**
 * @brief RTEMS Create Message Queue
 *
 * This routine implements the rtems_message_queue_create directive. The
 * message queue will have the @a name. If the @a attribute_set indicates
 * that the message queue is to be limited in the number of messages
 * that can be outstanding, then @a count indicates the maximum number of
 * messages that will be held. It returns the id of the created
 * message queue in @a id.
 *
 * @param[in] name is the user defined queue name
 * @param[in] count is the maximum message and reserved buffer count
 * @param[in] max_message_size is the maximum size of each message
 * @param[in] attribute_set is the process method
 * @param[in] id is the pointer to queue
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error. If successful, the @a id will
 *         be filled in with the queue id.
 */
rtems_status_code rtems_message_queue_create(
  rtems_name       name,
  uint32_t         count,
  size_t           max_message_size,
  rtems_attribute  attribute_set,
  rtems_id        *id
);

/**
 * @brief RTEMS Message Queue Name to Id
 *
 * This routine implements the rtems_message_queue_ident directive.
 * This directive returns the message queue ID associated with NAME.
 * If more than one message queue is named name, then the message
 * queue to which the ID belongs is arbitrary. node indicates the
 * extent of the search for the ID of the message queue named name.
 * The search can be limited to a particular node or allowed to
 * encompass all nodes.
 *
 * @param[in] name is the user defined message queue name
 * @param[in] node is the node(s) to be searched
 * @param[in] id is the pointer to message queue id
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful and
 * *id filled with the message queue id
 */
rtems_status_code rtems_message_queue_ident(
  rtems_name  name,
  uint32_t    node,
  rtems_id   *id
);

/**
 * @brief RTEMS Delete Message Queue
 *
 * This routine implements the rtems_message_queue_delete directive. The
 * message queue indicated by ID is deleted.
 *
 * @param[in] id is the queue id
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 */
rtems_status_code rtems_message_queue_delete(
  rtems_id id
);

/**
 *  @brief rtems_message_queue_send
 *
 *  Message Queue Manager - rtems_message_queue_send
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
  rtems_id    id,
  const void *buffer,
  size_t      size
);

/**
 * @brief RTEMS Urgent Message Queue
 *
 * This routine implements the rtems_message_queue_urgent directive.
 * This directive has the same behavior as rtems_message_queue_send
 * except that if no tasks are waiting, the message buffer will
 * be placed at the FRONT of the chain of pending messages rather
 * than at the REAR.
 *
 * @param[in] id is the pointer to message queue
 * @param[in] buffer is the pointer to message buffer
 * @param[in] size is the size of message to send urgently
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 */
rtems_status_code rtems_message_queue_urgent(
  rtems_id    id,
  const void *buffer,
  size_t      size
);

/**
 * @brief RTEMS Broadcast Message Queue
 *
 * This routine implements the rtems_message_queue_broadcast directive.
 * This directive sends the message buffer to all of the tasks blocked
 * waiting for a message on the message queue indicated by ID.
 * If no tasks are waiting, then the message buffer will not be queued.
 *
 * @param[in] id is the pointer to message queue
 * @param[in] buffer is the pointer to message buffer
 * @param[in] size is the size of message to broadcast
 * @param[in] count pointer to area to store number of threads made ready
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful and
 * 		*count filled in with number of threads made ready
 */
rtems_status_code rtems_message_queue_broadcast(
  rtems_id    id,
  const void *buffer,
  size_t      size,
  uint32_t   *count
);

/**
 * @brief RTEMS Message Queue Receive
 *
 * This routine implements the rtems_message_queue_receive directive.
 * This directive is invoked when the calling task wishes to receive
 * a message from the message queue indicated by ID. The received
 * message is to be placed in buffer. If no messages are outstanding
 * and the option_set indicates that the task is willing to block,
 * then the task will be blocked until a message arrives or until,
 * optionally, timeout clock ticks have passed.
 *
 * @param[in] id is the queue id
 * @param[in] buffer is the pointer to message buffer
 * @param[in] size is the size of message receive
 * @param[in] option_set is the options on receive
 * @param[in] timeout is the number of ticks to wait
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_message_queue_receive(
  rtems_id        id,
  void           *buffer,
  size_t         *size,
  rtems_option    option_set,
  rtems_interval  timeout
);

/**
 *  @brief rtems_message_queue_flush
 *
 *  This routine implements the rtems_message_queue_flush directive.
 *  This directive takes all outstanding messages for the message
 *  queue indicated by ID and returns them to the inactive message
 *  chain.  The number of messages flushed is returned in COUNT.
 *
 *  Message Queue Manager
 */
rtems_status_code rtems_message_queue_flush(
  rtems_id  id,
  uint32_t *count
);

/**
 *  @brief RTEMS Message Queue Get Number Pending
 *
 *  Message Queue Manager
 *
 *  This routine implements the rtems_message_queue_get_number_pending
 *  directive.  This directive returns the number of pending
 *  messages for the message queue indicated by ID
 *  chain.  The number of messages pending is returned in COUNT.
 */
rtems_status_code rtems_message_queue_get_number_pending(
  rtems_id  id,
  uint32_t *count
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
