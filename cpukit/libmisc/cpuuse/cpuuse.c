/*
 *  CPU Usage Reporter
 *
 *  COPYRIGHT (c) 1989-2007
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

#include <rtems/cpuuse.h>

uint32_t   CPU_usage_Ticks_at_last_reset;

/*PAGE
 *
 *  rtems_cpu_usage_report
 */

void rtems_cpu_usage_report( void )
{
  uint32_t             i;
  uint32_t             api_index;
  Thread_Control      *the_thread;
  Objects_Information *information;
  uint32_t             u32_name;
  char                *cname;
  char                 name[5];
  uint32_t             ival, fval;
  uint32_t             total_units = 0;

  for ( api_index = 1 ;
        api_index <= OBJECTS_APIS_LAST ;
        api_index++ ) {
    if ( !_Objects_Information_table[ api_index ] )
      continue;
    information = _Objects_Information_table[ api_index ][ 1 ];
    if ( information ) {
      for ( i=1 ; i <= information->maximum ; i++ ) {
        the_thread = (Thread_Control *)information->local_table[ i ];

        if ( the_thread )
          total_units += the_thread->ticks_executed;
      }
    }
  }

  fprintf(stdout,"CPU Usage by thread\n");
  fprintf(stdout, "   ID        NAME        TICKS    PERCENT\n" );

  for ( api_index = 1 ;
        api_index <= OBJECTS_APIS_LAST ;
        api_index++ ) {
    if ( !_Objects_Information_table[ api_index ] )
      continue;
    information = _Objects_Information_table[ api_index ][ 1 ];
    if ( information ) {
      for ( i=1 ; i <= information->maximum ; i++ ) {
        the_thread = (Thread_Control *)information->local_table[ i ];

        if ( !the_thread )
          continue;

        if ( information->is_string ) {
          cname = the_thread->Object.name;
          name[ 0 ] = cname[0];
          name[ 1 ] = cname[1];
          name[ 2 ] = cname[2];
          name[ 3 ] = cname[3];
          name[ 4 ] = '\0';
        } else {
          u32_name = (uint32_t  )the_thread->Object.name;
          name[ 0 ] = (u32_name >> 24) & 0xff;
          name[ 1 ] = (u32_name >> 16) & 0xff;
          name[ 2 ] = (u32_name >>  8) & 0xff;
          name[ 3 ] = (u32_name >>  0) & 0xff;
          name[ 4 ] = '\0';
        }

        if ( !isprint(name[0]) ) name[0] = '*';
        if ( !isprint(name[1]) ) name[1] = '*';
        if ( !isprint(name[2]) ) name[2] = '*';
        if ( !isprint(name[3]) ) name[3] = '*';

        ival = total_units ?
                 the_thread->ticks_executed * 10000 / total_units : 0;
        fval = ival % 100;
        ival /= 100;
        fprintf(stdout,
          "0x%08" PRIu32 "   %4s    %8" PRId32 "     %3" PRId32
             ".%2.2" PRId32"\n",
          the_thread->Object.id,
          name,
          the_thread->ticks_executed,
          ival,
          fval
        );
      }
    }
  }

  fprintf(stdout,
    "\nTicks since last reset = %" PRId32 "\n",
    _Watchdog_Ticks_since_boot - CPU_usage_Ticks_at_last_reset
  );
  fprintf(stdout, "\nTotal Units = %" PRId32 "\n", total_units );
}

static void CPU_usage_Per_thread_handler(
  Thread_Control *the_thread
)
{
  the_thread->ticks_executed = 0;
}

/*
 *  rtems_cpu_usage_reset
 */
void rtems_cpu_usage_reset( void )
{
  CPU_usage_Ticks_at_last_reset = _Watchdog_Ticks_since_boot;

  rtems_iterate_over_all_threads(CPU_usage_Per_thread_handler);
}
