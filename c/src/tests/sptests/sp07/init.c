/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:
 *    argument - task argument
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

#define TEST_INIT
#include "system.h"

rtems_extensions_table Extensions = {
  Task_create_extension,     /* task create user extension */
  Task_start_extension,      /* task start user extension */
  Task_restart_extension,    /* task restart user extension */
  Task_delete_extension,     /* task delete user extension */
  NULL,                      /* task switch user extension */
  NULL,                      /* begin user extension */
  Task_exit_extension,       /* task exitted user extension */
  NULL                       /* fatal error extension */
};

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "\n\n*** TEST 7 ***" );

  Extension_name[ 1 ] =  rtems_build_name( 'E', 'X', 'T', ' ' );

  status = rtems_extension_create(
    Extension_name[ 1 ],
    &Extensions,
    &Extension_id[ 1 ]
  );
  directive_failed( status, "rtems_extension_create" );

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] = rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ] = rtems_build_name( 'T', 'A', '4', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_create(
    Task_name[ 2 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_create(
    Task_name[ 3 ],
    250,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );

  status = rtems_task_create(
    Task_name[ 4 ],
    254,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 4 ]
  );
  directive_failed( status, "rtems_task_create of TA4" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_start( Task_id[ 3 ], Task_3, 0 );
  directive_failed( status, "rtems_task_start of TA3" );

  status = rtems_task_start( Task_id[ 4 ], Task_4, 0 );
  directive_failed( status, "rtems_task_start of TA4" );

  status = rtems_task_restart( Task_id[ 3 ], 0 );
  directive_failed( status, "rtems_task_restart of TA3" );

  status = rtems_task_set_note( Task_id[ 1 ], RTEMS_NOTEPAD_8, 4 );
  directive_failed( status, "task_set_node of TA1" );
  printf( "INIT - rtems_task_set_note - set TA1's RTEMS_NOTEPAD_8 " );
  puts  ( "to TA1's priority: 04" );

  status = rtems_task_set_note( Task_id[ 2 ], RTEMS_NOTEPAD_8, 4 );
  directive_failed( status, "task_set_node of TA2" );
  printf( "INIT - rtems_task_set_note - set TA2's RTEMS_NOTEPAD_8 " );
  puts  ( "to TA2's priority: 04");

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
