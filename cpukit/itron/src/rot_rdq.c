/*
 *  COPYRIGHT (c) 1989-1999.
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

#include <itron.h>

#include <rtems/score/thread.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

#include <rtems/itron/task.h>

/*
 *  rot_rdq - Rotate Tasks on the Ready Queue
 */

ER rot_rdq(
  PRI tskpri
)
{
  PRI priority;
  

  if (( tskpri <= 0 ) || ( tskpri >= 256 ))
    return E_PAR;

  _Thread_Disable_dispatch();

  /*
   * Yield of processor will rotate the queue for this processor.
   */

  priority = _ITRON_Task_Core_to_Priority(_Thread_Executing->current_priority);
  if ( priority == tskpri )
    _Thread_Yield_processor();
  else {
    _Thread_Rotate_Ready_Queue( _ITRON_Task_Core_to_Priority( tskpri ) ); 
  }
  _Thread_Enable_dispatch();
  
  return E_OK;
}


