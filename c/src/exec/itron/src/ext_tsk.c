/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

#include <rtems/score/thread.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

#include <rtems/itron/task.h>


/*
 *  ext_tsk - Exit Issuing Task
 */

void ext_tsk( void )
{
  /*
   *  Figure out what to do if this happens.
   *  We can not return regardless
      if ( _ITRON_Is_in_non_task_state() )
        return E_CTX;
  */

  _Thread_Disable_dispatch();

  _Thread_Set_state( _Thread_Executing, STATES_DORMANT );
  _Thread_Reset( _Thread_Executing, NULL, 0 );

  _Thread_Enable_dispatch();  
}
