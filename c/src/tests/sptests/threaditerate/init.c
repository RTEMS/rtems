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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day time;
  rtems_status_code rtems_status;

#ifdef RTEMS_POSIX_API
  int posix_status;
#endif

#ifdef RTEMS_ITRON_API
  ER itron_status;
  T_CTSK pk_ctsk;
#endif

  puts( "\n\n*** Thread Iteration Test ***" );

  build_time( &time, 12, 31, 2000, 9, 0, 0, 0 );
  rtems_status = rtems_clock_set( &time );
  directive_failed( rtems_status, "rtems_clock_set" );

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] = rtems_build_name( 'T', 'A', '3', ' ' );

  rtems_status = rtems_task_create(
     Task_name[ 1 ],
     1,
     RTEMS_MINIMUM_STACK_SIZE * 2,
     RTEMS_INTERRUPT_LEVEL(31),
     RTEMS_DEFAULT_ATTRIBUTES,
     &Task_id[ 1 ]
  );
  directive_failed( rtems_status, "rtems_task_create of TA1" );

  rtems_status = rtems_task_create(
     Task_name[ 2 ],
     1,
     RTEMS_MINIMUM_STACK_SIZE * 2,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES,
     &Task_id[ 2 ]
  );
  directive_failed( rtems_status, "rtems_task_create of TA2" );

  rtems_status = rtems_task_create(
     Task_name[ 3 ],
     1,
     RTEMS_MINIMUM_STACK_SIZE * 3,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES,
     &Task_id[ 3 ]
  );
  directive_failed( rtems_status, "rtems_task_create of TA3" );

  rtems_status = rtems_task_start( Task_id[ 1 ], RTEMS_task_1_through_3, 0 );
  if ( rtems_status == RTEMS_SUCCESSFUL )
    printf("Created and started an RTEMS task with id = 0x%08x\n", Task_id[1] );
  else
    directive_failed( rtems_status, "rtems_task_start of TA1" );

  rtems_status = rtems_task_start( Task_id[ 2 ], RTEMS_task_1_through_3, 0 );
  if ( rtems_status == RTEMS_SUCCESSFUL )
    printf("Created and started an RTEMS task with id = 0x%08x\n", Task_id[2] );
  else
    directive_failed( rtems_status, "rtems_task_start of TA2" );

  rtems_status = rtems_task_start( Task_id[ 3 ], RTEMS_task_1_through_3, 0 );
  if ( rtems_status == RTEMS_SUCCESSFUL )
    printf("Created and started an RTEMS task with id = 0x%08x\n", Task_id[3] );
  else
    directive_failed( rtems_status, "rtems_task_start of TA3" );

#ifdef RTEMS_POSIX_API
  posix_status = pthread_create( &pthread_id[0], NULL, pthread_1_through_3, NULL );
  if ( !posix_status )
    printf("Created and started a pthread with id = 0x%08x\n", pthread_id[0] );
  else
    assert( !posix_status );

  posix_status = pthread_create( &pthread_id[1], NULL, pthread_1_through_3, NULL );
  if ( !posix_status )
    printf("Created and started a pthread with id = 0x%08x\n", pthread_id[1] );
  else
    assert( !posix_status );

  posix_status = pthread_create( &pthread_id[2], NULL, pthread_1_through_3, NULL );
  if ( !posix_status )
    printf("Created and started a pthread with id = 0x%08x\n", pthread_id[2] );
  else
    assert( !posix_status );

#endif

#ifdef RTEMS_ITRON_API

#define ITRON_GET_TASK_ID(_index) \
  _ITRON_Task_Information.local_table[_index]->id

  pk_ctsk.exinf    = NULL;
  pk_ctsk.tskatr   = TA_HLNG;
  pk_ctsk.itskpri  = 1; 
  pk_ctsk.task     = ITRON_task_2_through_4;
  pk_ctsk.stksz    = RTEMS_MINIMUM_STACK_SIZE * 2;
  
  itron_status = cre_tsk( 2, &pk_ctsk );
  directive_failed( itron_status, "cre_tsk of task" );

  pk_ctsk.stksz = RTEMS_MINIMUM_STACK_SIZE * 2;
  itron_status = cre_tsk( 3, &pk_ctsk );
  directive_failed( itron_status, "cre_tsk of TA2" );

  pk_ctsk.stksz = RTEMS_MINIMUM_STACK_SIZE * 3; 
  itron_status = cre_tsk( 4, &pk_ctsk );
  directive_failed( itron_status, "cre_tsk of TA3" );

  itron_status  = sta_tsk( 2, 0 );
  if ( !itron_status )
    printf("Created and started an ITRON task with id = 0x%08x\n", ITRON_GET_TASK_ID( 2 ) );
  else
    directive_failed( itron_status, "sta_tsk of ITRON task 2" );

  itron_status  = sta_tsk( 3, 0 );
  if ( !itron_status )
    printf("Created and started an ITRON task with id = 0x%08x\n", ITRON_GET_TASK_ID( 3 ) );
  else
    directive_failed( itron_status, "sta_tsk of ITRON task 3" );

  itron_status  = sta_tsk( 4, 0 );
  if ( !itron_status )
    printf("Created and started an ITRON task with id = 0x%08x\n", ITRON_GET_TASK_ID( 4 ) );
  else
    directive_failed( itron_status, "sta_tsk of ITRON task 4" );
#endif

  printf( "Sleeping for 5 seconds\n" );
  fflush( stdout );

  rtems_status = rtems_task_wake_after( 5 * TICKS_PER_SECOND );
  directive_failed( rtems_status, "rtems_task_wake_after" );

  printf( "\nLooking for all threads\n\n" );
  _Thread_Local_iterate( print_thread_info, NULL, FALSE );

  printf( "That was it. Sleeping for 20 seconds.\n" );
  
  rtems_status = rtems_task_wake_after( 20 * TICKS_PER_SECOND );
  directive_failed( rtems_status, "rtems_task_wake_after" );

  /* If we get here, all threads were created */
  printf( "\nDeleting the second thread of each type\n\n" );

  rtems_status = rtems_task_suspend( Task_id[2] );
  directive_failed( rtems_status, "rtems_task_suspend" );
  rtems_status = rtems_task_delete( Task_id[2] );
  directive_failed( rtems_status, "rtems_task_delete" );

#ifdef RTEMS_POSIX_API
  posix_status = pthread_cancel( pthread_id[2] );
  assert( !posix_status );
#endif

#ifdef RTEMS_ITRON_API
  itron_status = ter_tsk( 3 );
   directive_failed( itron_status, "ter_tsk of ITRON task 3" );
#endif

  printf( "\nLooking for all threads\n\n" );
  _Thread_Local_iterate( print_thread_info, NULL, FALSE );

  printf( "That was it. Exiting\n" );
  fflush( stdout );
  
  exit( 0 );
}
