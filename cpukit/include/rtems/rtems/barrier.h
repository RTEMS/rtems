/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicBarrier
 *
 * @brief This header file defines the Barrier Manager API.
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH & Co. KG
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

/* Generated from spec:/rtems/barrier/if/header */

#ifndef _RTEMS_RTEMS_BARRIER_H
#define _RTEMS_RTEMS_BARRIER_H

#include <stdint.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/barrier/if/group */

/**
 * @defgroup RTEMSAPIClassicBarrier Barrier Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Barrier Manager provides a unique synchronization capability
 *   which can be used to have a set of tasks block and be unblocked as a set.
 */

/* Generated from spec:/rtems/barrier/if/create */

/**
 * @ingroup RTEMSAPIClassicBarrier
 *
 * @brief Creates a barrier.
 *
 * @param name is the object name of the barrier.
 *
 * @param attribute_set is the attribute set of the barrier.
 *
 * @param maximum_waiters is the maximum count of waiters on an automatic
 *   release barrier.
 *
 * @param id is the pointer to an ::rtems_id object.  When the directive call
 *   is successful, the identifier of the created barrier will be stored in
 *   this object.
 *
 * This directive creates a barrier which resides on the local node.  The
 * barrier has the user-defined object name specified in ``name`` and the
 * initial count specified in ``attribute_set``.  The assigned object
 * identifier is returned in ``id``.  This identifier is used to access the
 * barrier with other barrier related directives.
 *
 * The **attribute set** specified in ``attribute_set`` is built through a
 * *bitwise or* of the attribute constants described below.  Not all
 * combinations of attributes are allowed.  Some attributes are mutually
 * exclusive.  If mutually exclusive attributes are combined, the behaviour is
 * undefined.  Attributes not mentioned below are not evaluated by this
 * directive and have no effect.  Default attributes can be selected by using
 * the #RTEMS_DEFAULT_ATTRIBUTES constant.
 *
 * The **barrier class** is selected by the mutually exclusive
 * #RTEMS_BARRIER_MANUAL_RELEASE and #RTEMS_BARRIER_AUTOMATIC_RELEASE
 * attributes.
 *
 * * The **manual release class** is the default and can be emphasized through
 *   use of the #RTEMS_BARRIER_MANUAL_RELEASE attribute.  For this class, there
 *   is no limit on the number of tasks that will block at the barrier. Only
 *   when the rtems_barrier_release() directive is invoked, are the tasks
 *   waiting at the barrier unblocked.
 *
 * * The **automatic release class** is selected by the
 *   #RTEMS_BARRIER_AUTOMATIC_RELEASE attribute.  For this class, tasks calling
 *   the rtems_barrier_wait() directive will block until there are
 *   ``maximum_waiters`` minus one tasks waiting at the barrier.  When the
 *   ``maximum_waiters`` task invokes the rtems_barrier_wait() directive, the
 *   previous ``maximum_waiters`` - 1 tasks are automatically released and the
 *   caller returns.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NUMBER The ``maximum_waiters`` parameter was 0 for
 *   an automatic release barrier.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive object available to create a
 *   barrier.  The number of barriers available to the application is
 *   configured through the @ref CONFIGURE_MAXIMUM_BARRIERS application
 *   configuration option.
 *
 * @par Notes
 * For control and maintenance of the barrier, RTEMS allocates a BCB from the
 * local BCB free pool and initializes it.
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
 * * The number of barriers available to the application is configured through
 *   the @ref CONFIGURE_MAXIMUM_BARRIERS application configuration option.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may allocate memory from the RTEMS
 *   Workspace.
 * @endparblock
 */
rtems_status_code rtems_barrier_create(
  rtems_name      name,
  rtems_attribute attribute_set,
  uint32_t        maximum_waiters,
  rtems_id       *id
);

/* Generated from spec:/rtems/barrier/if/ident */

/**
 * @ingroup RTEMSAPIClassicBarrier
 *
 * @brief Identifies a barrier by the object name.
 *
 * @param name is the object name to look up.
 *
 * @param[out] id is the pointer to an ::rtems_id object.  When the directive
 *   call is successful, the object identifier of an object with the specified
 *   name will be stored in this object.
 *
 * This directive obtains a barrier identifier associated with the barrier name
 * specified in ``name``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_NAME There was no object with the specified name on
 *   the local node.
 *
 * @par Notes
 * @parblock
 * If the barrier name is not unique, then the barrier identifier will match
 * the first barrier with that name in the search order.  However, this barrier
 * identifier is not guaranteed to correspond to the desired barrier.
 *
 * The objects are searched from lowest to the highest index.  Only the local
 * node is searched.
 *
 * The barrier identifier is used with other barrier related directives to
 * access the barrier.
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
rtems_status_code rtems_barrier_ident( rtems_name name, rtems_id *id );

/* Generated from spec:/rtems/barrier/if/delete */

