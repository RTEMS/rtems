/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Periodic test task for EDF Scheduler 
 */

/*
 * Copyright (C) 2025 Gedare Bloom
 * Copyright (C) 2011 Petr Benes
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

rtems_task Tasks_Periodic(
  rtems_task_argument argument
)
{
  rtems_id          rmid;
  rtems_id          test_rmid;
  rtems_id          task_id;
  rtems_id          sched_id;
  rtems_status_code status;

  rtems_task_priority prio_cur;
  rtems_task_priority prio_max;

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

  task_id = Task_id[ argument ];
  status = rtems_task_get_scheduler( task_id, &sched_id);
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  status = rtems_scheduler_get_maximum_priority( sched_id, &prio_max );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );
  rtems_test_assert( prio_max == CONFIGURE_MAXIMUM_PRIORITY );

  put_name( Task_name[ argument ], FALSE );
  printf( "- (0x%08" PRIxrtems_id ") period %" PRIu32 "\n",
          rmid, Periods[ argument ] );

  status = rtems_task_wake_after( 2 + Phases[argument] );
  directive_failed( status, "rtems_task_wake_after" );

  while (FOREVER) {
    if (rtems_rate_monotonic_period(rmid, Periods[argument])==RTEMS_TIMEOUT)
      printf("P%" PRIdPTR " - Deadline miss\n", argument);

    status = rtems_task_get_priority( task_id, sched_id, &prio_cur );
    rtems_test_assert( status == RTEMS_SUCCESSFUL );
    rtems_test_assert( prio_cur == 0 );

    start = rtems_clock_get_ticks_since_boot();
    printf("P%" PRIdPTR "-S ticks:%d\n", argument, start);
    if ( start >= 2*HP_LENGTH ) break; /* stop */
    /* active computing */

    /* using periodic statistics */
    while(FOREVER) {
      now = rtems_clock_get_ticks_since_boot();
      if (now >= start + Execution[argument]) break;
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
  fflush(stdout);
  TEST_END();
  rtems_test_exit( 0 );
}
