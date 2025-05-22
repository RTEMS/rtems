/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicScheduler
 *
 * @brief This header file defines the main parts of the Scheduler Manager API.
 */

/*
 * Copyright (C) 2013, 2021 embedded brains GmbH & Co. KG
 * Copyright (C) 1988, 2017 On-Line Applications Research Corporation (OAR)
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

/* Generated from spec:/rtems/scheduler/if/header */

#ifndef _RTEMS_RTEMS_SCHEDULER_H
#define _RTEMS_RTEMS_SCHEDULER_H

#include <stddef.h>
#include <stdint.h>
#include <sys/cpuset.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/score/smp.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/scheduler/if/group */

/**
 * @defgroup RTEMSAPIClassicScheduler Scheduler Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The scheduling concepts relate to the allocation of processing time
 *   for tasks.
 *
 * The concept of scheduling in real-time systems dictates the ability to
 * provide an immediate response to specific external events, particularly the
 * necessity of scheduling tasks to run within a specified time limit after the
 * occurrence of an event. For example, software embedded in life-support
 * systems used to monitor hospital patients must take instant action if a
 * change in the patient's status is detected.
 *
 * The component of RTEMS responsible for providing this capability is
 * appropriately called the scheduler. The scheduler's sole purpose is to
 * allocate the all important resource of processor time to the various tasks
 * competing for attention.
 */

/* Generated from spec:/rtems/scheduler/if/ident */

/**
 * @ingroup RTEMSAPIClassicScheduler
 *
 * @brief Identifies a scheduler by the object name.
 *
 * @param name is the scheduler name to look up.
 *
 * @param[out] id is the pointer to an ::rtems_id object.  When the directive
 *   call is successful, the identifier of the scheduler will be stored in this
 *   object.
 *
 * This directive obtains a scheduler identifier associated with the scheduler
 * name specified in ``name``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME There was no scheduler associated with the
 *   name.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @par Notes
 * @parblock
 * The scheduler name is determined by the scheduler configuration.
 *
 * The scheduler identifier is used with other scheduler related directives to
 * access the scheduler.
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
rtems_status_code rtems_scheduler_ident( rtems_name name, rtems_id *id );

/* Generated from spec:/rtems/scheduler/if/ident-by-processor */

/**
 * @ingroup RTEMSAPIClassicScheduler
 *
 * @brief Identifies a scheduler by the processor index.
 *
 * @param cpu_index is the processor index to identify the scheduler.
 *
 * @param[out] id is the pointer to an ::rtems_id object.  When the directive
 *   call is successful, the identifier of the scheduler will be stored in this
 *   object.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The processor index was invalid.
 *
 * @retval ::RTEMS_INCORRECT_STATE The processor index was valid, however, the
 *   corresponding processor was not owned by a scheduler.
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
rtems_status_code rtems_scheduler_ident_by_processor(
  uint32_t  cpu_index,
  rtems_id *id
);

/* Generated from spec:/rtems/scheduler/if/ident-by-processor-set */

/**
 * @ingroup RTEMSAPIClassicScheduler
 *
 * @brief Identifies a scheduler by the processor set.
 *
 * @param cpusetsize is the size of the processor set referenced by ``cpuset``
 *   in bytes.  The size shall be positive.
 *
 * @param cpuset is the pointer to a cpu_set_t.  The referenced processor set
 *   will be used to identify the scheduler.
 *
 * @param[out] id is the pointer to an ::rtems_id object.  When the directive
 *   call is successful, the identifier of the scheduler will be stored in this
 *   object.
 *
 * The scheduler is selected according to the highest numbered online processor
 * in the specified processor set.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``cpuset`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_SIZE The processor set size was invalid.
 *
 * @retval ::RTEMS_INVALID_NAME The processor set contained no online
 *   processor.
 *
 * @retval ::RTEMS_INCORRECT_STATE The processor set was valid, however, the
 *   highest numbered online processor in the processor set was not owned by a
 *   scheduler.
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
rtems_status_code rtems_scheduler_ident_by_processor_set(
  size_t           cpusetsize,
  const cpu_set_t *cpuset,
  rtems_id        *id
);

/* Generated from spec:/rtems/scheduler/if/get-maximum-priority */

/**
 * @ingroup RTEMSAPIClassicScheduler
 *
 * @brief Gets the maximum task priority of the scheduler.
 *
 * @param scheduler_id is the scheduler identifier.
 *
 * @param[out] priority is the pointer to an ::rtems_task_priority object.
 *   When the directive the maximum priority of the scheduler will be stored in
 *   this object.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no scheduler associated with the
 *   identifier specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``priority`` parameter was NULL.
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
rtems_status_code rtems_scheduler_get_maximum_priority(
  rtems_id             scheduler_id,
  rtems_task_priority *priority
);

/* Generated from spec:/rtems/scheduler/if/map-priority-to-posix */

/**
 * @ingroup RTEMSAPIClassicScheduler
 *
 * @brief Maps a Classic API task priority to the corresponding POSIX thread
 *   priority.
 *
 * @param scheduler_id is the scheduler identifier.
 *
 * @param priority is the Classic API task priority to map.
 *
 * @param[out] posix_priority is the pointer to an ``int`` object.  When the
 *   directive call is successful, the POSIX thread priority value
 *   corresponding to the specified Classic API task priority value will be
 *   stored in this object.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``posix_priority`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no scheduler associated with the
 *   identifier specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_INVALID_PRIORITY The Classic API task priority was invalid.
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
rtems_status_code rtems_scheduler_map_priority_to_posix(
  rtems_id            scheduler_id,
  rtems_task_priority priority,
  int                *posix_priority
);

/* Generated from spec:/rtems/scheduler/if/map-priority-from-posix */

