/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_Get_affinity().
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

Status_Control _Scheduler_Get_affinity(
  Thread_Control *the_thread,
  size_t          cpusetsize,
  cpu_set_t      *cpuset
)
{
  const Scheduler_Control    *scheduler;
  ISR_lock_Context            lock_context;
  const Processor_mask       *affinity;
  Processor_mask_Copy_status  status;

  scheduler = _Thread_Scheduler_get_home( the_thread );
  _Scheduler_Acquire_critical( scheduler, &lock_context );

#if defined(RTEMS_SMP)
  affinity = &the_thread->Scheduler.Affinity;
#else
  affinity = &_Processor_mask_The_one_and_only;
#endif
  status = _Processor_mask_To_cpu_set_t( affinity, cpusetsize, cpuset );

  _Scheduler_Release_critical( scheduler, &lock_context );

  if ( status != PROCESSOR_MASK_COPY_LOSSLESS ) {
    return STATUS_INVALID_SIZE;
  }

  return STATUS_SUCCESSFUL;
}
