/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_1
 *
 *  This task verifies that the dual ported RAM directives work
 *  correctly by converting external addresses to internal addresses and
 *  internal addresses to external addresses.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_id           dpid;
  void              *to_be_converted;
  void              *converted;
  rtems_status_code  status;

  status = rtems_port_ident( Port_name[ 1 ], &dpid );
  directive_failed( status, "rtems_port_ident" );
  printf( "TA1 - rtems_port_ident - 0x%08" PRIxrtems_id "\n", dpid );

  to_be_converted = &External_port_area[ 0xe ];
  status = rtems_port_external_to_internal(
    Port_id[ 1 ],
    to_be_converted,
    &converted
  );
  directive_failed( status, "rtems_port_external_to_internal" );
  printf(
    "TA1 - rtems_port_external_to_internal - %p => %p\n",
    to_be_converted,
    converted
  );

  to_be_converted = &Internal_port_area[ 0xe ];
  status = rtems_port_internal_to_external(
    Port_id[ 1 ],
    to_be_converted,
    &converted
  );
  directive_failed( status, "rtems_port_internal_to_external" );
  printf(
    "TA1 - rtems_port_internal_to_external - %p => %p\n",
    to_be_converted,
    converted
  );

  to_be_converted = &Above_port_area[ 0xe ];
  status = rtems_port_external_to_internal(
    Port_id[ 1 ],
    to_be_converted,
    &converted
  );
  directive_failed( status, "rtems_port_external_to_internal" );
  printf(
    "TA1 - rtems_port_external_to_internal - %p => %p\n",
    to_be_converted,
    converted
  );

  to_be_converted = &Below_port_area[ 0xe ];
  status = rtems_port_internal_to_external(
    Port_id[ 1 ],
    to_be_converted,
    &converted
  );
  directive_failed( status, "rtems_port_internal_to_external" );
  printf(
    "TA1 - rtems_port_internal_to_external - %p => %p\n",
    to_be_converted,
    converted
  );

  status = rtems_port_delete( Port_id[ 1 ] );
  directive_failed( status, "rtems_port_delete" );
  puts( "TA1 - rtems_port_delete - DP1" );

  TEST_END();
  rtems_test_exit( 0 );
}
