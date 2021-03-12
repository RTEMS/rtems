/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines the Semaphore Manager API.
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

/* Generated from spec:/rtems/sem/if/header */

#ifndef _RTEMS_RTEMS_SEM_H
#define _RTEMS_RTEMS_SEM_H

#include <stdint.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/sem/if/group */

/**
 * @defgroup RTEMSAPIClassicSem Semaphore Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Semaphore Manager utilizes standard Dijkstra counting semaphores
 *   to provide synchronization and mutual exclusion capabilities.
 */

/* Generated from spec:/rtems/sem/if/create */

/**
 * @ingroup RTEMSAPIClassicSem
 *
 * @brief Creates a semaphore.
 *
 * @param name is the object name of the semaphore.
 *
 * @param count is the initial count of the semaphore.  If the semaphore is a
 *   binary semaphore, then a count of 0 will make the calling task the owner
 *   of the binary semaphore and a count of 1 will create a binary semaphore
 *   without an owner.
 *
 * @param attribute_set is the attribute set of the semaphore.
 *
 * @param priority_ceiling is the priority ceiling if the semaphore is a binary
 *   semaphore with the priority ceiling or MrsP locking protocol as defined by
 *   the attribute set.
 *
 * @param[out] id is the pointer to an object identifier variable.  When the
 *   directive call is successful, the identifier of the created semaphore will
 *   be stored in this variable.
 *
 * This directive creates a semaphore which resides on the local node.  The
 * semaphore has the user-defined object name specified in ``name`` and the
 * initial count specified in ``count``.  The assigned object identifier is
 * returned in ``id``.  This identifier is used to access the semaphore with
 * other semaphore related directives.
 *
 * The **attribute set** specified in ``attribute_set`` is built through a
 * *bitwise or* of the attribute constants described below.  Not all
 * combinations of attributes are allowed.  Some attributes are mutually
 * exclusive.  If mutually exclusive attributes are combined, the behaviour is
 * undefined.  Attributes not mentioned below are not evaluated by this
 * directive and have no effect.  Default attributes can be selected by using
 * the #RTEMS_DEFAULT_ATTRIBUTES constant.  The attribute set defines
 *
 * * the scope of the semaphore: #RTEMS_LOCAL (default) or #RTEMS_GLOBAL,
 *
 * * the task wait queue discipline used by the semaphore: #RTEMS_FIFO
 *   (default) or #RTEMS_PRIORITY,
 *
 * * the class of the semaphore: #RTEMS_COUNTING_SEMAPHORE (default),
 *   #RTEMS_BINARY_SEMAPHORE, or #RTEMS_SIMPLE_BINARY_SEMAPHORE, and
 *
 * * the locking protocol of a binary semaphore: no locking protocol (default),
 *   #RTEMS_INHERIT_PRIORITY, #RTEMS_PRIORITY_CEILING, or
 *   #RTEMS_MULTIPROCESSOR_RESOURCE_SHARING.
 *
 * The semaphore has a local or global **scope** in a multiprocessing network
 * (this attribute does not refer to SMP systems).  The scope is selected by
 * the mutually exclusive #RTEMS_LOCAL and #RTEMS_GLOBAL attributes.
 *
 * * A **local scope** is the default and can be emphasized through the use of
 *   the #RTEMS_LOCAL attribute.  A local semaphore can be only used by the
 *   node which created it.
 *
 * * A **global scope** is established if the #RTEMS_GLOBAL attribute is set.
 *   Setting the global attribute in a single node system has no effect.
 *
 * The **task wait queue discipline** is selected by the mutually exclusive
 * #RTEMS_FIFO and #RTEMS_PRIORITY attributes.
 *
 * * The **FIFO discipline** is the default and can be emphasized through use
 *   of the #RTEMS_FIFO attribute.
 *
 * * The **priority discipline** is selected by the #RTEMS_PRIORITY attribute.
 *   The locking protocols require the priority discipline.
 *
 * The **semaphore class** is selected by the mutually exclusive
 * #RTEMS_COUNTING_SEMAPHORE, #RTEMS_BINARY_SEMAPHORE, and
 * #RTEMS_SIMPLE_BINARY_SEMAPHORE attributes.
 *
 * * The **counting semaphore class** is the default and can be emphasized
 *   through use of the #RTEMS_COUNTING_SEMAPHORE attribute.
 *
 * * The **binary semaphore class** is selected by the #RTEMS_BINARY_SEMAPHORE
 *   attribute.  Binary semaphores are mutual exclusion (mutex) synchronization
 *   primitives which may have an owner.  The count of a binary semaphore is
 *   restricted to 0 and 1 values.
 *
 * * The **simple binary semaphore class** is selected by the
 *   #RTEMS_SIMPLE_BINARY_SEMAPHORE attribute.  Simple binary semaphores have
 *   no owner.  They may be used for task and interrupt synchronization.  The
 *   count of a simple binary semaphore is restricted to 0 and 1 values.
 *
 * Binary semaphores may use a **locking protocol**.  If a locking protocol is
 * selected, then the scope shall be local and the priority task wait queue
 * discipline shall be selected.  The locking protocol is selected by the
 * mutually exclusive #RTEMS_INHERIT_PRIORITY, #RTEMS_PRIORITY_CEILING, and
 * #RTEMS_MULTIPROCESSOR_RESOURCE_SHARING attributes.
 *
 * * The default is **no locking protocol**.  This can be emphasized through
 *   use of the #RTEMS_NO_INHERIT_PRIORITY,
 *   #RTEMS_NO_MULTIPROCESSOR_RESOURCE_SHARING, and #RTEMS_NO_PRIORITY_CEILING
 *   attributes.
 *
 * * The **priority inheritance locking protocol** is selected by the
 *   #RTEMS_INHERIT_PRIORITY attribute.
 *
 * * The **priority ceiling locking protocol** is selected by the
 *   #RTEMS_PRIORITY_CEILING attribute.  For this locking protocol a priority
 *   ceiling shall be specified in ``priority_ceiling``.
 *
 * * The **MrsP locking protocol** is selected by the
 *   #RTEMS_MULTIPROCESSOR_RESOURCE_SHARING attribute in SMP configurations,
 *   otherwise this attribute selects the **priority ceiling locking
 *   protocol**.  For these locking protocols a priority ceiling shall be
 *   specified in ``priority_ceiling``.  This priority is used to set the
 *   priority ceiling for all schedulers.  This can be changed later with the
 *   rtems_semaphore_set_priority() directive using the returned object
 *   identifier.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NUMBER The ``count`` parameter was invalid.
 *
 * @retval ::RTEMS_NOT_DEFINED The ``attribute_set`` parameter was invalid.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive object available to create a
 *   semaphore.  The number of semaphores available to the application is
 *   configured through the #CONFIGURE_MAXIMUM_SEMAPHORES application
 *   configuration option.
 *
 * @retval ::RTEMS_TOO_MANY In multiprocessing configurations, there was no
 *   inactive global object available to create a global semaphore.  The number
 *   of global objects available to the application is configured through the
 *   #CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS application configuration option.
 *
 * @retval ::RTEMS_INVALID_PRIORITY The ``priority_ceiling`` parameter was
 *   invalid.
 *
 * @par Notes
 * @parblock
 * For control and maintenance of the semaphore, RTEMS allocates a SMCB from
 * the local SMCB free pool and initializes it.
 *
 * The SMCB for a global semaphore is allocated on the local node.  Semaphores
 * should not be made global unless remote tasks must interact with the
 * semaphore.  This is to avoid the system overhead incurred by the creation of
 * a global semaphore.  When a global semaphore is created, the semaphore's
 * name and identifier must be transmitted to every node in the system for
 * insertion in the local copy of the global object table.
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
 * * The number of semaphores available to the application is configured
 *   through the #CONFIGURE_MAXIMUM_SEMAPHORES application configuration
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
rtems_status_code rtems_semaphore_create(
  rtems_name          name,
  uint32_t            count,
  rtems_attribute     attribute_set,
  rtems_task_priority priority_ceiling,
  rtems_id           *id
);

/* Generated from spec:/rtems/sem/if/ident */

