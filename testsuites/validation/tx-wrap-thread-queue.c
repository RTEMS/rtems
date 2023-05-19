/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the implementation of the thread queue
 *   wrapper.
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

#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

#include <string.h>

void WrapThreadQueueInitialize(
  WrapThreadQueueContext *ctx,
  void                 ( *handler )( void * ),
  void                   *arg
)
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->isr_request.handler = handler;
  ctx->isr_request.arg = arg;
  _Thread_queue_Initialize( &ctx->thread_queue, "Wrap" );
}

static void Prepare(
  WrapThreadQueueContext *ctx,
  Thread_Control         *thread
)
{
  if ( thread->Wait.queue != NULL ) {
    ctx->wrapped_ops = thread->Wait.operations;
    thread->Wait.operations = &ctx->tq_ops;
  } else {
    Thread_queue_Context queue_context;

    ctx->wrapped_ops = NULL;
    _Thread_queue_Context_initialize( &queue_context );
    _Thread_queue_Acquire( &ctx->thread_queue, &queue_context );
    _Thread_Wait_flags_set(
      thread,
      THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_INTEND_TO_BLOCK
    );
    _Thread_Wait_claim( thread, &ctx->thread_queue.Queue );
    _Thread_Wait_claim_finalize( thread, &ctx->tq_ops );
    _Thread_queue_Release( &ctx->thread_queue, &queue_context );
  }
}

static void WrappedExtract(
  WrapThreadQueueContext *ctx,
  Thread_queue_Queue     *queue,
  Thread_Control         *thread,
  Thread_queue_Context   *queue_context
)
{
  if ( ctx->wrapped_ops ) {
    thread->Wait.operations = ctx->wrapped_ops;
    ( *thread->Wait.operations->extract )( queue, thread, queue_context );
  }
}

static void Extract(
  Thread_queue_Queue   *queue,
  Thread_Control       *thread,
  Thread_queue_Context *queue_context
)
{
  WrapThreadQueueContext *ctx;

  ctx = (WrapThreadQueueContext *) thread->Wait.operations;
  CallWithinISRSubmit( &ctx->isr_request );
  WrappedExtract( ctx, queue, thread, queue_context );
}

void WrapThreadQueueExtract(
  WrapThreadQueueContext *ctx,
  Thread_Control         *thread
)
{
  ctx->tq_ops.extract = Extract;
  Prepare( ctx, thread );
}

static void ExtractDirect(
  Thread_queue_Queue   *queue,
  Thread_Control       *thread,
  Thread_queue_Context *queue_context
)
{
  WrapThreadQueueContext *ctx;

  ctx = (WrapThreadQueueContext *) thread->Wait.operations;
  ( *ctx->isr_request.handler )( ctx->isr_request.arg );
  WrappedExtract( ctx, queue, thread, queue_context );
}

void WrapThreadQueueExtractDirect(
  WrapThreadQueueContext *ctx,
  Thread_Control         *thread
)
{
  ctx->tq_ops.extract = ExtractDirect;
  Prepare( ctx, thread );
}

void WrapThreadQueueDestroy( WrapThreadQueueContext *ctx )
{
  _Thread_queue_Destroy( &ctx->thread_queue );
}
