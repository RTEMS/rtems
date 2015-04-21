/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/rbtreeimpl.h>

static void _Thread_queue_Do_nothing_priority_change(
  Thread_Control       *the_thread,
  Priority_Control      new_priority,
  Thread_queue_Control *queue
)
{
  /* Do nothing */
}

static void _Thread_queue_Priority_priority_change(
  Thread_Control       *the_thread,
  Priority_Control      new_priority,
  Thread_queue_Control *queue
)
{
  _RBTree_Extract( &queue->Queues.Priority, &the_thread->RBNode );
  _RBTree_Insert(
    &queue->Queues.Priority,
    &the_thread->RBNode,
    _Thread_queue_Compare_priority,
    false
  );
}

const Thread_queue_Operations _Thread_queue_Operations_default = {
  .priority_change = _Thread_queue_Do_nothing_priority_change
};

const Thread_queue_Operations _Thread_queue_Operations_FIFO = {
  .priority_change = _Thread_queue_Do_nothing_priority_change
};

const Thread_queue_Operations _Thread_queue_Operations_priority = {
  .priority_change = _Thread_queue_Priority_priority_change
};
