/*  Screen1
 *
 *  This routine generates error screen 1 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "system.h"

void Screen1()
{
  rtems_unsigned32    notepad_value;
  rtems_id            self_id;
  rtems_task_priority previous_priority;
  rtems_status_code   status;

  status = rtems_task_delete( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_task_delete with illegal id"
  );
  puts( "TA1 - rtems_task_delete - RTEMS_INVALID_ID" );

  status = rtems_task_get_note( RTEMS_SELF, 100, &notepad_value );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_task_get_note with illegal notepad"
  );
  puts( "TA1 - rtems_task_get_note - RTEMS_INVALID_NUMBER" );

  status = rtems_task_get_note( 100, RTEMS_NOTEPAD_LAST, &notepad_value );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_task_get_note with illegal id"
  );
  puts( "TA1 - rtems_task_get_note - RTEMS_INVALID_ID" );

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &self_id );
  directive_failed( status, "rtems_task_ident of self" );

  if ( self_id != Task_id[ 1 ] ) {
    puts( "ERROR - rtems_task_ident - incorrect ID returned!" );
  }
  puts( "TA1 - rtems_task_ident - current task RTEMS_SUCCESSFUL" );

  status = rtems_task_ident( 100, RTEMS_SEARCH_ALL_NODES, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_task_ident with illegal name (local)"
  );
  puts( "TA1 - rtems_task_ident - global RTEMS_INVALID_NAME" );

  status = rtems_task_ident( 100, 1, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_task_ident with illegal name (global)"
  );
  puts( "TA1 - rtems_task_ident - local RTEMS_INVALID_NAME" );

  /*
   *  This one case is different if MP is enabled/disabled.
   */

  status = rtems_task_ident( 100, 2, &Junk_id );
#if defined(RTEMS_MULTIPROCESSING)
  fatal_directive_status(
    status,
    RTEMS_INVALID_NODE,
    "rtems_task_ident with illegal node"
  );
#else
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_task_ident with illegal node"
  );
#endif
  puts( "TA1 - rtems_task_ident - RTEMS_INVALID_NODE" );

  status = rtems_task_restart( 100, 0 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_task_restart with illegal id"
  );
  puts( "TA1 - rtems_task_restart - RTEMS_INVALID_ID" );

  status = rtems_task_resume( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_task_resume with illegal id"
  );
  puts( "TA1 - rtems_task_resume - RTEMS_INVALID_ID" );

  status = rtems_task_resume( RTEMS_SELF );
  fatal_directive_status(
    status,
    RTEMS_INCORRECT_STATE,
    "rtems_task_resume of ready task"
  );
  puts( "TA1 - rtems_task_resume - RTEMS_INCORRECT_STATE" );

  status = rtems_task_set_priority( RTEMS_SELF, 512, &previous_priority );
  fatal_directive_status(
    status,
    RTEMS_INVALID_PRIORITY,
    "rtems_task_set_priority with illegal priority"
  );
  puts( "TA1 - rtems_task_set_priority - RTEMS_INVALID_PRIORITY" );

  status = rtems_task_set_priority( 100, 8, &previous_priority );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_task_set_priority with illegal id"
  );
  puts( "TA1 - rtems_task_set_priority - RTEMS_INVALID_ID" );

  status = rtems_task_set_note(
    RTEMS_SELF,
    RTEMS_NOTEPAD_LAST+10,
    notepad_value
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_task_set_note with illegal notepad"
  );
  puts( "TA1 - rtems_task_set_note - RTEMS_INVALID_NUMBER" );

  status = rtems_task_set_note( 100, RTEMS_NOTEPAD_LAST, notepad_value );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_task_set_note with illegal id"
  );
  puts( "TA1 - rtems_task_set_note - RTEMS_INVALID_ID" );

  status = rtems_task_start( 100, Task_1, 0 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_task_start with illegal id"
  );
  puts( "TA1 - rtems_task_start - RTEMS_INVALID_ID" );

  status = rtems_task_start( 0, Task_1, 0 );
  fatal_directive_status(
    status,
    RTEMS_INCORRECT_STATE,
    "rtems_task_start of ready task"
  );
  puts( "TA1 - rtems_task_start - RTEMS_INCORRECT_STATE" );

  status = rtems_task_suspend( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_task_suspend with illegal id"
  );
  puts( "TA1 - rtems_task_suspend - RTEMS_INVALID_ID" );
}
