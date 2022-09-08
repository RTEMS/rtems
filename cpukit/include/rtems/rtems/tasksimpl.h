/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicTask.
 */

/*  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_RTEMS_TASKSIMPL_H
#define _RTEMS_RTEMS_TASKSIMPL_H

#include <rtems/rtems/tasksdata.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicTask Task Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Task Manager implementation.
 *
 * @{
 */

/**
 *  @brief RTEMS User Task Initialization
 *
 *  This routine creates and starts all configured user
 *  initialization threads.
 */
void _RTEMS_tasks_Initialize_user_tasks( void );

typedef rtems_status_code ( *RTEMS_tasks_Prepare_stack )(
  Thread_Configuration *,
  const rtems_task_config *
);

rtems_status_code _RTEMS_tasks_Create(
  const rtems_task_config   *config,
  rtems_id                  *id,
  RTEMS_tasks_Prepare_stack  prepare_stack
);

static inline Thread_Control *_RTEMS_tasks_Allocate(void)
{
  _Objects_Allocator_lock();

  _Thread_Kill_zombies();

  return (Thread_Control *)
    _Objects_Allocate_unprotected( &_RTEMS_tasks_Information.Objects );
}

/**
 * @brief Converts the RTEMS API priority to the corresponding SuperCore
 * priority and validates it.
 *
 * The RTEMS API system priority is accepted as valid.
 *
 * @param[in] scheduler The scheduler instance.
 * @param[in] priority The RTEMS API priority to convert and validate.
 * @param[out] valid Indicates if the RTEMS API priority is valid and a
 *   corresponding SuperCore priority in the specified scheduler instance
 *   exists.
 *
 * @return The corresponding SuperCore priority.
 */
static inline Priority_Control _RTEMS_Priority_To_core(
  const Scheduler_Control *scheduler,
  rtems_task_priority      priority,
  bool                    *valid
)
{
  *valid = ( priority <= scheduler->maximum_priority );

  return _Scheduler_Map_priority( scheduler, (Priority_Control) priority );
}

/**
 * @brief Converts the SuperCore priority to the corresponding RTEMS API
 * priority.
 *
 * @param[in] scheduler The scheduler instance.
 * @param[in] priority The SuperCore priority to convert.
 *
 * @return The corresponding RTEMS API priority.
 */
static inline rtems_task_priority _RTEMS_Priority_From_core(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
)
{
  return (rtems_task_priority)
    _Scheduler_Unmap_priority( scheduler, priority );
}

/**@}*/

/**
 * @defgroup RTEMSImplClassicScheduler Scheduler Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Scheduler Manager implementation.
 */

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/taskmp.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
