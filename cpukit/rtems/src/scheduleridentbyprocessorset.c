/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicScheduler
 *
 * @brief This source file contains the implementation of
 *   rtems_scheduler_ident_by_processor_set().
 */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#include <rtems/rtems/scheduler.h>
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>

rtems_status_code rtems_scheduler_ident_by_processor_set(
  size_t           cpusetsize,
  const cpu_set_t *cpuset,
  rtems_id        *id
)
{
  Processor_mask              set;
  Processor_mask_Copy_status  status;
  uint32_t                    cpu_index;
  const Scheduler_Control    *scheduler;

  if ( id == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( cpuset == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  status = _Processor_mask_From_cpu_set_t( &set, cpusetsize, cpuset );
  if ( status == PROCESSOR_MASK_COPY_INVALID_SIZE ) {
    return RTEMS_INVALID_SIZE;
  }

  _Processor_mask_And( &set, &set, _SMP_Get_online_processors() );
  cpu_index = _Processor_mask_Find_last_set( &set );
  if ( cpu_index == 0 ) {
    return RTEMS_INVALID_NAME;
  }

  scheduler = _Scheduler_Get_by_CPU( _Per_CPU_Get_by_index( cpu_index - 1) );
#if defined(RTEMS_SMP)
  if ( scheduler == NULL ) {
    return RTEMS_INCORRECT_STATE;
  }
#else
  _Assert( scheduler != NULL );
#endif

  *id = _Scheduler_Build_id( _Scheduler_Get_index( scheduler ) );
  return RTEMS_SUCCESSFUL;
}
