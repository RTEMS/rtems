/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#include <rtems/cpuuse.h>
#include <rtems/print.h>

#include <ctype.h>
#include <inttypes.h>

#include <rtems/score/schedulerimpl.h>

static char bits_to_char( uint8_t bits )
{
  return isprint( bits ) ? (char) bits : '?';
}

static void name_to_str( uint32_t name, char str[ 5 ] )
{
  str[ 0 ] = bits_to_char( (uint8_t) ( name >> 24 ) );
  str[ 1 ] = bits_to_char( (uint8_t) ( name >> 16 ) );
  str[ 2 ] = bits_to_char( (uint8_t) ( name >> 8 ) );
  str[ 3 ] = bits_to_char( (uint8_t) ( name >> 0 ) );
  str[ 4 ] = '\0';
}

int rtems_cpu_info_report( const rtems_printer *printer )
{
  uint32_t cpu_max;
  uint32_t cpu_index;
  int      n;

  cpu_max = rtems_configuration_get_maximum_processors();

  n = rtems_printf(
    printer,
     "-------------------------------------------------------------------------------\n"
     "                            PER PROCESSOR INFORMATION\n"
     "-------+--------+--------------+-----------------------------------------------\n"
     " INDEX | ONLINE | SCHEDULER ID | SCHEDULER NAME\n"
     "-------+--------+--------------+-----------------------------------------------\n"
   );

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    const Per_CPU_Control *cpu;
    const Scheduler_Control *scheduler;
    char scheduler_str[ 5 ];
    uint32_t scheduler_id;

    cpu = _Per_CPU_Get_by_index( cpu_index );
    scheduler = _Scheduler_Get_by_CPU( cpu );

    if ( scheduler != NULL ) {
      scheduler_id = _Scheduler_Build_id( _Scheduler_Get_index( scheduler ) );
      name_to_str( scheduler->name, scheduler_str );
    } else {
      scheduler_id = 0;
      scheduler_str[ 0 ] = '\0';
    }

    n += rtems_printf(
      printer,
      " %5" PRIu32 " | %6i |   0x%08" PRIx32 " | %s\n",
      cpu_index,
      _Per_CPU_Is_processor_online( cpu ),
      scheduler_id,
      &scheduler_str[ 0 ]
    );
  }

  return n;
}
