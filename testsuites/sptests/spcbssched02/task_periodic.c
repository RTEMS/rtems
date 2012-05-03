/*  Tasks_Periodic
 *
 *  This routine serves as a test task for the CBS scheduler
 *  implementation.
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

#include "system.h"

rtems_task Task_Periodic(
  rtems_task_argument argument
)
{
  rtems_id          rmid;
  rtems_status_code status;

  time_t approved_budget, exec_time, abs_time, remaining_budget;

  int start, stop, now;

  rtems_cbs_server_id server_id, tsid;
  rtems_cbs_parameters params, tparams;

  params.deadline = Period;
  params.budget = Execution+1;

  /* Taks 1 will be attached to a server, task 2 not. */
  if ( argument == 1 ) {
    printf( "Periodic task: Create server and Attach thread\n" );
    if ( rtems_cbs_create_server( &params, NULL, &server_id ) )
      printf( "ERROR: CREATE SERVER FAILED\n" );
    if ( rtems_cbs_attach_thread( server_id, Task_id ) )
      printf( "ERROR: ATTACH THREAD FAILED\n" );

    printf( "Periodic task: ID and Get parameters\n" );
    if ( rtems_cbs_get_server_id( Task_id, &tsid ) )
      printf( "ERROR: GET SERVER ID FAILED\n" );
    if ( tsid != server_id )
      printf( "ERROR: SERVER ID MISMATCH\n" );
    if ( rtems_cbs_get_parameters( server_id, &tparams ) )
      printf( "ERROR: GET PARAMETERS FAILED\n" );
    if ( params.deadline != tparams.deadline ||
         params.budget != tparams.budget )
      printf( "ERROR: PARAMETERS MISMATCH\n" );

    printf( "Periodic task: Detach thread and Destroy server\n" );
    if ( rtems_cbs_detach_thread( server_id, Task_id ) )
      printf( "ERROR: DETACH THREAD FAILED\n" );
    if ( rtems_cbs_destroy_server( server_id ) )
      printf( "ERROR: DESTROY SERVER FAILED\n" );
    if ( rtems_cbs_create_server( &params, NULL, &server_id ) )
      printf( "ERROR: CREATE SERVER FAILED\n" );

    printf( "Periodic task: Remaining budget and Execution time\n" );
    if ( rtems_cbs_get_remaining_budget( server_id, &remaining_budget ) )
      printf( "ERROR: GET REMAINING BUDGET FAILED\n" );
    if ( remaining_budget != params.budget )
      printf( "ERROR: REMAINING BUDGET MISMATCH\n" );
    if ( rtems_cbs_get_execution_time( server_id, &exec_time, &abs_time ) )
      printf( "ERROR: GET EXECUTION TIME FAILED\n" );

    printf( "Periodic task: Set parameters\n" );
    if ( rtems_cbs_attach_thread( server_id, Task_id ) )
      printf( "ERROR: ATTACH THREAD FAILED\n" );
    params.deadline = Period * 2;
    params.budget = Execution * 2 +1;
    if ( rtems_cbs_set_parameters( server_id, &params ) )
      printf( "ERROR: SET PARAMS FAILED\n" );
    if ( rtems_cbs_get_parameters( server_id, &tparams ) )
      printf( "ERROR: GET PARAMS FAILED\n" );
    if ( params.deadline != tparams.deadline ||
         params.budget != tparams.budget )
      printf( "ERROR: PARAMS MISMATCH\n" );
    params.deadline = Period;
    params.budget = Execution+1;
    if ( rtems_cbs_set_parameters( server_id, &params ) )
      printf( "ERROR: SET PARAMS FAILED\n" );
    if ( rtems_cbs_get_approved_budget( server_id, &approved_budget ) )
      printf( "ERROR: GET APPROVED BUDGET FAILED\n" );

    printf( "Periodic task: Approved budget\n" );
    if ( approved_budget != params.budget )
      printf( "ERROR: APPROVED BUDGET MISMATCH\n" );
  }

  status = rtems_rate_monotonic_create( argument, &rmid );
  directive_failed( status, "rtems_rate_monotonic_create" );

  /* Starting periodic behavior of the task */
  printf( "Periodic task: Starting periodic behavior\n" );
  status = rtems_task_wake_after( 1 + Phase );
  directive_failed( status, "rtems_task_wake_after" );

  while ( FOREVER ) {
    if ( rtems_rate_monotonic_period(rmid, Period) == RTEMS_TIMEOUT )
      printf( "P%" PRIdPTR " - Deadline miss\n", argument );

    rtems_clock_get( RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start );
    printf( "P%" PRIdPTR "-S ticks:%d\n", argument, start );
    if ( start > 4*Period+Phase ) break; /* stop */
    /* active computing */
    while(FOREVER) {
      rtems_clock_get( RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now );
      if ( now >= start + Execution ) break;

      if ( rtems_cbs_get_execution_time( server_id, &exec_time, &abs_time ) )
        printf( "ERROR: GET EXECUTION TIME FAILED\n" );
      if ( rtems_cbs_get_remaining_budget( server_id, &remaining_budget) )
        printf( "ERROR: GET REMAINING BUDGET FAILED\n" );
      if ( (remaining_budget + exec_time) > (Execution + 1) ) {
        printf( "ERROR: REMAINING BUDGET AND EXECUTION TIME MISMATCH\n" );
        rtems_test_exit( 0 );
      }
    }
    rtems_clock_get( RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &stop );
    printf( "P%" PRIdPTR "-F ticks:%d\n", argument, stop );
  }

  /* delete period and SELF */
  status = rtems_rate_monotonic_delete( rmid );
  if ( status != RTEMS_SUCCESSFUL ) {
    printf("rtems_rate_monotonic_delete failed with status of %d.\n", status);
    rtems_test_exit( 0 );
  }
  printf( "Periodic task: Deleting self\n" );
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
