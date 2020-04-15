/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/lock.h>

#include <rtems/score/schedulerimpl.h>

int _Sched_Count( void )
{
  return (int) _Scheduler_Count;
}

int _Sched_Index( void )
{
  Thread_Control *executing = _Thread_Get_executing();

  return (int) _Scheduler_Get_index( _Thread_Scheduler_get_home( executing ) );
}

int _Sched_Name_to_index( const char *name, size_t len )
{
  uint32_t name_32 = 0;
  size_t i = 0;

  while ( i < 4 && i < len ) {
    name_32 |= ( (uint32_t) ( (uint8_t) *name ) ) << ( ( 3 - i ) * 8 );
    ++name;
    ++i;
  }

  for ( i = 0 ; i < _Scheduler_Count ; ++i ) {
    const Scheduler_Control *scheduler = &_Scheduler_Table[ i ];

    if ( scheduler->name == name_32 ) {
      return (int) i;
    }
  }

  return -1;
}

int _Sched_Processor_count( int index )
{
  size_t i = (size_t) index;

  if ( i < _Scheduler_Count ) {
    return _Scheduler_Get_processor_count( &_Scheduler_Table[ i ] );
  } else {
    return 0;
  }
}
