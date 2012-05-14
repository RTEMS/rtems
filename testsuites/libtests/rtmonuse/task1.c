/*  Task_1_through_5
 *
 *  This routine serves as a test task for the period capabilities of the
 *  Rate Monotonic Manager.
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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#include <rtems/cpuuse.h>

uint32_t      Periods[6]    = { 0,   2,   2,   2,   2, 100 };
uint32_t      Iterations[6] = { 0,  50,  50,  50,  50,   1 };
rtems_task_priority Priorities[6] = { 0,   1,   1,   3,   4,   5 };

rtems_task Task_1_through_5(
  rtems_task_argument argument
)
{
  rtems_id          rmid;
  rtems_id          test_rmid;
  uint32_t          index;
  uint32_t          pass;
  uint32_t          failed;
  rtems_status_code status;

  status = rtems_rate_monotonic_create( argument, &rmid );
  directive_failed( status, "rtems_rate_monotonic_create" );
  put_name( Task_name[ argument ], FALSE );
  printf( "- rtems_rate_monotonic_create id = 0x%08" PRIxrtems_id "\n", rmid );

  status = rtems_rate_monotonic_ident( argument, &test_rmid );
  directive_failed( status, "rtems_rate_monotonic_ident" );
  put_name( Task_name[ argument ], FALSE );
  printf( "- rtems_rate_monotonic_ident id = 0x%08" PRIxrtems_id "\n", test_rmid );

  if ( rmid != test_rmid ) {
     printf( "RMID's DO NOT MATCH (0x%" PRIxrtems_id " and 0x%" PRIxrtems_id ")\n", rmid, test_rmid );
     rtems_test_exit( 0 );
  }

  put_name( Task_name[ argument ], FALSE );
  printf( "- (0x%08" PRIxrtems_id ") period %" PRIu32 "\n", rmid, Periods[ argument ] );

  status = rtems_task_wake_after( 2 );
  directive_failed( status, "rtems_task_wake_after" );

  switch ( argument ) {
    case 1:
    case 2:
    case 3:
    case 4:
      while ( FOREVER ) {
        status = rtems_rate_monotonic_period( rmid, Periods[ argument ] );
        directive_failed( status, "rtems_rate_monotonic_period" );
        Count.count[ argument ]++;
      }
      break;
    case 5:
      pass   = 0;
      failed = 0;

      status = rtems_rate_monotonic_period( rmid, Periods[ argument ] );
      directive_failed( status, "rtems_rate_monotonic_period 1 of TA5" );

      Get_all_counters();

      while ( FOREVER ) {
        status = rtems_rate_monotonic_period( rmid, Periods[ argument ] );
        directive_failed( status, "rtems_rate_monotonic_period 2 of TA5" );

        Get_all_counters();

        for( index = 1 ; index <= 4 ; index++ ) {
          if ( Temporary_count.count[ index ] != Iterations[ index ] ) {
            puts_nocr( "FAIL -- " );
            put_name ( Task_name[ index ], FALSE );
            printf   ( " Actual=%" PRIu32 ", Expected=%" PRIu32 "\n",
                       Temporary_count.count[ index ],
                       Iterations[ index ]
                     );
            failed += 1;
          }
        }

        if ( failed == 5 )
          rtems_test_exit( 0 );

        pass += 1;

        printf( "TA5 - PERIODS CHECK OK (%" PRIu32 ")\n", pass );

        FLUSH_OUTPUT();

        if ( pass == 10 ) {
          puts( "" );
          rtems_rate_monotonic_report_statistics();

          rtems_rate_monotonic_reset_statistics( rmid );
          puts( "" );
          puts( "TA5 - PERIOD STATISTICS RESET" );
          puts( "" );
          rtems_rate_monotonic_report_statistics();

          rtems_rate_monotonic_reset_all_statistics();
          puts( "" );
          puts( "TA5 - ALL PERIOD STATISTICS RESET" );
          puts( "" );
          rtems_rate_monotonic_report_statistics();

          puts( "" );
          puts( "*** END OF RATE MONOTONIC PERIOD STATISTICS TEST ***" );

          rtems_test_exit( 0 );
        }

      }
      break;
  }
}
