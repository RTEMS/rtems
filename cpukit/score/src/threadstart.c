/**
 * @file
 *
 * @brief Initializes Thread and Executes it
 *
 * @ingroup RTEMSScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/userextimpl.h>

bool _Thread_Start(
  Thread_Control                 *the_thread,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
)
{
  Per_CPU_Control *cpu_self;

  _Thread_State_acquire_critical( the_thread, lock_context );

  if ( !_States_Is_dormant( the_thread->current_state ) ) {
    _Thread_State_release( the_thread, lock_context );
    return false;
  }

  the_thread->Start.Entry = *entry;
  _Thread_Load_environment( the_thread );
  _Thread_Clear_state_locked( the_thread, STATES_ALL_SET );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Thread_State_release( the_thread, lock_context );

  _User_extensions_Thread_start( the_thread );

  _Thread_Dispatch_enable( cpu_self );
  return true;
}