/**
 * @ingroup RTEMSAPIClassicSem
 *
 * @brief Identifies a semaphore by the object name.
 *
 * @param name is the object name to look up.
 *
 * @param node is the node or node set to search for a matching object.
 *
 * @param[out] id is the pointer to an object identifier variable.  When the
 *   directive call is successful, the object identifier of an object with the
 *   specified name will be stored in this variable.
 *
 * This directive obtains a semaphore identifier associated with the semaphore
 * name specified in ``name``.
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
 * If the semaphore name is not unique, then the semaphore identifier will
 * match the first semaphore with that name in the search order.  However, this
 * semaphore identifier is not guaranteed to correspond to the desired
 * semaphore.
 *
 * The objects are searched from lowest to the highest index.  If ``node`` is
 * #RTEMS_SEARCH_ALL_NODES, all nodes are searched with the local node being
 * searched first.  All other nodes are searched from lowest to the highest
 * node number.
 *
 * If node is a valid node number which does not represent the local node, then
 * only the semaphores exported by the designated node are searched.
 *
 * This directive does not generate activity on remote nodes.  It accesses only
 * the local copy of the global object table.
 *
 * The semaphore identifier is used with other semaphore related directives to
 * access the semaphore.
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
rtems_status_code rtems_semaphore_ident(
  rtems_name name,
  uint32_t   node,
  rtems_id  *id
);

/* Generated from spec:/rtems/sem/if/delete */

