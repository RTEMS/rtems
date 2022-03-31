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

const char rtems_test_name[] = "SP PORT ERROR 01";

rtems_task Init(
  rtems_task_argument argument
)
{
  void              *converted;
  rtems_status_code status;
  
  TEST_BEGIN();
  Port_name[ 1 ]       =  rtems_build_name( 'D', 'P', '1', ' ' );
  status = rtems_port_create(
     0,
     Internal_port_area,
     External_port_area,
     sizeof( Internal_port_area ),
     &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_port_create with illegal name"
  );
  puts( "TA1 - rtems_port_create - RTEMS_INVALID_NAME" );

#if defined(_C3x) || defined(_C4x)
  puts( "TA1 - rtems_port_create - RTEMS_INVALID_ADDRESS - SKIPPED" );
#else
  status = rtems_port_create(
     Port_name[ 1 ],
     &((char *)Internal_port_area)[ 1 ],
     External_port_area,
     sizeof( Internal_port_area ),
     &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_port_create with illegal address"
  );
  puts( "TA1 - rtems_port_create - bad range - RTEMS_INVALID_ADDRESS" );
#endif

  status = rtems_port_create(
     Port_name[ 1 ],
     Internal_port_area,
     External_port_area,
     sizeof( Internal_port_area ),
     NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_port_create null Id"
  );
  puts( "TA1 - rtems_port_create - null id - RTEMS_INVALID_ADDRESS" );

  status = rtems_port_create(
     Port_name[ 1 ],
     Internal_port_area,
     External_port_area,
     sizeof( Internal_port_area ),
     &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_port_create of too many"
  );
  puts( "TA1 - rtems_port_create - RTEMS_TOO_MANY" );

  status = rtems_port_delete( 0 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_port_delete with illegal id"
  );
  puts( "TA1 - rtems_port_delete - RTEMS_INVALID_ID" );

  status = rtems_port_ident( 0, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_port_ident with illegal name"
  );
  puts( "TA1 - rtems_port_ident - RTEMS_INVALID_NAME" );

  status = rtems_port_external_to_internal(
    100,
    Internal_port_area,
    &converted
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_port_external_to_internal with illegal id"
  );

  status = rtems_port_external_to_internal(
    100,
    Internal_port_area,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_port_external_to_internal with NULL param"
  );
  puts( "TA1 - rtems_port_external_to_internal - RTEMS_INVALID_ADDRESS" );

  status = rtems_port_internal_to_external(
    100,
    Internal_port_area,
    &converted
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_port_internal_to_external with illegal id"
  );
  puts( "TA1 - rtems_port_internal_to_external - RTEMS_INVALID_ID" );

  status = rtems_port_internal_to_external(
    100,
    Internal_port_area,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_port_internal_to_external with NULL param"
  );
  puts( "TA1 - rtems_port_external_to_internal - RTEMS_INVALID_ADDRESS" );
 
 TEST_END();
}
