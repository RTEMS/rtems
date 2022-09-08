/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief POSIX Threads Private Support
 *
 * This include file contains all the private support information for
 * POSIX threads.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#ifndef _RTEMS_POSIX_PTHREADATTRIMPL_H
#define _RTEMS_POSIX_PTHREADATTRIMPL_H

#include <errno.h>
#include <pthread.h>

#include <rtems/score/basedefs.h>
#include <rtems/score/assert.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/threadsup.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup POSIX_PTHREAD
 */
/**@{**/

/**
 * This variable contains the default POSIX Thread attributes.
 */
extern const pthread_attr_t _POSIX_Threads_Default_attributes;

static inline void _POSIX_Threads_Copy_attributes(
  pthread_attr_t        *dst_attr,
  const pthread_attr_t  *src_attr
)
{
  *dst_attr = *src_attr;
  _Assert(
    dst_attr->affinitysetsize == sizeof(dst_attr->affinitysetpreallocated)
  );
  dst_attr->affinityset = &dst_attr->affinitysetpreallocated;
}

static inline void _POSIX_Threads_Initialize_attributes(
  pthread_attr_t  *attr
)
{
  _POSIX_Threads_Copy_attributes(
    attr,
    &_POSIX_Threads_Default_attributes
  );
}

static inline void _POSIX_Threads_Get_sched_param_sporadic(
  const Thread_Control    *the_thread,
  const Scheduler_Control *scheduler,
  struct sched_param      *param
)
{
#if defined(RTEMS_POSIX_API)
  const POSIX_API_Control *api;

  api = (const POSIX_API_Control*) the_thread->API_Extensions[ THREAD_API_POSIX ];
  param->sched_ss_low_priority = _POSIX_Priority_From_core(
    scheduler,
    api->Sporadic.Low_priority.priority
  );
  param->sched_ss_repl_period = api->Sporadic.sched_ss_repl_period;
  param->sched_ss_init_budget = api->Sporadic.sched_ss_init_budget;
  param->sched_ss_max_repl = api->Sporadic.sched_ss_max_repl;
#else
  (void) the_thread;
  (void) scheduler;
  (void) param;
#endif
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