/**
 * @ingroup RTEMSAPIClassicSem
 *
 * @brief Deletes the semaphore.
 *
 * @param id is the semaphore identifier.
 *
 * This directive deletes the semaphore specified by ``id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no semaphore associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The semaphore resided on a remote
 *   node.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE The binary semaphore had an owner.
 *
 * @par Notes
 * @parblock
 * Binary semaphores with an owner cannot be deleted.
 *
 * When a semaphore is deleted, all tasks blocked waiting to obtain the
 * semaphore will be readied and returned a status code which indicates that
 * the semaphore was deleted.
 *
 * The SMCB for the deleted semaphore is reclaimed by RTEMS.
 *
 * When a global semaphore is deleted, the semaphore identifier must be
 * transmitted to every node in the system for deletion from the local copy of
 * the global object table.
 *
 * The semaphore must reside on the local node, even if the semaphore was
 * created with the #RTEMS_GLOBAL attribute.
 *
 * Proxies, used to represent remote tasks, are reclaimed when the semaphore is
 * deleted.
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
rtems_status_code rtems_semaphore_delete( rtems_id id );

/* Generated from spec:/rtems/sem/if/obtain */

/**
 * @ingroup RTEMSAPIClassicSem
 *
 * @brief Obtains the semaphore.
 *
 * @param id is the semaphore identifier.
 *
 * @param option_set is the option set.
 *
 * @param timeout is the timeout in clock ticks if the #RTEMS_WAIT option is
 *   set.  Use #RTEMS_NO_TIMEOUT to wait potentially forever.
 *
 * This directive obtains the semaphore specified by ``id``.
 *
 * The **option set** specified in ``option_set`` is built through a *bitwise
 * or* of the option constants described below.  Not all combinations of
 * options are allowed.  Some options are mutually exclusive.  If mutually
 * exclusive options are combined, the behaviour is undefined.  Options not
 * mentioned below are not evaluated by this directive and have no effect.
 * Default options can be selected by using the #RTEMS_DEFAULT_OPTIONS
 * constant.
 *
 * The calling task can **wait** or **try to obtain** the semaphore according
 * to the mutually exclusive #RTEMS_WAIT and #RTEMS_NO_WAIT options.
 *
 * * **Waiting to obtain** the semaphore is the default and can be emphasized
 *   through the use of the #RTEMS_WAIT option.  The ``timeout`` parameter
 *   defines how long the calling task is willing to wait.  Use
 *   #RTEMS_NO_TIMEOUT to wait potentially forever, otherwise set a timeout
 *   interval in clock ticks.
 *
 * * **Trying to obtain** the semaphore is selected by the #RTEMS_NO_WAIT
 *   option.  If this option is defined, then the ``timeout`` parameter is
 *   ignored.  When the semaphore cannot be immediately obtained, then the
 *   ::RTEMS_UNSATISFIED status is returned.
 *
 * With either #RTEMS_WAIT or #RTEMS_NO_WAIT if the current semaphore count is
 * positive, then it is decremented by one and the semaphore is successfully
 * obtained by returning immediately with the ::RTEMS_SUCCESSFUL status code.
 *
 * If the calling task chooses to return immediately and the current semaphore
 * count is zero, then the ::RTEMS_UNSATISFIED status code is returned
 * indicating that the semaphore is not available.
 *
 * If the calling task chooses to wait for a semaphore and the current
 * semaphore count is zero, then the calling task is placed on the semaphore's
 * wait queue and blocked.  If a local, binary semaphore was created with the
 * #RTEMS_INHERIT_PRIORITY attribute, then the priority of the task currently
 * holding the binary semaphore will inherit the current priority set of the
 * blocking task.  The priority inheritance is carried out recursively.  This
 * means, that if the task currently holding the binary semaphore is blocked on
 * another local, binary semaphore using the priority inheritance locking
 * protocol, then the owner of this semaphore will inherit the current priority
 * sets of both tasks, and so on.  A task has a current priority for each
 * scheduler.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no semaphore associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_UNSATISFIED The semaphore could not be obtained immediately.
 *
 * @retval ::RTEMS_INCORRECT_STATE Acquiring of the local, binary semaphore by
 *   the calling task would have cased a deadlock.
 *
 * @retval ::RTEMS_INCORRECT_STATE The calling task attempted to recursively
 *   obtain a local, binary semaphore using the MrsP locking protocol.
 *
 * @retval ::RTEMS_UNSATISFIED The semaphore was flushed while the calling task
 *   was waiting to obtain the semaphore.
 *
 * @retval ::RTEMS_TIMEOUT The timeout happened while the calling task was
 *   waiting to obtain the semaphore.
 *
 * @retval ::RTEMS_OBJECT_WAS_DELETED The semaphore was deleted while the
 *   calling task was waiting to obtain the semaphore.
 *
 * @par Notes
 * @parblock
 * If a local, binary semaphore was created with the #RTEMS_PRIORITY_CEILING or
 * #RTEMS_MULTIPROCESSOR_RESOURCE_SHARING attribute, a task successfully
 * obtains the semaphore, and the priority of that task is greater than the
 * ceiling priority for this semaphore, then the priority of the task acquiring
 * the semaphore is elevated to that of the ceiling.
 *
 * Deadlock situations are detected for local, binary semaphores.  If a
 * deadlock is detected, then the directive immediately returns the
 * ::RTEMS_INCORRECT_STATE status code.
 *
 * It is not allowed to recursively obtain (nested access) a local, binary
 * semaphore using the MrsP locking protocol and any attempt to do this will
 * just return the ::RTEMS_INCORRECT_STATE status code.  This error can only
 * happen in SMP configurations.
 *
 * If the semaphore was created with the #RTEMS_PRIORITY attribute, then the
 * calling task is inserted into the wait queue according to its priority.
 * However, if the semaphore was created with the #RTEMS_FIFO attribute, then
 * the calling task is placed at the rear of the wait queue.
 *
 * Attempting to obtain a global semaphore which does not reside on the local
 * node will generate a request to the remote node to access the semaphore.  If
 * the semaphore is not available and #RTEMS_NO_WAIT was not specified, then
 * the task must be blocked until the semaphore is released.  A proxy is
 * allocated on the remote node to represent the task until the semaphore is
 * released.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * When a local, counting semaphore or a local, simple binary semaphore is
 *   accessed and the #RTEMS_NO_WAIT option is set, the directive may be called
 *   from within interrupt context.
 *
 * * When a local semaphore is accessed and the request can be immediately
 *   satisfied, the directive may be called from within device driver
 *   initialization context.
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
rtems_status_code rtems_semaphore_obtain(
  rtems_id       id,
  rtems_option   option_set,
  rtems_interval timeout
);

/* Generated from spec:/rtems/sem/if/release */

