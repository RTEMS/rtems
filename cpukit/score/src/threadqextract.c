/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThreadQueue
 *
 * @brief This source file contains the implementation of
 *   _Thread_queue_Extract().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (C) 2015, 2016 embedded brains GmbH & Co. KG
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

#include <rtems/score/threadqimpl.h>
#include <rtems/score/threadimpl.h>

void _Thread_queue_Extract( Thread_Control *the_thread )
{
  Thread_queue_Context  queue_context;
  Thread_queue_Queue   *queue;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Thread_Wait_acquire( the_thread, &queue_context );

  queue = the_thread->Wait.queue;

  if ( queue != NULL ) {
    _Thread_Wait_remove_request( the_thread, &queue_context.Lock_context );
    _Thread_queue_Context_set_MP_callout(
      &queue_context,
      _Thread_queue_MP_callout_do_nothing
    );
#if defined(RTEMS_MULTIPROCESSING)
    _Thread_queue_MP_set_callout( the_thread, &queue_context );
#endif
    ( *the_thread->Wait.operations->extract )(
      queue,
      the_thread,
      &queue_context
    );
    _Thread_queue_Resume( queue, the_thread, &queue_context );
  } else {
    _Thread_Wait_release( the_thread, &queue_context );
  }
}
