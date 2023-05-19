/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_Set_affinity().
 */

/*
 * Copyright (C) 2014, 2017 embedded brains GmbH & Co. KG
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

#include <rtems/score/schedulerimpl.h>

Status_Control _Scheduler_Set_affinity(
  Thread_Control       *the_thread,
  size_t                cpusetsize,
  const cpu_set_t      *cpuset
)
{
  Processor_mask             affinity;
  Processor_mask_Copy_status copy_status;
  const Scheduler_Control   *scheduler;
  Scheduler_Node            *node;
  ISR_lock_Context           lock_context;
  Status_Control             status;

  copy_status = _Processor_mask_From_cpu_set_t( &affinity, cpusetsize, cpuset );
  if ( !_Processor_mask_Is_at_most_partial_loss( copy_status ) ) {
    return STATUS_INVALID_NUMBER;
  }

  /*
   * Reduce affinity set to the online processors to be in line with
   * _Thread_Initialize() which sets the default affinity to the set of online
   * processors.
   */
  _Processor_mask_And( &affinity, _SMP_Get_online_processors(), &affinity );

  scheduler = _Thread_Scheduler_get_home( the_thread );
  _Scheduler_Acquire_critical( scheduler, &lock_context );

  node = _Thread_Scheduler_get_home_node( the_thread );
#if defined(RTEMS_SMP)
  status = ( *scheduler->Operations.set_affinity )(
    scheduler,
    the_thread,
    node,
    &affinity
  );

  if ( status == STATUS_SUCCESSFUL ) {
    _Processor_mask_Assign( &the_thread->Scheduler.Affinity, &affinity );
  }
#else
  status = _Scheduler_default_Set_affinity_body(
    scheduler,
    the_thread,
    node,
    &affinity
  );
#endif

  _Scheduler_Release_critical( scheduler, &lock_context );
  return status;
}
