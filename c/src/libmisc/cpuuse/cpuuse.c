/*
 *  CPU Usage Reporter
 *
 *  COPYRIGHT (c) 1989-1997. 1996.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 *
 */

#include <rtems.h>

extern rtems_configuration_table BSP_Configuration;

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cpuuse.h"

unsigned32 CPU_usage_Ticks_at_last_reset;

/*PAGE
 *
 *  CPU_usage_Dump
 */

void CPU_usage_Dump( void )
{
  unsigned32           i;
  unsigned32           class_index;
  Thread_Control      *the_thread;
  Objects_Information *information;
  unsigned32           u32_name;
  char                 name[5];
  unsigned32           total_units = 0;

  for ( class_index = OBJECTS_CLASSES_FIRST ;
        class_index <= OBJECTS_CLASSES_LAST ;
        class_index++ ) {
    information = _Objects_Information_table[ class_index ];
    if ( information && information->is_thread ) {
      for ( i=1 ; i <= information->maximum ; i++ ) {
        the_thread = (Thread_Control *)information->local_table[ i ];
 
        if ( the_thread )
          total_units += the_thread->ticks_executed;
      }
    }
  }

  printf("CPU Usage by thread\n");
#if defined(unix) || ( CPU_HARDWARE_FP == TRUE )
  printf( "   ID        NAME        TICKS    PERCENT\n" );
#else
  printf( "   ID        NAME        TICKS\n" );
#endif

  for ( class_index = OBJECTS_CLASSES_FIRST ; 
        class_index <= OBJECTS_CLASSES_LAST ; 
        class_index++ ) {
    information = _Objects_Information_table[ class_index ];
    if ( information && information->is_thread ) {
      for ( i=1 ; i <= information->maximum ; i++ ) {
        the_thread = (Thread_Control *)information->local_table[ i ];
        
        if ( !the_thread )
          continue;

        u32_name = *(unsigned32 *)the_thread->Object.name;

        name[ 0 ] = (u32_name >> 24) & 0xff;
        name[ 1 ] = (u32_name >> 16) & 0xff;
        name[ 2 ] = (u32_name >>  8) & 0xff;
        name[ 3 ] = (u32_name >>  0) & 0xff;
        name[ 4 ] = '\0';

#if defined(unix) || ( CPU_HARDWARE_FP == TRUE )
        printf( "0x%08x   %4s    %8d     %5.3f\n",
          the_thread->Object.id,
          name,
          the_thread->ticks_executed,
          (total_units) ? 
            (double)the_thread->ticks_executed / (double)total_units :
            (double)total_units
        );
#else
        printf( "0x%08x   %4s   %8d\n",
          the_thread->Object.id,
          name,
          the_thread->ticks_executed
        );
#endif
      }
    }
  }

  printf(
    "\nTicks since last reset = %d\n",
    _Watchdog_Ticks_since_boot - CPU_usage_Ticks_at_last_reset
  );
  printf( "\nTotal Units = %d\n", total_units );
}

/*PAGE
 *
 *  CPU_usage_Reset
 */

void CPU_usage_Reset( void )
{
  unsigned32           i;
  unsigned32           class_index;
  Thread_Control      *the_thread;
  Objects_Information *information;
 
  CPU_usage_Ticks_at_last_reset = _Watchdog_Ticks_since_boot;

  for ( class_index = OBJECTS_CLASSES_FIRST ;
        class_index <= OBJECTS_CLASSES_LAST ;
        class_index++ ) {
    information = _Objects_Information_table[ class_index ];
    if ( information && information->is_thread ) {
      for ( i=1 ; i <= information->maximum ; i++ ) {
        the_thread = (Thread_Control *)information->local_table[ i ];
 
        if ( !the_thread )
          continue;
 
        the_thread->ticks_executed = 0;
      }
    }
  }
 
}

