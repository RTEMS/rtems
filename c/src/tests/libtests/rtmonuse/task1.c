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

#include <rtems/cpuuse.h>
#include <rtems/rtmonuse.h>

rtems_unsigned32    Periods[6]    = { 0,   2,   2,   2,   2, 100 };
rtems_unsigned32    Iterations[6] = { 0,  50,  50,  50,  50,   1 };
rtems_task_priority Priorities[6] = { 0,   1,   1,   3,   4,   5 };

rtems_task Task_1_through_5(
  rtems_unsigned32 argument
)
{
  rtems_id          rmid;
  rtems_id          test_rmid;
  rtems_unsigned32  index;
  rtems_unsigned32  pass;
  rtems_unsigned32  failed;
  rtems_status_code status;

  status = rtems_rate_monotonic_create( argument, &rmid );
  directive_failed( status, "rtems_rate_monotonic_create" );
  put_name( Task_name[ argument ], FALSE );
  printf( "- rtems_rate_monotonic_create id = 0x%08x\n", rmid );

  status = rtems_rate_monotonic_ident( argument, &test_rmid );
  directive_failed( status, "rtems_rate_monotonic_ident" );
  put_name( Task_name[ argument ], FALSE );
  printf( "- rtems_rate_monotonic_ident id = 0x%08x\n", test_rmid );

  if ( rmid != test_rmid ) {
     printf( "RMID's DO NOT MATCH (0x%x and 0x%x)\n", rmid, test_rmid );
     rtems_test_exit( 0 );
  }

  put_name( Task_name[ argument ], FALSE );
  printf( "- (0x%08x) period %d\n", rmid, Periods[ argument ] );

  status = rtems_task_wake_after( 2 );
  directive_failed( status, "rtems_task_wake_after" );

  switch ( argument ) {
    case 1:
    case 2:
    case 3:
    case 4:
      while ( FOREVER ) {
        Period_usage_Update( rmid );

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
        Period_usage_Update( rmid );

        status = rtems_rate_monotonic_period( rmid, Periods[ argument ] );
        directive_failed( status, "rtems_rate_monotonic_period 2 of TA5" );

        Get_all_counters();

        for( index = 1 ; index <= 4 ; index++ ) {
          if ( Temporary_count.count[ index ] != Iterations[ index ] ) {
            puts_nocr( "FAIL -- " );
            put_name ( Task_name[ index ], FALSE );
            printf   ( " Actual=%d, Expected=%d\n",
                       Temporary_count.count[ index ],
                       Iterations[ index ]
                     );
            failed += 1;
          }
        }

        if ( failed == 5 )
          rtems_test_exit( 0 );

        pass += 1;

        printf( "TA5 - PERIODS CHECK OK (%d)\n", pass );

        fflush( stdout );

        if ( pass == 10 ) {
          puts( "*** END OF RATE MONOTONIC PERIOD STATISTICS TEST ***" );
          CPU_usage_Dump();
          Period_usage_Dump();
          rtems_test_exit( 0 );
        }

      }
      break;
  }
}
