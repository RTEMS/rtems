/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines the Message Manager API.
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/message/if/header */

#ifndef _RTEMS_RTEMS_MESSAGE_H
#define _RTEMS_RTEMS_MESSAGE_H

#include <stddef.h>
#include <stdint.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/score/coremsgbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/message/if/group */

/**
 * @defgroup RTEMSAPIClassicMessage Message Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Message Manager provides communication and synchronization
 *   capabilities using RTEMS message queues.
 */

/* Generated from spec:/rtems/message/if/config */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
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

/* Generated from spec:/rtems/message/if/create */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
 * @brief Creates a message queue.
 *
 * @param name is the object name of the message queue.
 *
 * @param count is the maximum count of pending messages supported by the
 *   message queue.
 *
 * @param max_message_size is the maximum size in bytes of a message supported
 *   by the message queue.
 *
 * @param attribute_set is the attribute set of the message queue.
 *
 * @param[out] id is the pointer to an object identifier variable.  When the
 *   directive call is successful, the identifier of the created message queue
 *   will be stored in this variable.
 *
 * This directive creates a message queue which resides on the local node.  The
 * message queue has the user-defined object name specified in ``name``.
 * Memory is allocated from the RTEMS Workspace for the count of messages
 * specified in ``count``, each of ``max_message_size`` bytes in length.  The
 * assigned object identifier is returned in ``id``.  This identifier is used
 * to access the message queue with other message queue related directives.
 *
 * The **attribute set** specified in ``attribute_set`` is built through a
 * *bitwise or* of the attribute constants described below.  Not all
 * combinations of attributes are allowed.  Some attributes are mutually
 * exclusive.  If mutually exclusive attributes are combined, the behaviour is
 * undefined.  Attributes not mentioned below are not evaluated by this
 * directive and have no effect.  Default attributes can be selected by using
 * the #RTEMS_DEFAULT_ATTRIBUTES constant.  The attribute set defines
 *
 * * the scope of the message queue: #RTEMS_LOCAL (default) or #RTEMS_GLOBAL
 *   and
 *
 * * the task wait queue discipline used by the message queue: #RTEMS_FIFO
 *   (default) or #RTEMS_PRIORITY.
 *
 * The message queue has a local or global **scope** in a multiprocessing
 * network (this attribute does not refer to SMP systems).  The scope is
 * selected by the mutually exclusive #RTEMS_LOCAL and #RTEMS_GLOBAL
 * attributes.
 *
 * * A **local scope** is the default and can be emphasized through the use of
 *   the #RTEMS_LOCAL attribute.  A local message queue can be only used by the
 *   node which created it.
 *
 * * A **global scope** is established if the #RTEMS_GLOBAL attribute is set.
 *   Setting the global attribute in a single node system has no effect.
 *
 * The **task wait queue discipline** is selected by the mutually exclusive
 * #RTEMS_FIFO and #RTEMS_PRIORITY attributes. The discipline defines the order
 * in which tasks wait for a message to receive on a currently empty message
 * queue.
 *
 * * The **FIFO discipline** is the default and can be emphasized through use
 *   of the #RTEMS_FIFO attribute.
 *
 * * The **priority discipline** is selected by the #RTEMS_PRIORITY attribute.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NUMBER The ``count`` parameter was invalid.
 *
 * @retval ::RTEMS_INVALID_SIZE The ``max_message_size`` parameter was invalid.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive object available to create a
 *   message queue.  The number of message queue available to the application
 *   is configured through the #CONFIGURE_MAXIMUM_MESSAGE_QUEUES application
 *   configuration option.
 *
 * @retval ::RTEMS_TOO_MANY In multiprocessing configurations, there was no
 *   inactive global object available to create a global message queue.  The
 *   number of global objects available to the application is configured
 *   through the #CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS application configuration
 *   option.
 *
 * @retval ::RTEMS_INVALID_NUMBER The product of ``count`` and
 *   ``max_message_size`` is greater than the maximum storage size.
 *
 * @retval ::RTEMS_UNSATISFIED There was not enough memory available in the
 *   RTEMS Workspace to allocate the message buffers for the message queue.
 *
 * @par Notes
 * @parblock
 * For message queues with a global scope, the maximum message size is
 * effectively limited to the longest message which the MPCI is capable of
 * transmitting.
 *
 * For control and maintenance of the message queue, RTEMS allocates a QCB from
 * the local QCB free pool and initializes it.
 *
 * The QCB for a global message queue is allocated on the local node.  Message
 * queues should not be made global unless remote tasks must interact with the
 * message queue.  This is to avoid the system overhead incurred by the
 * creation of a global message queue.  When a global message queue is created,
 * the message queue's name and identifier must be transmitted to every node in
 * the system for insertion in the local copy of the global object table.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * When the directive operates on a global object, the directive sends a
 *   message to remote nodes.  This may preempt the calling task.
 *
 * * The number of message queues available to the application is configured
 *   through the #CONFIGURE_MAXIMUM_MESSAGE_QUEUES application configuration
 *   option.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may allocate memory from the RTEMS
 *   Workspace.
 *
 * * The number of global objects available to the application is configured
 *   through the #CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS application configuration
 *   option.
 * @endparblock
 */
