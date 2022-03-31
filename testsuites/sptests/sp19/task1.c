/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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
#include "inttest.h"

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id          tid;
  rtems_time_of_day time;
  uint32_t          task_index;
  INTEGER_DECLARE;

  status = rtems_task_ident( RTEMS_WHO_AM_I, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

  task_index = task_number( tid );

  INTEGER_LOAD( INTEGER_factors[ task_index ] );

  put_name( Task_name[ task_index ], FALSE );
  printf(
    " - integer base = (0x%" PRIx32 ")\n",
    INTEGER_factors[ task_index ]
  );

  while( FOREVER ) {
    status = rtems_clock_get_tod( &time );
    directive_failed( status, "rtems_clock_get_tod" );

    put_name( Task_name[ task_number( tid ) ], FALSE );
    print_time( " - rtems_clock_get_tod - ", &time, "\n" );

    INTEGER_CHECK( INTEGER_factors[ task_index ] );

    status = rtems_task_wake_after(
      ( task_number( tid ) ) * 5 * rtems_clock_get_ticks_per_second()
    );
    directive_failed( status, "rtems_task_wake_after" );
  }
}
