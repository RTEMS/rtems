/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_get_scheduler().
 */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/schedulerimpl.h>

rtems_status_code rtems_task_get_scheduler(
  rtems_id  task_id,
  rtems_id *scheduler_id
)
{
  Thread_Control          *the_thread;
  ISR_lock_Context         lock_context;
  const Scheduler_Control *scheduler;

  if ( scheduler_id == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_thread = _Thread_Get( task_id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_MP_Is_remote( task_id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  _Thread_State_acquire_critical( the_thread, &lock_context );

  scheduler = _Thread_Scheduler_get_home( the_thread );
  *scheduler_id = _Scheduler_Build_id( _Scheduler_Get_index( scheduler ) );

  _Thread_State_release( the_thread, &lock_context );
  return RTEMS_SUCCESSFUL;
}
