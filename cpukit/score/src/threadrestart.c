/*
 *  Thread Handler
 *
 *
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

#include <rtems/system.h>
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>

/*
 *  _Thread_Restart
 *
 *  DESCRIPTION:
 *
 *  This support routine restarts the specified task in a way that the
 *  next time this thread executes, it will begin execution at its
 *  original starting point.
 */

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

    if ( _Thread_Is_executing ( the_thread ) )
      _Thread_Restart_self();

    return true;
  }

  return false;
}