/**
 * @ingroup RTEMSAPIClassicSem
 *
 * @brief Releases the semaphore.
 *
 * @param id is the semaphore identifier.
 *
 * This directive releases the semaphore specified by ``id``.  If the
 * semaphore's wait queue is not empty, then
 *
 * * the first task on the wait queue is removed and unblocked, the semaphore's
 *   count is not changed, otherwise
 *
 * * the semaphore's count is incremented by one for counting semaphores and
 *   set to one for binary and simple binary semaphores.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no semaphore associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_NOT_OWNER_OF_RESOURCE The calling task was not the owner of
 *   the semaphore.
 *
 * @retval ::RTEMS_INTERNAL_ERROR The semaphore's count already had the maximum
 *   value of UINT32_MAX.
 *
 * @par Notes
 * @parblock
 * The calling task may be preempted if it causes a higher priority task to be
 * made ready for execution.
 *
 * The outermost release of a local, binary semaphore using the priority
 * inheritance, priority ceiling, or MrsP locking protocol may result in the
 * calling task having its priority lowered.  This will occur if the highest
 * priority of the calling task was available due to the ownership of the
 * released semaphore.  If a task was on the semaphore's wait queue, then the
 * priority associated with the semaphore will be transferred to the new owner.
 *
 * Releasing a global semaphore which does not reside on the local node will
 * generate a request telling the remote node to release the semaphore.
 *
 * If the task to be unblocked resides on a different node from the semaphore,
 * then the semaphore allocation is forwarded to the appropriate node, the
 * waiting task is unblocked, and the proxy used to represent the task is
 * reclaimed.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * When a local, counting semaphore or a local, simple binary semaphore is
 *   accessed, the directive may be called from within interrupt context.
 *
 * * When a local semaphore is accessed, the directive may be called from
 *   within device driver initialization context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may unblock another task which may preempt the calling task.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_semaphore_release( rtems_id id );

/* Generated from spec:/rtems/sem/if/flush */

