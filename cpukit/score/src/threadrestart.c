/**
 * @file
 *
 * @brief Restart Thread 
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/userextimpl.h>

bool _Thread_Restart(
  Thread_Control            *the_thread,
  void                      *pointer_argument,
  Thread_Entry_numeric_type  numeric_argument
)
{
  if ( !_States_Is_dormant( the_thread->current_state ) ) {

    _Thread_Set_transient( the_thread );

    _Thread_Reset( the_thread, pointer_argument, numeric_argument );

    _Thread_Load_environment( the_thread );

    _Thread_Ready( the_thread );

    _User_extensions_Thread_restart( the_thread );

    return true;
  }

  return false;
}