/**
 * @ingroup RTEMSAPIClassicBarrier
 *
 * @brief Deletes the barrier.
 *
 * @param id is the barrier identifier.
 *
 * This directive deletes the barrier specified by ``id``.  All tasks blocked
 * waiting for the barrier to be released will be readied and returned a status
 * code which indicates that the barrier was deleted.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no barrier associated with the
 *   identifier specified by ``id``.
 *
 * @par Notes
 * The BCB for the deleted barrier is reclaimed by RTEMS.
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
 * * The calling task does not have to be the task that created the object.
 *   Any local task that knows the object identifier can delete the object.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may free memory to the RTEMS Workspace.
 * @endparblock
 */
rtems_status_code rtems_barrier_delete( rtems_id id );

/* Generated from spec:/rtems/barrier/if/wait */

/**
 * @ingroup RTEMSAPIClassicBarrier
 *
 * @brief Waits at the barrier.
 *
 * @param id is the barrier identifier.
 *
 * @param timeout is the timeout in clock ticks.  Use #RTEMS_NO_TIMEOUT to wait
 *   potentially forever.
 *
 * This directive waits at the barrier specified by ``id``.  The ``timeout``
 * parameter defines how long the calling task is willing to wait.  Use
 * #RTEMS_NO_TIMEOUT to wait potentially forever, otherwise set a timeout
 * interval in clock ticks.
 *
 * Conceptually, the calling task should always be thought of as blocking when
 * it makes this call and being unblocked when the barrier is released.  If the
 * barrier is configured for manual release, this rule of thumb will always be
 * valid.  If the barrier is configured for automatic release, all callers will
 * block except for the one which trips the automatic release condition.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no barrier associated with the
 *   identifier specified by ``id``.
 *
 * @retval ::RTEMS_TIMEOUT The timeout happened while the calling task was
 *   waiting at the barrier.
 *
 * @retval ::RTEMS_OBJECT_WAS_DELETED The barrier was deleted while the calling
 *   task was waiting at the barrier.
 *
 * @par Notes
 * For automatic release barriers, the maximum count of waiting tasks is
 * defined during barrier creation, see rtems_barrier_create().
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The timeout functionality of the directive requires a clock tick.
 * @endparblock
 */
rtems_status_code rtems_barrier_wait( rtems_id id, rtems_interval timeout );

/* Generated from spec:/rtems/barrier/if/release */

/**
 * @ingroup RTEMSAPIClassicBarrier
 *
 * @brief Releases the barrier.
 *
 * @param id is the barrier identifier.
 *
 * @param[out] released is the pointer to an uint32_t object.  When the
 *   directive call is successful, the number of released tasks will be stored
 *   in this object.
 *
 * This directive releases the barrier specified by ``id``.  All tasks waiting
 * at the barrier will be unblocked.  The number of released tasks will be
 * returned in ``released``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``released`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no barrier associated with the
 *   identifier specified by ``id``.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may unblock a task.  This may cause the calling task to be
 *   preempted.
 * @endparblock
 */
rtems_status_code rtems_barrier_release( rtems_id id, uint32_t *released );

/* Generated from spec:/rtems/barrier/if/get-number-waiting */

/**
 * @ingroup RTEMSAPIClassicBarrier
 *
 * @brief Gets the number of tasks waiting at the barrier.
 *
 * @param id is the barrier identifier.
 *
 * @param[out] waiting is the pointer to an uint32_t object. When the directive
 *   call is successful, the number of waiting tasks will be stored in this
 *   object.
 *
 * This directive gets the number of tasks waiting at the barrier specified by
 * ``id``. The number of waiting tasks will be returned in ``waiting``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``waiting`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no barrier associated with the
 *   identifier specified by ``id``.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within task context.
 * @endparblock
 */
rtems_status_code rtems_barrier_get_number_waiting(
  rtems_id  id,
  uint32_t *waiting
);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_BARRIER_H */
