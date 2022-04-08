/* SPDX-License-Identifier: BSD-2-Clause */

/*
* @file
* @brief pthread_setschedparam() API Conformance Test
*/

/*
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

#include <pthread.h>

#ifndef _POSIX_THREAD_PRIORITY_SCHEDULING
#error "RTEMS is supposed to have pthread_setschedparam"
#endif

int test( void );

int test( void )
{
  pthread_t           thread = 0;
  int                 policy;
  struct sched_param  param;
  int                 result;

  policy = SCHED_OTHER;
  policy = SCHED_FIFO;
  policy = SCHED_RR;
#ifdef _POSIX_SPORADIC_SERVER
  policy = SCHED_SPORADIC;
#endif

  /*
   *  really should use sched_get_priority_min() and sched_get_priority_max()
   */

  param.sched_priority = 0;
#ifdef _POSIX_SPORADIC_SERVER
  param.sched_ss_low_priority = 0;
  param.sched_ss_repl_period.tv_sec = 0;
  param.sched_ss_repl_period.tv_nsec = 0;
  param.sched_ss_init_budget.tv_sec = 0;
  param.sched_ss_init_budget.tv_nsec = 0;
#endif

  result = pthread_setschedparam( thread, policy, &param );

  return result;
}
