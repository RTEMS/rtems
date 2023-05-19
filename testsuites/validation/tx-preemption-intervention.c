/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the implementation of
 *   SetPreemptionIntervention().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include "tx-support.h"

#include <rtems/sysinit.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threadimpl.h>

#include <rtems/test.h>

#if defined(RTEMS_SMP)
typedef struct {
  void           ( *handler )( void * );
  void             *arg;
  Scheduler_Context scheduler_context;
  Scheduler_Node    scheduler_node;
  Thread_Control    thread;
} PreemptionInterventionContext;

static PreemptionInterventionContext preemption_intervention_instance;

static bool PreemptionInterventionAskForHelp(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  PreemptionInterventionContext *ctx;
  void                        ( *handler )( void * );
  void                          *arg;

  (void) scheduler;
  (void) thread;
  (void) node;

  ctx = &preemption_intervention_instance;
  handler = ctx->handler;
  arg = ctx->arg;
  ctx->handler = NULL;
  ctx->arg = NULL;
  ( *handler )( arg );

  return true;
}

static const Scheduler_Control preemption_intervention_scheduler = {
  .context = &preemption_intervention_instance.scheduler_context,
  .Operations = {
    .ask_for_help = PreemptionInterventionAskForHelp
  }
};

static void PreemptionInterventionInitialize( void )
{
  PreemptionInterventionContext *ctx;

  ctx = &preemption_intervention_instance;

  _Chain_Initialize_node( &ctx->thread.Scheduler.Help_node );
  _Thread_queue_Initialize(
    &ctx->thread.Join_queue,
    "Preemption Intervention"
  );
  _ISR_lock_Initialize(
    &ctx->scheduler_context.Lock,
    "Preemption Intervention"
  );
  _Scheduler_Node_do_initialize(
    &preemption_intervention_scheduler,
    &ctx->scheduler_node,
    &ctx->thread,
    0
  );
  _Chain_Initialize_one(
    &ctx->thread.Scheduler.Scheduler_nodes,
    &ctx->scheduler_node.Thread.Scheduler_node.Chain
  );
}

RTEMS_SYSINIT_ITEM(
  PreemptionInterventionInitialize,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
#endif

void SetPreemptionIntervention(
  struct Per_CPU_Control *cpu,
  void                 ( *handler )( void * ),
  void                   *arg
)
{
#if defined(RTEMS_SMP)
  PreemptionInterventionContext *ctx;
  rtems_interrupt_level          level;
  ISR_lock_Context               lock_context;

  ctx = &preemption_intervention_instance;
  T_quiet_assert_null( ctx->handler );
  ctx->handler = handler;
  ctx->arg = arg;

  rtems_interrupt_local_disable( level );
  _Per_CPU_Acquire( cpu, &lock_context );
  _Chain_Append_unprotected(
    &cpu->Threads_in_need_for_help,
    &ctx->thread.Scheduler.Help_node
  );
  _Per_CPU_Release( cpu, &lock_context );
  rtems_interrupt_local_enable( level );
#else
  (void) cpu;
  (void) handler;
  (void) arg;
#endif
}
