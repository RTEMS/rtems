/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This source file contains the implementation of
 *   _Sched_Count(), _Sched_Index(), _Sched_Name_to_index(), and
 *   _Sched_Processor_count().
 */

/*
 * Copyright (c) 2015 embedded brains GmbH & Co. KG
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