/**
 * @ingroup RTEMSAPIClassicScheduler
 *
 * @brief Maps a POSIX thread priority to the corresponding Classic API task
 *   priority.
 *
 * @param scheduler_id is the scheduler identifier.
 *
 * @param posix_priority is the POSIX thread priority to map.
 *
 * @param[out] priority is the pointer to an ::rtems_task_priority object.
 *   When the directive call is successful, the Classic API task priority value
 *   corresponding to the specified POSIX thread priority value will be stored
 *   in this object.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``priority`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no scheduler associated with the
 *   identifier specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_INVALID_PRIORITY The POSIX thread priority was invalid.
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
rtems_status_code rtems_scheduler_map_priority_from_posix(
  rtems_id             scheduler_id,
  int                  posix_priority,
  rtems_task_priority *priority
);

/* Generated from spec:/rtems/scheduler/if/get-processor */

/**
 * @ingroup RTEMSAPIClassicScheduler
 *
 * @brief Returns the index of the current processor.
 *
 * Where the system was built with SMP support disabled, this directive
 * evaluates to a compile time constant of zero.
 *
 * Where the system was built with SMP support enabled, this directive returns
 * the index of the current processor.  The set of processor indices is the
 * range of integers starting with zero up to
 * rtems_scheduler_get_processor_maximum() minus one.
 *
 * @return Returns the index of the current processor.
 *
 * @par Notes
 * Outside of sections with disabled thread dispatching the current processor
 * index may change after every instruction since the thread may migrate from
 * one processor to another.  Sections with disabled interrupts are sections
 * with thread dispatching disabled.
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
uint32_t rtems_scheduler_get_processor( void );

/* Generated from spec:/rtems/scheduler/if/get-processor-macro */
#define rtems_scheduler_get_processor() _SMP_Get_current_processor()

/* Generated from spec:/rtems/scheduler/if/get-processor-maximum */

/**
 * @ingroup RTEMSAPIClassicScheduler
 *
 * @brief Returns the processor maximum supported by the system.
 *
 * Where the system was built with SMP support disabled, this directive
 * evaluates to a compile time constant of one.
 *
 * Where the system was built with SMP support enabled, this directive returns
 * the minimum of the processors (physically or virtually) available at the
 * target and the configured processor maximum (see @ref
 * CONFIGURE_MAXIMUM_PROCESSORS).  Not all processors in the range from
 * processor index zero to the last processor index (which is the processor
 * maximum minus one) may be configured to be used by a scheduler or may be
 * online (online processors have a scheduler assigned).
 *
 * @return Returns the processor maximum supported by the system.
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
uint32_t rtems_scheduler_get_processor_maximum( void );

/* Generated from spec:/rtems/scheduler/if/get-processor-maximum-macro */
#define rtems_scheduler_get_processor_maximum() _SMP_Get_processor_maximum()

/* Generated from spec:/rtems/scheduler/if/get-processor-set */

/**
 * @ingroup RTEMSAPIClassicScheduler
 *
 * @brief Gets the set of processors owned by the scheduler.
 *
 * @param scheduler_id is the scheduler identifier.
 *
 * @param cpusetsize is the size of the processor set referenced by ``cpuset``
 *   in bytes.
 *
 * @param[out] cpuset is the pointer to a cpu_set_t object.  When the directive
 *   call is successful, the processor set of the scheduler will be stored in
 *   this object.  A set bit in the processor set means that the corresponding
 *   processor is owned by the scheduler, otherwise the bit is cleared.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``cpuset`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no scheduler associated with the
 *   identifier specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_INVALID_SIZE The provided processor set was too small for
 *   the set of processors owned by the scheduler.
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
rtems_status_code rtems_scheduler_get_processor_set(
  rtems_id   scheduler_id,
  size_t     cpusetsize,
  cpu_set_t *cpuset
);

/* Generated from spec:/rtems/scheduler/if/add-processor */

/**
 * @ingroup RTEMSAPIClassicScheduler
 *
 * @brief Adds the processor to the set of processors owned by the scheduler.
 *
 * @param scheduler_id is the scheduler identifier.
 *
 * @param cpu_index is the index of the processor to add.
 *
 * This directive adds the processor specified by the ``cpu_index`` to the
 * scheduler specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no scheduler associated with the
 *   identifier specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_NOT_CONFIGURED The processor was not configured to be used
 *   by the application.
 *
 * @retval ::RTEMS_INCORRECT_STATE The processor was configured to be used by
 *   the application, however, it was not online.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE The processor was already assigned to a
 *   scheduler.
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
 * @endparblock
 */
rtems_status_code rtems_scheduler_add_processor(
  rtems_id scheduler_id,
  uint32_t cpu_index
);

/* Generated from spec:/rtems/scheduler/if/remove-processor */

/**
 * @ingroup RTEMSAPIClassicScheduler
 *
 * @brief Removes the processor from the set of processors owned by the
 *   scheduler.
 *
 * @param scheduler_id is the scheduler identifier.
 *
 * @param cpu_index is the index of the processor to remove.
 *
 * This directive removes the processor specified by the ``cpu_index`` from the
 * scheduler specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no scheduler associated with the
 *   identifier specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_INVALID_NUMBER The processor was not owned by the scheduler.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE The processor was required by at least one
 *   non-idle task that used the scheduler as its home scheduler.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE The processor was the last processor owned
 *   by the scheduler and there was at least one task that used the scheduler
 *   as a helping scheduler.
 *
 * @par Notes
 * Removing a processor from a scheduler is a complex operation that involves
 * all tasks of the system.
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
 * @endparblock
 */
rtems_status_code rtems_scheduler_remove_processor(
  rtems_id scheduler_id,
  uint32_t cpu_index
);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_SCHEDULER_H */
