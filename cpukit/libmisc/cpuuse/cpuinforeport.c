/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
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
