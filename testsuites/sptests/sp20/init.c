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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP 20";

rtems_printer rtems_test_printer;

#define BUFFER_COUNT 20

#define BUFFER_SIZE 100

static rtems_printer_task_context printer_task;

static long buffers[ BUFFER_COUNT ][ BUFFER_SIZE / sizeof(long) ];

void end_of_test( void )
{
  rtems_test_printf( TEST_END_STRING );
  rtems_printer_task_drain( &printer_task );
  rtems_test_exit( 0 );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  int               error;
  uint32_t          index;
  rtems_status_code status;

  rtems_printer_task_set_priority( &printer_task, 254 );
  rtems_printer_task_set_file_descriptor( &printer_task, 1 );
  rtems_printer_task_set_buffer_table( &printer_task, &buffers[ 0 ][ 0 ] );
  rtems_printer_task_set_buffer_count( &printer_task, BUFFER_COUNT );
  rtems_printer_task_set_buffer_size( &printer_task, BUFFER_SIZE );
  error = rtems_print_printer_task( &rtems_test_printer, &printer_task );
  rtems_test_assert( error == 0 );

  rtems_test_printf( TEST_BEGIN_STRING );

  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] =  rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] =  rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ] =  rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 5 ] =  rtems_build_name( 'T', 'A', '5', ' ' );
  Task_name[ 6 ] =  rtems_build_name( 'T', 'A', '6', ' ' );

  for ( index = 1 ; index <= 6 ; index++ ) {
    status = rtems_task_create(
      Task_name[ index ],
      Priorities[ index ],
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );
  }

  for ( index = 1 ; index <= 6 ; index++ ) {
    status = rtems_task_start( Task_id[ index ], Task_1_through_6, index );
    directive_failed( status, "rtems_task_start loop" );
  }

  Count.count[ 1 ] = 0;
  Count.count[ 2 ] = 0;
  Count.count[ 3 ] = 0;
  Count.count[ 4 ] = 0;
  Count.count[ 5 ] = 0;
  Count.count[ 6 ] = 0;

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
