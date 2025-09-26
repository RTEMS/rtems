/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2014.
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

#define MESSAGE_SIZE (sizeof(long) * 4)

const char rtems_test_name[] = "SP SEMAPHORE ERROR 01";

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;

  TEST_BEGIN();
  
  Semaphore_name[ 1 ]  =  rtems_build_name( 'S', 'M', '1', ' ' );
  Semaphore_name[ 2 ]  =  rtems_build_name( 'S', 'M', '2', ' ' );
  Semaphore_name[ 3 ]  =  rtems_build_name( 'S', 'M', '3', ' ' );

  /* release overflow */
  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    UINT32_MAX,
    RTEMS_COUNTING_SEMAPHORE,
    0,
    &Semaphore_id[ 1 ]
  );
  fatal_directive_status(
    status,
    RTEMS_SUCCESSFUL,
    "rtems_semaphore_create"
  );
  puts( "TA1 - rtems_semaphore_create - RTEMS_SUCCESSFUL" );
  status = rtems_semaphore_release( Semaphore_id[ 1 ] );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_semaphore_release"
  );
  puts( "TA1 - rtems_semaphore_release - RTEMS_UNSATISFIED" );
  status = rtems_semaphore_delete( Semaphore_id[ 1 ] );
  fatal_directive_status(
    status,
    RTEMS_SUCCESSFUL,
    "rtems_semaphore_delete"
  );
  puts( "TA1 - rtems_semaphore_delete - RTEMS_SUCCESSFUL" );

  /* invalid name */
  status = rtems_semaphore_create(
    0,
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_semaphore_create with illegal name"
  );
  puts( "TA1 - rtems_semaphore_create - RTEMS_INVALID_NAME" );

  /* NULL Id parameter */
  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_semaphore_create with NULL param"
  );
  puts( "TA1 - rtems_semaphore_create - RTEMS_INVALID_ADDRESS" );

  /* OK */
  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 1 ]
  );
  directive_failed( status, "rtems_semaphore_create" );
  puts( "TA1 - rtems_semaphore_create - 1 - RTEMS_SUCCESSFUL" );

  status = rtems_semaphore_create(
    Semaphore_name[ 2 ],
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 2 ]
  );
  directive_failed( status, "rtems_semaphore_create" );
  puts( "TA1 - rtems_semaphore_create - 2 - RTEMS_SUCCESSFUL" );

  do {
      status = rtems_semaphore_create(
          Semaphore_name[ 3 ],
          1,
          RTEMS_DEFAULT_ATTRIBUTES,
          RTEMS_NO_PRIORITY,
          &Junk_id
      );
  } while (status == RTEMS_SUCCESSFUL);

  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_semaphore_create of too many"
  );
  puts( "TA1 - rtems_semaphore_create - 3 - RTEMS_TOO_MANY" );

  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_INHERIT_PRIORITY | RTEMS_BINARY_SEMAPHORE | RTEMS_FIFO,
    RTEMS_NO_PRIORITY,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_semaphore_create of RTEMS_FIFO RTEMS_INHERIT_PRIORITY"
  );
  puts( "TA1 - rtems_semaphore_create - FIFO and inherit - RTEMS_NOT_DEFINED" );

  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_PRIORITY_CEILING | RTEMS_BINARY_SEMAPHORE | RTEMS_FIFO,
    RTEMS_NO_PRIORITY,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_semaphore_create of RTEMS_FIFO RTEMS_CEILING_PRIORITY"
  );
  puts( "TA1 - rtems_semaphore_create - FIFO and ceiling - RTEMS_NOT_DEFINED" );

  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY_CEILING |
      RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY,
    10,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_semaphore_create of binary with ceiling and inherit"
  );
  puts(
    "TA1 - rtems_semaphore_create - ceiling and inherit - RTEMS_NOT_DEFINED" );

  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_INHERIT_PRIORITY | RTEMS_COUNTING_SEMAPHORE | RTEMS_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_semaphore_create of RTEMS_COUNTING_SEMAPHORE RTEMS_INHERIT_PRIORITY"
  );
  puts( "TA1 - rtems_semaphore_create - RTEMS_NOT_DEFINED" );

  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    2,
    RTEMS_BINARY_SEMAPHORE,
    RTEMS_NO_PRIORITY,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_semaphore_create of binary semaphore with count > 1"
  );
  puts( "TA1 - rtems_semaphore_create - RTEMS_INVALID_NUMBER" );

  status = rtems_semaphore_delete( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_semaphore_delete with illegal id"
  );
  puts( "TA1 - rtems_semaphore_delete - RTEMS_INVALID_ID" );

  status = rtems_semaphore_delete( rtems_build_id( 1, 0, 0, 0 ) );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_semaphore_delete with local illegal id"
  );
  puts( "TA1 - rtems_semaphore_delete - local RTEMS_INVALID_ID" );

  status = rtems_semaphore_ident( 100, RTEMS_SEARCH_ALL_NODES, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_semaphore_ident will illegal name (local)"
  );
  puts( "TA1 - rtems_semaphore_ident - global RTEMS_INVALID_NAME" );

  status = rtems_semaphore_ident( 100, 1, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_semaphore_ident will illegal name (global)"
  );
  puts( "TA1 - rtems_semaphore_ident - local RTEMS_INVALID_NAME" );

  status = rtems_semaphore_release( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_semaphore_release with illegal id"
  );
  puts( "TA1 - rtems_semaphore_release - RTEMS_INVALID_ID" );

  status = rtems_semaphore_flush( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_semaphore_flush with illegal id"
  );
  puts( "TA1 - rtems_semaphore_flush - RTEMS_INVALID_ID" );

  TEST_END();
}