/**
 * @ingroup RTEMSAPIClassicSem
 *
 * @brief Flushes the semaphore.
 *
 * @param id is the semaphore identifier.
 *
 * This directive unblocks all tasks waiting on the semaphore specified by
 * ``id``.  The semaphore's count is not changed by this directive.  Tasks
 * which are unblocked as the result of this directive will return from the
 * rtems_semaphore_obtain() directive with a status code of ::RTEMS_UNSATISFIED
 * to indicate that the semaphore was not obtained.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no semaphore associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The semaphore resided on a remote
 *   node.
 *
 * @retval ::RTEMS_NOT_DEFINED Flushing a semaphore using the MrsP locking
 *   protocol is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * If the task to be unblocked resides on a different node from the semaphore,
 * then the waiting task is unblocked, and the proxy used to represent the task
 * is reclaimed.
 *
 * It is not allowed to flush a local, binary semaphore using the MrsP locking
 * protocol and any attempt to do this will just return the ::RTEMS_NOT_DEFINED
 * status code.  This error can only happen in SMP configurations.
 *
 * For barrier synchronization, the @ref RTEMSAPIClassicBarrier offers a
 * cleaner alternative to using the semaphore flush directive.  Unlike POSIX
 * barriers, they have a manual release option.
 *
 * Using the semaphore flush directive for condition synchronization in concert
 * with another semaphore may be subject to the lost wake-up problem.  The
 * following attempt to implement a condition variable is broken.
 *
 * @code
 * #include <rtems.h>
 * #include <assert.h>
 *
 * void cnd_wait( rtems_id cnd, rtems_id mtx )
 * {
 *   rtems_status_code sc;
 *
 *   sc = rtems_semaphore_release( mtx );
 *   assert( sc == RTEMS_SUCCESSFUL );
 *
 *   // Here, a higher priority task may run and satisfy the condition.
 *   // We may never wake up from the next semaphore obtain.
 *
 *   sc = rtems_semaphore_obtain( cnd, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
 *   assert( sc == RTEMS_UNSATISFIED );
 *
 *   sc = rtems_semaphore_obtain( mtx, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
 *   assert( sc == RTEMS_SUCCESSFUL );
 * }
 *
 * void cnd_broadcast( rtems_id cnd )
 * {
 *   rtems_status_code sc;
 *
 *   sc = rtems_semaphore_flush( cnd );
 *   assert( sc == RTEMS_SUCCESSFUL );
 * }
 * @endcode
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * When a local, counting semaphore or a local, simple binary semaphore is
 *   accessed, the directive may be called from within interrupt context.
 *
 * * When a local semaphore is accessed, the directive may be called from
 *   within device driver initialization context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may unblock another task which may preempt the calling task.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_semaphore_flush( rtems_id id );

/* Generated from spec:/rtems/sem/if/set-priority */

