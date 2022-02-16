/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   _RTEMS_tasks_Initialize_user_task().
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/interr.h>

void _RTEMS_tasks_Initialize_user_task( void )
{
  rtems_id                                id;
  rtems_status_code                       return_value;
  const rtems_initialization_tasks_table *user_task;

  user_task = &_RTEMS_tasks_User_task_table;
  return_value = rtems_task_create(
    user_task->name,
    user_task->initial_priority,
    user_task->stack_size,
    user_task->mode_set,
    user_task->attribute_set,
    &id
  );
  if ( !rtems_is_status_successful( return_value ) ) {
    _Internal_error( INTERNAL_ERROR_RTEMS_INIT_TASK_CREATE_FAILED );
  }

  return_value = rtems_task_start(
    id,
    user_task->entry_point,
    user_task->argument
  );
  _Assert( rtems_is_status_successful( return_value ) );
  (void) return_value;

  _Assert( _Thread_Global_constructor == 0 );
  _Thread_Global_constructor = id;
}
