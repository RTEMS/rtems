/*
 * Copyright (c) 2014, 2017 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulersmpimpl.h>

void _Scheduler_Request_ask_for_help( Thread_Control *the_thread )
{
  ISR_lock_Context lock_context;

  _Thread_Scheduler_acquire_critical( the_thread, &lock_context );

  if ( _Chain_Is_node_off_chain( &the_thread->Scheduler.Help_node ) ) {
    Per_CPU_Control *cpu;

    cpu = _Thread_Get_CPU( the_thread );
    _Per_CPU_Acquire( cpu );

    _Chain_Append_unprotected(
      &cpu->Threads_in_need_for_help,
      &the_thread->Scheduler.Help_node
    );

    _Per_CPU_Release( cpu );

    _Thread_Dispatch_request( _Per_CPU_Get(), cpu );
  }

  _Thread_Scheduler_release_critical( the_thread, &lock_context );
}
