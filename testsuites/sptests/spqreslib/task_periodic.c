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

#include "system.h"

rtems_task Task_Periodic(
  rtems_task_argument argument
)
{
  rtems_id          rmid;
  rtems_status_code status;

  time_t approved_budget, exec_time, abs_time, current_budget;

  int start, stop, now;

  qres_sid_t server_id, tsid;
  qres_params_t params, tparams;

  params.P = Period;
  params.Q = Execution+1;

  printf( "Periodic task: Create server and Attach thread\n" );
  if ( qres_create_server( &params, &server_id ) )
    printf( "ERROR: CREATE SERVER FAILED\n" );
  if ( qres_attach_thread( server_id, 0, Task_id ) )
    printf( "ERROR: ATTACH THREAD FAILED\n" );

  printf( "Periodic task: ID and Get parameters\n" );
  if ( qres_get_sid( 0, Task_id, &tsid ) )
    printf( "ERROR: GET SERVER ID FAILED\n" );
  if ( tsid != server_id )
    printf( "ERROR: SERVER ID MISMATCH\n" );
  if ( qres_get_params( server_id, &tparams ) )
    printf( "ERROR: GET PARAMETERS FAILED\n" );
  if ( params.P != tparams.P ||
       params.Q != tparams.Q )
    printf( "ERROR: PARAMETERS MISMATCH\n" );

  printf( "Periodic task: Detach thread and Destroy server\n" );
  if ( qres_detach_thread( server_id, 0, Task_id ) )
    printf( "ERROR: DETACH THREAD FAILED\n" );
  if ( qres_destroy_server( server_id ) )
    printf( "ERROR: DESTROY SERVER FAILED\n" );
  if ( qres_create_server( &params, &server_id ) )
    printf( "ERROR: CREATE SERVER FAILED\n" );

  printf( "Periodic task: Current budget and Execution time\n" );
  if ( qres_get_curr_budget( server_id, &current_budget ) )
    printf( "ERROR: GET REMAINING BUDGET FAILED\n" );
  if ( current_budget != params.Q )
    printf( "ERROR: REMAINING BUDGET MISMATCH\n" );
  if ( qres_get_exec_time( server_id, &exec_time, &abs_time ) )
    printf( "ERROR: GET EXECUTION TIME FAILED\n" );

  printf( "Periodic task: Set parameters\n" );
  if ( qres_attach_thread( server_id, 0, Task_id ) )
    printf( "ERROR: ATTACH THREAD FAILED\n" );
  params.P = Period * 2;
  params.Q = Execution * 2 +1;
  if ( qres_set_params( server_id, &params ) )
    printf( "ERROR: SET PARAMS FAILED\n" );
  if ( qres_get_params( server_id, &tparams ) )
    printf( "ERROR: GET PARAMS FAILED\n" );
  if ( params.P != tparams.P ||
       params.Q != tparams.Q )
    printf( "ERROR: PARAMS MISMATCH\n" );
  params.P = Period;
  params.Q = Execution+1;
  if ( qres_set_params( server_id, &params ) )
    printf( "ERROR: SET PARAMS FAILED\n" );
  if ( qres_get_appr_budget( server_id, &approved_budget ) )
    printf( "ERROR: GET APPROVED BUDGET FAILED\n" );

  printf( "Periodic task: Approved budget\n" );
  if ( approved_budget != params.Q )
    printf( "ERROR: APPROVED BUDGET MISMATCH\n" );

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

      if ( qres_get_exec_time( server_id, &exec_time, &abs_time ) )
        printf( "ERROR: GET EXECUTION TIME FAILED\n" );
      if ( qres_get_curr_budget( server_id, &current_budget) )
        printf( "ERROR: GET CURRENT BUDGET FAILED\n" );
      if ( (current_budget + exec_time) > (Execution + 1) ) {
        printf( "ERROR: CURRENT BUDGET AND EXECUTION TIME MISMATCH\n" );
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
  if ( qres_cleanup() )
    printf( "ERROR: QRES CLEANUP\n" );

  fflush(stdout);
  puts( "*** END OF TEST QRES LIBRARY ***" );
  rtems_test_exit( 0 );
}
