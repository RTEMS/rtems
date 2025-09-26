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

#define CONFIGURE_INIT
#include "system.h"

#include "edfparams.h"

const char rtems_test_name[] = "SPEDFSCHED 3";

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  uint32_t    index;
  rtems_status_code status;

  TEST_BEGIN();

  build_task_name();

  for ( index = 1 ; index <= NUM_TASKS ; index++ ) {
    status = rtems_task_create(
      Task_name[ index ],
      Priorities[ index ],
      RTEMS_MINIMUM_STACK_SIZE * 4,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );
  }

  for ( index = 1 ; index <= NUM_PERIODIC_TASKS ; index++ ) {
    status = rtems_task_start( Task_id[ index ], Tasks_Periodic, index );
    directive_failed( status, "rtems_task_start loop" );
  }

  for ( index = NUM_PERIODIC_TASKS+1 ; index <= NUM_TASKS ; index++ ) {
    status = rtems_task_start( Task_id[ index ], Tasks_Aperiodic, index );
    directive_failed( status, "rtems_task_start loop" );
  }

  rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );

  rtems_task_exit();
}