rtems_status_code rtems_message_queue_create(
  rtems_name      name,
  uint32_t        count,
  size_t          max_message_size,
  rtems_attribute attribute_set,
  rtems_id       *id
);

/* Generated from spec:/rtems/message/if/construct */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
 * @brief Constructs a message queue from the specified the message queue
 *   configuration.
 *
 * @param config is the message queue configuration.
 *
 * @param[out] id is the pointer to an object identifier variable.  When the
 *   directive call is successful, the identifier of the constructed message
 *   queue will be stored in this variable.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``config`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The message queue name in the configuration was
 *   invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
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
 *
 * @par Notes
 * @parblock
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
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * When the directive operates on a global object, the directive sends a
 *   message to remote nodes.  This may preempt the calling task.
 *
 * * The number of message queues available to the application is configured
 *   through the #CONFIGURE_MAXIMUM_MESSAGE_QUEUES application configuration
 *   option.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may allocate memory from the RTEMS
 *   Workspace.
 *
 * * The number of global objects available to the application is configured
 *   through the #CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS application configuration
 *   option.
 * @endparblock
 */
rtems_status_code rtems_message_queue_construct(
  const rtems_message_queue_config *config,
  rtems_id                         *id
);

/* Generated from spec:/rtems/message/if/ident */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
 * @brief Identifies a message queue by the object name.
 *
 * @param name is the object name to look up.
 *
 * @param node is the node or node set to search for a matching object.
 *
 * @param[out] id is the pointer to an object identifier variable.  When the
 *   directive call is successful, the object identifier of an object with the
 *   specified name will be stored in this variable.
 *
 * This directive obtains a message queue identifier associated with the
 * message queue name specified in ``name``.
 *
 * The node to search is specified in ``node``.  It shall be
 *
 * * a valid node number,
 *
 * * the constant #RTEMS_SEARCH_ALL_NODES to search in all nodes,
 *
 * * the constant #RTEMS_SEARCH_LOCAL_NODE to search in the local node only, or
 *
 * * the constant #RTEMS_SEARCH_OTHER_NODES to search in all nodes except the
 *   local node.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_NAME There was no object with the specified name on
 *   the specified nodes.
 *
 * @retval ::RTEMS_INVALID_NODE In multiprocessing configurations, the
 *   specified node was invalid.
 *
 * @par Notes
 * @parblock
 * If the message queue name is not unique, then the message queue identifier
 * will match the first message queue with that name in the search order.
 * However, this message queue identifier is not guaranteed to correspond to
 * the desired message queue.
 *
 * The objects are searched from lowest to the highest index.  If ``node`` is
 * #RTEMS_SEARCH_ALL_NODES, all nodes are searched with the local node being
 * searched first.  All other nodes are searched from lowest to the highest
 * node number.
 *
 * If node is a valid node number which does not represent the local node, then
 * only the message queues exported by the designated node are searched.
 *
 * This directive does not generate activity on remote nodes.  It accesses only
 * the local copy of the global object table.
 *
 * The message queue identifier is used with other message related directives
 * to access the message queue.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_message_queue_ident(
  rtems_name name,
  uint32_t   node,
  rtems_id  *id
);

/* Generated from spec:/rtems/message/if/delete */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
 * @brief Deletes the message queue.
 *
 * @param id is the message queue identifier.
 *
 * This directive deletes the message queue specified by ``id``. As a result of
 * this directive, all tasks blocked waiting to receive a message from this
 * queue will be readied and returned a status code which indicates that the
 * message queue was deleted.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no message queue associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The message queue resided on a
 *   remote node.
 *
 * @par Notes
 * @parblock
 * When the message queue is deleted, any messages in the queue are returned to
 * the free message buffer pool.  Any information stored in those messages is
 * lost.  The message buffers allocated for the message queue are reclaimed.
 *
 * The QCB for the deleted message queue is reclaimed by RTEMS.
 *
 * When a global message queue is deleted, the message queue identifier must be
 * transmitted to every node in the system for deletion from the local copy of
 * the global object table.
 *
 * The message queue must reside on the local node, even if the message queue
 * was created with the #RTEMS_GLOBAL attribute.
 *
 * Proxies, used to represent remote tasks, are reclaimed when the message
 * queue is deleted.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * When the directive operates on a global object, the directive sends a
 *   message to remote nodes.  This may preempt the calling task.
 *
 * * The calling task does not have to be the task that created the object.
 *   Any local task that knows the object identifier can delete the object.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may free memory to the RTEMS Workspace.
 * @endparblock
 */
rtems_status_code rtems_message_queue_delete( rtems_id id );

/* Generated from spec:/rtems/message/if/send */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
 * @brief Puts the message at the rear of the queue.
 *
 * @param id is the queue identifier.
 *
 * @param buffer is the begin address of the message buffer to send.
 *
 * @param size is the size in bytes of the message buffer to send.
 *
 * This directive sends the message ``buffer`` of ``size`` bytes in length to
 * the queue specified by ``id``.  If a task is waiting at the queue, then the
 * message is copied to the waiting task's buffer and the task is unblocked. If
 * no tasks are waiting at the queue, then the message is copied to a message
 * buffer which is obtained from this message queue's message buffer pool.  The
 * message buffer is then placed at the rear of the queue.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no queue associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``buffer`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_SIZE The size of the message exceeded the maximum
 *   message size of the queue as defined by rtems_message_queue_create() or
 *   rtems_message_queue_construct().
 *
 * @retval ::RTEMS_TOO_MANY The maximum number of pending messages supported by
 *   the queue as defined by rtems_message_queue_create() or
 *   rtems_message_queue_construct() has been reached.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may unblock another task which may preempt the calling task.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_message_queue_send(
  rtems_id    id,
  const void *buffer,
  size_t      size
);

