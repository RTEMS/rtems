/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreBarrier
 *
 * @brief This source file contains the implementation of
 *   _CORE_barrier_Seize().
 */

/*
 *  COPYRIGHT (c) 1989-2006.
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

#include <rtems/score/corebarrierimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqops.h>

static void _CORE_barrier_Thread_queue_extract(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  CORE_barrier_Control *the_barrier;

  the_barrier = RTEMS_CONTAINER_OF(
    queue,
    CORE_barrier_Control,
    Wait_queue.Queue
  );
  --the_barrier->number_of_waiting_threads;
  _Thread_queue_FIFO_extract(
    &the_barrier->Wait_queue.Queue,
    the_thread,
    queue_context
  );
}

const Thread_queue_Operations _CORE_barrier_Thread_queue_operations = {
  .priority_actions = _Thread_queue_Do_nothing_priority_actions,
  .enqueue = _Thread_queue_FIFO_enqueue,
  .extract = _CORE_barrier_Thread_queue_extract,
  .surrender = _Thread_queue_FIFO_surrender,
  .first = _Thread_queue_FIFO_first
};

Status_Control _CORE_barrier_Seize(
  CORE_barrier_Control *the_barrier,
  Thread_Control       *executing,
  bool                  wait,
  Thread_queue_Context *queue_context
)
{
  uint32_t new_number_of_waiting_threads;

  _CORE_barrier_Acquire_critical( the_barrier, queue_context );

  /*
   * In theory, this calculation can overflow.  If this happens, then about 4
   * billion threads are accidentally released.  Currently, the system limit
   * for threads is a bit lower with three times OBJECTS_INDEX_MASK - 1.
   */
  new_number_of_waiting_threads = the_barrier->number_of_waiting_threads + 1;

  if ( new_number_of_waiting_threads == the_barrier->maximum_count ) {
    _CORE_barrier_Surrender( the_barrier, queue_context );
    return STATUS_BARRIER_AUTOMATICALLY_RELEASED;
  } else {
    the_barrier->number_of_waiting_threads = new_number_of_waiting_threads;
    _Thread_queue_Context_set_thread_state(
      queue_context,
      STATES_WAITING_FOR_BARRIER
    );
    _Thread_queue_Enqueue(
      &the_barrier->Wait_queue.Queue,
      &_CORE_barrier_Thread_queue_operations,
      executing,
      queue_context
    );
    return _Thread_Wait_get_status( executing );
  }
}
