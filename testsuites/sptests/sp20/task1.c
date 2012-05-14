/*
 *  COPYRIGHT (c) 1989-2011.
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

/*
 runtime of TA6 should be shorter than TA5
 */
#define TA6_ITERATIONS 10
#define TA6_PERIOD_FACTOR 10

uint32_t      Periods[7]    = { 0,   2,   2,   2,   2, 100, 0 };
uint32_t      Iterations[7] = { 0,  50,  50,  50,  50,   1, TA6_ITERATIONS };
rtems_task_priority Priorities[7] = { 0,   1,   1,   3,   4,   5, 1 };

rtems_task Task_1_through_6(
  rtems_task_argument argument
)
{
  rtems_id          rmid;
  rtems_id          test_rmid;
  int               index;
  int               pass;
  uint32_t          failed;
  rtems_status_code status;

  status = rtems_rate_monotonic_create( argument, &rmid );
  directive_failed( status, "rtems_rate_monotonic_create" );
  put_name( Task_name[ argument ], FALSE );
  printf( "- rtems_rate_monotonic_create id = 0x%08" PRIxrtems_id "\n", rmid );

  status = rtems_rate_monotonic_ident( argument, &test_rmid );
  directive_failed( status, "rtems_rate_monotonic_ident" );
  put_name( Task_name[ argument ], FALSE );
  printf(
    "- rtems_rate_monotonic_ident id = 0x%08" PRIxrtems_id "\n",
    test_rmid
  );

  if ( rmid != test_rmid ) {
    printf(
      "RMID's DO NOT MATCH (0x%" PRIxrtems_id " and 0x%" PRIxrtems_id ")\n",
       rmid,
       test_rmid
    );
    rtems_test_exit( 0 );
  }

  put_name( Task_name[ argument ], FALSE );
  printf(
    "- (0x%08" PRIxrtems_id ") period %" PRIu32 "\n",
    rmid,
    Periods[ argument ]
  );

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

        printf( "TA5 - PERIODS CHECK OK (%d)\n", pass );

        fflush( stdout );

        if ( pass == 10 ) {
          puts( "*** END OF TEST 20 ***" );
          rtems_test_exit( 0 );
        }

      }
      break;
    case 6:
      /* test changing periods */
      {
        uint32_t   time[TA6_ITERATIONS+1];
        rtems_interval period;

        period = 1*TA6_PERIOD_FACTOR;
        status = rtems_rate_monotonic_period( rmid, period);
        directive_failed( status, "rtems_rate_monotonic_period of TA6" );
        time[0] = _Watchdog_Ticks_since_boot; /* timestamp */
        /*printf("%d - %d\n", period, time[0]);*/

        for (index = 1; index <= TA6_ITERATIONS; index++) {
          period = (index+1)*TA6_PERIOD_FACTOR;
          status = rtems_rate_monotonic_period( rmid,  period);
          directive_failed( status, "rtems_rate_monotonic_period of TA6" );
          time[index] = _Watchdog_Ticks_since_boot; /* timestamp */
          /*printf("%d - %d\n", period, time[index]);*/
        }

        for (index = 1; index <= TA6_ITERATIONS; index++) {
          rtems_interval meas = time[index] - time[index-1];
          period = index*TA6_PERIOD_FACTOR;
          printf(
            "TA6 - Actual: %" PRIdrtems_interval
              " Expected: %" PRIdrtems_interval,
            meas,
            period
          );
          if (period == meas) printf(" - OK\n");
          else                printf(" - FAILED\n");
        }
      }
      rtems_task_suspend(RTEMS_SELF);
      break;
  }
}
