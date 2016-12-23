/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

Objects_Id _Thread_Wait_get_id( const Thread_Control *the_thread )
{
  States_Control current_state;

  current_state = the_thread->current_state;

#if defined(RTEMS_MULTIPROCESSING)
  if ( ( current_state & STATES_WAITING_FOR_RPC_REPLY ) != 0 ) {
    return the_thread->Wait.remote_id;
  }
#endif

  if ( ( current_state & STATES_THREAD_QUEUE_WITH_IDENTIFIER ) != 0 ) {
    const Thread_queue_Object *queue_object;

    queue_object = THREAD_QUEUE_QUEUE_TO_OBJECT( the_thread->Wait.queue );

    return queue_object->Object.id;
  }

  return 0;
}
