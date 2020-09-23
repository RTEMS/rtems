/**
 * @file
 *
 * @ingroup ClassicMessageQueue
 *
 * @brief Classic Message Queue Manager API
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

#include <rtems/rtems/attr.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/score/coremsgbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicMessageQueue Message Queues
 *
 *  @ingroup RTEMSAPIClassic
 *
 *  This encapsulates functionality related to the Classic API Message Queue
 *  Manager.
 */
/**@{*/

/**
 * @brief This structure defines the configuration of a message queue
 *   constructed by rtems_message_queue_construct().
 */
typedef struct {
  /**
   * @brief This member defines the name of the message queue.
   */
  rtems_name name;

  /**
   * @brief This member defines the maximum number of pending messages supported
   *   by the message queue.
   */
  uint32_t maximum_pending_messages;

  /**
   * @brief This member defines the maximum message size supported by the message
   *   queue.
   */
  size_t maximum_message_size;

  /**
   * @brief This member shall point to the message buffer storage area begin.
   *
   * The message buffer storage area for the message queue shall be an array of
   * the type defined by RTEMS_MESSAGE_QUEUE_BUFFER() with a maximum message size
   * equal to the maximum message size of this configuration.
   */
  void *storage_area;

  /**
   * @brief This member defines size of the message buffer storage area in bytes.
   */
  size_t storage_size;

  /**
   * @brief This member defines the optional handler to free the message buffer
   *   storage area.
   *
   * It is called when the message queue is deleted.  It is called from task
   * context under protection of the object allocator lock.  It is allowed to
   * call free() in this handler.  If handler is NULL, then no action will be
   * performed.
   */
  void ( *storage_free )( void * );

  /**
   * @brief This member defines the attributes of the message queue.
   */
  rtems_attribute attributes;
} rtems_message_queue_config;

/**
 * @brief Defines a structure which can be used as a message queue buffer for
 *   messages of the specified maximum size.
 *
 * Use this macro to define the message buffer storage area for
 * rtems_message_queue_construct().
 *
 * @param _maximum_message_size is the maximum message size in bytes.
 */
#define RTEMS_MESSAGE_QUEUE_BUFFER( _maximum_message_size ) \
  struct { \
    CORE_message_queue_Buffer _buffer; \
    char _message[ _maximum_message_size ]; \
  }

/**
 * @brief Constructs a message queue from the specified the message queue
 *   configuration.
 *
 * In contrast to message queues created by rtems_message_queue_create(), the
 * message queues constructed by this directive use a user-provided message
 * buffer storage area.
 *
 * This directive is intended for applications which do not want to use the
 * RTEMS Workspace and instead statically allocate all operating system
 * resources.  An application based solely on static allocation can avoid any
 * runtime memory allocators.  This can simplify the application architecture
 * as well as any analysis that may be required.
 *
 * The value for #CONFIGURE_MESSAGE_BUFFER_MEMORY should not include memory for
 * message queues constructed by rtems_message_queue_construct().
 *
 * @param config is the message queue configuration.
 *
 * @param[out] id is the pointer to an object identifier variable.  The
 *   identifier of the constructed message queue object will be stored in this
 *   variable, in case of a successful operation.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The id parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The message queue name in the configuration was
 *   invalid.
 *
 * @retval ::RTEMS_INVALID_NUMBER The maximum number of pending messages in the
 *   configuration was zero.
 *
 * @retval ::RTEMS_INVALID_SIZE The maximum message size in the configuration
 *   was zero.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive message queue object
 *   available to construct a message queue.
 *
 * @retval ::RTEMS_TOO_MANY In multiprocessing configurations, there was no
 *   inactive global object available to construct a global message queue.
 *
 * @retval ::RTEMS_INVALID_SIZE The maximum message size in the configuration
 *   was too big and resulted in integer overflows in calculations carried out
 *   to determine the size of the message buffer area.
 *
 * @retval ::RTEMS_INVALID_NUMBER The maximum number of pending messages in the
 *   configuration was too big and resulted in integer overflows in
 *   calculations carried out to determine the size of the message buffer area.
 *
 * @retval ::RTEMS_UNSATISFIED The message queue storage area begin pointer in
 *   the configuration was NULL.
 *
 * @retval ::RTEMS_UNSATISFIED The message queue storage area size in the
 *   configuration was not equal to the size calculated from the maximum number
 *   of pending messages and the maximum message size.
 */
rtems_status_code rtems_message_queue_construct(
  const rtems_message_queue_config *config,
  rtems_id                         *id
);

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
 * @brief Sends a message to the message queue.
 *
 * This directive sends the message buffer to the message queue indicated by
 * ID.  If one or more tasks is blocked waiting to receive a message from this
 * message queue, then one will receive the message.  The task selected to
 * receive the message is based on the task queue discipline algorithm in use
 * by this particular message queue.  If no tasks are waiting, then the message
 * buffer will be placed at the rear of the chain of pending messages for this
 * message queue.
 *
 * @param id The message queue ID.
 * @param buffer The message content buffer.
 * @param size The size of the message.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Invalid message queue ID.
 * @retval RTEMS_INVALID_ADDRESS The message buffer pointer is @c NULL.
 * @retval RTEMS_INVALID_SIZE The message size is larger than the maximum
 *   message size of the message queue.
 * @retval RTEMS_TOO_MANY The new message would exceed the message queue limit
 *   for pending messages.
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
 * @brief Receives a message from the message queue
 *
 * This directive is invoked when the calling task wishes to receive a message
 * from the message queue indicated by ID. The received message is to be placed
 * in the buffer. If no messages are outstanding and the option set indicates
 * that the task is willing to block, then the task will be blocked until a
 * message arrives or until, optionally, timeout clock ticks have passed.
 *
 * @param id The message queue ID.
 * @param[out] buffer The buffer for the message content.  The buffer must be
 *   large enough to store maximum size messages of this message queue.
 * @param[out] size The size of the message.
 * @param option_set The option set, e.g. RTEMS_NO_WAIT or RTEMS_WAIT.
 * @param timeout The number of ticks to wait if the RTEMS_WAIT is set.  Use
 *   RTEMS_NO_TIMEOUT to wait indefinitely.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID Invalid message queue ID.
 * @retval RTEMS_INVALID_ADDRESS The message buffer pointer or the message size
 *   pointer is @c NULL.
 * @retval RTEMS_TIMEOUT A timeout occurred and no message was received.
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
