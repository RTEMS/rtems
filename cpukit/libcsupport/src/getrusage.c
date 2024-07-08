/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#define _GNU_SOURCE 1
#include <sys/resource.h>
#undef _GNU_SOURCE

#include <errno.h>

#include <rtems.h>
#include <rtems/seterr.h>

#include <rtems/score/threadimpl.h>

struct usage_stats {
  Timestamp_Control task;
  Timestamp_Control idle;
};

static bool usage_visitor( Thread_Control *the_thread, void *arg )
{
  struct usage_stats *stats = (struct usage_stats *) arg;
  Timestamp_Control   usage;
  Timestamp_Control  *total;
  usage = _Thread_Get_CPU_time_used( the_thread );
  if ( the_thread->is_idle ) {
    total = &stats->idle;
  } else {
    total = &stats->task;
  }
  _Timestamp_Add_to( total, &usage );
  return false;
}

static int getrusage_RUSAGE_SELF(
  struct rusage *usage
)
{
  /*
   *  RTEMS only has a single process so there are no children. The
   *  single process has been running since the system was booted. We
   *  account for IDLE time as system time so user or task time is the
   *  uptime time.
   */
  struct usage_stats stats;

  _Timestamp_Set_to_zero( &stats.task );
  _Timestamp_Set_to_zero( &stats.idle );

  rtems_task_iterate( usage_visitor, &stats );

  _Timestamp_To_timeval( &stats.task, &usage->ru_utime );
  _Timestamp_To_timeval( &stats.idle, &usage->ru_stime );

  return 0;
}

static int getrusage_RUSAGE_THREAD(
  struct rusage *usage
)
{
  Thread_Control    *the_thread;
  ISR_lock_Context   lock_context;
  Timestamp_Control  used;
  the_thread = _Thread_Get( OBJECTS_ID_OF_SELF, &lock_context );
  used = _Thread_Get_CPU_time_used( the_thread );
  _ISR_lock_ISR_enable( &lock_context );
  _Timestamp_To_timeval( &used, &usage->ru_utime );
  _Timestamp_Set_to_zero( &used );
  _Timestamp_To_timeval( &used, &usage->ru_stime );
  return 0;
}

int getrusage(
  int who, struct rusage *usage
)
{
  if ( !usage )
    rtems_set_errno_and_return_minus_one( EFAULT );

  switch ( who ) {
  case RUSAGE_SELF:
    return getrusage_RUSAGE_SELF( usage );
  case RUSAGE_THREAD:
    return getrusage_RUSAGE_THREAD( usage );
  case RUSAGE_CHILDREN:
    rtems_set_errno_and_return_minus_one( ENOSYS );
  default:
    break;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
