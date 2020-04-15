/*
 * Copyright (c) 2016, 2017 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

Objects_Id _Thread_Wait_get_id( const Thread_Control *the_thread )
{
  const Thread_queue_Queue *queue;

#if defined(RTEMS_MULTIPROCESSING)
  if ( _States_Is_waiting_for_rpc_reply( the_thread->current_state ) ) {
    return the_thread->Wait.remote_id;
  }
#endif

  queue = the_thread->Wait.queue;

  if ( queue != NULL && queue->name == _Thread_queue_Object_name ) {
    const Thread_queue_Object *queue_object;

    queue_object = THREAD_QUEUE_QUEUE_TO_OBJECT( queue );
    return queue_object->Object.id;
  }

  return 0;
}
