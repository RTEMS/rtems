/*  Preempt_task
 *
 *  This routine serves as a test task.  It verifies the task manager.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
#include <assert.h>
#include "system.h"

void Preempt_task()
{
  puts( "PREEMPT - ext_tsk - going to DORMANT state" );
  ext_tsk( );

  puts( "ext_tsk of RTEMS_PREEMPT" );
  assert(0);
}
