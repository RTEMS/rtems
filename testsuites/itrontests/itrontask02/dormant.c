/*  Dormant
 *
 *  This routine serves as two test tasks.
 *  It has one dormant and one sleeping tasks.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "system.h"

void Dormant_task(void)
{
  puts( "DORMANT - ext_tsk - going to DORMANT state" );
  ext_tsk( );

  puts( "ERROR==>ext_tsk of DORMANT returned" );
  rtems_test_assert(0);
}


void Non_Dormant_task(void)
{
  ER       status;

  while (TRUE) {
    puts( "NON-DORMANT - Sleep for 2 minutes" );
    status = rtems_task_wake_after( 120*rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );
  }
}
