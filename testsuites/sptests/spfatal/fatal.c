/*  Fatal Error Test
 *
 *  NOTE:
 *
 *  This test actually modifies the Configuration table and restarts
 *  the executive.  It is very carefully constructed to do this and
 *  uses the Configuration very carefully.
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

#include <setjmp.h>

char Workspace[ 64 * 1024 ] CPU_STRUCTURE_ALIGNMENT;

typedef enum {
  FATAL_WORKSPACE_OF_ZERO,
  FATAL_NULL_WORKSPACE,
  FATAL_WORKSPACE_TOO_SMALL,
  FATAL_TASK_CREATE,
  FATAL_TASK_START
}  Fatal_errors_t;

#define FATAL_LAST FATAL_TASK_START

volatile Fatal_errors_t Case_in_switch;

rtems_status_code Expected_Errors[] = {
  RTEMS_UNSATISFIED,
  RTEMS_INVALID_ADDRESS,
  RTEMS_UNSATISFIED,
  RTEMS_INVALID_PRIORITY,
  RTEMS_TASK_EXITTED
};

rtems_status_code Error_Happened[ FATAL_LAST + 1];

jmp_buf Restart_Context;

/*
 *  We depend on this being zeroed during initialization.  This
 *  occurs automatically because this is part of the BSS.
 */

rtems_unsigned32  First_Time_Through;

void Process_case();

rtems_extension Fatal_extension(
  rtems_unsigned32 source,
  boolean          is_internal,
  rtems_unsigned32 error
)
{
  int index;

  Error_Happened[ Case_in_switch ] = error;

  if ( First_Time_Through == 0 ) {
    Case_in_switch = FATAL_WORKSPACE_OF_ZERO;
    First_Time_Through = 1;
    setjmp( Restart_Context );
  } else if ( Case_in_switch == FATAL_LAST ) {

    /*
     *  Depending on the C library we use, we cannot get the
     *  task exitted error so do not check for it.
     */

    puts( "*** TEST FATAL ***" );
    for ( index=0 ; index< FATAL_LAST ; index++ )
      put_error( Error_Happened[ index ], Expected_Errors[ index ] );
    puts( "NOT TESTING FATAL ERROR WHEN TASK EXITS -- C LIBRARY CATCHES THIS" );
    puts( "*** END OF TEST FATAL ***" );

    /*
     *  returns to the default fatal error handler instead of
     *  calling rtems_shutdown_executive
     */
    return;

  } else {

    longjmp( Restart_Context, 1 );
  }

  Process_case();
}



void Process_case()
{
  switch ( Case_in_switch ) {
    case FATAL_WORKSPACE_OF_ZERO:
      New_Configuration = rtems_configuration_get_table();
      New_Configuration.work_space_start = NULL;
      Case_in_switch = FATAL_NULL_WORKSPACE;
      break;

    case FATAL_NULL_WORKSPACE:
      New_Configuration.work_space_start = Workspace;
      New_Configuration.work_space_size  = 256;
      Case_in_switch = FATAL_WORKSPACE_TOO_SMALL;
      break;

    case FATAL_WORKSPACE_TOO_SMALL:
      Initialization_tasks[ 0 ].initial_priority = RTEMS_CURRENT_PRIORITY;
      New_Configuration.work_space_size = sizeof( Workspace );
      Case_in_switch = FATAL_TASK_CREATE;
      break;

    case FATAL_TASK_CREATE:
      Initialization_tasks[ 0 ].initial_priority = 1;
      Initialization_tasks[ 0 ].entry_point      = NULL;
      Case_in_switch = FATAL_TASK_START;
      break;

    case FATAL_TASK_START:
      /* this extension exits the test */
      Initialization_tasks[ 0 ].entry_point = Init;
      break;
  }
  rtems_initialize_executive(
    &New_Configuration, rtems_cpu_configuration_get_table() );
}