/* Generated from spec:/rtems/message/if/urgent */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
 * @brief Puts the message at the front of the queue.
 *
 * @param id is the queue identifier.
 *
 * @param buffer is the begin address of the message buffer to send urgently.
 *
 * @param size is the size in bytes of the message buffer to send urgently.
 *
 * This directive sends the message ``buffer`` of ``size`` bytes in length to
 * the queue specified by ``id``.  If a task is waiting at the queue, then the
 * message is copied to the waiting task's buffer and the task is unblocked. If
 * no tasks are waiting at the queue, then the message is copied to a message
 * buffer which is obtained from this message queue's message buffer pool.  The
 * message buffer is then placed at the front of the queue.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no queue associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``buffer`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_SIZE The size of the message exceeded the maximum
 *   message size of the queue as defined by rtems_message_queue_create() or
 *   rtems_message_queue_construct().
 *
 * @retval ::RTEMS_TOO_MANY The maximum number of pending messages supported by
 *   the queue as defined by rtems_message_queue_create() or
 *   rtems_message_queue_construct() has been reached.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may unblock another task which may preempt the calling task.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_message_queue_urgent(
  rtems_id    id,
  const void *buffer,
  size_t      size
);

/* Generated from spec:/rtems/message/if/broadcast */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
 * @brief Broadcasts the messages to the tasks waiting at the queue.
 *
 * @param id is the queue identifier.
 *
 * @param buffer is the begin address of the message buffer to broadcast.
 *
 * @param size is the size in bytes of the message buffer to broadcast.
 *
 * @param[out] count is the pointer to an uint32_t variable.  When the
 *   directive call is successful, the number of unblocked tasks will be stored
 *   in this variable.
 *
 * This directive causes all tasks that are waiting at the queue specified by
 * ``id`` to be unblocked and sent the message contained in ``buffer``.  Before
 * a task is unblocked, the message ``buffer`` of ``size`` byes in length is
 * copied to that task's message buffer.  The number of tasks that were
 * unblocked is returned in ``count``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no queue associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``buffer`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``count`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_SIZE The size of the message exceeded the maximum
 *   message size of the queue as defined by rtems_message_queue_create() or
 *   rtems_message_queue_construct().
 *
 * @par Notes
 * The execution time of this directive is directly related to the number of
 * tasks waiting on the message queue, although it is more efficient than the
 * equivalent number of invocations of rtems_message_queue_send().
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may unblock another task which may preempt the calling task.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_message_queue_broadcast(
  rtems_id    id,
  const void *buffer,
  size_t      size,
  uint32_t   *count
);

/* Generated from spec:/rtems/message/if/receive */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
 * @brief Receives a message from the queue.
 *
 * @param id is the queue identifier.
 *
 * @param buffer is the begin address of the buffer to receive the message.
 *   The buffer shall be large enough to receive a message of the maximum
 *   length of the queue as defined by rtems_message_queue_create() or
 *   rtems_message_queue_construct().  The ``size`` parameter cannot be used to
 *   specify the size of the buffer.
 *
 * @param size is the pointer to a size_t variable.  When the directive call is
 *   successful, the size in bytes of the received messages will be stored in
 *   this variable.  This parameter cannot be used to specify the size of the
 *   buffer.
 *
 * @param option_set is the option set.
 *
 * @param timeout is the timeout in clock ticks if the #RTEMS_WAIT option is
 *   set.  Use #RTEMS_NO_TIMEOUT to wait potentially forever.
 *
 * This directive receives a message from the queue specified by ``id``.
 *
 * The **option set** specified in ``option_set`` is built through a *bitwise
 * or* of the option constants described below.  Not all combinations of
 * options are allowed.  Some options are mutually exclusive.  If mutually
 * exclusive options are combined, the behaviour is undefined.  Options not
 * mentioned below are not evaluated by this directive and have no effect.
 * Default options can be selected by using the #RTEMS_DEFAULT_OPTIONS
 * constant.
 *
 * The calling task can **wait** or **try to receive** a message from the queue
 * according to the mutually exclusive #RTEMS_WAIT and #RTEMS_NO_WAIT options.
 *
 * * **Waiting to receive** a message from the queue is the default and can be
 *   emphasized through the use of the #RTEMS_WAIT option. The ``timeout``
 *   parameter defines how long the calling task is willing to wait.  Use
 *   #RTEMS_NO_TIMEOUT to wait potentially forever, otherwise set a timeout
 *   interval in clock ticks.
 *
 * * **Trying to receive** a message from the queue is selected by the
 *   #RTEMS_NO_WAIT option.  If this option is defined, then the ``timeout``
 *   parameter is ignored.  When a message from the queue cannot be immediately
 *   received, then the ::RTEMS_UNSATISFIED status is returned.
 *
 * With either #RTEMS_WAIT or #RTEMS_NO_WAIT if there is at least one message
 * in the queue, then it is copied to the buffer, the size is set to return the
 * length of the message in bytes, and this directive returns immediately with
 * the ::RTEMS_SUCCESSFUL status code.  The buffer has to be big enough to
 * receive a message of the maximum length with respect to this message queue.
 *
 * If the calling task chooses to return immediately and the queue is empty,
 * then the directive returns immediately with the ::RTEMS_UNSATISFIED status
 * cod.  If the calling task chooses to wait at the message queue and the queue
 * is empty, then the calling task is placed on the message wait queue and
 * blocked.  If the queue was created with the #RTEMS_PRIORITY option
 * specified, then the calling task is inserted into the wait queue according
 * to its priority.  But, if the queue was created with the #RTEMS_FIFO option
 * specified, then the calling task is placed at the rear of the wait queue.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no queue associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``buffer`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``size`` parameter was NULL.
 *
 * @retval ::RTEMS_UNSATISFIED The queue was empty.
 *
 * @retval ::RTEMS_UNSATISFIED The queue was flushed while the calling task was
 *   waiting to receive a message.
 *
 * @retval ::RTEMS_TIMEOUT The timeout happened while the calling task was
 *   waiting to receive a message
 *
 * @retval ::RTEMS_OBJECT_WAS_DELETED The queue was deleted while the calling
 *   task was waiting to receive a message.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * When a local queue is accessed and the #RTEMS_NO_WAIT option is set, the
 *   directive may be called from within interrupt context.
 *
 * * The directive may be called from within task context.
 *
 * * When the request cannot be immediately satisfied and the #RTEMS_WAIT
 *   option is set, the calling task blocks at some point during the directive
 *   call.
 *
 * * The timeout functionality of the directive requires a clock tick.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_message_queue_receive(
  rtems_id       id,
  void          *buffer,
  size_t        *size,
  rtems_option   option_set,
  rtems_interval timeout
);

