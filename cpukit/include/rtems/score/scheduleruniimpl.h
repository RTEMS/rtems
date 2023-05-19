/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This header file provides interfaces of the supporting the
 *   implementation of uniprocessor schedulers.
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *  Copyright (C) 2014, 2022 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_SCHEDULERUNIIMPL_H
#define _RTEMS_SCORE_SCHEDULERUNIIMPL_H

#include <rtems/score/schedulerimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreScheduler
 *
 * @{
 */

/**
 * @brief Updates the heir thread of the processor.
 *
 * @param[in, out] heir is the current heir thread.
 * @param[in, out] new_heir is the new heir thread.
 */
static inline void _Scheduler_uniprocessor_Update_heir(
  Thread_Control *heir,
  Thread_Control *new_heir
)
{
  _Assert( heir != new_heir );
#if defined(RTEMS_SMP)
  /*
   * We need this state only for _Thread_Get_CPU_time_used_locked().  Cannot
   * use _Scheduler_Thread_change_state() since THREAD_SCHEDULER_BLOCKED to
   * THREAD_SCHEDULER_BLOCKED state changes are illegal for the real SMP
   * schedulers.
   */
  heir->Scheduler.state = THREAD_SCHEDULER_BLOCKED;
  new_heir->Scheduler.state = THREAD_SCHEDULER_SCHEDULED;
#endif
  _Thread_Update_CPU_time_used( heir, _Thread_Get_CPU( heir ) );
  _Thread_Heir = new_heir;
  _Thread_Dispatch_necessary = true;
}

/**
 * @brief Updates the heir thread of the processor if the current heir is
 *   not equal to the new heir thread.
 *
 * The update takes place even if the current heir thread is not preemptible.
 *
 * @param[in, out] new_heir is the new heir thread.
 */
static inline void _Scheduler_uniprocessor_Update_heir_if_necessary(
  Thread_Control *new_heir
)
{
  Thread_Control *heir = _Thread_Heir;

  if ( heir != new_heir ) {
    _Scheduler_uniprocessor_Update_heir( heir, new_heir );
  }
}

/**
 * @brief Updates the heir thread of the processor if the current heir thread
 *   is preemptible.
 *
 * @param[in, out] heir is the current heir thread.
 * @param[in, out] new_heir is the new heir thread.
 */
static inline void _Scheduler_uniprocessor_Update_heir_if_preemptible(
  Thread_Control *heir,
  Thread_Control *new_heir
)
{
  if ( heir != new_heir && heir->is_preemptible ) {
    _Scheduler_uniprocessor_Update_heir( heir, new_heir );
  }
}

/**
 * @brief Blocks the thread.
 *
 * @param scheduler is the scheduler.
 * @param the_thread is the thread to block.
 * @param node is the scheduler node of the thread.
 * @param extract is the handler to extract the thread.
 * @param get_highest_ready is the handler to get the highest ready thread.
 */
static inline void _Scheduler_uniprocessor_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  void                  ( *extract )(
                             const Scheduler_Control *,
                             Thread_Control *,
                             Scheduler_Node *
                        ),
  Thread_Control       *( *get_highest_ready )( const Scheduler_Control * )
)
{
  ( *extract )( scheduler, the_thread, node );

  /* TODO: flash critical section? */

  if ( _Thread_Is_heir( the_thread ) ) {
    Thread_Control *highest_ready;

    highest_ready = ( *get_highest_ready )( scheduler );
    _Scheduler_uniprocessor_Update_heir( _Thread_Heir, highest_ready );
  }
}

/**
 * @brief Schedule the unblocked thread if it is the highest ready thread.
 *
 * @param scheduler is the scheduler.
 * @param the_thread is the thread.
 * @param priority is the priority of the thread.
 */
static inline void _Scheduler_uniprocessor_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  Thread_Control *heir;

  heir = _Thread_Heir;

  /*
   * If the thread is more important than the heir, then we have a new heir.
   * This may or may not result in a context switch.  If the current heir
   * thread is preemptible, then we need to do a context switch.
   */
  if ( priority < _Thread_Get_priority( heir ) ) {
    _Scheduler_uniprocessor_Update_heir_if_preemptible( heir, the_thread );
  }
}

/**
 * @brief Schedules the highest ready thread if the current heir thread of the
 * processor is preemptible.
 *
 * @param scheduler is the scheduler.
 * @param get_highest_ready is the handler to get the highest ready thread.
 */
static inline void _Scheduler_uniprocessor_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control       *( *get_highest_ready )( const Scheduler_Control * )
)
{
  Thread_Control *highest_ready;

  highest_ready = ( *get_highest_ready )( scheduler );
  _Scheduler_uniprocessor_Update_heir_if_preemptible(
    _Thread_Heir,
    highest_ready
  );
}

/**
 * @brief Yields to the highest ready thread.
 *
 * @param scheduler is the scheduler.
 * @param get_highest_ready is the handler to get the highest ready thread.
 */
static inline void _Scheduler_uniprocessor_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control       *( *get_highest_ready )( const Scheduler_Control * )
)
{
  Thread_Control *highest_ready;

  highest_ready = ( *get_highest_ready )( scheduler );
  _Scheduler_uniprocessor_Update_heir_if_necessary( highest_ready );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_SCHEDULERUNIIMPL_H */
