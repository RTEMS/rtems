/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/itron.h>
#include <assert.h>

#include <rtems/score/thread.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/sysstate.h>

#include <rtems/itron/task.h>

/*
 *  exd_tsk - Exit and Delete Task
 */

void exd_tsk( void )
{
  Objects_Information     *the_information;

  _RTEMS_Lock_allocator();
  _Thread_Disable_dispatch();

    the_information = _Objects_Get_information_id(_Thread_Executing->Object.id);

    /* This should never happen if _Thread_Get() works right */
    assert( the_information );

    _Thread_Close( the_information, _Thread_Executing );

    _ITRON_Task_Free( _Thread_Executing );

  _RTEMS_Unlock_allocator();
  _Thread_Enable_dispatch();
}
