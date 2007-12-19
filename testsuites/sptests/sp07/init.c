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
 *  COPYRIGHT (c) 1989-2007.
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
  rtems_id          id;

  puts( "\n\n*** TEST 7 ***" );

  buffered_io_initialize();

  Extension_name[ 1 ] =  rtems_build_name( 'E', 'X', 'T', '1' );
  Extension_name[ 2 ] =  rtems_build_name( 'E', 'X', 'T', '2' );

  puts  ( "rtems_extension_create - bad id pointer -- RTEMS_INVALID_ADDRESS" );
  status = rtems_extension_create( 0xa5a5a5a5, &Extensions, NULL );
  fatal_directive_status(
    status, RTEMS_INVALID_ADDRESS, "rtems_extension_create" );

  puts  ( "rtems_extension_create - bad name -- RTEMS_INVALID_NAME" );
  status = rtems_extension_create( 0, &Extensions, &id );
  fatal_directive_status(
    status, RTEMS_INVALID_NAME, "rtems_extension_create #1" );

  puts( "rtems_extension_create - first one -- OK" );
  status = rtems_extension_create(
    Extension_name[ 1 ],
    &Extensions,
    &Extension_id[ 1 ]
  );
  directive_failed( status, "rtems_extension_create" );

  puts( "rtems_extension_create - second one-- OK" );
  status = rtems_extension_create(
    Extension_name[ 2 ],
    &Extensions,
    &Extension_id[ 2 ]
  );
  directive_failed( status, "rtems_extension_create #2" );

  puts  ( "rtems_extension_create -- RTEMS_TOO_MANY" );
  status = rtems_extension_create( 0xa5a5a5a5, &Extensions, &id );
  fatal_directive_status( status, RTEMS_TOO_MANY, "rtems_extension_create" );

  puts( "rtems_extension_delete - second one -- OK" );
  status = rtems_extension_delete( Extension_id[ 2 ] );
  directive_failed( status, "rtems_extension_delete #2" );

  puts( "rtems_extension_delete - second one again -- RTEMS_INVALID_ID" );
  status = rtems_extension_delete( Extension_id[ 2 ] );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_extension_delete #2 bad"
  );

  puts  ( "rtems_extension_ident -- OK" );
  status = rtems_extension_ident( Extension_name[1], &id );
  directive_failed( status, "rtems_extension_ident" );

  puts  ( "rtems_extension_ident - bad name -- RTEMS_INVALID_NAME" );
  status = rtems_extension_ident( Extension_name[2], &id );
  fatal_directive_status( status, RTEMS_INVALID_NAME, "rtems_extension_ident" );

  puts  ( "rtems_extension_ident - bad name -- RTEMS_INVALID_ADDRESS" );
  status = rtems_extension_ident( Extension_name[2], NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_extension_ident"
  );

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

  buffered_io_flush();

  status = rtems_task_set_note( RTEMS_SELF, RTEMS_NOTEPAD_4, 32 );
  directive_failed( status, "task_set_node of Self" );
  printf( "INIT - rtems_task_set_note - set my RTEMS_NOTEPAD_4 " );
  puts  ( "to TA1's priority: 04" );

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