/**
 * @ingroup RTEMSAPIClassicSem
 *
 * @brief Sets the priority by scheduler for the semaphore.
 *
 * @param semaphore_id is the semaphore identifier.
 *
 * @param scheduler_id is the identifier of the scheduler corresponding to the
 *   new priority.
 *
 * @param new_priority is the new priority corresponding to the specified
 *   scheduler.
 *
 * @param[out] old_priority is the pointer to a task priority variable.  When
 *   the directive call is successful, the old priority of the semaphore
 *   corresponding to the specified scheduler will be stored in this variable.
 *
 * This directive sets the priority of the semaphore specified by
 * ``semaphore_id``.  The priority corresponds to the scheduler specified by
 * ``scheduler_id``.
 *
 * The special priority value #RTEMS_CURRENT_PRIORITY can be used to get the
 * current priority without changing it.
 *
 * The availability and use of a priority depends on the class and locking
 * protocol of the semaphore:
 *
 * * For local, binary semaphores using the MrsP locking protocol, the ceiling
 *   priority for each scheduler can be set by this directive.
 *
 * * For local, binary semaphores using the priority ceiling protocol, the
 *   ceiling priority can be set by this directive.
 *
 * * For other semaphore classes and locking protocols, setting a priority is
 *   undefined behaviour.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``old_priority`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no scheduler associated with the
 *   identifier specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_INVALID_ID There was no semaphore associated with the
 *   identifier specified by ``semaphore_id``.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The semaphore resided on a remote
 *   node.
 *
 * @retval ::RTEMS_INVALID_PRIORITY The ``new_priority`` parameter was invalid.
 *
 * @retval ::RTEMS_NOT_DEFINED Setting a priority for the class or locking
 *   protocol of the semaphore is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * Please have a look at the following example:
 *
 * @code
 * #include <assert.h>
 * #include <rtems.h>
 *
 * #define SCHED_A rtems_build_name( ' ', ' ', ' ', 'A' )
 * #define SCHED_B rtems_build_name( ' ', ' ', ' ', 'B' )
 *
 * static void Init( rtems_task_argument arg )
 * {
 *   rtems_status_code   sc;
 *   rtems_id            semaphore_id;
 *   rtems_id            scheduler_a_id;
 *   rtems_id            scheduler_b_id;
 *   rtems_task_priority prio;
 *
 *   (void) arg;
 *
 *   // Get the scheduler identifiers
 *   sc = rtems_scheduler_ident( SCHED_A, &scheduler_a_id );
 *   assert( sc == RTEMS_SUCCESSFUL );
 *   sc = rtems_scheduler_ident( SCHED_B, &scheduler_b_id );
 *   assert( sc == RTEMS_SUCCESSFUL );
 *
 *   // Create a local, binary semaphore using the MrsP locking protocol
 *   sc = rtems_semaphore_create(
 *     rtems_build_name( 'M', 'R', 'S', 'P' ),
 *     1,
 *     RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
 *       RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
 *     1,
 *     &semaphore_id
 *   );
 *   assert( sc == RTEMS_SUCCESSFUL );
 *
 *   // The ceiling priority for each scheduler is equal to the priority
 *   // specified for the semaphore creation.
 *   prio = RTEMS_CURRENT_PRIORITY;
 *   sc = rtems_semaphore_set_priority( semaphore_id, scheduler_a_id, prio, &prio );
 *   assert( sc == RTEMS_SUCCESSFUL );
 *   assert( prio == 1 );
 *
 *   // Check the old value and set a new ceiling priority for scheduler B
 *   prio = 2;
 *   sc = rtems_semaphore_set_priority( semaphore_id, scheduler_b_id, prio, &prio );
 *   assert( sc == RTEMS_SUCCESSFUL );
 *   assert( prio == 1 );
 *
 *   // Check the ceiling priority values
 *   prio = RTEMS_CURRENT_PRIORITY;
 *   sc = rtems_semaphore_set_priority( semaphore_id, scheduler_a_id, prio, &prio );
 *   assert( sc == RTEMS_SUCCESSFUL );
 *   assert( prio == 1 );
 *   prio = RTEMS_CURRENT_PRIORITY;
 *   sc = rtems_semaphore_set_priority( semaphore_id, scheduler_b_id, prio, &prio );
 *   assert( sc == RTEMS_SUCCESSFUL );
 *   assert( prio == 2 );
 *
 *   sc = rtems_semaphore_delete( semaphore_id );
 *   assert( sc == RTEMS_SUCCESSFUL );
 *
 *   rtems_shutdown_executive( 0 );
 * }
 *
 * #define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
 * #define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
 * #define CONFIGURE_MAXIMUM_TASKS 1
 * #define CONFIGURE_MAXIMUM_SEMAPHORES 1
 * #define CONFIGURE_MAXIMUM_PROCESSORS 2
 *
 * #define CONFIGURE_SCHEDULER_SIMPLE_SMP
 *
 * #include <rtems/scheduler.h>
 *
 * RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP( a );
 * RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP( b );
 *
 * #define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
 *     RTEMS_SCHEDULER_TABLE_SIMPLE_SMP( a, SCHED_A ), \
 *     RTEMS_SCHEDULER_TABLE_SIMPLE_SMP( b, SCHED_B )
 *
 * #define CONFIGURE_SCHEDULER_ASSIGNMENTS \
 *     RTEMS_SCHEDULER_ASSIGN( 0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY ), \
 *     RTEMS_SCHEDULER_ASSIGN( 1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY )
 *
 * #define CONFIGURE_RTEMS_INIT_TASKS_TABLE
 * #define CONFIGURE_INIT
 *
 * #include <rtems/confdefs.h>
 * @endcode
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may change the priority of another task which may preempt
 *   the calling task.
 * @endparblock
 */
rtems_status_code rtems_semaphore_set_priority(
  rtems_id             semaphore_id,
  rtems_id             scheduler_id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_SEM_H */
