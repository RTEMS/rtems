/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerSimple
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreSchedulerSimple which are used by the implementation and
 *   the @ref RTEMSImplApplConfig.
 */

/*
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_SCORE_SCHEDULERSIMPLE_H
#define _RTEMS_SCORE_SCHEDULERSIMPLE_H

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreSchedulerSimple Simple Priority Scheduler
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This group contains the Simple Priority Scheduler implementation.
 *
 * @{
 */

#define SCHEDULER_SIMPLE_MAXIMUM_PRIORITY 255

/**
 *  Entry points for Scheduler Simple
 */
#define SCHEDULER_SIMPLE_ENTRY_POINTS \
  { \
    _Scheduler_simple_Initialize,         /* initialize entry point */ \
    _Scheduler_simple_Schedule,           /* schedule entry point */ \
    _Scheduler_simple_Yield,              /* yield entry point */ \
    _Scheduler_simple_Block,              /* block entry point */ \
    _Scheduler_simple_Unblock,            /* unblock entry point */ \
    _Scheduler_simple_Update_priority,    /* update priority entry point */ \
    _Scheduler_default_Map_priority,      /* map priority entry point */ \
    _Scheduler_default_Unmap_priority,    /* unmap priority entry point */ \
    SCHEDULER_DEFAULT_SMP_OPERATIONS \
    _Scheduler_default_Node_initialize,   /* node initialize entry point */ \
    _Scheduler_default_Node_destroy,      /* node destroy entry point */ \
    _Scheduler_default_Release_job,       /* new period of task */ \
    _Scheduler_default_Cancel_job,        /* cancel period of task */ \
    _Scheduler_default_Start_idle         /* start idle entry point */ \
    SCHEDULER_DEFAULT_SET_AFFINITY_OPERATION \
  }

/**
 * @brief Simple scheduler context.
 */
typedef struct {
  /**
   * @brief Basic scheduler context.
   */
  Scheduler_Context Base;

  /**
   * @brief One ready queue for all ready threads.
   */
  Chain_Control Ready;
} Scheduler_simple_Context;

/**
 * @brief Initializes simple scheduler.
 *
 * This routine initializes the simple scheduler.
 *
 * @param scheduler The scheduler to be initialized.
 */
void _Scheduler_simple_Initialize( const Scheduler_Control *scheduler );

/**
 * @brief Schedules threads.
 *
 * This routine sets the heir thread to be the next ready thread
 * on the ready queue by getting the first node in the scheduler
 * information.
 *
 * @param scheduler The scheduler instance.
 * @param the_thread causing the scheduling operation.
 */
void _Scheduler_simple_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 * @brief Performs the yield of a thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] the_thread The thread that performed the yield operation.
 * @param node The scheduler node of @a the_thread.
 */
void _Scheduler_simple_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Blocks the thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] the_thread The thread to block.
 * @param[in, out] node The @a thread's scheduler node.
 */
void _Scheduler_simple_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Unblocks the thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] the_thread The thread to unblock.
 * @param[in, out] node The @a thread's scheduler node.
 */
void _Scheduler_simple_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Updates the priority of the node.
 *
 * @param scheduler The scheduler instance.
 * @param the_thread The thread for the operation.
 * @param node The thread's scheduler node.
 */
void _Scheduler_simple_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
