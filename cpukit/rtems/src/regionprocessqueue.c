/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicRegion
 *
 * @brief This source file contains the implementation of
 *   _Region_Process_queue().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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

#include <rtems/rtems/regionimpl.h>
#include <rtems/score/status.h>
#include <rtems/score/threadqimpl.h>

void _Region_Process_queue(
  Region_Control *the_region
)
{
  Per_CPU_Control *cpu_self;
  Thread_Control  *the_thread;
  void            *the_segment;

  /*
   *  Switch from using the memory allocation mutex to using a
   *  dispatching disabled critical section.  We have to do this
   *  because this thread may unblock one or more threads that were
   *  waiting on memory.
   *
   *  NOTE: Be sure to disable dispatching before unlocking the mutex
   *        since we do not want to open a window where a context
   *        switch could occur.
   */
  cpu_self = _Thread_Dispatch_disable();
  _Region_Unlock( the_region );

  /*
   *  NOTE: The following loop is O(n) where n is the number of
   *        threads whose memory request is satisfied.
   */
  for ( ; ; ) {
    the_thread = _Thread_queue_First(
      &the_region->Wait_queue,
      the_region->wait_operations
    );

    if ( the_thread == NULL )
      break;

    the_segment = (void **) _Region_Allocate_segment(
      the_region,
      the_thread->Wait.count
    );

    if ( the_segment == NULL )
      break;

    *(void **)the_thread->Wait.return_argument = the_segment;
    _Thread_queue_Extract( the_thread );
    the_thread->Wait.return_code = STATUS_SUCCESSFUL;
  }

  _Thread_Dispatch_enable( cpu_self );
}
