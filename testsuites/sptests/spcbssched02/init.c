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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_cbs_server_id server_id, server_id2;
  time_t approved_budget, exec_time, abs_time, remaining_budget;
  rtems_cbs_parameters params, params1, params2, params3, params4;

  Priority = 30;
  Period    = 30;
  Execution = 10;
  Phase = 0;

  params.deadline = 1;
  params.budget = 1;

  params1 = params2 = params3 = params4 = params;
  params1.budget = -1;
  params2.budget = SCHEDULER_EDF_PRIO_MSB + 1;
  params3.deadline = -1;
  params4.deadline = SCHEDULER_EDF_PRIO_MSB + 1;

  puts( "\n\n*** TEST CBS SCHEDULER 2 ***" );

  Task_name = rtems_build_name( 'P', 'T', '1', ' ' );
  Task_name2 = rtems_build_name( 'P', 'T', '2', ' ' );

  status = rtems_task_create(
    Task_name,
    Priority,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id
  );
  directive_failed( status, "rtems_task_create loop" );

  status = rtems_task_create(
    Task_name2,
    Priority,
    RTEMS_MINIMUM_STACK_SIZE * 4,
    RTEMS_NO_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id2
  );
  directive_failed( status, "rtems_task_create loop" );

  printf( "Init: Initializing the CBS\n" );
  if ( rtems_cbs_initialize() )
    printf( "ERROR: CBS INITIALIZATION FAILED\n" );

  /* Error checks for Create server and Destroy server  */
  printf( "Init: Create server and Destroy server\n" );
  if ( rtems_cbs_destroy_server( -5 ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: DESTROY SERVER PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_destroy_server( 5 ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: DESTROY SERVER PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_destroy_server( 0 ) != SCHEDULER_CBS_ERROR_NOSERVER )
    printf( "ERROR: DESTROY SERVER PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_create_server( &params1, NULL, &server_id ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: CREATE SERVER PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_create_server( &params2, NULL, &server_id ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: CREATE SERVER PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_create_server( &params3, NULL, &server_id ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: CREATE SERVER PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_create_server( &params4, NULL, &server_id ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: CREATE SERVER PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_create_server( &params, NULL, &server_id2 ) )
    printf( "ERROR: CREATE SERVER FAILED\n" );
  if ( rtems_cbs_create_server( &params, NULL, &server_id ) )
    printf( "ERROR: CREATE SERVER FAILED\n" );
  if ( rtems_cbs_create_server( &params, NULL, &server_id ) )
    printf( "ERROR: CREATE SERVER FAILED\n" );
  if ( rtems_cbs_create_server( &params, NULL, &server_id ) !=
       SCHEDULER_CBS_ERROR_FULL )
    printf( "ERROR: CREATE SERVER PASSED UNEXPECTEDLY\n" );

  /* Error checks for Attach thread and Detach thread */
  printf( "Init: Attach thread\n" );
  if ( rtems_cbs_attach_thread( -5, RTEMS_SELF ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: ATTACH THREAD PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_attach_thread( 5, RTEMS_SELF ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: ATTACH THREAD PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_attach_thread( server_id, 1234 ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: ATTACH THREAD PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_attach_thread( server_id, RTEMS_SELF ) )
    printf( "ERROR: ATTACH THREAD FAILED\n" );
  if ( rtems_cbs_attach_thread( server_id, RTEMS_SELF ) !=
       SCHEDULER_CBS_ERROR_FULL )
    printf( "ERROR: ATTACH THREAD AGAIN PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_attach_thread( server_id, Task_id ) !=
       SCHEDULER_CBS_ERROR_FULL )
    printf( "ERROR: ATTACH THREAD TO FULL SERVER PASSED UNEXPECTEDLY \n" );
  if ( rtems_cbs_destroy_server( server_id ) )
    printf( "ERROR: DESTROY SERVER WITH THREAD ATTACHED FAILED\n" );
  if ( rtems_cbs_attach_thread( server_id, RTEMS_SELF ) !=
       SCHEDULER_CBS_ERROR_NOSERVER )
    printf( "ERROR: ATTACH THREAD PASSED UNEXPECTEDLY\n" );

  printf( "Init: Detach thread\n" );
  if ( rtems_cbs_detach_thread( -5, RTEMS_SELF ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: DETACH THREAD PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_detach_thread( 5, RTEMS_SELF ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: DETACH THREAD PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_detach_thread( server_id2, 1234 ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: DETACH THREAD PASSED UNEXPECTEDLY \n" );
  if ( rtems_cbs_detach_thread( server_id, RTEMS_SELF ) !=
       SCHEDULER_CBS_ERROR_NOSERVER )
    printf( "ERROR: DETACH THREAD PASSED UNEXPECTEDLY4\n" );
  rtems_cbs_destroy_server( server_id2 );

  /* Error checks for Set parameters and Get parameters */
  printf( "Init: Set parameters and Get parameters\n" );
  if ( rtems_cbs_set_parameters( -5, &params ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: SET PARAMETERS PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_set_parameters( 5, &params ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: SET PARAMETERS PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_set_parameters( server_id, &params ) !=
       SCHEDULER_CBS_ERROR_NOSERVER )
    printf( "ERROR: SET PARAMETERS PASSED UNEXPECTEDLY\n" );

  if ( rtems_cbs_get_parameters( -5, &params ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: GET PARAMETERS PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_get_parameters( 5, &params ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: GET PARAMETERS PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_get_parameters( server_id, &params ) !=
       SCHEDULER_CBS_ERROR_NOSERVER )
    printf( "ERROR: GET PARAMETERS PASSED UNEXPECTEDLY\n" );

  if ( rtems_cbs_set_parameters( server_id, &params1 ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: SET PARAMETERS PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_set_parameters( server_id, &params2 ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: SET PARAMETERS PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_set_parameters( server_id, &params3 ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: SET PARAMETERS PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_set_parameters( server_id, &params4 ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: SET PARAMETERS PASSED UNEXPECTEDLY\n" );

  /* Error checks for Get server id */
  printf( "Init: Get server id\n" );
  if ( rtems_cbs_get_server_id( RTEMS_SELF, &server_id ) !=
       SCHEDULER_CBS_ERROR_NOSERVER )
    printf( "ERROR: GET SERVER ID PASSED UNEXPECTEDLY\n" );

  /* Error checks for Get approved budget */
  printf( "Init: Get approved budget\n" );
  if ( rtems_cbs_get_approved_budget( -5, &approved_budget ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: GET APPROVED BUDGET PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_get_approved_budget( 5, &approved_budget ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: GET APPROVED BUDGET PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_get_approved_budget( server_id, &approved_budget ) !=
       SCHEDULER_CBS_ERROR_NOSERVER )
    printf( "ERROR: GET APPROVED BUDGET PASSED UNEXPECTEDLY\n" );

  /* Error checks for Get remaining budget */
  printf( "Init: Get remaining budget\n" );
  if ( rtems_cbs_get_remaining_budget( -5, &remaining_budget ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: GET REMAINING BUDGET PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_get_remaining_budget( 5, &remaining_budget ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: GET REMAINING BUDGET PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_get_remaining_budget( server_id, &remaining_budget ) !=
       SCHEDULER_CBS_ERROR_NOSERVER )
    printf( "ERROR: GET REMAINING BUDGET PASSED UNEXPECTEDLY\n" );

  /* Error checks for Get execution time */
  printf( "Init: Get execution time\n" );
  if ( rtems_cbs_get_execution_time( -5, &exec_time, &abs_time ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: GET EXECUTION TIME PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_get_execution_time( 5, &exec_time, &abs_time ) !=
       SCHEDULER_CBS_ERROR_INVALID_PARAMETER )
    printf( "ERROR: GET EXECUTION TIME PASSED UNEXPECTEDLY\n" );
  if ( rtems_cbs_get_execution_time( server_id, &exec_time, &abs_time ) !=
       SCHEDULER_CBS_ERROR_NOSERVER )
    printf( "ERROR: GET EXECUTION TIME PASSED UNEXPECTEDLY\n" );

  /* Restart CBS library */
  printf( "Init: Cleaning up CBS\n" );
  if ( rtems_cbs_cleanup() )
    printf( "ERROR: CBS CLEANUP FAILED\n" );
  printf( "Init: Initializing the CBS\n" );
  if ( rtems_cbs_initialize() )
    printf( "ERROR: CBS INITIALIZATION FAILED\n" );

  /* Start periodic tasks */
  printf( "Init: Starting periodic task\n" );
  status = rtems_task_start( Task_id, Task_Periodic, 1 );
  directive_failed( status, "rtems_task_start periodic" );
  status = rtems_task_start( Task_id2, Task_Periodic, 2 );
  directive_failed( status, "rtems_task_start periodic" );

  rtems_task_wake_after( 130 );

  printf( "Init: Checking server with a deleted task\n" );
  if ( rtems_cbs_get_execution_time( 0, &exec_time, &abs_time ) )
    printf( "ERROR: GET EXECUTION TIME FAILED\n" );
  if ( rtems_cbs_get_remaining_budget( 0, &remaining_budget) )
    printf( "ERROR: GET REMAINING BUDGET FAILED\n" );

  if ( rtems_cbs_cleanup() )
    printf( "ERROR: CBS CLEANUP\n" );

  fflush(stdout);
  puts( "*** END OF TEST CBS SCHEDULER 2 ***" );
  rtems_test_exit( 0 );
}
