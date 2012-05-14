/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input params:
 *    argument - task argument
 *
 *  Output params:  NONE
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define CONFIGURE_INIT
#include "system.h"

rtems_id   Task_id;
rtems_task_priority Priority;
time_t  Period;
time_t  Execution;
time_t  Phase;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  qres_sid_t server_id, server_id2;
  time_t approved_budget, exec_time, abs_time, current_budget;
  qres_params_t params, params1, params2, params3, params4;

  Priority = 30;
  Period    = 30;
  Execution = 10;
  Phase = 0;

  params.P = 1;
  params.Q = 1;

  params1 = params2 = params3 = params4 = params;
  params1.Q = -1;
  params2.Q = SCHEDULER_EDF_PRIO_MSB + 1;
  params3.P = -1;
  params4.P = SCHEDULER_EDF_PRIO_MSB + 1;

  puts( "\n\n*** TEST QRES LIBRARY ***" );


  status = rtems_task_create(
    rtems_build_name( 'P', 'T', '1', ' ' ),
    Priority,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id
  );
  directive_failed( status, "rtems_task_create loop" );

  printf( "Init: Initializing the qres library\n" );
  if ( qres_init() )
    printf( "ERROR: QRES INITIALIZATION FAILED\n" );

  /* Error checks for Create server and Destroy server  */
  printf( "Init: Create server and Destroy server\n" );
  if ( qres_destroy_server( -5 ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: DESTROY SERVER PASSED UNEXPECTEDLY\n" );
  if ( qres_destroy_server( 5 ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: DESTROY SERVER PASSED UNEXPECTEDLY\n" );
  if ( qres_destroy_server( 0 ) != QOS_E_NOSERVER )
    printf( "ERROR: DESTROY SERVER PASSED UNEXPECTEDLY\n" );
  if ( qres_create_server( &params1, &server_id ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: CREATE SERVER PASSED UNEXPECTEDLY\n" );
  if ( qres_create_server( &params2, &server_id ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: CREATE SERVER PASSED UNEXPECTEDLY\n" );
  if ( qres_create_server( &params3, &server_id ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: CREATE SERVER PASSED UNEXPECTEDLY\n" );
  if ( qres_create_server( &params4, &server_id ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: CREATE SERVER PASSED UNEXPECTEDLY\n" );
  if ( qres_create_server( &params, &server_id2 ) )
    printf( "ERROR: CREATE SERVER FAILED\n" );
  if ( qres_create_server( &params, &server_id ) )
    printf( "ERROR: CREATE SERVER FAILED\n" );
  if ( qres_create_server( &params, &server_id ) !=
       QOS_E_FULL )
    printf( "ERROR: CREATE SERVER PASSED UNEXPECTEDLY\n" );

  /* Error checks for Attach thread and Detach thread */
  printf( "Init: Attach thread\n" );
  if ( qres_attach_thread( -5, 0, RTEMS_SELF ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: ATTACH THREAD PASSED UNEXPECTEDLY\n" );
  if ( qres_attach_thread( 5, 0, RTEMS_SELF ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: ATTACH THREAD PASSED UNEXPECTEDLY\n" );
  if ( qres_attach_thread( server_id, 0, 1234 ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: ATTACH THREAD PASSED UNEXPECTEDLY\n" );
  if ( qres_attach_thread( server_id, 0, RTEMS_SELF ) )
    printf( "ERROR: ATTACH THREAD FAILED\n" );
  if ( qres_attach_thread( server_id, 0, RTEMS_SELF ) !=
       QOS_E_FULL )
    printf( "ERROR: ATTACH THREAD AGAIN PASSED UNEXPECTEDLY\n" );
  if ( qres_attach_thread( server_id, 0, Task_id ) !=
       QOS_E_FULL )
    printf( "ERROR: ATTACH THREAD TO FULL SERVER PASSED UNEXPECTEDLY \n" );
  if ( qres_destroy_server( server_id ) )
    printf( "ERROR: DESTROY SERVER WITH THREAD ATTACHED FAILED\n" );
  if ( qres_attach_thread( server_id, 0, RTEMS_SELF ) !=
       QOS_E_NOSERVER )
    printf( "ERROR: ATTACH THREAD PASSED UNEXPECTEDLY\n" );

  printf( "Init: Detach thread\n" );
  if ( qres_detach_thread( -5, 0, RTEMS_SELF ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: DETACH THREAD PASSED UNEXPECTEDLY\n" );
  if ( qres_detach_thread( 5, 0, RTEMS_SELF ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: DETACH THREAD PASSED UNEXPECTEDLY\n" );
  if ( qres_detach_thread( server_id2, 0, 1234 ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: DETACH THREAD PASSED UNEXPECTEDLY \n" );
  if ( qres_detach_thread( server_id, 0, RTEMS_SELF ) !=
       QOS_E_NOSERVER )
    printf( "ERROR: DETACH THREAD PASSED UNEXPECTEDLY4\n" );
  qres_destroy_server( server_id2 );

  /* Error checks for Set params and Get params */
  printf( "Init: Set params and Get params\n" );
  if ( qres_set_params( -5, &params ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: SET PARAMS PASSED UNEXPECTEDLY\n" );
  if ( qres_set_params( 5, &params ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: SET PARAMS PASSED UNEXPECTEDLY\n" );
  if ( qres_set_params( server_id, &params ) !=
       QOS_E_NOSERVER )
    printf( "ERROR: SET PARAMS PASSED UNEXPECTEDLY\n" );

  if ( qres_get_params( -5, &params ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: GET PARAMS PASSED UNEXPECTEDLY\n" );
  if ( qres_get_params( 5, &params ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: GET PARAMS PASSED UNEXPECTEDLY\n" );
  if ( qres_get_params( server_id, &params ) !=
       QOS_E_NOSERVER )
    printf( "ERROR: GET PARAMS PASSED UNEXPECTEDLY\n" );

  if ( qres_set_params( server_id, &params1 ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: SET PARAMS PASSED UNEXPECTEDLY\n" );
  if ( qres_set_params( server_id, &params2 ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: SET PARAMS PASSED UNEXPECTEDLY\n" );
  if ( qres_set_params( server_id, &params3 ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: SET PARAMS PASSED UNEXPECTEDLY\n" );
  if ( qres_set_params( server_id, &params4 ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: SET PARAMS PASSED UNEXPECTEDLY\n" );

  /* Error checks for Get server id */
  printf( "Init: Get server id\n" );
  if ( qres_get_sid( 0, RTEMS_SELF, &server_id ) !=
       QOS_E_NOSERVER )
    printf( "ERROR: GET SERVER ID PASSED UNEXPECTEDLY\n" );

  /* Error checks for Get approved budget */
  printf( "Init: Get approved budget\n" );
  if ( qres_get_appr_budget( -5, &approved_budget ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: GET APPROVED BUDGET PASSED UNEXPECTEDLY\n" );
  if ( qres_get_appr_budget( 5, &approved_budget ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: GET APPROVED BUDGET PASSED UNEXPECTEDLY\n" );
  if ( qres_get_appr_budget( server_id, &approved_budget ) !=
       QOS_E_NOSERVER )
    printf( "ERROR: GET APPROVED BUDGET PASSED UNEXPECTEDLY\n" );

  /* Error checks for Get current budget */
  printf( "Init: Get current budget\n" );
  if ( qres_get_curr_budget( -5, &current_budget ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: GET REMAINING BUDGET PASSED UNEXPECTEDLY\n" );
  if ( qres_get_curr_budget( 5, &current_budget ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: GET REMAINING BUDGET PASSED UNEXPECTEDLY\n" );
  if ( qres_get_curr_budget( server_id, &current_budget ) !=
       QOS_E_NOSERVER )
    printf( "ERROR: GET REMAINING BUDGET PASSED UNEXPECTEDLY\n" );

  /* Error checks for Get execution time */
  printf( "Init: Get execution time\n" );
  if ( qres_get_exec_time( -5, &exec_time, &abs_time ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: GET EXECUTION TIME PASSED UNEXPECTEDLY\n" );
  if ( qres_get_exec_time( 5, &exec_time, &abs_time ) !=
       QOS_E_INVALID_PARAM )
    printf( "ERROR: GET EXECUTION TIME PASSED UNEXPECTEDLY\n" );
  if ( qres_get_exec_time( server_id, &exec_time, &abs_time ) !=
       QOS_E_NOSERVER )
    printf( "ERROR: GET EXECUTION TIME PASSED UNEXPECTEDLY\n" );

  /* Restart QRES library */
  printf( "Init: Cleaning up QRES\n" );
  if ( qres_cleanup() )
    printf( "ERROR: QRES CLEANUP FAILED\n" );
  printf( "Init: Initializing the QRES\n" );
  if ( qres_init() )
    printf( "ERROR: QRES INITIALIZATION FAILED\n" );

  /* Start periodic task */
  printf( "Init: Starting periodic task\n" );
  status = rtems_task_start( Task_id, Task_Periodic, 1 );
  directive_failed( status, "rtems_task_start periodic" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
