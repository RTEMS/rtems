/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Copyright (C) 2011 Petr Benes.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#include <rtems/bspIo.h>

/* forward declarations to avoid warnings */
void overrun_handler_task_4(rtems_cbs_server_id server_id);

void overrun_handler_task_4(
  rtems_cbs_server_id server_id
)
{
  (void) server_id;

  printk( "Signal overrun, fixing the task\n" );
  Violating_task[ 4 ] = 0;
  /* rtems_task_restart( RTEMS_SELF, 4 ); might be also possible*/
  return;
}

rtems_task Tasks_Periodic(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_id          rmid;
  rtems_id          test_rmid;
  rtems_status_code status;
  bool              scenario_done = 0;

  int start, stop, now;

  rtems_cbs_server_id server_id, tsid;
  rtems_cbs_parameters params, tparams;

  params.deadline = Periods[ argument ];
  params.budget = Execution[ argument ]+1;

  if ( argument == 4 ) {
    if ( rtems_cbs_create_server( &params, &overrun_handler_task_4, &server_id ))
      printf( "ERROR: CREATE SERVER FAILED\n" );
  }
  else {
    if ( rtems_cbs_create_server( &params, NULL, &server_id ) )
      printf( "ERROR: CREATE SERVER FAILED\n" );
  }
  if ( rtems_cbs_attach_thread( server_id, Task_id[ argument ] ) )
    printf( "ERROR: ATTACH THREAD FAILED\n" );
  if ( rtems_cbs_get_server_id( Task_id[ argument ], &tsid ) )
    printf( "ERROR: GET SERVER ID FAILED\n" );
  if ( tsid != server_id )
    printf( "ERROR: SERVER ID MISMATCH\n" );
  if ( rtems_cbs_get_parameters( server_id, &tparams ) )
    printf( "ERROR: GET PARAMETERS FAILED\n" );
  if ( params.deadline != tparams.deadline ||
       params.budget != tparams.budget )
    printf( "ERROR: PARAMETERS MISMATCH\n" );

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

    start = rtems_clock_get_ticks_since_boot();
    printf("P%" PRIdPTR "-S ticks:%d\n", argument, start);
    if ( start >= 2*HP_LENGTH ) break; /* stop */

    /* Specific scenario for task 4: tries to exceed announced budget,
       the task priority has to be pulled down to background. */
    now = rtems_clock_get_ticks_since_boot();
    if ( !scenario_done && argument == 4 && now >= 200 ) {
      Violating_task[ argument ] = 1;
      scenario_done = 1;
    }
    /* Specific scenario for task 3: changes scheduling parameters. */
    if ( !scenario_done && argument == 3 && now >= 250 ) {
      Periods[ argument ]   = Periods[ argument ] * 2;
      Execution[ argument ] = Execution[ argument ] * 2;
      params.deadline = Periods[ argument ];
      params.budget   = Execution[ argument ]+1;
      if ( rtems_cbs_set_parameters( server_id, &params) )
        printf( "ERROR: SET PARAMETERS FAILED\n" );
      if ( rtems_cbs_get_parameters( server_id, &tparams ) )
        printf( "ERROR: GET PARAMETERS FAILED\n" );
      if ( params.deadline != tparams.deadline ||
           params.budget != tparams.budget )
        printf( "ERROR: PARAMETERS MISMATCH\n" );
      scenario_done = 1;
    }
    /* Specific scenario for task 2: late unblock after being blocked by
       itself, the task priority has to be pulled down to background. */
    if ( !scenario_done && argument == 2 && now >= 500 ) {
      Violating_task[ argument ] = 1;
      scenario_done = 1;
    }
    if (argument == 2 && Violating_task[ argument ])
      rtems_task_wake_after( 10 );

    /* active computing */
    while(FOREVER) {
      now = rtems_clock_get_ticks_since_boot();
      if ( argument == 4 && !Violating_task[ argument ] &&
          (now >= start + Execution[argument]))
        break;
      if ( argument != 4 && (now >= start + Execution[argument]) )
        break;
    }
    stop = rtems_clock_get_ticks_since_boot();
    printf("P%" PRIdPTR "-F ticks:%d\n", argument, stop);
  }

  /* delete period and SELF */
  status = rtems_rate_monotonic_delete( rmid );
  if ( status != RTEMS_SUCCESSFUL ) {
    printf("rtems_rate_monotonic_delete failed with status of %d.\n",status);
    rtems_test_exit( 0 );
  }
  if ( rtems_cbs_cleanup() )
    printf( "ERROR: CBS CLEANUP\n" );
  fflush(stdout);
  TEST_END();
  rtems_test_exit( 0 );
}