/* Generated from spec:/rtems/message/if/get-number-pending */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
 * @brief Gets the number of messages pending on the queue.
 *
 * @param id is the queue identifier.
 *
 * @param[out] count is the pointer to an uint32_t variable.  When the
 *   directive call is successful, the number of pending messages will be
 *   stored in this variable.
 *
 * This directive returns the number of messages pending on the queue specified
 * by ``id`` in ``count``.  If no messages are present on the queue, count is
 * set to zero.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no queue associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``count`` parameter was NULL.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may be called from within interrupt context.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_message_queue_get_number_pending(
  rtems_id  id,
  uint32_t *count
);

/* Generated from spec:/rtems/message/if/flush */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
 * @brief Flushes all messages on the queue.
 *
 * @param id is the queue identifier.
 *
 * @param[out] count is the pointer to an uint32_t variable.  When the
 *   directive call is successful, the number of unblocked tasks will be stored
 *   in this variable.
 *
 * This directive removes all pending messages from the queue specified by
 * ``id``.  The number of messages removed is returned in ``count``.  If no
 * messages are present on the queue, count is set to zero.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no queue associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``count`` parameter was NULL.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may be called from within interrupt context.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_message_queue_flush( rtems_id id, uint32_t *count );

/* Generated from spec:/rtems/message/if/buffer */

/**
 * @ingroup RTEMSAPIClassicMessage
 *
 * @brief Defines a structure which can be used as a message queue buffer for
 *   messages of the specified maximum size.
 *
 * @param _maximum_message_size is the maximum message size in bytes.
 *
 * @par Notes
 * Use this macro to define the message buffer storage area for
 * rtems_message_queue_construct().
 */
#define RTEMS_MESSAGE_QUEUE_BUFFER( _maximum_message_size ) \
  struct { \
    CORE_message_queue_Buffer _buffer; \
    char _message[ _maximum_message_size ]; \
  }

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_MESSAGE_H */
