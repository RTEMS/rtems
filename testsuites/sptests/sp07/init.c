/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2009.
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

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP 7";

static void Task_harmless_extension_one(
  rtems_tcb *unused_one
)
{
  (void) unused_one;

  /* Do nothing */
}

static void Task_harmless_extension_two(
  rtems_tcb *unused_one,
  rtems_tcb *unused_two
)
{
  (void) unused_one;
  (void) unused_two;

  /* Do nothing */
}

static bool Task_harmless_extension_true_two(
  rtems_tcb *unused_one,
  rtems_tcb *unused_two
)
{
  (void) unused_one;
  (void) unused_two;

  return true;
}

static const rtems_extensions_table Harmless_extensions = {
  Task_harmless_extension_true_two,
  Task_harmless_extension_two,
  Task_harmless_extension_two,
  Task_harmless_extension_two,
  Task_harmless_extension_two,
  Task_harmless_extension_one,
  Task_harmless_extension_one,
  NULL
};

rtems_extensions_table Extensions = {
  Task_create_extension,     /* task create user extension */
  Task_start_extension,      /* task start user extension */
  Task_restart_extension,    /* task restart user extension */
  Task_delete_extension,     /* task delete user extension */
  NULL,                      /* task switch user extension */
  NULL,                      /* begin user extension */
  Task_exit_extension,       /* task exitted user extension */
  NULL                       /* fatal error extension */
};

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;
  rtems_id          id;

  TEST_BEGIN();

  Extension_name[ 1 ] =  rtems_build_name( 'E', 'X', 'T', '1' );
  Extension_name[ 2 ] =  rtems_build_name( 'E', 'X', 'T', '2' );

  puts  ( "rtems_extension_create - bad table -- RTEMS_INVALID_ADDRESS" );
  status = rtems_extension_create( 0xa5a5a5a5, NULL, &id );
  fatal_directive_status(
    status, RTEMS_INVALID_ADDRESS, "rtems_extension_create" );

  puts  ( "rtems_extension_create - bad id pointer -- RTEMS_INVALID_ADDRESS" );
  status = rtems_extension_create( 0xa5a5a5a5, &Extensions, NULL );
  fatal_directive_status(
    status, RTEMS_INVALID_ADDRESS, "rtems_extension_create" );

  puts  ( "rtems_extension_create - bad name -- RTEMS_INVALID_NAME" );
  status = rtems_extension_create( 0, &Extensions, &id );
  fatal_directive_status(
    status, RTEMS_INVALID_NAME, "rtems_extension_create #1" );

  puts( "rtems_extension_create - first one -- OK" );
  status = rtems_extension_create(
    Extension_name[ 1 ],
    &Extensions,
    &Extension_id[ 1 ]
  );
  directive_failed( status, "rtems_extension_create" );

  puts( "rtems_extension_create - second one-- OK" );
  status = rtems_extension_create(
    Extension_name[ 2 ],
    &Extensions,
    &Extension_id[ 2 ]
  );
  directive_failed( status, "rtems_extension_create #2" );

  puts  ( "rtems_extension_create -- RTEMS_TOO_MANY" );
  status = rtems_extension_create( 0xa5a5a5a5, &Extensions, &id );
  fatal_directive_status( status, RTEMS_TOO_MANY, "rtems_extension_create" );

  puts( "rtems_extension_delete - second one -- OK" );
  status = rtems_extension_delete( Extension_id[ 2 ] );
  directive_failed( status, "rtems_extension_delete #2" );

  puts( "rtems_extension_delete - second one again -- RTEMS_INVALID_ID" );
  status = rtems_extension_delete( Extension_id[ 2 ] );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_extension_delete #2 bad"
  );

  puts  ( "rtems_extension_ident -- OK" );
  status = rtems_extension_ident( Extension_name[1], &id );
  directive_failed( status, "rtems_extension_ident" );

  puts  ( "rtems_extension_ident - bad name -- RTEMS_INVALID_NAME" );
  status = rtems_extension_ident( Extension_name[2], &id );
  fatal_directive_status( status, RTEMS_INVALID_NAME, "rtems_extension_ident" );

  puts  ( "rtems_extension_ident - bad name -- RTEMS_INVALID_ADDRESS" );
  status = rtems_extension_ident( Extension_name[2], NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_extension_ident"
  );

  puts( "rtems_extension_create - harmless -- OK" );
  status = rtems_extension_create(
    Extension_name[ 2 ],
    &Harmless_extensions,
    &Extension_id[ 2 ]
  );
  directive_failed( status, "rtems_extension_create #3" );

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] = rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ] = rtems_build_name( 'T', 'A', '4', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );
  assert_extension_counts( &Task_created[ 0 ], 0x2 );

  status = rtems_task_create(
    Task_name[ 2 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );
  assert_extension_counts( &Task_created[ 0 ], 0x2 | 0x4 );

#define TA3_PRIORITY (RTEMS_MAXIMUM_PRIORITY - 4u)
  status = rtems_task_create(
    Task_name[ 3 ],
    TA3_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );
  assert_extension_counts( &Task_created[ 0 ], 0x2 | 0x4 | 0x8 );

#define TA4_PRIORITY (RTEMS_MAXIMUM_PRIORITY - 1u)
  status = rtems_task_create(
    Task_name[ 4 ],
    TA4_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 4 ]
  );
  directive_failed( status, "rtems_task_create of TA4" );
  assert_extension_counts( &Task_created[ 0 ], 0x2 | 0x4 | 0x8 | 0x10 );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );
  assert_extension_counts( &Task_started[ 0 ], 0x2 );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of TA2" );
  assert_extension_counts( &Task_started[ 0 ], 0x2 | 0x4 );

  status = rtems_task_start( Task_id[ 3 ], Task_3, 0 );
  directive_failed( status, "rtems_task_start of TA3" );
  assert_extension_counts( &Task_started[ 0 ], 0x2 | 0x4 | 0x8 );

  status = rtems_task_start( Task_id[ 4 ], Task_4, 0 );
  directive_failed( status, "rtems_task_start of TA4" );
  assert_extension_counts( &Task_started[ 0 ], 0x2 | 0x4 | 0x8 | 0x10 );

  status = rtems_task_restart( Task_id[ 2 ], 0 );
  directive_failed( status, "rtems_task_restart of TA3" );
  assert_extension_counts( &Task_restarted[ 0 ], 0x0 );

  rtems_task_exit();
}
