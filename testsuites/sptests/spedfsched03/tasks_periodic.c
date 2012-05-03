/*  Tasks_Periodic
 *
 *  This routine serves as a test task for the EDF scheduler
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

rtems_task Tasks_Periodic(
  rtems_task_argument argument
)
{
  rtems_id          rmid;
  rtems_id          test_rmid;
  rtems_status_code status;

  int start, stop, now;

  status = rtems_rate_monotonic_create( argument, &rmid );
  directive_failed( status, "rtems_rate_monotonic_create" );
  put_name( Task_name[ argument ], FALSE );
  printf( "- rtems_rate_monotonic_create id = 0x%08" PRIxrtems_id "\n",
          rmid );

  status = rtems_rate_monotonic_ident( argument, &test_rmid );
  directive_failed( status, "rtems_rate_monotonic_ident" );
  put_name( Task_name[ argument ], FALSE );
  printf( "- rtems_rate_monotonic_ident id = 0x%08" PRIxrtems_id "\n",
          test_rmid );

  if ( rmid != test_rmid ) {
     printf( "RMID's DO NOT MATCH (0x%" PRIxrtems_id " and 0x%"
             PRIxrtems_id ")\n", rmid, test_rmid );
     rtems_test_exit( 0 );
  }

  put_name( Task_name[ argument ], FALSE );
  printf( "- (0x%08" PRIxrtems_id ") period %" PRIu32 "\n",
          rmid, Periods[ argument ] );

  status = rtems_task_wake_after( 2 + Phases[argument] );
  directive_failed( status, "rtems_task_wake_after" );

  while (FOREVER) {
    if (rtems_rate_monotonic_period(rmid, Periods[argument])==RTEMS_TIMEOUT)
      printf("P%" PRIdPTR " - Deadline miss\n", argument);

    rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start);
    printf("P%" PRIdPTR "-S ticks:%d\n", argument, start);
    if ( start >= 2*HP_LENGTH ) break; /* stop */
    /* active computing */

    /* using periodic statistics */
    while(FOREVER) {
      rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
      if (now >= start + Execution[argument]) break;
    }
    rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &stop);
    printf("P%" PRIdPTR "-F ticks:%d\n", argument, stop);
  }

  /* delete period and SELF */
  status = rtems_rate_monotonic_delete( rmid );
  if ( status != RTEMS_SUCCESSFUL ) {
    printf("rtems_rate_monotonic_delete failed with status of %d.\n",status);
    rtems_test_exit( 0 );
  }
  fflush(stdout);
  puts( "*** END OF TEST EDF SCHEDULER 3 ***" );
  rtems_test_exit( 0 );
}
